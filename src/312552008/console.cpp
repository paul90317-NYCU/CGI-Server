#include <unistd.h>  // for write
#include <cstdlib>
#include <iostream>

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

    return 0;
}