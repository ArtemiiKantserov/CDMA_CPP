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
  temp_debpsk.hpp
  temp_debpsk.cpp
  temp_bpsk.hpp
  temp_bpsk.cpp
)