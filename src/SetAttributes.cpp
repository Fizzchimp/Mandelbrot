#include "SetAttributes.h"

SetAttributes::SetAttributes(float maxIterations, float brightness, float zoom, vec2 center, Side side)
    : maxIterations(maxIterations), brightness(brightness), zoom(zoom), center(center), complexPlanePos(vec2()), side(side) {}