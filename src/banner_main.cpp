#include <GLES3/gl3.h>
#include <emscripten.h>
#include <emscripten/html5.h>

#include <stdio.h>
#include <math.h>

#include "defines.h"
#include "maths.h"
#include "random.h"
#include "banner_opengl.h"
#include "banner.h"

static state GlobalState;

#include "banner_opengl.cpp"

static EMSCRIPTEN_WEBGL_CONTEXT_HANDLE WebGLContext;

extern "C" {
    EMSCRIPTEN_KEEPALIVE
    void OnWindowResize() {
        i32 Width = 0;
        i32 Height = 0;
        emscripten_get_canvas_element_size("#canvas", &Width, &Height);

        f64 DevicePixelRatio = emscripten_get_device_pixel_ratio();

        GlobalState.Width = (int)(Width * DevicePixelRatio);
        GlobalState.Height = (int)(Height * DevicePixelRatio);
    }
}

void MainLoop() {
    if (!GlobalState.Initialized) {
        InitOpenGL();
        GlobalState.Initialized = true;
    }

    f64 TimeThisFrame = emscripten_get_now(); // milliseconds
    f64 Frametime = TimeThisFrame - GlobalState.TimeLastFrame;
    GlobalState.TimeLastFrame = TimeThisFrame;

    EndFrame(Frametime);
}

int main() {
    EmscriptenWebGLContextAttributes Attributes;
    emscripten_webgl_init_context_attributes(&Attributes);
    Attributes.majorVersion = 2;
    Attributes.minorVersion = 0;
    
    // Create WebGL context
    WebGLContext = emscripten_webgl_create_context("#canvas", &Attributes);
    if (WebGLContext <= 0) {
        emscripten_console_error("Failed to create WebGL context");
        return 0;
    }
    
    EMSCRIPTEN_RESULT Result = emscripten_webgl_make_context_current(WebGLContext);
    if (Result != EMSCRIPTEN_RESULT_SUCCESS) {
        emscripten_console_error("Failed to make WebGL context current");
        return 0;
    }
    
    emscripten_get_canvas_element_size("#canvas", &GlobalState.Width, &GlobalState.Height);
    emscripten_set_main_loop(MainLoop, 0, 1);
    return 0;
}
