#include "Ui.h"

#include <algorithm>
#include <cmath>

namespace
{
    const Font* _font = nullptr;

    float Spacing(float size)
    {
        return size / 10.0f;
    }
}

void Ui::SetFont(const Font& font)
{
    _font = &font;
}

void Ui::DrawTextCentered(const char* text, float centerX, float y, float size, Color color)
{
    if (_font == nullptr) return;

    const float spacing = Spacing(size);

    const Vector2 textSize = MeasureTextEx(*_font, text, size, spacing);

    const Vector2 position = { std::round(centerX - textSize.x / 2.0f), std::round(y) };
    
    const float shadowOffset = std::max(2.0f, size / 12.0f);
    const float shadowAlpha = (color.a / 255.0f) * 0.7f;
    DrawTextEx(*_font, text, { position.x + shadowOffset, position.y + shadowOffset }, size, spacing, Fade(BLACK, shadowAlpha));

    DrawTextEx(*_font, text, position, size, spacing, color);
}

void Ui::DrawTextAt(const char* text, Vector2 position, float size, Color color)
{
    if (_font == nullptr) return;

    const float spacing = Spacing(size);
    
    const float shadowOffset = std::max(2.0f, size / 12.0f);
    const float shadowAlpha = (color.a / 255.0f) * 0.7f;
    DrawTextEx(*_font, text, { position.x + shadowOffset, position.y + shadowOffset }, size, spacing, Fade(BLACK, shadowAlpha));

    DrawTextEx(*_font, text, position, size, spacing, color);
}

void Ui::DrawBar(Rectangle rect, float ratio, Color fill, Color background)
{
    ratio = std::clamp(ratio, 0.0f, 1.0f);

    DrawRectangleRec(rect, background);

    Rectangle fillRect = rect;
    fillRect.width *= ratio;

    DrawRectangleRec(fillRect, fill);
    DrawRectangleLinesEx(rect, 2.0f, WHITE);
}
