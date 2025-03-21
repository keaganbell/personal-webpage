@echo off

REM compiler configuration
REM set compiler_flags=

REM linker configuration
REM set linker_flags= "-s USE_WEBGL2=1 -s FULL_ES3=1"

REM build the core module
em++ "src/main.cpp" -o "offline-build/index.html"

