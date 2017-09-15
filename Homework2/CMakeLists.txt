cmake_minimum_required(VERSION 3.0)
project(assign2)

if (CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "-std=c++11 -o3 ${CMAKE_CXX_FLAGS}")
endif()

add_executable(assign2
	main.cpp
	compute-pi.hpp
	compute-pi.cpp
	compute-fib.hpp
	compute-fib.cpp
	compute-e.hpp
	compute-e.cpp
)
