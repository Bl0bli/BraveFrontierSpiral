#pragma once
#include <vector>
#include <raylib.h>
#include "Animator.h"

// Effets visuels animés temporaires (impacts, slashs).
// Chaque effet joue une animation sans boucle, puis disparaît.
struct Effect
{
    Animator animator;
    Vector2 position{};
    Vector2 pivot{};
    float scale = 1.0f;
    bool flipX = false;
};

class Effects
{
public:
    void Spawn(const Texture2D& texture, int frameCount, float fps, Vector2 position, float scale, bool flipX = false);
    void Update(float dt); // met à jour, puis supprime les effets terminés
    void Draw() const;
    void Clear();

private:
    std::vector<Effect> _effects;
};
