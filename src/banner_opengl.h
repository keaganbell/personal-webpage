#pragma once

struct vertex {
    vec3 Position;
    vec2 UV;
};

struct mesh_object {
    GLuint VAO;
    GLuint VBO;
};

#if 1
#define  BANNER_WIDTH 1024
#define BANNER_HEIGHT 1024
#else
#define  BANNER_WIDTH 256
#define BANNER_HEIGHT 256
#endif

struct frame_buffer {
    GLuint FBO;
    GLuint Texture;
    u32 Width;
    u32 Height;
};

struct opengl {
    mesh_object FullscreenQuad;
    mesh_object FloatyQuad;

    frame_buffer *Dest;
    frame_buffer *Source;
    frame_buffer TempFramebufferA;
    frame_buffer TempFramebufferB;

    frame_buffer PerlinNoiseBuffer;

    GLuint InitialConditionProgram;

    GLuint HeatProgram;
    GLint UniformLocImageX;
    GLint UniformLocImageY;
    GLint UniformLocDiffusionCoefficient;
    GLint UniformLocDeltaTime;

    GLuint ResolveProgram;
};

