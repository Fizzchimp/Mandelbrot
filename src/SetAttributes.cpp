#include "SetAttributes.h"

SetAttributes::SetAttributes(float maxIterations, float brightness, float zoom, float maxZoom, vec2 center, Side side)
    : maxIterations(maxIterations), brightness(brightness), zoom(zoom), maxZoom(maxZoom), center(center), complexPlanePos(vec2()), side(side) {}

// Ensure the zoom value is lower than the maximum zoom value
void SetAttributes::enforceMaxZoom()
{
    if (zoom > maxZoom) {zoom = maxZoom;}
}

void SetAttributes::enforceSetBoundaries(int width, int height, bool renderJuliaSet)
{
    // If rendering both sets, the measured window widths are decreased
    float juliaSetFactor = (1 + renderJuliaSet);

    // Pre calculation because of the screen dimensions
    float horizontalScale = 1.0f / (1.0f + renderJuliaSet);
    float verticalScale = 1.0f;

    // Factor in screen dimension inequalities
    if (width >= height)
    {
        verticalScale *= (float)height / width;
    } 
    else 
    {
        horizontalScale *= (float)width / height;
    }

    // Calculate if the edge of the screen is past the boundaries and move them back inside
    if (center.x / horizontalScale - zoom < -maxZoom) {center.x = (-maxZoom + zoom) * horizontalScale;} // Left side of screen
    else if (center.x / horizontalScale + zoom > maxZoom) {center.x = (maxZoom - zoom) * horizontalScale;} // Right side of screen

    if (center.y / verticalScale - zoom < -maxZoom) {center.y = (-maxZoom + zoom) * verticalScale;} // Bottom of screen
    else if (center.y / verticalScale + zoom > maxZoom) {center.y = (maxZoom - zoom) * verticalScale;} // Top of screen

}
