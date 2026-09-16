#include "Background.h"

#include <cmath>

#include "Config.h"

void Background::AddLayer(const Texture2D& texture, float speedFactor, bool foreground)
{
    ParallaxLayer layer;
    layer.texture = texture;
    layer.speedFactor = speedFactor;
    layer.foreground = foreground;
    _layers.push_back(layer);
}

void Background::DrawBack(float scroll) const
{
    for (const auto& layer : _layers)
    {
        if (!layer.foreground)
        {
            DrawLayer(layer, scroll);
        }
    }
}

void Background::DrawFront(float scroll) const
{
    for (const auto& layer : _layers)
    {
        if (layer.foreground)
        {
            DrawLayer(layer, scroll);
        }
    }
}

void Background::DrawLayer(const ParallaxLayer& layer, float scroll) const
{
    float scale = static_cast<float>(Config::ScreenHeight) / layer.texture.height;
    float layerWidth = layer.texture.width * scale;
    float offset = std::fmod(scroll * layer.speedFactor, layerWidth);

    // L'image entière
    Rectangle source = { 0.0f, 0.0f, static_cast<float>(layer.texture.width), static_cast<float>(layer.texture.height) };

    for (float x = -offset; x < Config::ScreenWidth; x += layerWidth)
    {
        Rectangle dest = { std::floor(x), 0.0f, layerWidth + 1.0f, static_cast<float>(Config::ScreenHeight) };

        // origin {0, 0} : (dest.x, dest.y) désigne le coin HAUT-GAUCHE de la copie
        DrawTexturePro(layer.texture, source, dest, { 0.0f, 0.0f }, 0.0f, WHITE);
    }
}
