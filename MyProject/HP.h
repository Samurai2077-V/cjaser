#pragma once
#include <fstream>

class Health {
    float MaxHp;
    float CurrentHp;

public:
    Health()
    {
        MaxHp = 300;
        CurrentHp = 300;
    }

    Health(float _max)
    {
        MaxHp = _max;
        CurrentHp = _max;
    }

    void takeDamage(float damage) {
        CurrentHp -= damage;
        if (CurrentHp < 0.0f) {
            CurrentHp = 0.0f;
        }
    }

    void heal(float Heal) {
        CurrentHp += Heal;
        if (CurrentHp > MaxHp) {
            CurrentHp = MaxHp;
        }
    }

    bool isDead()  {
        return CurrentHp <= 0.0f;
    }

    float getHp()  {
        return CurrentHp;
    }

    float getMaxHp() {
        return MaxHp;
    }
};

