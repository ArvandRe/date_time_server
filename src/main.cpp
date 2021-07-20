#include <cstdlib>
#include <iostream>

#include "server.h"

int main() {
    try {
        DateTime::Server server(std::stoi(std::getenv("DATE_TIME_PORT")), std::getenv("TZ_DATABASE_PATH"));
        server.Run();
    } catch (std::exception &e) {
        std::cout << "Error: " << e.what() << std::endl;
    }
}