#pragma once
#include "GamesEngineeringBase.h"
#include <cmath>

class Vector {  //define a vector(x,y)
public:
    float x;
    float y;

    Vector()
    {
        x = 0;
        y = 0;
    }

    Vector(float _x, float _y)
    {
        x = _x;
        y = _y;
    }

    float distance(Vector pos) {
        return sqrtf((x - pos.x) * (x - pos.x) + (y - pos.y) * (y - pos.y));
    }

    Vector operator+(const Vector& other) const {
        return Vector(x + other.x, y + other.y);
    }

    Vector operator-(const Vector& other) const {
        return Vector(x - other.x, y - other.y);
    }

};

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

