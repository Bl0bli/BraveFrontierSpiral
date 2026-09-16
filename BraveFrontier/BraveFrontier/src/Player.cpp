#include "Player.h"

#include <algorithm>

void Player::TakeDamage(int amount)
{
    hp -= amount;
    hp = std::max(hp, 0);
}

void Player::Heal(int amount)
{
    hp += amount;
    hp = std::min(hp, maxHp);
}

bool Player::IsDead() const
{
    return hp <= 0;
}
