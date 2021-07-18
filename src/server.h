#include <boost/asio/co_spawn.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "date_time.h"

using boost::asio::awaitable;
using boost::asio::ip::tcp;

namespace DateTime {

class Connection {
public:
    Connection(tcp::socket &socket, boost::local_time::tz_database &tz_db);
    awaitable<void> Connect();

private:
    tcp::socket &socket_;
    boost::local_time::tz_database &tz_db_;

    awaitable<std::string> Read();
    awaitable<void> Write(const std::string &msg);
};

class Server {
public:
    using Port = boost::asio::ip::port_type;

    Server(Port port, const std::string &tz_database_path);
    void Run();

private:
    boost::asio::io_context context_;
    Port port_;
    boost::local_time::tz_database tz_db_;

    awaitable<void> Listen();
    awaitable<void> MakeConnection(tcp::socket socket);
};

}