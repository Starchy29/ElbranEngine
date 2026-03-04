@echo off

mkdir builds\windows-x64-release
pushd builds\windows-x64-release
mkdir Licenses
del ElbranGame.exe

cl /std:c++20 /MD /DWINDOWS /D_UNICODE /D_MBCS ..\..\cpp\*.cpp /FeElbranGame kernel32.lib user32.lib d3d11.lib dxguid.lib DXGI.lib XInput.lib Xinput9_1_0.lib Xaudio2.lib d3dcompiler.lib
del *.obj

popd

for %%I in (hlsl\*PS.hlsl) do fxc /T ps_5_0 /Fo assets\%%~nI.cso %%I
for %%I in (hlsl\*VS.hlsl) do fxc /T vs_5_0 /Fo assets\%%~nI.cso %%I
for %%I in (hlsl\*GS.hlsl) do fxc /T gs_5_0 /Fo assets\%%~nI.cso %%I
for %%I in (hlsl\*CS.hlsl) do fxc /T cs_5_0 /Fo assets\%%~nI.cso %%I

ElbranFilePacker.exe assets builds\windows-x64-release\gameAssets.bin
del assets\*.cso

robocopy "Licenses" "builds\windows-x64-release\Licenses" /mir
