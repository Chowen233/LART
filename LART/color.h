#ifndef COLOR_H
#define COLOR_H

#include "interval.h"
#include "vec3.h"

using color = vec3;

inline double linear_to_gamma(double linear_component)
{
    if (linear_component > 0)
        return std::sqrt(linear_component);

    return 0;
}

void write_color(const color & pixel_color, unsigned char& rbyte, unsigned char& gbyte, unsigned char& bbyte) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // Apply a linear to gamma transform for gamma 2
    r = linear_to_gamma(r);
    g = linear_to_gamma(g);
    b = linear_to_gamma(b);
    
    // Translate the [0,1] component values to the byte range [0,255].
    static const interval intensity(0.000, 0.999);
    /*rbyte = unsigned char(256 * intensity.clamp(pixel_color.x()));
    gbyte = unsigned char(256 * intensity.clamp(pixel_color.y()));
    bbyte = unsigned char(256 * intensity.clamp(pixel_color.z()));*/
    
    rbyte = unsigned char(255.999 * r);
    gbyte = unsigned char(255.999 * g);
    bbyte = unsigned char(255.999 * b);
    
    // Write out the pixel color components.
}

#endif