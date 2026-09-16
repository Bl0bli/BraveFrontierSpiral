#include "MashGame.h"

#include <algorithm>
#include <cmath>
#include <raylib.h>

#include "Config.h"
#include "Ui.h"

namespace
{
    const float CenterX = Config::ScreenWidth / 2.0f;
    const float TopY = Config::ScreenHeight * 0.18f;
    const float PunchDuration = 0.1f;
}

void MashGame::Start(Player& player, int difficulty)
{
    _player = &player;
    _difficulty = difficulty;
    _targetPresses = 15 + 6 * _difficulty;
    _duration = 5.0f;
    _readyTimer = 1.0f;
    _punchTimer = 0.0f;
    _presses = 0;
    _timer = _duration;
}

MiniGameResult MashGame::Update(float dt)
{
    _punchTimer = std::max(0.0f, _punchTimer - dt);

    if (_readyTimer > 0.0f)
    {
        _readyTimer -= dt;
        return MiniGameResult::Running;
    }

    if (IsKeyPressed(KEY_SPACE))
    {
        ++_presses;
        _punchTimer = PunchDuration;
        Emit(CombatEvent::HeroAttack);
        Emit(CombatEvent::EnemyHurt);

        if (_presses >= _targetPresses)
        {
            return MiniGameResult::Win;
        }
    }

    _timer -= dt;
    if (_timer <= 0.0f)
    {
        if (_damageOnFail > 0) _player->TakeDamage(_damageOnFail);
        Emit(CombatEvent::EnemyFlee);
        return MiniGameResult::Lose;
    }

    return MiniGameResult::Running;
}

void MashGame::Draw() const
{
    Ui::DrawTextCentered("BRISE L'ARMURE !", CenterX, TopY, 34.0f, RAYWHITE);

    if (_readyTimer > 0.0f)
    {
        const float pulse = 0.5f + 0.5f * std::sin(static_cast<float>(GetTime()) * 10.0f);
        Ui::DrawTextCentered("PRET ?", CenterX, TopY + 120.0f, 48.0f, Fade(GOLD, pulse));
        return;
    }

    const float armorRatio = _targetPresses > 0
        ? 1.0f - std::clamp(static_cast<float>(_presses) / _targetPresses, 0.0f, 1.0f)
        : 0.0f;
    Ui::DrawBar({ CenterX - 260.0f, TopY + 60.0f, 520.0f, 30.0f }, armorRatio, RED);

    const int remaining = std::max(_targetPresses - _presses, 0);
    Ui::DrawTextCentered(TextFormat("%i restants", remaining), CenterX, TopY + 66.0f, 20.0f, WHITE);

    const float timeRatio = _duration > 0.0f ? std::clamp(_timer / _duration, 0.0f, 1.0f) : 0.0f;
    const Color timeColor = timeRatio < 0.25f ? ORANGE : SKYBLUE;
    Ui::DrawBar({ CenterX - 200.0f, TopY + 104.0f, 400.0f, 14.0f }, timeRatio, timeColor);
    Ui::DrawTextCentered(TextFormat("%.1f s", std::max(_timer, 0.0f)), CenterX, TopY + 128.0f, 18.0f, LIGHTGRAY);

    const float punch = 1.0f + (_punchTimer / PunchDuration) * 0.35f;
    Ui::DrawTextCentered("ESPACE !", CenterX, TopY + 190.0f, 44.0f * punch, GOLD);

    Ui::DrawTextCentered("Martele ESPACE le plus vite possible", CenterX, Config::ScreenHeight - 90.0f, 20.0f, RAYWHITE);
}

void MashGame::SetDamageOnFail(int damage)
{
    _damageOnFail = damage;
}
