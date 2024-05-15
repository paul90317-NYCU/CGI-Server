#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

using boost::asio::ip::tcp;

class session : public std::enable_shared_from_this<session>
{
public:
    session(tcp::socket socket, std::string server_addr)
        : socket_(std::move(socket)), SERVER_ADDR(server_addr)
    {
    }

    void start() { read_request(); }

private:
    void read_request()
    {
        auto self(shared_from_this());
        // Read data until newline (HTTP request headers usually end with a
        // blank line)
        boost::asio::async_read_until(
            socket_, boost::asio::dynamic_buffer(request), "\r\n\r\n",
            [this, self](boost::system::error_code ec, std::size_t length) {
                if (!ec)
                    handle_request();
            });
    }
    void handle_request()
    {
        auto self(shared_from_this());
        // Parse request data
        std::stringstream ss(request);
        std::string request_method, request_uri, query_string, server_protocol,
            http_host;

        // Parse request line
        ss >> request_method >> request_uri >> server_protocol;
        std::string::size_type pos = request_uri.find('?');
        if (pos != std::string::npos) {
            query_string = request_uri.substr(pos + 1);
            request_uri = request_uri.substr(0, pos);
        }

        // Parse HTTP headers
        std::string line;
        std::unordered_map<std::string, std::string> headers;
        while (std::getline(ss, line) && !line.empty()) {
            std::istringstream iss(line);
            std::string header_name, header_value;
            std::getline(iss, header_name, ':');
            std::getline(iss, header_value);
            boost::algorithm::trim(header_value);
            headers[header_name] = header_value;
        }

        // Set environment variables
        REQUEST_METHOD = request_method;
        REQUEST_URI = request_uri;
        QUERY_STRING = query_string;
        SERVER_PROTOCOL = server_protocol;
        HTTP_HOST = headers["Host"];
        // SERVER_ADDR = "0.0.0.0";
        SERVER_PORT = std::to_string(socket_.local_endpoint().port());
        REMOTE_ADDR = socket_.remote_endpoint().address().to_string();
        REMOTE_PORT = std::to_string(socket_.remote_endpoint().port());

        // Write HTTP response header
        boost::asio::async_write(
            socket_, boost::asio::buffer(std::string("HTTP/1.1 200 OK\r\n")),
            [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                if (ec)
                    return;
                std::string full_uri = REQUEST_URI + "?" + QUERY_STRING;
                setenv("REQUEST_METHOD", REQUEST_METHOD.c_str(), 1);
                setenv("REQUEST_URI", full_uri.c_str(), 1);
                setenv("QUERY_STRING", QUERY_STRING.c_str(), 1);
                setenv("SERVER_PROTOCOL", SERVER_PROTOCOL.c_str(), 1);
                setenv("HTTP_HOST", HTTP_HOST.c_str(), 1);
                setenv("SERVER_ADDR", SERVER_ADDR.c_str(), 1);
                setenv("SERVER_PORT", SERVER_PORT.c_str(), 1);
                setenv("REMOTE_ADDR", REMOTE_ADDR.c_str(), 1);
                setenv("REMOTE_PORT", REMOTE_PORT.c_str(), 1);

                switch (fork()) {
                case -1:
                    perror("exit()");
                    exit(EXIT_FAILURE);
                case 0:

                    if (REQUEST_URI.find_last_of("/")) {
                        std::cerr << "URI [" << REQUEST_URI
                                  << "] is not allowed\n";
                        exit(EXIT_FAILURE);
                    }

                    dup2(socket_.native_handle(), STDOUT_FILENO);
                    dup2(socket_.native_handle(), STDIN_FILENO);
                    close(socket_.native_handle());

                    // Execute CGI script
                    std::string cgi("." + REQUEST_URI);

                    execlp(cgi.c_str(), cgi.c_str(), NULL);
                    std::string errs("execlp(" + cgi + ")");
                    perror(errs.c_str());
                    exit(EXIT_FAILURE);
                }
            });
    }
    tcp::socket socket_;
    std::string SERVER_PORT;
    std::string request;
    std::string REQUEST_METHOD, REQUEST_URI, QUERY_STRING, SERVER_PROTOCOL,
        HTTP_HOST, SERVER_ADDR, REMOTE_ADDR, REMOTE_PORT;
};

class server
{
public:
    server(boost::asio::io_context &io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
    {
        boost::system::error_code error = boost::asio::error::host_not_found;
        boost::asio::ip::tcp::socket socket(io_context);

        try {
            // Fetch the public IP address
            boost::asio::ip::tcp::resolver resolver(io_context);
            boost::asio::ip::tcp::resolver::query query("api.ipify.org",
                                                        "http");
            boost::asio::ip::tcp::resolver::iterator endpoint_iterator =
                resolver.resolve(query);
            boost::asio::ip::tcp::resolver::iterator end;

            while (error && endpoint_iterator != end) {
                socket.close();
                socket.connect(*endpoint_iterator++, error);
            }
        } catch (...) {
        }


        if (!error) {
            boost::asio::streambuf request;
            std::ostream request_stream(&request);
            request_stream << "GET / HTTP/1.1\r\n";
            request_stream << "Host: api.ipify.org\r\n";
            request_stream << "Accept: */*\r\n";
            request_stream << "Connection: close\r\n\r\n";

            boost::asio::write(socket, request);

            std::string response;
            boost::asio::read_until(
                socket, boost::asio::dynamic_buffer(response), "\0");

            std::stringstream response_stream(response);

            std::string http_version;
            response_stream >> http_version;
            unsigned int status_code;
            response_stream >> status_code;
            std::string status_message;
            std::getline(response_stream, status_message);

            if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
                std::cout << "Invalid response\n";
                return;
            }

            if (status_code != 200) {
                std::cout << "Response returned with status code "
                          << status_code << "\n";
                return;
            }

            std::string header;
            while (std::getline(response_stream, header) && header != "\r") {
            }

            std::getline(response_stream, SERVER_ADDR);
        } else {
            std::cerr << "Error: " << error.message() << std::endl;
        }

        do_accept();
    }

private:
    void do_accept()
    {
        acceptor_.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket) {
                if (!ec) {
                    std::make_shared<session>(std::move(socket), SERVER_ADDR)
                        ->start();
                }

                do_accept();
            });
    }

    tcp::acceptor acceptor_;
    std::string SERVER_ADDR;
};

int main(int argc, char *argv[])
{
    try {
        if (argc != 2) {
            std::cerr << "Usage: http_server <port>\n";
            return 1;
        }

        boost::asio::io_context io_context;

        server s(io_context, std::atoi(argv[1]));

        io_context.run();
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}