<h1 align="center" style="border-bottom: none;">
 <a href="https://github.com/ThomasJowett/Cross-Platform-Game-Engine/">Cross Platform Game Engine</a>
</h1>
<p align="center">
 <img width="128" height="128" src="Editor/resources/Icons/Logo.png">
</p>

<h3 align="center">Cross-Platform Game Engine written in C++ that supports OpenGL.</h3>
<p align="center">
 <img width="864" height="528" src="Resources/Linux Screenshot.png">
</p>

To clone this repository run: `git clone --recursive https://github.com/ThomasJowett/Cross-Platform-Game-Engine`

## Project Generation
### Windows
For Visual studio, run the batch files `Progen_vs2019.bat` or `Progen_vs2022.bat` in the Scripts folder
### Mac
Make the premake5 and shell scripts executable by running
```
chmod +x vendor/bin/premake/premake5_arm
chmod +x Scripts/Progen_gmake_arm.sh
chmod +x Scripts/Progen_Xcode_arm.sh
```
For Xcode, run the shell script `Progen_Xcode_arm.sh` in the Scripts folder.

For gcc, run the shell script `Progen_gmake_arm.sh` in the Scripts folder.
### Linux
First install dependencies by opening a console window and running the following command:
```
sudo apt-get install -y libx11-dev libxrandr-dev libxi-dev libxinerama-dev libgl1-mesa-dev libxcursor-dev mesa-common-dev g++ make
```
Then make the premake5 and shell script file executable by running 
```
chmod +x vendor/bin/premake/premake5
chmod +x Scripts/Progen_gmake.sh
```
Then run the shell script to generate the make files 
```
./Scripts/Progen_gmake.sh
```
The engine can now be built by running `make`.
## Example Projects
If you would like to check out some example projects they can be downloaded from [here](https://github.com/ThomasJowett/ExampleGameProjects)
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
* [LiquidFun](https://github.com/google/liquidfun) : 2D Physics library and Extension of Box2D that provides particle and fluid dynamics.
* [lua](https://github.com/lua/lua) : A powerful, efficient, lightweight, embeddable scripting language.
* [sol2](https://github.com/ThePhD/sol2) : A C++ single header library binding to Lua.
* [SPIR-V Cross](https://github.com/KhronosGroup/SPIRV-Cross) : A tool designed for parsing and converting SPIR-V to other shader languages.
* [msdf atlas](https://github.com/Chlumsky/msdf-atlas-gen) : A utility for generating compact font atlases. 
