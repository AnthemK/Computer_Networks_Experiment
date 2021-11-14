@echo off
echo Running this file will build the VS project of TFTP Client
echo.
echo You need the CMake with a minimum version of 3.17
echo  and the Visual Stdio 2019
echo.
pause
mkdir build
cmake -S .\ -B .\build -G "Visual Studio 16 2019" -A "Win32"