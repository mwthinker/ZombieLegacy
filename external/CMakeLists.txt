cmake_minimum_required(VERSION 3.1...3.12)

# SdlGui ---------------------------
add_subdirectory(SdlGui)

# Box2D ----------------------------
file(GLOB_RECURSE SOURCES
    "Box2D/Box2D/**.h"
    "Box2D/Box2D/**.cpp"
)

add_library(Box2D STATIC
	${SOURCES}
)

target_include_directories(Box2D
	PUBLIC
		"Box2D"
)

# json -----------------------------
add_library(json INTERFACE)

target_include_directories(json
	INTERFACE
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/json/single_include>"
)
