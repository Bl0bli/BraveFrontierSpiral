#pragma once
#include <string>
#include <raylib.h>
#include "Animator.h"
#include "Assets.h"
#include "Characters.h"

class Character
{
public:
    void Setup(const CharacterDef& def, Assets& assets);
    void Update(float dt);
    void Draw(Color tint = WHITE) const;

    void Play(const std::string& animation);
    void PlayOnce(const std::string& animation, const std::string& then);
    void Kill();
    bool IsDeathFinished() const;
    const std::string& GetCurrentAnimation() const;

    // Effets visuels
    void Flash(Color color, float duration); // teinte le personnage un court instant
    Vector2 GetBodyCenter() const;           // milieu du corps à l'écran (pour les effets)

    Vector2 position{}; // les pieds, à l'écran
    bool faceLeft = false;

private:
    const CharacterDef* _def = nullptr;
    Animator _animator;
    std::string _next;
    bool _dead = false;

    // Effets visuels
    Color _flashColor = WHITE;
    float _flashTimer = 0.0f;
};
