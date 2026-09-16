#include "CircleGame.h"

#include <algorithm>
#include <cassert>
#include <cmath>

#include <raymath.h>

#include "Config.h"
#include "Ui.h"

namespace
{
    const Vector2 Center = { Config::ScreenWidth / 2.0f, Config::ScreenHeight * 0.42f };
    const float Radius = 170.0f;
    const float TrackThickness = 10.0f;
    const float CursorRadius = 12.0f;
    
    const int AttackZoneCount = 3;
    const float AttackZoneSize = 32.0f;
    const float ZoneMargin = 12.0f;
    const float MinDistanceAhead = 45.0f;

    const float GuardZoneSize = 40.0f;
    const float FeedbackDuration = 0.6f;

    float AngleDistance(float from, float to)
    {
        float distance = std::fmod(to - from, 360.0f);
        if (distance < 0.0f) distance += 360.0f;
        return distance;
    }
    
    bool ZonesOverlap(const Zone& a, const Zone& b)
    {
        return AngleDistance(a.start, b.start) < a.size + ZoneMargin
            || AngleDistance(b.start, a.start) < b.size + ZoneMargin;
    }
}

void TestZones()
{
    const Zone normal{ .start = 100.0f, .size = 30.0f };
    assert(normal.Contains(100.0f));  // bord gauche
    assert(normal.Contains(115.0f));
    assert(normal.Contains(130.0f));  // bord droit
    assert(!normal.Contains(99.0f));
    assert(!normal.Contains(131.0f));

    const Zone wrapped{ .start = 350.0f, .size = 30.0f };
    assert(wrapped.Contains(355.0f));
    assert(wrapped.Contains(0.0f));
    assert(wrapped.Contains(5.0f));
    assert(!wrapped.Contains(25.0f));
    assert(!wrapped.Contains(340.0f));
    assert(!wrapped.Contains(180.0f));

    TraceLog(LOG_INFO, "ZONES: tous les tests de Zone::Contains passent");
}

bool Zone::Contains(float angle) const
{
    float end = std::fmod(start + size, 360.0f);
    if (start < end)
    {
        return angle >= start && angle <= end;
    }
    else
    {
        return angle >= start || angle <= end;
    }
}

void CircleGame::Start(Player& player, int difficulty)
{
    _player = &player;
    _difficulty = difficulty;

    _cursorAngle = 0.0f;
    _attackZones.clear();
    _guardZone.reset();
    _feedbackText.clear();

    switch (difficulty)
    {
        case 1:
        _enemyMaxHp = 2;
        _maxSpeed = 200.0f;
        _baseSpeed = 100.0f;
        break;
        case 2:
        _enemyMaxHp = 3;
        _maxSpeed = 250.0f;
        _baseSpeed = 125.0f;
        break;
        case 3:
        _enemyMaxHp = 5;
        _maxSpeed = 250.0f;
        _baseSpeed = 150.0f;
        break;
        case 4:
        _enemyMaxHp = 8;
        _maxSpeed = 300.0f;
        _baseSpeed = 175.0f;
        break;
        case 5:
        _enemyMaxHp = 10;
        _maxSpeed = 350.0f;
        _baseSpeed = 175.0f;
        break;
        default:
        _enemyMaxHp = 5;
        _maxSpeed = 250.0f;
        _baseSpeed = 150.0f;
        break;
    }

    _enemyHp = _enemyMaxHp;
    _speed = _baseSpeed;

    _enemyAttackInterval = std::max(2.0f, 5.0f - difficulty * 0.5f);
    _enemyAttackTimer = _enemyAttackInterval;
    _enemyDamage = 1 + difficulty / 3;
    _feedbackTimer = 0.0f;

    for (int i = 0; i < AttackZoneCount; i++)
    {
        SpawnAttackZone();
    }
}

MiniGameResult CircleGame::Update(float dt)
{
    const float step = _speed * dt;

    _cursorAngle = std::fmod(_cursorAngle + step, 360.0f);
    if (_cursorAngle < 0) _cursorAngle += 360.0f;

    _speed = Lerp(_speed, _baseSpeed, 0.4f * dt);

    if (_guardZone.has_value())
    {
        _guardDistanceLeft -= step;
        if (_guardDistanceLeft < 0.0f)
        {
            _player->TakeDamage(_enemyDamage);
            Emit(CombatEvent::EnemyAttack);
            Emit(CombatEvent::HeroHurt);
            ShowFeedback("TOUCHE !", RED);

            _guardZone.reset();
            _enemyAttackTimer = _enemyAttackInterval;
        }
    }
    else
    {
        _enemyAttackTimer -= dt;
        if (_enemyAttackTimer <= 0.0f)
        {
            StartEnemyAttack();
        }
    }

    if (IsKeyPressed(KEY_SPACE))
    {
        OnSpacePressed();
    }

    if (_feedbackTimer > 0.0f) _feedbackTimer -= dt;

    if (_player->IsDead()) return MiniGameResult::Lose;
    if (_enemyHp <= 0) return MiniGameResult::Win;
    return MiniGameResult::Running;
}

void CircleGame::Draw() const
{
    const float hpRatio = _enemyMaxHp > 0 ? (float)std::max(_enemyHp, 0) / _enemyMaxHp : 0.0f;
    Ui::DrawBar({ Center.x - 220.0f, 40.0f, 440.0f, 24.0f }, hpRatio, RED);
    Ui::DrawTextCentered(TextFormat("%i / %i", std::max(_enemyHp, 0), _enemyMaxHp), Center.x, 44.0f, 18.0f, WHITE);

    const float attackRatio = _guardZone.has_value()
        ? 1.0f
        : 1.0f - _enemyAttackTimer / _enemyAttackInterval;
    Ui::DrawBar({ Center.x - 120.0f, 76.0f, 240.0f, 10.0f }, attackRatio, PURPLE);

    DrawCircleV(Center, Radius - TrackThickness, Fade(BLACK, 0.5f));

    DrawRing(Center, Radius - TrackThickness, Radius + TrackThickness, 0.0f, 360.0f, 72, Fade(BLACK, 0.7f));

    const float angle = _cursorAngle * DEG2RAD;
    const Vector2 cursor = { Center.x + std::cos(angle) * Radius, Center.y + std::sin(angle) * Radius };
    for (const Zone& zone : _attackZones)
        DrawRing(Center, Radius - TrackThickness, Radius + TrackThickness, zone.start, zone.start + zone.size, 16, RED);

    if (_guardZone.has_value())
    {
        const float pulse = 0.6f + 0.4f * std::sin((float)GetTime() * 14.0f);
        DrawRing(Center, Radius - TrackThickness - 6.0f, Radius + TrackThickness + 6.0f,
                 _guardZone->start, _guardZone->start + _guardZone->size, 16, Fade(SKYBLUE, pulse));
    }

    DrawCircleV(cursor, CursorRadius, WHITE);
    DrawCircleLinesV(cursor, CursorRadius, BLACK);

    if (_feedbackTimer > 0.0f)
    {
        const float alpha = std::min(1.0f, _feedbackTimer / FeedbackDuration * 2.0f);
        Ui::DrawTextCentered(_feedbackText.c_str(), Center.x, Center.y - 60.0f, 34.0f, Fade(_feedbackColor, alpha));
    }

    Ui::DrawTextCentered(TextFormat("Vitesse x%.1f", _speed / _baseSpeed), Center.x, Center.y - 12.0f, 20.0f, LIGHTGRAY);
    Ui::DrawTextCentered("ESPACE : zones rouges = attaque | zone bleue = parade", Center.x, Config::ScreenHeight - 90.0f, 20.0f, RAYWHITE);
}

void CircleGame::OnSpacePressed()
{
    if (_guardZone.has_value() && _guardZone->Contains(_cursorAngle)) //garde en prio
    {
        _enemyHp -= 2; //parade
        Emit(CombatEvent::HeroParry);
        Emit(CombatEvent::EnemyHurt);
        ShowFeedback("PARADE ! -2", SKYBLUE);

        _guardZone.reset();
        _enemyAttackTimer = _enemyAttackInterval;
        return;
    }

    for (size_t i = 0; i < _attackZones.size(); i++)
    {
        if (_attackZones[i].Contains(_cursorAngle))
        {
            --_enemyHp;
            _attackZones.erase(_attackZones.begin() + i);
            SpawnAttackZone();

            Emit(CombatEvent::HeroAttack);
            Emit(CombatEvent::EnemyHurt);
            ShowFeedback("-1", ORANGE);
            return;
        }
    }

    _speed = std::min(_speed * 1.2f, _maxSpeed);
    ShowFeedback("RATE !", GRAY);
}

void CircleGame::SpawnAttackZone()
{
    for (int attempt = 0; attempt < 10; attempt++)
    {
        Zone candidate;
        candidate.size = AttackZoneSize;
        candidate.start = (float)GetRandomValue(0, 359);

        if (candidate.Contains(_cursorAngle)) continue;
        if (AngleDistance(_cursorAngle, candidate.start) < MinDistanceAhead) continue;
        if (OverlapsExistingZone(candidate)) continue;

        _attackZones.push_back(candidate);
        return;
    }

    TraceLog(LOG_WARNING, "CIRCLE: aucune place libre trouvee pour une nouvelle zone");
}

bool CircleGame::OverlapsExistingZone(const Zone& candidate) const
{
    for (const Zone& zone : _attackZones)
    {
        if (ZonesOverlap(zone, candidate)) return true;
    }

    return _guardZone.has_value() && ZonesOverlap(*_guardZone, candidate);
}

void CircleGame::StartEnemyAttack()
{
    _guardZone = Zone{ .start = std::fmod(_cursorAngle + GetRandomValue(150, 230), 360.0f), .size = GuardZoneSize };

    _guardDistanceLeft = AngleDistance(_cursorAngle, _guardZone->start + _guardZone->size);
    
    _attackZones.erase(std::remove_if(_attackZones.begin(), _attackZones.end(),
        [this](const Zone& zone) { return ZonesOverlap(*_guardZone, zone); }),
        _attackZones.end());

    const int missing = AttackZoneCount - (int)_attackZones.size();
    for (int i = 0; i < missing; i++) SpawnAttackZone();

    ShowFeedback("GARDE !", SKYBLUE);
}

void CircleGame::ShowFeedback(const std::string& text, Color color)
{
    _feedbackText = text;
    _feedbackColor = color;
    _feedbackTimer = FeedbackDuration;
}
