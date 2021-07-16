#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/write.hpp>
#include "date_time.h"

using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::use_awaitable;
using boost::asio::ip::tcp;


namespace DateTime {

class Server {
public:
    using Port = boost::asio::ip::port_type;

    Server(Port port, const std::string &tz_database_path);

    void Run();
    awaitable<void> Listen();

private:
    boost::asio::io_context context_;
    Port port_;
    boost::local_time::tz_database tz_db;

    awaitable<void> Connect(tcp::socket socket);
    awaitable<std::string> Read(tcp::socket &socket);
    awaitable<void> Write(tcp::socket &socket, const std::string &msg);
};

}