cmake_minimum_required(VERSION 2.8)
project(Zombie)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")
if (MSVC)
	# Exception handler model.
	SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /EHsc")
	SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /EHsc")	

	ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS -DWIN32)	
else (MSVC)
	SET(CMAKE_CXX_FLAGS "-std=c++11")
endif (MSVC)

# Source files.
set(SOURCES_ZOMBIE
	src/animation.h
	src/auxiliary.cpp
	src/auxiliary.h
	src/box2ddef.h
	src/building.h
	src/building2d.h	
	src/car.cpp
	src/car.h
	src/car2d.h
	src/closestraycastcallback.h
	src/contactlistener.h
	src/contactlistener.cpp
	src/datainterface.h
	src/device.h
	src/driver.h
	src/explosion.h
	src/gamedata.cpp
	src/gamedata.h
	src/gameinterface.h
	src/graphic.h
	src/graphicanimation.h
	src/gun.cpp
	src/gun.h
	src/humanplayer.h
	src/input.h
	src/inputkeyboard.h
	src/inviewquerycallback.h
	src/main.cpp
	src/missilelauncher.h
	src/missilelauncher.cpp
	src/missilelauncher2d.h
	src/missile.cpp
	src/missile.h
	src/missile2d.h
	src/movingobject.h
	src/object.cpp
	src/object.h
	src/player.cpp
	src/player.h
	src/state.h
	src/terrain2d.cpp
	src/terrain2d.h
	src/tree.h
	src/tree2d.h
	src/shot.h
	src/survivorbehavior.cpp
	src/survivorbehavior.h
	src/unit.cpp
	src/unit.h
	src/unit2d.h
	src/weapon.h
	src/weapon2d.h
	src/weaponinterface.h
	src/weaponitem.h
	src/weaponitem2d.h
	src/zombiebehavior.cpp
	src/zombiebehavior.h
	src/zombieengine.cpp
	src/zombieengine.h
	src/zombiegame.cpp
	src/zombiegame.h
	src/zombiewindow.h
	src/zombiewindow.cpp
)
# Source files.
set(SOURCES_EFFECT
	src/emitter.h
	src/explosion.h
	src/fog.h
	src/fog.cpp
	src/particleengine.h
	src/shockwave.h
)
# End of source files.

find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)

add_subdirectory(SdlGui)
add_subdirectory(Box2D)
add_subdirectory(TinyXML2)

file(COPY data/. DESTINATION ${CMAKE_CURRENT_BINARY_DIR}) # Copy data to build folder.

include_directories(${SDL2_INCLUDE_DIRS}
	${SDL2_TTF_INCLUDE_DIRS}
	${SDL2_MIXER_INCLUDE_DIRS}
	${SDL2_IMAGE_INCLUDE_DIRS}
	${OpenGL_INCLUDE_DIR}
	SdlGui/src
	SdlGui/MwLibrary2/src
	SdlGui/Signal/src
	Spark/src
	"."
	TinyXML2/src
)

add_executable(Zombie ${SOURCES_ZOMBIE} ${SOURCES_EFFECT})

if (MSVC)
	set_target_properties(Zombie PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:windows")
	set_target_properties(Zombie PROPERTIES LINK_FLAGS_DEBUG "/NODEFAULTLIB:msvcrt")
endif (MSVC)

target_link_libraries(Zombie
	Box2D
	SdlGui
	MwLibrary2
	Signal
	TinyXML2
	${SDL2_LIBRARIES}
	${SDL2_TTF_LIBRARIES}
	${SDL2_MIXER_LIBRARIES}
	${SDL2_IMAGE_LIBRARIES}
	${OPENGL_LIBRARIES}
)
