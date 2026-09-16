
#include <algorithm>
#include <raylib.h>

#include "Config.h"
#include "Game.h"

int main()
{

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(Config::ScreenWidth, Config::ScreenHeight, "Brave Spiral");
    SetWindowMinSize(640, 360);
    SetTargetFPS(Config::TargetFPS);

    RenderTexture2D canvas = LoadRenderTexture(Config::ScreenWidth, Config::ScreenHeight);
    Game game;
    game.Init();
    
    while (!WindowShouldClose())
    {
        float deltaTime = std::clamp(GetFrameTime(), 0.0f, 0.1f);
        game.Update(deltaTime);
        if (IsKeyPressed(KEY_F11)) ToggleBorderlessWindowed();

        
        BeginTextureMode(canvas);
        game.Draw();
        EndTextureMode();
        
        float scale = std::min((float)GetScreenWidth() / Config::ScreenWidth, (float)GetScreenHeight() / Config::ScreenHeight);
        BeginDrawing();

        ClearBackground(BLACK);
        DrawTexturePro(canvas.texture,
            {0, 0, (float)canvas.texture.width, (float)-canvas.texture.height},
            {(float)GetScreenWidth() / 2.0f, (float)GetScreenHeight() / 2.0f, Config::ScreenWidth * scale, Config::ScreenHeight * scale},
            {Config::ScreenWidth * scale / 2.0f, Config::ScreenHeight * scale / 2.0f}, 0.0f, WHITE);
        
        EndDrawing();
        
    }

    game.Unload();
    UnloadRenderTexture(canvas);
    CloseWindow();
    return 0;
}
