#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"

#include <iostream>

color normalizeColor(color pixel_color){
    int x = pixel_color.x > 255 ? 255 : static_cast<int>(pixel_color.x);
    int y = pixel_color.y > 255 ? 255 : static_cast<int>(pixel_color.y);
    int z = pixel_color.z > 255 ? 255 : static_cast<int>(pixel_color.z);
    return color(x, y, z);
}

#endif