cmake_minimum_required(VERSION 3.0)
project(jsoncpp)
if (NOT MSVC)
	set(CMAKE_CXX_FLAGS "-std=c++1y")
	option(Debug "Set projects to debug mode.")
	if (Debug)
		set(CMAKE_BUILD_TYPE "Debug" CACHE INTERNAL "Build type.")
		add_definitions(-D_DEBUG)
	else ()
		set(CMAKE_BUILD_TYPE "Release" CACHE INTERNAL "Build type.")
	endif ()
endif ()

# Source files.
set(SOURCES
	src/json/json.h
	src/json/json-forwards.h
	src/jsoncpp.cpp
)
# End of source files.

add_library(jsoncpp ${SOURCES})
