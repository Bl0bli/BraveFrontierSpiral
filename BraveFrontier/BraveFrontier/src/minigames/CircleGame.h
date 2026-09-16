#pragma once
#include <optional>
#include <string>
#include <vector>
#include <raylib.h>
#include "MiniGame.h"

struct Zone
{
    float start = 0.0f; // angle de début
    float size = 30.0f; // taille de l'arc en degrés

    bool Contains(float angle) const;
};

void TestZones();

class CircleGame : public MiniGame
{
public:
   
    void Start(Player& player, int difficulty) override;
    MiniGameResult Update(float dt) override;
    void Draw() const override;

private:
    void OnSpacePressed();
    void SpawnAttackZone();
    bool OverlapsExistingZone(const Zone& candidate) const;

    void StartEnemyAttack();
    void ShowFeedback(const std::string& text, Color color);

    float _cursorAngle = 0.0f;
    float _speed = 150.0f;

    std::vector<Zone> _attackZones;
    float _baseSpeed = 150.0f;
    float _maxSpeed = 400.0f;
    int _enemyHp = 5;
    int _enemyMaxHp = 5;

    std::optional<Zone> _guardZone; 
    int _enemyDamage = 1;
    float _enemyAttackInterval = 4.0f;
    float _enemyAttackTimer = 4.0f;
    float _guardDistanceLeft = 0.0f;
    std::string _feedbackText;
    Color _feedbackColor = WHITE;
    float _feedbackTimer = 0.0f;
};
