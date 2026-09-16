#pragma once
#include "CircleGame.h"
#include "MashGame.h"
#include "MiniGame.h"
#include "QteGame.h"

class BossBattle : public MiniGame
{
public:
    void Start(Player& player, int difficulty) override;
    MiniGameResult Update(float dt) override;
    void Draw() const override;

private:
    void StartPhase(int phase, bool isRetry);

    CircleGame _circle;
    QteGame _qte;
    MashGame _mash;

    MiniGame* _current = nullptr; // observe la phase en cours
    int _phase = 0;
    bool _isRetry = false;
    float _transitionTimer = 0.0f;
};
