#pragma once
#include <cmath>
#include <fstream>

class Vector {
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

    void save(std::ofstream& output) const {
        output.write(reinterpret_cast<const char*>(&x), sizeof(x));
        output.write(reinterpret_cast<const char*>(&y), sizeof(y));
    }
    void load(std::ifstream& input) {
        input.read(reinterpret_cast<char*>(&x), sizeof(x));
        input.read(reinterpret_cast<char*>(&y), sizeof(y));
    }
};
