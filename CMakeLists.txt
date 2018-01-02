cmake_minimum_required(VERSION 3.6)
project(honk)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")

file(GLOB_RECURSE SOURCE_FILES
		"src/*.cpp"
		"src/*.hpp"
		)

file(GLOB_RECURSE TEST_SOURCE_FILES
		"src/*.cpp"
		"src/*.hpp"
		"test/*.cpp"
		"test/*.hpp"
		)
#list(REMOVE_ITEM TEST_SOURCE_FILES
#		"${CMAKE_SOURCE_DIR}/src/main.cpp"
#		)

add_executable(honk ${SOURCE_FILES})
add_executable(unittests ${TEST_SOURCE_FILES})

target_include_directories(unittests PUBLIC "src/")
target_include_directories(honk PUBLIC "src/")
