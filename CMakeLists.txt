cmake_minimum_required(VERSION 3.28)
project(WhoWantsToBeAMillionaire C)

set(CMAKE_C_STANDARD 11)

add_executable(WhoWantsToBeAMillionaire main.c)

target_link_libraries(WhoWantsToBeAMillionaire sqlite3)
