#include <cstdlib>
#include <boost/algorithm/string/trim.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/write.hpp>
#include <iostream>
#include <string>
#include "date_time.h"

using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::use_awaitable;
using boost::asio::ip::tcp;


class DateTimeServer {
public:
    DateTimeServer() {
	    tz_db.load_from_file(std::getenv("TZ_DATABASE_PATH"));
    }

    awaitable<void> Listen() {
        auto executor = co_await boost::asio::this_coro::executor;
        tcp::acceptor acceptor(executor, {tcp::v4(), std::stoi(std::getenv("DATE_TIME_PORT"))});
        while (true) {
            co_spawn(executor, Connect(co_await acceptor.async_accept(use_awaitable)), boost::asio::detached);
        }
    }

private:
    boost::local_time::tz_database tz_db;

    awaitable<void> Connect(tcp::socket socket) {
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

    awaitable<std::string> Read(tcp::socket &socket) {
        char data[64];
        std::size_t n = co_await socket.async_read_some(boost::asio::buffer(data), use_awaitable);
        std::string result(data, n);
        boost::trim(result);
        co_return result;
    }

    awaitable<void> Write(tcp::socket &socket, const std::string &msg) {
        co_await async_write(socket, boost::asio::const_buffer(msg.c_str(), msg.size()), use_awaitable);
    }

};


int main()
{
    try
    {
        boost::asio::io_context io_context(1);
        boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
        signals.async_wait([&](auto, auto) { io_context.stop(); });

        DateTimeServer server;
        co_spawn(io_context, server.Listen(), boost::asio::detached);
        io_context.run();
    }
    catch (std::exception &e)
    {
        std::cout << "Error: "  << e.what() << std::endl;
    }
}