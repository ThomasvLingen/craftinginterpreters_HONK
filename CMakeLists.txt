cmake_minimum_required(VERSION 3.6)
project(honk)

set(CMAKE_CXX_COMPILER g++-8)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")

file(GLOB_RECURSE SOURCE_FILES
		"src/*.cpp"
		"src/*.hpp"
		"third_party/*.h"
		)

add_executable(honk ${SOURCE_FILES})

target_include_directories(honk PUBLIC "src/")
target_include_directories(honk PUBLIC "./")
