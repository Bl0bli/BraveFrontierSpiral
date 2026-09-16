#include "Characters.h"

#include <unordered_map>

namespace
{
    std::unordered_map<std::string, CharacterDef> BuildCharacters()
    {
        std::unordered_map<std::string, CharacterDef> characters;

        characters["hero"] = CharacterDef{
            .id = "hero",
            .scale = 4.0f,
            .pivot = { 15.0f, 32.0f },
            .facesRight = true,
            .animations = {
                { .name = "idle", .file = "assets/sprites/hero/idle.png", .frameCount = 4, .fps = 8.0f },
                { .name = "run",  .file = "assets/sprites/hero/run.png",  .frameCount = 6, .fps = 12.0f },
                { .name = "walk", .file = "assets/sprites/hero/walk.png", .frameCount = 6, .fps = 10.0f },
                { .name = "attack", .file = "assets/sprites/hero/attack.png", .frameCount = 6, .fps = 14.0f, .loop = false },
                { .name = "hurt",   .file = "assets/sprites/hero/hurt.png",   .frameCount = 4, .fps = 12.0f, .loop = false },
                { .name = "death",  .file = "assets/sprites/hero/death.png",  .frameCount = 8, .fps = 10.0f, .loop = false },
                { .name = "jump",   .file = "assets/sprites/hero/jump.png",   .frameCount = 8, .fps = 12.0f, .loop = false },

            },
        };
        
        characters["goblin"] = CharacterDef{
            .id = "goblin",
            .scale = 3.0f,
            .pivot = { 15.0f, 32.0f },
            .facesRight = true,
            .animations = {
                { .name = "idle", .file = "assets/sprites/goblin/idle.png", .frameCount = 4, .fps = 6.0f },
                { .name = "run",  .file = "assets/sprites/goblin/run.png",  .frameCount = 6, .fps = 10.0f },
                { .name = "attack", .file = "assets/sprites/goblin/attack.png", .frameCount = 6, .fps = 12.0f, .loop = false },
                { .name = "hurt",  .file = "assets/sprites/goblin/hurt.png",  .frameCount = 4, .fps = 12.0f, .loop = false },
                { .name = "death", .file = "assets/sprites/goblin/death.png", .frameCount = 8, .fps = 10.0f, .loop = false },
            },
        };

        characters["orc"] = CharacterDef{
            .id = "orc",
            .scale = 3.0f,
            .pivot = { 15.0f, 32.0f },
            .facesRight = true,
            .animations = {
                { .name = "idle", .file = "assets/sprites/orc/idle.png", .frameCount = 4, .fps = 6.0f },
                { .name = "run",  .file = "assets/sprites/orc/run.png",  .frameCount = 6, .fps = 10.0f },
                { .name = "attack", .file = "assets/sprites/orc/attack.png", .frameCount = 6, .fps = 12.0f, .loop = false },
                { .name = "hurt",  .file = "assets/sprites/orc/hurt.png",  .frameCount = 4, .fps = 12.0f, .loop = false },
                { .name = "death", .file = "assets/sprites/orc/death.png", .frameCount = 8, .fps = 10.0f, .loop = false },
            },
        };

        characters["troll"] = CharacterDef{
            .id = "troll",
            .scale = 3.0f,
            .pivot = { 15.0f, 32.0f },
            .facesRight = true,
            .animations = {
                { .name = "idle", .file = "assets/sprites/troll/idle.png", .frameCount = 4, .fps = 6.0f },
                { .name = "run",  .file = "assets/sprites/troll/run.png",  .frameCount = 6, .fps = 10.0f },
                { .name = "attack", .file = "assets/sprites/troll/attack.png", .frameCount = 6, .fps = 12.0f, .loop = false },
                { .name = "hurt",  .file = "assets/sprites/troll/hurt.png",  .frameCount = 4, .fps = 12.0f, .loop = false },
                { .name = "death", .file = "assets/sprites/troll/death.png", .frameCount = 8, .fps = 10.0f, .loop = false },
            },
        };
        
        characters["merchant"] = CharacterDef{
            .id = "merchant",
            .scale = 3.0f,
            .pivot = { 14.0f, 40.0f },
            .facesRight = true,
            .animations = {
                { .name = "idle", .file = "assets/sprites/merchant/idle.png", .frameCount = 4, .fps = 5.0f },
                { .name = "run",  .file = "assets/sprites/merchant/run.png",  .frameCount = 4, .fps = 10.0f },
            },
        };

        return characters;
    }
    
    const std::unordered_map<std::string, CharacterDef>& AllCharacters()
    {
        static const std::unordered_map<std::string, CharacterDef> characters = BuildCharacters();
        return characters;
    }
}

const CharacterDef& GetCharacterDef(const std::string& id)
{
    const auto& characters = AllCharacters();
    
    auto it = characters.find(id);
    if (it != characters.end())
    {
        return it->second;
    }

    TraceLog(LOG_WARNING, "CHARACTERS: personnage inconnu [%s]", id.c_str());

    static const CharacterDef empty{};
    return empty;
}

std::vector<std::string> GetAllCharacterIds()
{
    std::vector<std::string> ids;
    for (const auto& [id, def] : AllCharacters())
    {
        ids.push_back(id);
    }
    return ids;
}
