#pragma once
#include "MiniGame.h"

class MashGame : public MiniGame
{
public:
    void Start(Player& player, int difficulty) override;
    MiniGameResult Update(float dt) override;
    void Draw() const override;

    void SetDamageOnFail(int damage);

private:
    int _presses = 0;
    int _targetPresses = 25;
    float _duration = 5.0f;
    float _timer = 5.0f;
    float _readyTimer = 1.0f;
    float _punchTimer = 0.0f; // effet de zoom à chaque appui
    int _damageOnFail = 0;
};
