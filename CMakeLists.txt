cmake_minimum_required(VERSION 3.28)
project(deque_final)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(deque_final
        class-bint.hpp
        class-integer.hpp
        class-matrix.hpp
        deque.hpp
        exceptions.hpp
        utility.hpp
        #private_test.cpp
        tests/three/code.cpp
)
