#pragma once
#include "Vector.h"
#include "GamesEngineeringBase.h"
#include <cmath>

class Camera {
public:
    Vector Position;
    float width;
    float height;

  
    Camera(int _w, int _h)
    {
        Position = Vector();
        width = _w;
        height = _h;
    }

    void update(float targetX, float targetY, float CanvasWidth, float CanvasHeight, GamesEngineeringBase::Image& sprite) {
        //look at
        Position.x = targetX - width / 2.0f;
        Position.y = targetY - height / 2.0f;
        //bound
        if (Position.x < 0.0f) Position.x = 0.0f;
        if (Position.x + width > CanvasWidth)
        {
            Position.x = CanvasWidth - width;
        }
        if (Position.y < 0.0f) Position.y = 0.0f;
        {
            if (Position.y + height > CanvasHeight) Position.y = CanvasHeight - height;
        }
        
    }
};

