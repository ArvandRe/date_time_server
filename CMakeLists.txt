cmake_minimum_required(VERSION 3.13)
project(TimeServer)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(time_server ./src/main.cpp ./src/date_time.cpp ./src/server.cpp)
target_link_libraries(time_server ${CONAN_LIBS})
