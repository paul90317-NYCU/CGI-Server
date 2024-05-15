#include <unistd.h>  // for write
#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <unordered_map>

using boost::asio::ip::tcp;

extern boost::asio::io_context io_context;

void output_connection(tcp::socket &socket_, std::string is, std::string hi, std::string pi)
{
    boost::asio::streambuf buf;
    std::ostream bout(&buf);
    bout << "<script>document.getElementById('connection').innerHTML += "
                 "'<th scope=\"col\">" +
                     hi + ":" + pi + "</th>';</script>";
    bout << "<script>document.getElementById('output').innerHTML += "
                 "'<td><pre id=\"s" +
                     is + "\" class=\"mb-0\"></pre></td>';</script>";
    boost::asio::write(socket_, buf);
}

void replace_all(std::string &line)
{
    boost::replace_all(line, "&", "&amp;");
    boost::replace_all(line, "\"", "&quot;");
    boost::replace_all(line, "\'", "&apos;");
    boost::replace_all(line, "<", "&lt;");
    boost::replace_all(line, ">", "&gt;");
    boost::replace_all(line, "\r", "");
    boost::replace_all(line, "\n", "&NewLine;");
}

const char *body = R"MAIN(
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="UTF-8" />
    <title>NP Project 3 Sample Console</title>
    <link
      rel="stylesheet"
      href="https://cdn.jsdelivr.net/npm/bootstrap@4.5.3/dist/css/bootstrap.min.css"
      integrity="sha384-TX8t27EcRE3e/ihU7zmQxVncDAy5uIKz4rEkgIXeMed4M0jlfIDPvg6uqKI2xXr2"
      crossorigin="anonymous"
    />
    <link
      href="https://fonts.googleapis.com/css?family=Source+Code+Pro"
      rel="stylesheet"
    />
    <link
      rel="icon"
      type="image/png"
      href="https://cdn0.iconfinder.com/data/icons/small-n-flat/24/678068-terminal-512.png"
    />
    <style>
      * {
        font-family: 'Source Code Pro', monospace;
        font-size: 1rem !important;
      }
      body {
        background-color: #212529;
      }
      pre {
        color: #cccccc;
      }
      b {
        color: #01b468;
      }
    </style>
  </head>
  <body>
    <table class="table table-dark table-bordered">
      <thead>
        <tr id="connection">
        </tr>
      </thead>
      <tbody>
        <tr id="output">
        </tr>
      </tbody>
    </table>
  </body>
</html>
)MAIN";

class golden_session : public std::enable_shared_from_this<golden_session>
{
public:
    golden_session(std::shared_ptr<tcp::socket> http, tcp::socket golden, std::fstream file, std::string is)
        : http_(http), golden_(std::move(golden)), file_(std::move(file)), is_(is)
    {
    }

    void start() { read_response(); }

private:
    void output_shell(std::string is, std::string shell)
    {
        replace_all(shell);
        boost::asio::streambuf buf;
        std::ostream bout(&buf);
        bout << "<script>document.getElementById('s" + is +
                        "').innerHTML += '" + shell + "';</script>";
        boost::asio::write(*http_.get(), buf);
    }

    void output_command(std::string is, std::string command)
    {
        replace_all(command);
        boost::asio::streambuf buf;
        std::ostream bout(&buf);
        bout << "<script>document.getElementById('s" + is +
                        "').innerHTML += '<b>" + command +
                        "</b>&NewLine;';</script>";
        boost::asio::write(*http_.get(), buf);
    }

    void write_command()
    {
        if (!std::getline(file_, line_))
            return;
        auto self(shared_from_this());
        output_command(is_, line_);
        boost::asio::async_write(
            golden_, boost::asio::buffer(line_ + "\n"),
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
            golden_, boost::asio::dynamic_buffer(response_), '%',
            [this, self](const boost::system::error_code &error,
                         size_t bytes_transferred) {
                if (!error) {
                    output_shell(is_, response_);
                    write_command();  // Continue reading until EOF
                } else if (error != boost::asio::error::eof) {
                    std::cerr << "Read error: " << error.message() << std::endl;
                }
            });
    }
    std::shared_ptr<tcp::socket> http_;
    tcp::socket golden_;
    std::fstream file_;
    std::string is_;
    std::string line_;
    std::string response_;
};

void execute(std::shared_ptr<tcp::socket> http,
             std::string host,
             std::string port,
             std::string filename,
             std::string is)
{
    // Open file for reading
    std::fstream file("./test_case/" + filename,
                      std::ios_base::openmode::_S_in);
    if (!file.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Create socket
    tcp::socket golden(io_context);

    // Resolve endpoint
    tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve(host, port);

    // Connect to server
    boost::asio::connect(golden, endpoints);
    std::make_shared<golden_session>(std::move(http), std::move(golden), std::move(file), is)->start();
}

void console_run(std::shared_ptr<tcp::socket> http, std::string QUERY_STRING)
{
    // Print HTTP headers
    boost::asio::streambuf buf;
    std::ostream bout(&buf);
    bout << "Content-Type: text/html\r\n\r\n";
    bout << body;
    boost::asio::write(*http.get(), buf);

    // parse query
    char *v, *qstr = &QUERY_STRING[0];
    std::unordered_map<std::string, std::string> querys;
    while ((v = strtok_r(qstr, "&", &qstr)) != 0) {
        char *k = strtok_r(v, "=", &v);
        querys[k] = v;
    }

    for (int i = 0; i < 5; ++i) {
        std::string is = std::to_string(i);
        std::string hi = querys["h" + is], pi = querys["p" + is],
                    fi = querys["f" + is];
        if (hi.size() && pi.size() && fi.size()) {
            output_connection(*http.get(), is, hi, pi);
            execute(http, hi, pi, fi, is);
        }
    }
}