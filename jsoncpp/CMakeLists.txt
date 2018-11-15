cmake_minimum_required(VERSION 3.1...3.12)
project(jsoncpp)

set(SOURCES
	src/json/json.h
	src/json/json-forwards.h
	src/jsoncpp.cpp
)

add_library(jsoncpp ${SOURCES})

target_include_directories(SdlGui
	PUBLIC
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
)
