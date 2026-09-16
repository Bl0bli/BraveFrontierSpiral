#include "Character.h"

void Character::Setup(const CharacterDef& def, Assets& assets)
{
    _def = &def;
    for (const AnimationDef& a : def.animations)
    {
        _animator.AddAnimation(a.name, assets.GetTexture(a.file), a.frameCount, a.fps, a.loop);
    }
    _dead = false;
    _next.clear();
    Play("idle");
}

void Character::Update(float dt)
{
    _animator.Update(dt);
    if (!_next.empty() && _animator.IsFinished())
    {
        Play(_next);
        _next.clear();
    }
}

void Character::Draw(Color tint) const
{
    if (_def == nullptr) return;
    Vector2 pos = position;
    const bool flipX = (faceLeft == _def->facesRight);
    _animator.Draw(pos, _def->pivot, _def->scale, flipX, tint);
}

void Character::Play(const std::string& animation)
{
    if (_dead) return;
    _animator.Play(animation);
}

void Character::PlayOnce(const std::string& animation, const std::string& then)
{
    if (_dead) return;
    _animator.Play(animation, true);
    _next = then;
}

void Character::Kill()
{
    Play("death");
    _next.clear();
    _dead = true;
}

const std::string& Character::GetCurrentAnimation() const
{
    return _animator.GetCurrentAnimation();
}

bool Character::IsDeathFinished() const
{
    return _dead && _animator.IsFinished();
}
