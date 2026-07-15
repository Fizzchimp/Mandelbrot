#pragma once
#include "vec2.h"

enum Side : char { left = -1, right = 1 };

struct SetAttributes // Attributes used for rendering both sets
{
    int maxIterations; // Maximum number of iterations the fragment shader is allowed to calculate
    float brightness; // Relative brightness of the fractal
    float zoom; // Zoom level of the complex plane
    vec2 center; // The position of the center of the rendered object (complex coordinates)
    vec2 complexPlanePos; // Position of the mouse (complex coordinates)
    
    Side side; // Specifies the side of the screen the set is rendered
    // vec2 windowCenter; // The position on the window the center of the set is rendered

    SetAttributes(float maxIterations, float brightness, float zoom, vec2 center, Side side);

};