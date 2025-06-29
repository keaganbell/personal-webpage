@echo off

set functions=-sEXPORTED_FUNCTIONS=_main,_OnWindowResize
set runtime=-sWASM=1 -sNO_EXIT_RUNTIME=1 -sEXPORTED_RUNTIME_METHODS=ccall
set webgl=-sUSE_WEBGPU=1 -sFULL_ES3=1 -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 -sGL_ASSERTIONS
set flags=-Os -Wno-writable-strings %webgl% %runtime% %functions%

rem compile the wasm file
emcc "src/banner_main.cpp" -o "offline-build/banner.js" %flags%

