cd ..
call vendor\bin\premake\premake5.exe vs2022
IF %ERRORLEVEL% NEQ 0 (PAUSE)