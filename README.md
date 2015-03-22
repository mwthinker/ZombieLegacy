Zombie
======
A project aiming to produce a simple 2D-shooter zombie game.

<b> About </b>
The project is using CMake to generate the environment specific development 
tools. E.g. Visual Studio solution (Windows) and makefile (Unix). See more 
at http://www.cmake.org/.

Open source
======
The project is under the MIT license (see LICENSE.txt).

Building
======
CMake must be installed, at least version 2.8. Create a directory named 
"build" in the project directory. Make sure all dependencies and a native 
compiler are installed. The dependencies must also be recognized by CMAKE.

Dependencies
======
SDL2 (http://www.libsdl.org/, at least 2.0.3), SDL2_image, SDL2_ttf and SDL2_mixer. And the 
application uses GLEW (http://glew.sourceforge.net/) to load OpengGl 2.1.

The external library such as TinyXML2 (https://github.com/leethomason/tinyxml2)is included as source.
