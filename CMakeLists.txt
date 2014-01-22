CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(Zombie)
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")
if (MSVC)
	# Exception handler model.
	SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /EHsc -D_SCL_SECURE_NO_WARNINGS")
	SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /EHsc")
	
	SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIBS:MSVCRT")

	ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS -DWIN32)	
else (MSVC)
	SET(CMAKE_CXX_FLAGS "-std=c++11")
endif (MSVC)

# Source files.
SET(SOURCES_ZOMBIE
	src/animation.h
	src/auxiliary.cpp
	src/auxiliary.h
	src/blood.cpp
	src/blood.h
	src/box2ddef.h
	src/building.h
	src/building2d.h
	src/buildingproperties.h
	src/bullet.h
	src/car.h
	src/car2d.h
	src/carproperties.h
	src/device.h
	src/gamedata.cpp
	src/gamedata.h
	src/gamefont.cpp
	src/gamefont.h
	src/gameinterface.h
	src/gamesound.cpp
	src/gamesound.h
	src/gamesprite.cpp
	src/gamesprite.h
	src/human2d.h
	src/humanplayer.h
	src/input.h
	src/inputkeyboard.h
	src/load.cpp
	src/load.h
	src/main.cpp
	src/movingobject.h
	src/object.h
	src/player.cpp
	src/player.h
	src/settings.h
	src/state.h
	src/survivorbehavior.cpp
	src/survivorbehavior.h
	src/unit.cpp
	src/unit.h
	src/unitproperties.h
	src/weapon.h
	src/weaponitem.h
	src/weaponitem2d.h
	src/weaponproperties.h
	src/zombie2d.h
	src/zombiebehavior.cpp
	src/zombiebehavior.h
	src/zombieengine.cpp
	src/zombieengine.h
	src/zombiegame.cpp
	src/zombiegame.h
	src/zombiewindow.h
)
# End of source files.

FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(SDL2 REQUIRED)
FIND_PACKAGE(SDL2_image REQUIRED)
FIND_PACKAGE(SDL2_mixer REQUIRED)
FIND_PACKAGE(SDL2_ttf REQUIRED)

ADD_SUBDIRECTORY(SdlGui)
ADD_SUBDIRECTORY(MwExtra)
ADD_SUBDIRECTORY(Box2D)
ADD_SUBDIRECTORY(TinyXML2)

FILE(COPY data/. DESTINATION ${CMAKE_CURRENT_BINARY_DIR}) # Copy data to build folder.

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS}
	${SDL2_TTF_INCLUDE_DIRS}
	${SDL2_MIXER_INCLUDE_DIRS}
	${SDL2_IMAGE_INCLUDE_DIRS}
	${OpenGL_INCLUDE_DIR}
	MwExtra/src
	SdlGui/src
	SdlGui/MwLibrary2/src
	SdlGui/Signal/src
	"."
	TinyXML2/src
)

ADD_EXECUTABLE(Zombie ${SOURCES_ZOMBIE})

IF (WIN32)
	SET_TARGET_PROPERTIES (Zombie PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:windows")
ENDIF (WIN32)

TARGET_LINK_LIBRARIES(Zombie Box2D MwExtra MwLibrary2 SdlGui Signal TinyXML2 ${SDL2_LIBRARIES} ${SDL2_TTF_LIBRARIES} ${SDL2_MIXER_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${OPENGL_LIBRARIES})

# Test project. Uses Boost Test. Unit Test.
FIND_PACKAGE(Boost 1.54)
IF (Boost_FOUND)
	# Source files.
	SET(SOURCES_TEST
		src/test.cpp
	)

	INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
	LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
	ADD_EXECUTABLE(Test ${SOURCES_TEST})
	# ENABLE_TESTING()
	# ADD_TEST(Test Test)
	IF (MSVC)
		# Runs the code in build output.
		ADD_CUSTOM_COMMAND(TARGET Test
			POST_BUILD
			COMMAND "\$(TargetDir)/\$(TargetName).exe" --result_code=no --report_level=no
			COMMENT "Auto run Test after build"
		)
	ENDIF (MSVC)
ENDIF (Boost_FOUND)
