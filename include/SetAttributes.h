#pragma once
#include "vec2.h"

enum Side : char { left = -1, right = 1 };

struct SetAttributes // Attributes used for rendering both sets
{
    int maxIterations; // Maximum number of iterations the fragment shader is allowed to calculate
    float brightness; // Relative brightness of the fractal
    float zoom; // Zoom level of the complex plane
    float maxZoom; // Maximum zoom level allowed

    vec2 center; // The position of the center of the rendered object (complex coordinates)
    vec2 complexPlanePos; // Position of the mouse (complex coordinates)
    
    Side side; // Specifies the side of the screen the set is rendered
    // vec2 windowCenter; // The position on the window the center of the set is rendered

    SetAttributes(float maxIterations, float brightness, float zoom, float maxZoom, vec2 center, Side side);


    // Ensure the zoom value is lower than the maximum zoom value
    void enforceMaxZoom();

    // Ensure the edges of the screen do not go past a certain point on the complex plane
    void enforceSetBoundaries(int width, int height, bool renderJuliaSet);
};