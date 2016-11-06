cmake_minimum_required(VERSION 2.8.4)
project(abaqs)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    abaqs.ino
    converter.js)

add_executable(abaqs ${SOURCE_FILES})