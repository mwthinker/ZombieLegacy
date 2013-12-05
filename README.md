Zombie
======
A project aiming to produce a simple 2D-shooter zombie game.

<p><b> About </b></p>
The project is using CMake to generate the environment specific development 
tools. E.g. Visual Studio solutions (windows) and makefile (Unix). See more 
at http://www.cmake.org/

The zombie project has library dependencies to SDL2 (http://www.libsdl.org/), 
SDL2_image, SDL2_mixer, SDL2_ttf and OpenGL. It also uses the Box2D library 
(http://box2d.org/) but that is embedded in the project, it is under the 
open source license "License.txt" found in the Box2D directory. To read 
XML files, the TinyXML2 (http://www.grinninglizard.com/tinyxml2/) library is 
used and the source is embedded in the zombie project. TinyXML2 is under 
the ZLIB license.

There is also two other project embedded in the zombie project, MwLibrary2 
and MwExtra. The former is basically a C++ wrapper around SDL2 and the latter 
auxiliary classes intended to make a coders life easier.

Open source
======
All code in the src folder is under the MIT license (see LICENSE.txt).

Building
======
CMake must be installed with at least version 2.8. Create a directory named 
"build" in the project directory. Make sure all dependencies and a native 
compiler are installed. The dependencies must also be recognized by CMAKE.

<p><b> Unix </b></p>
Open a terminal inside the "build" directory and run:

<p>$ cmake ..</p>
<p>$ make</p>

The first to generate the makefile and the second to compile the zombie project.

<p>$ ./zombie</p>

to run the executable.

<p><b> Windows </b></p>
Run CMake (cmake-gui). Choose "Browse Source.." and point to the project folder.
Do the same for "Browse Build..." but point to the build folder.
Push the configure button and choose at least "Visual Studio 11" and push finish.
Push "Generate" then the visual studio solution will appear in the build folder.

This file is written by MwThinker.
