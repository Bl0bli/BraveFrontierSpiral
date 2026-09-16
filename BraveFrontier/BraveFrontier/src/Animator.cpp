#include "Animator.h"

#include <cmath>

void Animator::AddAnimation(const std::string& name, const Texture2D& texture, int frameCount, float fps, bool loop)
{
    _animations[name] = Animation(texture, frameCount, fps, loop);
}

void Animator::Play(const std::string& name, bool restart)
{
    if (_currentName == name && !restart) return;
    if (_animations.find(name) == _animations.end())
    {
        TraceLog(LOG_WARNING, "ANIMATOR: impossible de jouer l'animation [%s] : elle n'existe pas", name.c_str());
        return;
    }

    _currentName = name;
    _currentFrame = 0;
    _frameTimer = 0.0f;
    _finished = false;
}

void Animator::Update(float dt)
{
    if (_currentName.empty() || _finished) return;
    
    auto it = _animations.find(_currentName);
    if (it == _animations.end()) return;
    const Animation& animation = it->second;

    if (animation.fps <= 0.0f || animation.frameCount <= 0) return;

    const float frameDuration = 1.0f / animation.fps;
    _frameTimer += dt;

    while (_frameTimer >= frameDuration)
    {
        _frameTimer -= frameDuration;
        _currentFrame++;

        if (_currentFrame >= animation.frameCount)
        {
            if (animation.loop)
            {
                _currentFrame = 0;
            }
            else
            {
                _currentFrame = animation.frameCount - 1;
                _finished = true;
                break;
            }
        }
    }
}

void Animator::Draw(Vector2 position, Vector2 pivot, float scale, bool flipX, Color tint) const
{
    auto it = _animations.find(_currentName);
    if (it == _animations.end()) return;
    const Animation& animation = it->second;
    if (animation.frameCount <= 0) return;

    const float frameWidth = static_cast<float>(animation.texture.width) / animation.frameCount;
    const float frameHeight = static_cast<float>(animation.texture.height);

    //découpe du sprite
    Rectangle source = { frameWidth * _currentFrame, 0.0f, flipX ? -frameWidth : frameWidth, frameHeight };

    //position du sprite à l'écran
    Rectangle dest = { std::round(position.x), std::round(position.y), frameWidth * scale, frameHeight * scale };

    const float pivotX = flipX ? frameWidth - pivot.x : pivot.x;
    Vector2 origin = { pivotX * scale, pivot.y * scale };

    DrawTexturePro(animation.texture, source, dest, origin, 0.0f, tint);
}

bool Animator::IsFinished() const
{
    return _finished;
}

const std::string& Animator::GetCurrentAnimation() const
{
    return _currentName;
}

int Animator::GetCurrentFrame() const
{
    return _currentFrame;
}
