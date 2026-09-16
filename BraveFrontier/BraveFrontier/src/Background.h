#pragma once
#include <vector>
#include <raylib.h>

struct ParallaxLayer
{
    Texture2D texture{};
    float speedFactor = 1.0f; // 0 = fixe, 1 = vitesse du sol, > 1 = premier plan
    bool foreground = false; 
};

class Background
{
public:
    void AddLayer(const Texture2D& texture, float speedFactor, bool foreground = false);
    void DrawBack(float scroll) const; 
    void DrawFront(float scroll) const;

private:
    void DrawLayer(const ParallaxLayer& layer, float scroll) const;

    std::vector<ParallaxLayer> _layers;
};
