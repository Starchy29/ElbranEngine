@echo off

mkdir builds\windows-x64-debug
pushd builds\windows-x64-debug
mkdir assets
mkdir shaders
cl /std:c++20 /Zi /DWINDOWS ..\..\cpp\*.cpp kernel32.lib user32.lib DXGI.lib XInput.lib Xinput9_1_0.lib Xaudio2.lib
(robocopy "$(SolutionDir)Assets" "$(TargetDir)Assets" /mir) ^& IF %ERRORLEVEL% LSS 8 SET ERRORLEVEL = 0
popd