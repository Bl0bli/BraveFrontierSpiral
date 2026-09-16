#pragma once
#include <raylib.h>

namespace Ui
{
    void SetFont(const Font& font);

    void DrawTextCentered(const char* text, float centerX, float y, float size, Color color);

    void DrawTextAt(const char* text, Vector2 position, float size, Color color);

    // Barre de progression : ratio 0 = vide, 1 = pleine
    void DrawBar(Rectangle rect, float ratio, Color fill, Color background = Color{ 0, 0, 0, 150 });
}
