cmake_minimum_required(VERSION 3.13)
project(sm1pro_ce)

set(CMAKE_CXX_STANDARD 14)

add_executable(sm1pro_ce main.cpp)
target_link_libraries(sm1pro_ce jsoncpp sqlite3)