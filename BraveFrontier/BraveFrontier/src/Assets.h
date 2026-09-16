#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <raylib.h>

class Assets
{
public:
    void Init();
    void UnloadAll();

    const Texture2D& GetTexture(const std::string& path);
    const Texture2D& FindTexture(const std::string& path) const;

    void LoadMainFont(const std::string& path, int size);
    const Font& GetFont() const;

    // Audio
    Sound& GetSound(const std::string& path);
    Music& GetMusic(const std::string& path);

private:
    std::unordered_map<std::string, Texture2D> _textures;
    std::unordered_set<std::string> _failedTextures;
    Texture2D _missingTexture{};
    
    Font _font{};

    // Audio
    std::unordered_map<std::string, Sound> _sounds;
    std::unordered_map<std::string, Music> _musics;
};
