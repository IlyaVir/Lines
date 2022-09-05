cmake_minimum_required(VERSION 3.14)
project(Lines)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES main.cpp)

find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)

add_executable(Lines ${SOURCE_FILES})

target_link_libraries(Lines sfml-graphics sfml-audio)