cmake_minimum_required(VERSION 3.28)
project(CDMA_simulation)

set(CMAKE_CXX_STANDARD 20)

add_executable(
  main
  main.cpp
  encode.cpp
  encode.hpp
  hadamard.cpp
  hadamard.hpp
  bpsk.hpp
  bpsk.cpp
)