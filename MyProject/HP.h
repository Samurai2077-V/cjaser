#pragma once
#include <fstream>

class Health {
    float maxHp;
    float currentHp;

public:
    Health()
    {
        maxHp = 300;
        currentHp = 300;
    }

    Health(float _max)
    {
        maxHp = _max;
        currentHp = _max;
    }

    void takeDamage(float damage) {
        currentHp -= damage;
        if (currentHp < 0.0f) {
            currentHp = 0.0f;
        }
    }

    void heal(float Heal) {
        currentHp += Heal;
        if (currentHp > maxHp) {
            currentHp = maxHp;
        }
    }

    bool isDead()  {
        return currentHp <= 0.0f;
    }

    float getHp()  {
        return currentHp;
    }

    float getMaxHp() {
        return maxHp;
    }
    void save(std::ofstream& out) {
        if (out.is_open()) {
            out.write(reinterpret_cast<const char*>(&maxHp), sizeof(maxHp));
            out.write(reinterpret_cast<const char*>(&currentHp), sizeof(currentHp));
        }
    }
    void load(std::ifstream& in) {
        if (in.is_open()) {
            in.read(reinterpret_cast<char*>(&maxHp), sizeof(maxHp));
            in.read(reinterpret_cast<char*>(&currentHp), sizeof(currentHp));
        }
    }
};

