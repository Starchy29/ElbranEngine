@echo off

mkdir builds\windows-x64-debug
pushd builds\windows-x64-debug

del ElbranGame_DEBUG.exe
cl /std:c++20 /MDd /DWINDOWS /D_DEBUG /DDEBUG /D_UNICODE /D_MBCS /Zi ..\..\cpp\*.cpp /FeElbranGame_DEBUG kernel32.lib user32.lib d3d11.lib dxguid.lib DXGI.lib XInput.lib Xinput9_1_0.lib Xaudio2.lib d3dcompiler.lib

del *.obj
del *.ilk

popd

for %%I in (hlsl\*PS.hlsl) do fxc /T ps_5_0 /Fo assets\%%~nI.cso %%I
for %%I in (hlsl\*VS.hlsl) do fxc /T vs_5_0 /Fo assets\%%~nI.cso %%I
for %%I in (hlsl\*GS.hlsl) do fxc /T gs_5_0 /Fo assets\%%~nI.cso %%I
for %%I in (hlsl\*CS.hlsl) do fxc /T cs_5_0 /Fo assets\%%~nI.cso %%I

ElbranFilePacker.exe assets builds\windows-x64-debug\gameAssets.bin
del assets\*.cso
