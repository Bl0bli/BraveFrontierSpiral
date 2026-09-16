#pragma once
#include "MiniGame.h"

class TestGame : public MiniGame
{
public:
    void Start(Player& player, int difficulty) override;
    MiniGameResult Update(float dt) override;
    void Draw() const override;

private:
    int _presses = 0;
    float _timer = 5.0f;
};
