cmake_minimum_required(VERSION 3.13)
project(OSMP C)

set(CMAKE_C_STANDARD 99)

add_executable(OSMP OSMPStarter.c OSMPLib.c OSMPLib.h OSMPStarter.h)