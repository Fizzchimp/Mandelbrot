#pragma once
#include "vec2.h"

struct SetAttributes // Attributes used for rendering both sets
{
    int maxIterations; // Maximum number of iterations the fragment shader is allowed to calculate
    float brightness; // Relative brightness of the fractal
    float zoom; // Zoom level of the complex plane
    vec2 center; // The position of the center of the rendered object on the complex plane
    vec2 complexPlanePos; // Position of the mouse on the complex plane

    SetAttributes(float maxIterations, float brightness, float zoom, vec2 center);
};