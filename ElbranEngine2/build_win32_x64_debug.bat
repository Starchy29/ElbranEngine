@echo off

mkdir builds\windows-x64-debug
pushd builds\windows-x64-debug
mkdir assets
mkdir shaders
cl /std:c++20 /DWINDOWS /Zi ..\..\cpp\*.cpp /FeElbranGame_DEBUG kernel32.lib user32.lib DXGI.lib XInput.lib Xinput9_1_0.lib Xaudio2.lib
del *.obj
del *.ilk

for %%I in (..\..\hlsl\*PS.hlsl ..\..\hlsl\*PP.hlsl) do fxc /T ps_5_1 /Fo shaders\%%~nI.cso %%I
for %%I in (..\..\hlsl\*VS.hlsl) do fxc /T vs_5_1 /Fo shaders\%%~nI.cso %%I
for %%I in (..\..\hlsl\*GS.hlsl) do fxc /T gs_5_1 /Fo shaders\%%~nI.cso %%I
for %%I in (..\..\hlsl\*CS.hlsl) do fxc /T cs_5_1 /Fo shaders\%%~nI.cso %%I

robocopy "..\..\Assets" "assets" /mir
popd