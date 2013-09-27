CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(Zombie)
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")
if (WIN32)
	# Exception handler model.
	SET (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /EHsc -D_SCL_SECURE_NO_WARNINGS")
	SET (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_DEBUG} /EHsc")
	
	ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)
	ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
	ADD_DEFINITIONS(-DWIN32)
	
	SET( CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /nodefaultlib:libcmt.lib /nodefaultlib:msvcrt")
	SET( CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /nodefaultlib:libcmt.lib /nodefaultlib:msvcrt.lib")
else (WIN32)
	SET(CMAKE_CXX_FLAGS "-std=c++11")
endif (WIN32)

# Source files.
SET(SOURCES_ZOMBIE
src/aibehavior.h
src/aiplayer.cpp
src/aiplayer.h
src/auxiliary.cpp
src/auxiliary.h
src/blood.cpp
src/blood.h
src/building.h
src/buildingdraw.h
src/bullet.h
src/car.h
src/caranimation.cpp
src/caranimation.h
src/device.h
src/drawweaponobject.cpp
src/drawweaponobject.h
src/gamefont.cpp
src/gamefont.h
src/gamesound.cpp
src/gamesound.h
src/gamesprite.cpp
src/gamesprite.h
src/graphictask.h
src/humananimation.cpp
src/humananimation.h
src/humanplayer.h
src/humanstatus.cpp
src/humanstatus.h
src/input.h
src/inputkeyboard.h
src/linefeature.h
src/main.cpp
src/map.cpp
src/map.h
src/mapdraw.cpp
src/mapdraw.h
src/movingobject.h
src/object.cpp
src/object.h
src/order.cpp
src/order.h
src/pathfinder.h
src/player.h
src/roaddraw.cpp
src/roaddraw.h
src/shot.cpp
src/shot.h
src/sky.h
src/state.h
src/survivaltimer.h
src/survivorbehavior.cpp
src/survivorbehavior.h
src/task.cpp
src/task.h
src/taskmanager.cpp
src/taskmanager.h
src/tile.h
src/typedefs.h
src/unit.cpp
src/unit.h
src/weapon.h
src/weaponobject.h
src/zombieanimation.cpp
src/zombieanimation.h
src/zombiebehavior.cpp
src/zombiebehavior.h
src/zombieengine.cpp
src/zombieengine.h
src/zombiegame.cpp
src/zombiegame.h
src/zombiewindow.h)
# End of source files.

FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(SDL2 REQUIRED)
FIND_PACKAGE(SDL2_image REQUIRED)
FIND_PACKAGE(SDL2_mixer REQUIRED)
FIND_PACKAGE(SDL2_ttf REQUIRED)

ADD_SUBDIRECTORY(MwLibrary2)
ADD_SUBDIRECTORY(MwExtra)
ADD_SUBDIRECTORY(Box2D)

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS} ${SDL2_MIXER_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} ${OpenGL_INCLUDE_DIR} "MwExtra/src" "MwLibrary2/src" ".")
ADD_EXECUTABLE(Zombie ${SOURCES_ZOMBIE})
TARGET_LINK_LIBRARIES(Zombie Box2D MwExtra MwLibrary2 ${SDL2_LIBRARIES} ${SDL2_TTF_LIBRARIES} ${SDL2_MIXER_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${OPENGL_LIBRARIES})
