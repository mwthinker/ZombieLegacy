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

# Only use OpenGL ES 2.
set(OpenGLES2_REMOVE_OPTION 1)
add_definitions(-DMW_OPENGLES2)

# Source files.
set(SOURCES_ZOMBIE
	src/actionhandler.h
	src/animation.cpp
	src/animation.h
	src/array.h
	src/auxiliary.cpp
	src/auxiliary.h
	src/box2ddef.h
	src/building.h
	src/buildingshader.h
	src/buildingshader.cpp
	src/building2d.cpp
	src/building2d.h
	src/car.cpp
	src/car.h
	src/car2d.cpp
	src/car2d.h
	src/closestraycastcallback.h
	src/contactlistener.cpp
	src/contactlistener.h	
	src/device.h
	src/drawbuildings.cpp
	src/drawbuildings.h
	src/gamedataentry.cpp
	src/gamedataentry.h
	src/gameinterface.h
	src/gameshader.cpp
	src/gameshader.h
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
	src/missilelauncher2d.cpp
	src/missile.cpp
	src/missile.h
	src/missile2d.cpp
	src/missile2d.h
	src/movingobject.h
	src/object.cpp
	src/object.h
	src/player.h
	src/shot.h
	src/state.h
	src/survivorbehavior.cpp
	src/survivorbehavior.h
	src/terrain2d.cpp
	src/terrain2d.h
	src/tree.h
	src/tree2d.h
	src/unit.cpp
	src/unit.h
	src/unit2d.cpp
	src/unit2d.h
	src/watershader.cpp
	src/watershader.h
	src/weapon.h
	src/weapon2d.cpp
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
	src/explosion.cpp
	src/explosion.h
	src/fog.cpp
	src/fog.h
	src/particle.h
	src/particleengine.cpp
	src/particleengine.h
	src/shockwave.h
	src/splash.h
	src/water.cpp
	src/water.h
)
# End of source files.

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)

add_subdirectory(DataEntry)
add_subdirectory(SdlGui)
add_subdirectory(Box2D)

file(COPY data/. DESTINATION ${CMAKE_CURRENT_BINARY_DIR}) # Copy data to build folder.

include_directories(${SDL2_INCLUDE_DIRS}
	${SDL2_TTF_INCLUDE_DIRS}
	${SDL2_MIXER_INCLUDE_DIRS}
	${SDL2_IMAGE_INCLUDE_DIRS}
	SdlGui/src
	SdlGui/MwLibrary2/src
	SdlGui/Signal/src
	DataEntry/src
	DataEntry/TinyXML2/src
	"."
)

if (WIN32)
	set(WIN_RESOURCE_FILE zombie.rc)
	add_executable(Zombie ${SOURCES_ZOMBIE} ${SOURCES_EFFECT} ${WIN_RESOURCE_FILE})
else (WIN32)
	add_executable(Zombie ${SOURCES_ZOMBIE} ${SOURCES_EFFECT})
endif (WIN32)

if (MSVC)
	set_target_properties(Zombie PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:windows")
	set_target_properties(Zombie PROPERTIES LINK_FLAGS_DEBUG "/NODEFAULTLIB:msvcrt")
endif (MSVC)

target_link_libraries(Zombie
	Box2D
	SdlGui
	MwLibrary2
	Signal
	DataEntry
	TinyXML2
	${SDL2_LIBRARIES}
	${SDL2_TTF_LIBRARIES}
	${SDL2_MIXER_LIBRARIES}
	${SDL2_IMAGE_LIBRARIES}
)

############### CPack

if (WIN32)
	set(CPACK_NSIS_MODIFY_PATH "ON")

	install(TARGETS Zombie
        DESTINATION .
        COMPONENT application
	)

	set(DLL_FILES
		"$ENV{DLL_FOLDER}\\d3dcompiler_43.dll"
		"$ENV{DLL_FOLDER}\\d3dcompiler_46.dll"
		"$ENV{DLL_FOLDER}\\libEGL.dll"
		"$ENV{DLL_FOLDER}\\libGLESv2.dll"
		"$ENV{DLL_FOLDER}\\libFLAC-8.dll"
		"$ENV{DLL_FOLDER}\\libfreetype-6.dll"
		"$ENV{DLL_FOLDER}\\libjpeg-9.dll"
		"$ENV{DLL_FOLDER}\\libmikmod-2.dll"
		"$ENV{DLL_FOLDER}\\libmodplug-1.dll"
		"$ENV{DLL_FOLDER}\\libogg-0.dll"
		"$ENV{DLL_FOLDER}\\libpng16-16.dll"
		"$ENV{DLL_FOLDER}\\libtiff-5.dll"
		"$ENV{DLL_FOLDER}\\libwebp-4.dll"
		"$ENV{DLL_FOLDER}\\libvorbis-0.dll"
		"$ENV{DLL_FOLDER}\\libvorbisfile-3.dll"
		"$ENV{DLL_FOLDER}\\SDL2.dll"
		"$ENV{DLL_FOLDER}\\SDL2_image.dll"
		"$ENV{DLL_FOLDER}\\SDL2_mixer.dll"
		"$ENV{DLL_FOLDER}\\SDL2_net.dll"
		"$ENV{DLL_FOLDER}\\SDL2_ttf.dll"
		"$ENV{DLL_FOLDER}\\smpeg2.dll"
		"$ENV{DLL_FOLDER}\\zlib1.dll"
	)

	install(FILES ${DLL_FILES}
			DESTINATION .
			COMPONENT runtime
	)

	# Get the list of all data file names and specify where to install them and
	# that they belong to the "data" component.	
	file(GLOB DATA_FILES "${CMAKE_SOURCE_DIR}/data/*.xml")
	file(GLOB DATA_DIR "${CMAKE_SOURCE_DIR}/data")

	install(FILES ${DATA_FILES}
			DESTINATION .
			COMPONENT data
	)
	install(DIRECTORY ${DATA_DIR}/fonts ${DATA_DIR}/images ${DATA_DIR}/sounds
			DESTINATION .
			COMPONENT data
	)

#	install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/vcredist_x86.exe
#		DESTINATION tmp
#		COMPONENT vcredist
#	)

	########################################################################
	# Tell CMake to install the windows runtime libraries to the programs
	# directory and tell CPack that they belong to the applications component
	include(InstallRequiredSystemLibraries)
	install(PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
			DESTINATION .
			COMPONENT application
	)

	# CPack configuration
	set(CPACK_PACKAGE_NAME "Zombie Installer")
	set(CPACK_PACKAGE_VENDOR "MW")
	set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Installs the Zombie game")
	set(CPACK_PACKAGE_VERSION_MAJOR "1")
	set(CPACK_PACKAGE_VERSION_MINOR "0")
	set(CPACK_PACKAGE_VERSION_PATCH "0")
	set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
	set(CPACK_PACKAGE_INSTALL_DIRECTORY "Zombie")

	# Tell CPack about the components and group the data components together.
	set(CPACK_COMPONENTS_ALL application data runtime vcredist) # instructionData)

	# More descriptive names for each of the components, and component groups
	set(CPACK_COMPONENT_APPLICATION_DISPLAY_NAME "Application")
	set(CPACK_COMPONENT_APPLICATION_REQUIRED true)
	#set(CPACK_COMPONENT_GROUP_DATA_DESCRIPTION "Data")

	set(CPACK_COMPONENT_DATA_DISPLAY_NAME "Data")
	set(CPACK_COMPONENT_DATA_REQUIRED false)

	set(CPACK_COMPONENT_RUNTIME_DISPLAY_NAME "Runtime")
	set(CPACK_COMPONENT_RUNTIME_REQUIRED false)

	set(CPACK_COMPONENT_VCREDIST_DISPLAY_NAME "Visual C++ Redistributable Packages for Visual Studio 2013")
	set(CPACK_COMPONENT_VCREDIST_DESCRIPTION "The Visual C++ Redistributable Packages install run-time components that are required to run C++ applications that are built by using Visual Studio 2013.")

	# Text from "LICENSE.txt" is displayed in the installer's license tab.
	set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE.txt")

	# Set the icon used inside the installer wizard and the icons for the installer and uninstaller.
	set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}\\\\mw.bmp")
	set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/zombie.ico")
	set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/zombie.ico")
	set(CPACK_NSIS_INSTALLED_ICON_NAME "Zombie.exe")
 
	# Set the programs displayed under the Start menu shortcut
	# We do not use CPACK_PACKAGE_EXECUTABLES because it expects the binaries to be in a
	# directory named "bin" (CMake bug tracker ID: 0007829).
	set(CPACK_NSIS_MENU_LINKS
		"Zombie" "Zombie"
		"https://github.com/mwthinker/Zombie" "Github Zombie"
	)

	include(CPack)

endif (WIN32)
