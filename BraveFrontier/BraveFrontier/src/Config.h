#pragma once

namespace Config
{
    inline constexpr int ScreenWidth = 1280;
    inline constexpr int ScreenHeight = 720;
    inline constexpr int TargetFPS = 144;
    
    inline constexpr float GroundY = 576.0f;

    inline constexpr float WalkSpeed = 300.0f;
    inline constexpr float HeroX = 200.0f;

    inline constexpr float EnemyX = 820.0f;
    inline constexpr float EnemySpeed = 150.0f;
    inline constexpr float EnemyFleeSpeed = 400.0f; // quand il detale vers la droite
    inline constexpr float EncounterSpacing = 1050.0f;

    inline constexpr int PlayerMaxHp = 10;
    
    inline constexpr float ResultDuration = 1.5f;
    inline constexpr float ShakeDuration = 0.15f;
    inline constexpr float EndScreenInputDelay = 0.5f;
}
