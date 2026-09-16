#include "Assets.h"

void Assets::Init()
{
    const int textureSize = 64;
    const int checkSize = 8;
    Image image = GenImageChecked(textureSize, textureSize, checkSize, checkSize, MAGENTA, GRAY);
    _missingTexture = LoadTextureFromImage(image);
    UnloadImage(image);
}

const Texture2D& Assets::GetTexture(const std::string& path)
{
    auto it = _textures.find(path);
    if (it != _textures.end())
    {
        return it->second;
    }

    if (_failedTextures.contains(path))
    {
        return _missingTexture;
    }

    Texture2D texture = LoadTexture(path.c_str());
    if (texture.id == 0)
    {
        TraceLog(LOG_WARNING, "ASSETS: impossible de charger la texture [%s]", path.c_str());
        _failedTextures.insert(path);
        return _missingTexture;
    }

    SetTextureFilter(texture, TEXTURE_FILTER_POINT);

    auto inserted = _textures.emplace(path, texture);
    return inserted.first->second;
}

const Texture2D& Assets::FindTexture(const std::string& path) const
{
    auto it = _textures.find(path);
    if (it != _textures.end())
    {
        return it->second;
    }
    return _missingTexture;
}

void Assets::LoadMainFont(const std::string& path, int size)
{
    _font = LoadFontEx(path.c_str(), size, 0, 250 );
}

const Font& Assets::GetFont() const
{
    return _font;
}

void Assets::UnloadAll()
{
    for (const auto& [path, texture] : _textures)
    {
        UnloadTexture(texture);
    }
    _textures.clear();
    _failedTextures.clear();

    UnloadTexture(_missingTexture);
    _missingTexture = {};

    UnloadFont(_font);
}
