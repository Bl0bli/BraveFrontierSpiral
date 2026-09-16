#include "TestGame.h"

#include <raylib.h>

#include "Config.h"
#include "Ui.h"

namespace
{
    const int TargetPresses = 5;
    const float Duration = 5.0f;
}

void TestGame::Start(Player& player, int difficulty)
{
    _player = &player;
    _difficulty = difficulty;
    _presses = 0;
    _timer = Duration;
}

MiniGameResult TestGame::Update(float dt)
{
    _timer -= dt;

    if (IsKeyPressed(KEY_SPACE))
    {
        _presses++;
        Emit(CombatEvent::HeroAttack);
        Emit(CombatEvent::EnemyHurt);
    }

    if (_presses >= TargetPresses) return MiniGameResult::Win;
    if (_timer <= 0.0f) return MiniGameResult::Lose;
    return MiniGameResult::Running;
}

void TestGame::Draw() const
{
    const float centerX = Config::ScreenWidth / 2.0f;

    Ui::DrawTextCentered("TEST : ESPACE 5 fois", centerX, 110.0f, 30.0f, GOLD);
    Ui::DrawTextCentered(TextFormat("%i / %i", _presses, TargetPresses), centerX, 170.0f, 48.0f, WHITE);
    Ui::DrawTextCentered(TextFormat("temps : %.1f s", _timer > 0.0f ? _timer : 0.0f), centerX, 240.0f, 24.0f, WHITE);
}
