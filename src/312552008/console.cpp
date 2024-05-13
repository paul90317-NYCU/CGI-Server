#include <unistd.h>  // for write
#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <unordered_map>

using boost::asio::ip::tcp;

class session : public std::enable_shared_from_this<session>
{
public:
    session(tcp::socket socket, std::fstream file)
        : socket_(std::move(socket)), file_(std::move(file))
    {
    }

    void start() { read_response(); }

private:
    void write_command()
    {
        if (!std::getline(file_, line_))
            return;
        auto self(shared_from_this());
        boost::asio::async_write(
            socket_, boost::asio::buffer(line_ + "\n"),
            [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                if (!ec)
                    read_response();
            });
    }
    void read_response()
    {
        auto self(shared_from_this());
        response_ = "";
        boost::asio::async_read_until(
            socket_, boost::asio::dynamic_buffer(response_), '%',
            [this, self](const boost::system::error_code &error,
                         size_t bytes_transferred) {
                if (!error) {
                    std::cout << response_;
                    write_command();  // Continue reading until EOF
                } else if (error != boost::asio::error::eof) {
                    std::cerr << "Read error: " << error.message() << std::endl;
                }
            });
    }

    tcp::socket socket_;
    std::fstream file_;
    std::string line_;
    std::string response_;
};

void execute(boost::asio::io_context &io_context,
             std::string host,
             std::string port,
             std::string filename)
{
    // Open file for reading
    std::fstream file(filename, std::ios_base::openmode::_S_in);
    if (!file.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Create socket
    tcp::socket socket(io_context);

    // Resolve endpoint
    tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve(host, port);

    // Connect to server
    boost::asio::connect(socket, endpoints);
    std::make_shared<session>(std::move(socket), std::move(file))->start();
}

int main()
{
    // Print HTTP headers
    std::cout << "Content-Type: text/plain\r\n\r\n";

    // Print specific environment variables
    std::cout << "REQUEST_METHOD=" << getenv("REQUEST_METHOD") << "\r\n";
    std::cout << "REQUEST_URI=" << getenv("REQUEST_URI") << "\r\n";
    std::cout << "QUERY_STRING=" << getenv("QUERY_STRING") << "\r\n";
    std::cout << "SERVER_PROTOCOL=" << getenv("SERVER_PROTOCOL") << "\r\n";
    std::cout << "HTTP_HOST=" << getenv("HTTP_HOST") << "\r\n";
    std::cout << "SERVER_ADDR=140.113.235.235"
              << "\r\n";  // Assuming this is your server's IP address
    std::cout << "SERVER_PORT=" << getenv("SERVER_PORT") << "\r\n";
    std::cout << "REMOTE_ADDR=" << getenv("REMOTE_ADDR") << "\r\n";
    std::cout << "REMOTE_PORT=" << getenv("REMOTE_PORT") << "\r\n";

    // parse query
    char *v, *qstr = strdup(getenv("QUERY_STRING")), *qstart = qstr;
    std::unordered_map<std::string, std::string> querys;
    while ((v = strtok_r(qstr, "&", &qstr)) != 0) {
        char *k = strtok_r(v, "=", &v);
        querys[k] = v;
    }
    free(qstart);

    boost::asio::io_context io_context;
    for (int i = 0; i < 5; ++i) {
        std::string is = std::to_string(i);
        std::string hi = querys["h" + is], pi = querys["p" + is],
                    fi = querys["f" + is];
        if (hi.size() && pi.size() && fi.size()) {
            execute(io_context, hi, pi, fi);
        }
    }

    // execute(io_context, "127.0.0.1", "25569", "t1.txt");

    io_context.run();

    return 0;
}