#include <boost/algorithm/string/trim.hpp>
#include "server.h"

namespace DateTime {

Server::Server(Port port, const std::string &tz_database_path) : context_(1), port_(port) {
    tz_db.load_from_file(tz_database_path);
}

void Server::Run() {
    boost::asio::signal_set signals(context_, SIGINT, SIGTERM);
    signals.async_wait([&](auto, auto) { context_.stop(); });
    co_spawn(context_, Listen(), boost::asio::detached);
    context_.run();
}

awaitable<void> Server::Listen() {
    auto executor = co_await boost::asio::this_coro::executor;
    tcp::acceptor acceptor(executor, {tcp::v4(), port_});
    while (true) {
        co_spawn(executor, Connect(co_await acceptor.async_accept(use_awaitable)), boost::asio::detached);
    }
}

awaitable<void> Server::Connect(tcp::socket socket) {
    try {
        while (true) {
            const auto tz_name = co_await Read(socket);    
            const auto datetime = GetDateTimeWithTz(tz_db, tz_name);
            if (datetime) {
                co_await Write(socket, datetime.value() + '\n');
                break;
            }
            const auto error_msg = "Bad request: " + tz_name + "\n";
            co_await Write(socket, error_msg);
        }
    }
    catch (std::exception &e) {
        std::cout << "Connect error: " << e.what() << std::endl;
    }
}

awaitable<std::string> Server::Read(tcp::socket &socket) {
    char data[64];
    std::size_t n = co_await socket.async_read_some(boost::asio::buffer(data), use_awaitable);
    std::string result(data, n);
    boost::trim(result);
    co_return result;
}

awaitable<void> Server::Write(tcp::socket &socket, const std::string &msg) {
    co_await async_write(socket, boost::asio::const_buffer(msg.c_str(), msg.size()), use_awaitable);
}

}
