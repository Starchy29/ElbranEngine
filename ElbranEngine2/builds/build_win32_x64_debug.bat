@echo off

set cpp=false
set hlsl=false
set assets=false
if "%~1" == "" (
set cpp=true
set hlsl=true
set assets=true
)
if "%~1" == "cpp" set cpp=true
if "%~1" == "hlsl" set hlsl=true
if "%~1" == "assets" set assets=true

mkdir windows-x64-debug

if %cpp%==true (
pushd windows-x64-debug
del ElbranGame_DEBUG.exe

cl /std:c++20 /MDd /DWINDOWS /D_DEBUG /DDEBUG /D_UNICODE /D_MBCS /Zi ..\..\cpp\*.cpp ..\..\cpp\engine\*.cpp /FeElbranGame_DEBUG kernel32.lib user32.lib d3d11.lib dxguid.lib DXGI.lib XInput.lib Xinput9_1_0.lib Xaudio2.lib d3dcompiler.lib

del *.obj
del *.ilk
popd
)

if %hlsl%==true call compile_shaders.bat debug

if %assets%==true ElbranFilePacker.exe ..\assets windows-x64-debug\game_assets.bin
