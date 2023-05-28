#include <algorithm>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/config.hpp>
#include <boost/thread.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

class HttpSession : public std::enable_shared_from_this<HttpSession> {
    beast::tcp_stream stream_;
    beast::flat_buffer buffer_;
    http::request<http::string_body> req_;
    http::response<http::string_body> res_;

  public:
    explicit HttpSession(tcp::socket &&socket) : stream_(std::move(socket)) {}

    void run() { readRequest(); }

  private:
    void readRequest() {
        auto self = shared_from_this();

        http::async_read(
            stream_, buffer_, req_,
            [self](beast::error_code ec, std::size_t bytes_transferred) {
                boost::ignore_unused(bytes_transferred);
                if (!ec)
                    self->handleRequest();
            });
    }

    void handleRequest() {
        res_.version(req_.version());
        res_.keep_alive(false);

        // Check if this is a GET request to /get/users
        if (req_.method() == http::verb::get && req_.target() == "/get/users") {
            res_.result(http::status::ok);
            res_.set(http::field::server, "Boost Beast");
            res_.set(http::field::content_type, "application/json");

            std::cout << "OK Calll" << std::endl;

            boost::this_thread::sleep(boost::posix_time::seconds(10));
            // Sample JSON response body
            res_.body() =
                R"({"users":[{"id":1,"name":"John Doe"},{"id":2,"name":"Jane Doe"}]})";
        } else {
            // For other requests, return 404 not found
            res_.result(http::status::not_found);
            res_.set(http::field::content_type, "text/plain");
            res_.body() = "404 Not Found\n";
        }

        res_.prepare_payload();
        writeResponse();
    }

    void writeResponse() {
        auto self = shared_from_this();

        http::async_write(
            stream_, res_, [self](beast::error_code ec, std::size_t) {
                self->stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
            });
    }
};

class HttpServer {
    net::io_context ioc_;
    tcp::acceptor acceptor_;

  public:
    explicit HttpServer(int port)
        : acceptor_(ioc_, {tcp::v4(), static_cast<unsigned short>(port)}) {
        accept();
    }

    void run() { ioc_.run(); }

  private:
    void accept() {
        acceptor_.async_accept(
            [this](beast::error_code ec, tcp::socket socket) {
                if (!ec)
                    std::make_shared<HttpSession>(std::move(socket))->run();

                accept();
            });
    }
};

int main() {
    const int port = 8080;

    std::make_shared<HttpServer>(port)->run();

    return 0;
}