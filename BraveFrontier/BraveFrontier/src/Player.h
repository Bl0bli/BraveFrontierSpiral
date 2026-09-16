#pragma once

#include "Config.h"

struct Player
{
    int hp = Config::PlayerMaxHp;
    int maxHp = Config::PlayerMaxHp;
    int zel = 0;

    void TakeDamage(int amount);
    void Heal(int amount);
    bool IsDead() const;
};
