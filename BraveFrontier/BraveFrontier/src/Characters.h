#pragma once
#include <string>
#include <vector>
#include <raylib.h>

struct AnimationDef
{
    std::string name;
    std::string file;
    int frameCount = 1;
    float fps = 10.0f;
    bool loop = true;
};

struct CharacterDef
{
    std::string id;
    float scale = 3.0f;
    Vector2 pivot{}; //pieds
    bool facesRight = true;
    float bodyHeight = 0.0f; // hauteur du corps à l'échelle 1, pour placer les effets
    std::vector<AnimationDef> animations;
};

const CharacterDef& GetCharacterDef(const std::string& id);
std::vector<std::string> GetAllCharacterIds();
