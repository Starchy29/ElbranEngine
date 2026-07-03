@echo off

mkdir engine-libs
pushd engine-libs

cl /c /std:c++20 /MDd /DWINDOWS /D_DEBUG /DDEBUG /D_UNICODE /D_MBCS /Zi ..\..\cpp\engine\*.cpp
lib *.obj /OUT:elbranEngine_win32_debug.lib 
del *.obj

set release=true
if "%~1" == "debug" set release=false
if %release%==true (
cl /c /std:c++20 /MD /DWINDOWS /D_UNICODE /D_MBCS ..\..\cpp\engine\*.cpp
lib *.obj /OUT:elbranEngine_win32_release.lib
del *.obj
)

popd