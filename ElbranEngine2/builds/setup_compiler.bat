:: Run this once at startup to enable the compiler
@echo off
pushd "C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\"
call vcvarsall.bat x64
call vcvarsall.bat x86
popd