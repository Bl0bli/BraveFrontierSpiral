#pragma once
#include <map>
#include <string>
#include <raylib.h>

struct Animation
{
    Texture2D texture{};
    int frameCount = 1;
    float fps = 10.0f;
    bool loop = true; // false = reste sur la dernière frame
};

class Animator
{
public:
    void AddAnimation(const std::string& name, const Texture2D& texture, int frameCount, float fps = 10.0f, bool loop = true);

    void Play(const std::string& name, bool restart = false);

    void Update(float dt);

    void Draw(Vector2 position, Vector2 pivot, float scale, bool flipX, Color tint = WHITE) const;

    bool IsFinished() const;
    const std::string& GetCurrentAnimation() const;
    int GetCurrentFrame() const;

private:
    std::map<std::string, Animation> _animations;
    std::string _currentName;
    int _currentFrame = 0;
    float _frameTimer = 0.0f;
    bool _finished = false;
};
