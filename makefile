functions=-sEXPORTED_FUNCTIONS=_main,_OnWindowResize
runtime=-sWASM=1 -sNO_EXIT_RUNTIME=1 -sEXPORTED_RUNTIME_METHODS=ccall
webgl=-sUSE_WEBGPU=1 -sFULL_ES3=1 -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 -sGL_ASSERTIONS
flags=-Os -Wno-writable-strings $(webgl) $(runtime) $(functions)

.PHONY: site
site:
	emcc "src/banner_main.cpp" -o "offline-build/banner.js" $(flags)
