cmake_minimum_required(VERSION 3.5.0)
project (hello_cmake C CXX)
file (GLOB_RECURSE SOURCES *.c*)
add_executable (hello_cmake ${SOURCES})