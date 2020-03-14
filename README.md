# Cross-Platform-Game-Engine

To clone this repository run: `git clone --recursive https://github.com/ThomasJowett/Cross-Platform-Game-Engine`

## Project Generation
 - For Visual studio on windows run the batch files `ProGen_vs2019.bat`
 - For Xcode on MacOS run the shell script `ProGen_Xcode.sh`
 - For Linux:
first install libx11-dev by opening a console window and running `sudo apt-get install -y libx11-dev` , `sudo apt-get install xserver-xorg-input-all`, `sudo apt-get install -y libxrandr-dev`, `sudo apt-get install -y libxi-dev`, `sudo apt-get install -y libxinerama-dev`, `sudo apt-get install -y libgl1-mesa-dev` and `sudo apt-get install -y libxcursor-dev`.
Then make the premake5 file executable by running `chmod +x /vendor/bin/premake/premake5`.
Then make the shell script executable by running `chmod +x ProGen_gmake.sh`.
Then run the shell script with `./ProGen_gmake.sh`.
