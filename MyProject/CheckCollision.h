#pragma once
#include "GamesEngineeringBase.h"
#include "camera.h"
#include <cmath>



bool CheckCollision(GamesEngineeringBase::Image& image1, unsigned int x1, unsigned int y1, GamesEngineeringBase::Image& image2, unsigned int x2, unsigned int y2)
{
    unsigned int Start_x = max(x1, x2);
    unsigned int Start_y = max(y1, y2);
    unsigned int End_x = min(x1 + image1.width, x2 + image2.width);
    unsigned int End_y = min(y1 + image1.height, y2 + image2.height);

    if (Start_x >= End_x || Start_y >= End_y) {
        return false;
    }
    for (unsigned int y = Start_y; y < End_y; ++y) {
        for (unsigned int x = Start_x; x < End_x; ++x) {
            unsigned int relativeX1 = x - x1;
            unsigned int relativeY1 = y - y1;
            unsigned int relativeX2 = x - x2;
            unsigned int relativeY2 = y - y2;
            if (image1.alphaAt(relativeX1, relativeY1) > 0 && image2.alphaAt(relativeX2, relativeY2) > 0) {
                return true; 
            }
        }
    }

    return false; 
}




