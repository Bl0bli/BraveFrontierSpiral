#pragma once
#include <vector>
#include "MiniGame.h"

class QteGame : public MiniGame
{
public:
    void Start(Player& player, int difficulty) override;
    MiniGameResult Update(float dt) override;
    void Draw() const override;

private:
    enum class StepState { Pending, Success, Fail };

    void ResolveCurrentStep(bool success);

    std::vector<int> _sequence;
    std::vector<StepState> _steps;
    int _currentIndex = 0;
    int _failCount = 0;
    int _maxFails = 1;
    float _timePerKey = 1.2f;
    float _timer = 0.0f;
    float _pauseTimer = 0.0f; // pause entre deux touches
};
