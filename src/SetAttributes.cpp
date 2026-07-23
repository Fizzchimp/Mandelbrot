#include "SetAttributes.h"

SetAttributes::SetAttributes(float maxIterations, float brightness, float zoom, float maxZoom, vec2 center, Side side)
    : maxIterations(maxIterations), brightness(brightness), zoom(zoom), maxZoom(maxZoom), center(center), complexPlanePos(vec2()), side(side) {}


// Updates set properties when zooming in or out (called on scrollwheel callback) // TODO: SHOULD THIS BE A SetAttribute METHOD?
void SetAttributes::updateZoom(double zoomOffset, int width, int height, bool renderJuliaSet)
{
    zoom *= std::pow(1.1, zoomOffset);

    // Increase the maximum number of iterations used in shader calculation
    maxIterations = int(50 * std::log2(1.0f / zoom));
    if (maxIterations < 50)
        maxIterations = 50; // Ensure a minimum number of iterations

    // Increase the brightness value used in the shader calculation
    brightness = int(50 * std::log2(1.0f / zoom));
    if (brightness < 50.0f)
        brightness = 50.0f; // Ensure a minimum brightness value

    enforceMaxZoom();
    enforceSetBoundaries(width, height, renderJuliaSet);
}

// Updates the center set property (called on mouse movement callback) // TODO: SHOULD THIS BE A SetAttribute METHOD?
void SetAttributes::updateCenter(vec2 windowPosOffset, int width, int height, bool renderJuliaSet)
{
    center = center + windowPosOffset * 2.0f * zoom / std::min(width, height);
    
    enforceMaxZoom();
    enforceSetBoundaries(width, height, renderJuliaSet);
}



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
