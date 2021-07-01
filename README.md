<h1 align="center" style="border-bottom: none;">
 <a href="https://github.com/ThomasJowett/Cross-Platform-Game-Engine/">Cross Platform Game Engine</a>
</h1>
<p align="center">
 <img width="128" height="128" src="/Editor/resources/Icons/Logo.png">
</p>

<h3 align="center">Cross-Platform Game Engine written in C++ that supports OpenGL.</h3>

To clone this repository run: `git clone --recursive https://github.com/ThomasJowett/Cross-Platform-Game-Engine`

## Project Generation
### Windows
For Visual studio, run the batch files `Progen_vs2019.bat` or `Progen_vs2017.bat`
### Mac
For Xcode, run the shell script `Progen_Xcode.sh`
#### Linux
First install dependencies by opening a console window and running the following commands:
* `sudo apt-get install -y libx11-dev`
* `sudo apt-get install -y libxrandr-dev`
* `sudo apt-get install -y libxi-dev`
* `sudo apt-get install -y libxinerama-dev`
* `sudo apt-get install -y libgl1-mesa-dev`
* `sudo apt-get install -y libxcursor-dev`.

If using a virtual machine you may also need:
* `sudo apt-get install xserver-xorg-input-all`

You will also require g++ compiler and make commands to be installed.

Then make the premake5 file executable by running `chmod +x vendor/bin/premake/premake5`.

Then make the shell script executable by running `chmod +x Progen_gmake.sh`.

Then run the shell script with `./Progen_gmake.sh`.

The engine can now be build by running `make`.
## Dependencies
* [imgui](https://github.com/ocornut/imgui) : Dear ImGui: Bloat-free Immediate Mode Graphical User interface for C++ with minimal dependencies.
* [Entt](https://github.com/skypjack/entt) : Fast and reliable entity-component system (ECS) 
* [glfw](https://github.com/glfw/glfw) : A multi-platform library for OpenGL, OpenGL ES, Vulkan, window and input.
* [spdlog](https://github.com/gabime/spdlog) : Fast C++ logging library.
* [stb](https://github.com/nothings/stb) : Single-file public domain (or MIT licensed) libraries for C/C++.
* [simpleini](https://github.com/brofield/simpleini) : Simple initialization file parser and writer.
* [glad](https://github.com/Dav1dde/glad) : Meta loader for OpenGL API.
* [OpenFBX](https://github.com/nem0/OpenFBX) : Lightweight FBX Importer.
* [cereal](https://github.com/USCiLab/cereal) : A C++11 library for serialization.
* [tinyxml2](https://github.com/leethomason/tinyxml2) : A simple, small, efficient, C++ XML parser.
