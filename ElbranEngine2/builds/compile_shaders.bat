@echo off

mkdir cso

for %%I in (..\hlsl\*PS.hlsl) do fxc /T ps_5_0 /Fo cso\%%~nI.cso %%I
for %%I in (..\hlsl\*VS.hlsl) do fxc /T vs_5_0 /Fo cso\%%~nI.cso %%I
for %%I in (..\hlsl\*GS.hlsl) do fxc /T gs_5_0 /Fo cso\%%~nI.cso %%I
for %%I in (..\hlsl\*CS.hlsl) do fxc /T cs_5_0 /Fo cso\%%~nI.cso %%I

:: parameter 1 should be either "debug" or "release"
ElbranFilePacker.exe cso windows-x64-%~1\shaders.bin
rmdir cso /s /q