#include "QteGame.h"

#include <array>
#include <raylib.h>
#include <algorithm>

#include "Config.h"
#include "Ui.h"

namespace
{
    const std::array<int, 5> Keys = {
        KEY_UP,
        KEY_RIGHT,
        KEY_DOWN,
        KEY_LEFT,
        KEY_SPACE
    };

    const float BoxSize = 84.0f;
    const float BoxSpacing = 26.0f;
    const float RowY = Config::ScreenHeight * 0.42f;

    float ArrowRotation(int key)
    {
        switch (key)
        {
        case KEY_RIGHT: return 0.0f;
        case KEY_DOWN:  return 90.0f;
        case KEY_LEFT:  return 180.0f;
        case KEY_UP:    return 270.0f;
        default:        return 0.0f;
        }
    }
}

void QteGame::Start(Player& player, int difficulty)
{
    _player = &player;
    _difficulty = difficulty;

    _sequence.clear();
    _steps.clear();
    _currentIndex = 0;
    _failCount = 0;

    for (int i = 0; i < _difficulty + 3; i++)
    {
        int randomKeyIndex = GetRandomValue(0, static_cast<int>(Keys.size()) - 1);
        _sequence.push_back(Keys[randomKeyIndex]);
    }

    _steps.assign(_sequence.size(), StepState::Pending);
    _timePerKey = std::max(0.5f, 1.5f - difficulty * 0.2f);
    _timer = _timePerKey;
    _maxFails = std::max(1, static_cast<int>(_sequence.size()) / 3);
    _pauseTimer = 0.5f;
}

MiniGameResult QteGame::Update(float dt)
{
    if (_pauseTimer > 0.0f)
    {
        _pauseTimer -= dt;
        return MiniGameResult::Running;
    }

    if (_failCount > _maxFails || _player->IsDead())
    {
        return MiniGameResult::Lose;
    }
    
    if (_currentIndex >= static_cast<int>(_sequence.size()))
    {
        return MiniGameResult::Win;
    }

    for (int key : Keys)
    {
        if (IsKeyPressed(key))
        {
            ResolveCurrentStep(key == _sequence[_currentIndex]);
            return MiniGameResult::Running;
        }
    }

    _timer -= dt;
    if (_timer <= 0.0f) ResolveCurrentStep(false);
    return MiniGameResult::Running;
}

void QteGame::Draw() const
{
    const int count = static_cast<int>(_sequence.size());
    if (count <= 0) return;

    const float step = BoxSize + BoxSpacing;
    const float totalWidth = count * BoxSize + (count - 1) * BoxSpacing;
    const float startX = Config::ScreenWidth / 2.0f - totalWidth / 2.0f;

    for (int i = 0; i < count; i++)
    {
        const Rectangle box = { startX + i * step, RowY - BoxSize / 2.0f, BoxSize, BoxSize };
        const Vector2 center = { box.x + BoxSize / 2.0f, RowY };

        Color background = Fade(BLACK, 0.6f);
        if (_steps[i] == StepState::Success) background = Fade(GREEN, 0.7f);
        else if (_steps[i] == StepState::Fail) background = Fade(RED, 0.7f);

        DrawRectangleRounded(box, 0.25f, 8, background);

        if (_sequence[i] == KEY_SPACE)
        {
            Ui::DrawTextCentered("ESPACE", center.x, center.y - 5.0f, 10.0f, WHITE);
        }
        else
        {
            DrawPoly(center, 3, 22.0f, ArrowRotation(_sequence[i]), WHITE);
        }

    }

    if (_currentIndex >= 0 && _currentIndex < count)
    {
        const Rectangle box = { startX + _currentIndex * step, RowY - BoxSize / 2.0f, BoxSize, BoxSize };
        const Vector2 center = { box.x + BoxSize / 2.0f, RowY };

        DrawRectangleRoundedLinesEx(box, 0.25f, 8, 4.0f, GOLD);

        const float ratio = std::clamp(_timer / _timePerKey, 0.0f, 1.0f);
        DrawRing(center, BoxSize * 0.58f, BoxSize * 0.66f,
                 -90.0f, -90.0f + 360.0f * ratio, 48, GOLD);
    }

    Ui::DrawTextCentered(TextFormat("Erreurs : %i / %i max", _failCount, _maxFails),
                         Config::ScreenWidth / 2.0f, RowY + BoxSize, 20.0f, RAYWHITE);
    Ui::DrawTextCentered("Presse la touche affichee avant la fin du cercle",
                         Config::ScreenWidth / 2.0f, Config::ScreenHeight - 90.0f, 20.0f, RAYWHITE);
}

void QteGame::ResolveCurrentStep(bool success)
{
    _steps[_currentIndex] = success ? StepState::Success : StepState::Fail;
    if (success)
    {
        Emit(CombatEvent::HeroDodge);
    }
    else
    {
        _player->TakeDamage(1);
        Emit(CombatEvent::EnemyAttack);
        Emit(CombatEvent::HeroHurt);
        _failCount++;
    }

    _pauseTimer = 0.25f;
    _timer = _timePerKey;
    _currentIndex++;
}
