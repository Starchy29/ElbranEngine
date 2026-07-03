@echo off

rmdir windows-x64-release /s /q
mkdir windows-x64-release
pushd windows-x64-release

cl /std:c++20 /MD /DWINDOWS /D_UNICODE /D_MBCS ..\..\cpp\*.cpp ..\engine-libs\elbranEngine_win32_release.lib /FeElbranGame kernel32.lib user32.lib d3d11.lib dxguid.lib DXGI.lib XInput.lib Xinput9_1_0.lib Xaudio2.lib d3dcompiler.lib /link /SUBSYSTEM:WINDOWS
del *.obj

popd

call compile_shaders.bat release

ElbranFilePacker.exe ..\assets windows-x64-release\game_assets.bin

mkdir windows-x64-release\Licenses
robocopy "..\Licenses" "windows-x64-release\Licenses" /mir
