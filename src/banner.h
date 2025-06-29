#pragma once

struct simulation {
    b32 Initialized;

    random_series Series;

    vec2 Position;
    vec2 Velocity;
};

struct state {
    b32 Initialized;
    i32 Width;
    i32 Height;

    f64 TimeLastFrame;

    opengl OpenGL;

    simulation Simulation;
};

