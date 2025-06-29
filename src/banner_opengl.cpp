#pragma once

static GLuint CompileShader(char *VertexShaderSource, char *FragmentShaderSource) {
    GLint Success;

    GLuint VertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(VertexShader, 1, &VertexShaderSource, 0);
    glCompileShader(VertexShader);
    glGetShaderiv(VertexShader, GL_COMPILE_STATUS, &Success);
    if (Success != GL_TRUE) {
        char Buffer[1024] = {};
        glGetShaderInfoLog(VertexShader, sizeof(Buffer), 0, Buffer);
        printf("ERROR: Vertex Shader: %s\n", Buffer);
    }

    GLuint FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(FragmentShader, 1, &FragmentShaderSource, 0);
    glCompileShader(FragmentShader);
    glGetShaderiv(FragmentShader, GL_COMPILE_STATUS, &Success);
    if (Success != GL_TRUE) {
        char Buffer[1024] = {};
        glGetShaderInfoLog(FragmentShader, sizeof(Buffer), 0, Buffer);
        printf("ERROR: Fragment Shader: %s\n", Buffer);
    }

    GLuint Program = glCreateProgram();
    glAttachShader(Program, VertexShader);
    glAttachShader(Program, FragmentShader);
    glLinkProgram(Program);
    glGetProgramiv(Program, GL_LINK_STATUS, &Success);
    if (Success != GL_TRUE) {
        char Buffer[1024] = {};
        glGetProgramInfoLog(Program, sizeof(Buffer), 0, Buffer);
        printf("ERROR: Program Link: %s\n", Buffer);
    }

    glValidateProgram(Program);
    glGetProgramiv(Program, GL_VALIDATE_STATUS, &Success);
    if (Success != GL_TRUE) {
        char Buffer[1024] = {};
        glGetProgramInfoLog(Program, sizeof(Buffer), 0, Buffer);
        printf("ERROR: Program Validation: %s\n", Buffer);
    }

    glDeleteShader(VertexShader);
    glDeleteShader(FragmentShader);

    return Program;
}

static mesh_object CreateFullscreenQuad() {
    mesh_object Result = {};

    f32 QuadVertices[] = {
        // Positions   // TexCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
    };

    glGenVertexArrays(1, &Result.VAO);
    glGenBuffers(1, &Result.VBO);

    glBindVertexArray(Result.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, Result.VBO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBufferData(GL_ARRAY_BUFFER, sizeof(QuadVertices), QuadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void *)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void *)(2*sizeof(float)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return Result;
}

static mesh_object CreateSimpleQuad() {
    mesh_object Result = {};

    glGenVertexArrays(1, &Result.VAO);
    glGenBuffers(1, &Result.VBO);

    glBindVertexArray(Result.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, Result.VBO);

    glBufferData(GL_ARRAY_BUFFER, 4*4*sizeof(f32), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void *)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void *)(2*sizeof(float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return Result;
}

static GLuint CreateInitialConditionProgram() {
    char *VertexSource = R"FOO(#version 300 es 

    in vec2 _Position;
    in vec2 _UV;

    out vec2 UV;

    void main() {
        UV = _UV;
        gl_Position = vec4(_Position, 0.0, 1.0);
    }
    )FOO";

    char *FragmentSource = R"FOO(#version 300 es
    precision mediump float;
    
    in vec2 UV;
    
    out vec4 FragColor;
    
    void main() {
        float Distance = distance(UV, vec2(0.5, 0.5));
        float Temp = exp(-1000.0*Distance*Distance);

        //float Temp = 0.0;
        //if (Distance < 0.01) {
        //    Temp = 1.0;
        //}
        FragColor = vec4(Temp, 0.0, 0.0, 1.0);
    }
    )FOO";

    return CompileShader(VertexSource, FragmentSource);
}

static GLuint CreateHeatProgram() {
    char *VertexSource = R"FOO(#version 300 es 

    in vec2 _Position;
    in vec2 _UV;

    out vec2 UV;

    void main() {
        UV = _UV;
        gl_Position = vec4(_Position, 0.0, 1.0);
    }
    )FOO";

    char *FragmentSource = R"FOO(#version 300 es
    precision highp float;
    
    in vec2 UV;

    uniform sampler2D HeatTexture;
    uniform int ImageX;
    uniform int ImageY;
    uniform float DeltaTime;
    
    out vec4 FragColor;
    
    void main() {
        ivec2 Coord = ivec2(gl_FragCoord.xy);
        float T0 = texelFetch(HeatTexture, Coord, 0).r;

        int LeftX  = Coord.x - 1 + 0*ImageX;
        int RightX = Coord.x + 1 + 0*ImageY;
        int DownY  = Coord.y - 1;
        int UpY    = Coord.y + 1;

        // Sink Boundary Conditions
        //float TempLeft  = 0.0;
        //float TempRight = 0.0;
        //float TempDown  = 0.0;
        //float TempUp    = 0.0;

        // Wrapping Boundary Conditions
        float TempLeft  = texelFetch(HeatTexture, ivec2( ImageX, Coord.y), 0).r;
        float TempRight = texelFetch(HeatTexture, ivec2(      0, Coord.y), 0).r;
        float TempDown  = texelFetch(HeatTexture, ivec2(Coord.x,  ImageY), 0).r;
        float TempUp    = texelFetch(HeatTexture, ivec2(Coord.x,       0), 0).r;

        if (LeftX > 0) {
            TempLeft = texelFetch(HeatTexture, ivec2(LeftX, Coord.y), 0).r;
        }
        if (RightX < ImageX) {
            TempRight = texelFetch(HeatTexture, ivec2(RightX, Coord.y), 0).r;
        }
        if (DownY > 0) {
            TempDown = texelFetch(HeatTexture, ivec2(Coord.x, DownY), 0).r;
        }
        if (UpY < ImageY) {
            TempUp = texelFetch(HeatTexture, ivec2(Coord.x, UpY), 0).r;
        }

        float ddTx = (TempLeft - 2.0*T0 + TempRight);
        float ddTy = (TempUp - 2.0*T0 + TempDown);
        float Laplacian = ddTx + ddTy;

        float D = 5.0;
        float Temperature = T0 + D*Laplacian*DeltaTime;
        FragColor = vec4(Temperature, 0.0, 0.0, 1.0);
    }
    )FOO";

    return CompileShader(VertexSource, FragmentSource);
}

static GLuint CreateResolveProgram() {
    char *VertexSource = R"FOO(#version 300 es 

    in vec2 _Position;
    in vec2 _UV;

    out vec2 UV;

    void main() {
        UV = _UV;
        gl_Position = vec4(_Position, 0.0, 1.0);
    }
    )FOO";

    char *FragmentSource = R"FOO(#version 300 es
    precision mediump float;
    
    in vec2 UV;

    uniform sampler2D HeatTexture;
    
    out vec4 FragColor;
    
    void main() {
        float t = clamp(texture(HeatTexture, UV).r, 0.0, 1.0);
        float r = sqrt(t);
        float g = t*t;
        float b = -0.15 - 2.0/(10.0*t - 12.0);
        FragColor = vec4(r, g, b, 1.0);
    }
    )FOO";

    return CompileShader(VertexSource, FragmentSource);
}

static GLuint CreateScalarTexture(u32 Width, u32 Height) {
    GLuint Texture;
    glGenTextures(1, &Texture);
    glBindTexture(GL_TEXTURE_2D, Texture);

    // Use GL_R32F or GL_RGBA32F for floating point data (GLES3 supports it)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F,
            Width, Height,
            0, GL_RED, GL_FLOAT, NULL);

    // Texture filtering & wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);
    return Texture;
}

static frame_buffer CreateFrameBuffer(u32 Width, u32 Height) {
    frame_buffer Result = {};
    Result.Width = Width;
    Result.Height = Height;
    Result.Texture = CreateScalarTexture(Result.Width, Result.Height);

    glGenFramebuffers(1, &Result.FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, Result.FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Result.Texture, 0);

    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (Status != GL_FRAMEBUFFER_COMPLETE) {
        printf("ERROR: Framebuffer incomplete: 0x%x\n", Status);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return Result;
}

static void InitOpenGL() {
    opengl *OpenGL = &GlobalState.OpenGL;

    OpenGL->InitialConditionProgram = CreateInitialConditionProgram();
    OpenGL->HeatProgram = CreateHeatProgram();
    OpenGL->UniformLocImageX = glGetUniformLocation(OpenGL->HeatProgram, "ImageX");
    OpenGL->UniformLocImageY = glGetUniformLocation(OpenGL->HeatProgram, "ImageY");
    OpenGL->UniformLocDeltaTime = glGetUniformLocation(OpenGL->HeatProgram, "DeltaTime");
    
    OpenGL->ResolveProgram = CreateResolveProgram();

    OpenGL->FullscreenQuad = CreateFullscreenQuad();
    OpenGL->FloatyQuad = CreateSimpleQuad();

    OpenGL->TempFramebufferA = CreateFrameBuffer(BANNER_WIDTH, BANNER_HEIGHT);
    OpenGL->TempFramebufferB = CreateFrameBuffer(BANNER_WIDTH, BANNER_HEIGHT);
    OpenGL->PerlinNoiseBuffer = CreateFrameBuffer(256, 256);
}

static void RenderInitialCondition(frame_buffer *Buffer) {
    opengl *OpenGL = &GlobalState.OpenGL;

    glBindFramebuffer(GL_FRAMEBUFFER, Buffer->FBO);
    glViewport(0, 0, Buffer->Width, Buffer->Height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(OpenGL->FullscreenQuad.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->FullscreenQuad.VBO);
    glUseProgram(OpenGL->InitialConditionProgram);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

static void RenderTemperatureTexture(frame_buffer *Dest, frame_buffer *Source, f64 Frametime) {
    opengl *OpenGL = &GlobalState.OpenGL;

    glBindFramebuffer(GL_FRAMEBUFFER, Dest->FBO);
    glViewport(0, 0, Dest->Width, Dest->Height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(OpenGL->FullscreenQuad.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->FullscreenQuad.VBO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Source->Texture);

    glUseProgram(OpenGL->HeatProgram);
    glUniform1i(OpenGL->UniformLocImageX, BANNER_WIDTH);
    glUniform1i(OpenGL->UniformLocImageY, BANNER_HEIGHT);
    glUniform1f(OpenGL->UniformLocDeltaTime, Frametime/1000.0);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

static void RenderResolvedFrame(GLuint Texture, f64 Frametime) {
    opengl *OpenGL = &GlobalState.OpenGL;
    f64 DeltaTime = Frametime/1000.0f;

    glViewport(0, 0, GlobalState.Width, GlobalState.Height);
    glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture);

    glBindVertexArray(OpenGL->FloatyQuad.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->FloatyQuad.VBO);

    f32 Zoom = 0.5f;
    f32 Aspect = (f32)GlobalState.Width/GlobalState.Height;
    f32 HalfW = Zoom*0.5f;
    f32 HalfH = HalfW/Aspect;
    f32 Speed = 0.00f;

    vec2 P = GlobalState.Simulation.Position + Speed*DeltaTime*Normalized(GlobalState.Simulation.Velocity);
    if (P.x < HalfW) {
        P.x = HalfW;
        GlobalState.Simulation.Velocity.x *= -1.0f;
    }
    else if (P.x > 1.0f - HalfW) {
        P.x = 1.0f - HalfW;
        GlobalState.Simulation.Velocity.x *= -1.0f;
    }
    if (P.y < HalfH) {
        P.y = HalfH;
        GlobalState.Simulation.Velocity.y *= -1.0f;
    }
    else if (P.y > 1.0f - HalfH) {
        P.y = 1.0f - HalfH;
        GlobalState.Simulation.Velocity.y *= -1.0f;
    }
    GlobalState.Simulation.Position = P;

    f32 QuadVertices[] = {
        // Positions  // TexCoords
        -1.0f,  1.0f, P.x - HalfW, P.y + HalfH,
        -1.0f, -1.0f, P.x - HalfW, P.y - HalfH,
         1.0f,  1.0f, P.x + HalfW, P.y + HalfH,
         1.0f, -1.0f, P.x + HalfW, P.y - HalfH,
    };
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(QuadVertices), QuadVertices);
    glUseProgram(OpenGL->ResolveProgram);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

static void EndFrame(f64 Frametime) {
    opengl *OpenGL = &GlobalState.OpenGL;

    if (!GlobalState.Simulation.Initialized) {
        OpenGL->Dest = &OpenGL->TempFramebufferA;
        OpenGL->Source = &OpenGL->TempFramebufferB;
        RenderInitialCondition(OpenGL->Source);

        simulation *Sim = &GlobalState.Simulation;
        Sim->Series = InitRandom(6969); // nicenice
        Sim->Position = vec2(0.5f, 0.5f);
        Sim->Velocity = Normalized(vec2(RandomBilateral(&Sim->Series), RandomBilateral(&Sim->Series)));
        Sim->Initialized = true;
    }
    else {
        i32 SimulationsPerFrame = 50;
        for (i32 i = 0; i < SimulationsPerFrame; ++i) {
            RenderTemperatureTexture(OpenGL->Dest, OpenGL->Source, Frametime);

            frame_buffer *Temp = OpenGL->Dest;
            OpenGL->Dest = OpenGL->Source;
            OpenGL->Source = Temp;
        }
    }

    RenderResolvedFrame(OpenGL->Source->Texture, Frametime);
}
