#include "server.h"

#include <boost/algorithm/string/trim.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/write.hpp>
namespace DateTime {

Connection::Connection(tcp::socket &socket, boost::local_time::tz_database &tz_db) : socket_(socket), tz_db_(tz_db) {}

awaitable<void> Connection::Connect() {
    try {
        while (true) {
            const auto tz_name = co_await Read();
            const auto datetime = GetDateTimeWithTz(tz_db_, tz_name);
            if (datetime) {
                co_await Write(datetime.value() + '\n');
                break;
            }
            const auto error_msg = "Bad request: " + tz_name + "\n";
            co_await Write(error_msg);
        }
    } catch (std::exception &e) {
        std::cout << "Connect error: " << e.what() << std::endl;
    }
}

awaitable<std::string> Connection::Read() {
    char data[64];
    std::size_t n = co_await socket_.async_read_some(boost::asio::buffer(data), boost::asio::use_awaitable);
    std::string result(data, n);
    boost::trim(result);
    co_return result;
}

awaitable<void> Connection::Write(std::string msg) {
    co_await async_write(socket_, boost::asio::const_buffer(msg.c_str(), msg.size()), boost::asio::use_awaitable);
}

Server::Server(Port port, const std::string &tz_database_path) : context_(1), port_(port) { tz_db_.load_from_file(tz_database_path); }

void Server::Run() {
    boost::asio::signal_set signals(context_, SIGINT, SIGTERM);
    signals.async_wait([&](auto, auto) { context_.stop(); });
    boost::asio::co_spawn(context_, Listen(), boost::asio::detached);
    context_.run();
}

awaitable<void> Server::Listen() {
    auto executor = co_await boost::asio::this_coro::executor;
    tcp::acceptor acceptor(executor, {tcp::v4(), port_});
    while (true) {
        boost::asio::co_spawn(executor, MakeConnection(co_await acceptor.async_accept(boost::asio::use_awaitable)), boost::asio::detached);
    }
}

awaitable<void> Server::MakeConnection(tcp::socket socket) {
    Connection connection(socket, tz_db_);
    co_await connection.Connect();
}

}  // namespace DateTime
