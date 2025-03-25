@echo off
setlocal

rem Set up DirectX SDK paths
set "DXSDK_PATH=C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)"
set "DXSDK_INCLUDE=%DXSDK_PATH%\Include"
set "DXSDK_LIB=%DXSDK_PATH%\Lib\x64"

rem Set up source and build directories
set "SRC_DIR=..\src"
set "BUILD_DIR=.\build"

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
pushd "%BUILD_DIR%"

rem Compile all cpp files in the src directory and link them.
rem Also add /EHsc to enable exception handling.
cl -Zi /EHsc /I"%DXSDK_INCLUDE%" "%SRC_DIR%\*.cpp" /link /LIBPATH:"%DXSDK_LIB%" d3dx9.lib d3d9.lib user32.lib winmm.lib

popd
endlocal