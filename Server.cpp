#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/bind.hpp>
#include <boost/bind/bind.hpp>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>

boost::asio::io_context io_context;

using boost::asio::ip::tcp;

tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8888));

void start_accept();
void read_handler(const boost::system::error_code &ec, std::size_t len,
                  char *buffer, std::shared_ptr<tcp::socket> sock);

void write_handler(const boost::system::error_code &ec, std::size_t len,
                   std::shared_ptr<tcp::socket> sock);

void write_handler(const boost::system::error_code &ec, std::size_t len,
                   std::shared_ptr<tcp::socket> sock) {
    char *buf = new char[1024];
    memset(buf, 0, 1024);
    sock->async_read_some(
        boost::asio::buffer(buf, 1024),
        boost::bind(read_handler, boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred, buf,
                    std::shared_ptr<tcp::socket>(sock)));
}

void read_handler(const boost::system::error_code &ec, std::size_t len,
                  char *buffer, std::shared_ptr<tcp::socket> sock) {
    std::cout << "Read " << len << " bytes from the socket\n\n"
              << "Writing back\n\n";
    sock->async_write_some(
        boost::asio::buffer(buffer, 1024),
        boost::bind(write_handler, boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred,
                    std::shared_ptr<tcp::socket>(sock)));
    delete[] buffer;
}

void accept_handler(const boost::system::error_code &ec,
                    std::shared_ptr<tcp::socket> sock) {
    char *buf = new char[1024];
    sock->async_read_some(
        boost::asio::buffer(buf, 1024),
        boost::bind(read_handler, boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred, buf,
                    std::shared_ptr<tcp::socket>(sock)));
    start_accept();
}

void start_accept() {
    std::shared_ptr<tcp::socket> conn(new tcp::socket(io_context));
    acceptor.async_accept(
        *conn, boost::bind(accept_handler, boost::asio::placeholders::error,
                           std::shared_ptr<tcp::socket>(conn)));
}

int main(int argc, char *argv[]) {
    start_accept();
    io_context.run();
    return 0;
}
