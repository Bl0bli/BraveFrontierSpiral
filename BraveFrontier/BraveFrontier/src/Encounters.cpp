#include "Encounters.h"

#include <raylib.h>
//todo base de donnée pour les noms
std::vector<Encounter> CreateAdventure()
{
    int encounters = GetRandomValue(3, 6);

    std::vector<Encounter> adventure;
    for (int i = 0; i < encounters; i++)
    {
        Encounter encounter;
        encounter.type = static_cast<EncounterType>(GetRandomValue(0, 2)); // types jouables : cercle, QTE, mash
        switch (encounter.type)
        {
        case EncounterType::Circle:
                encounter.name = "Goblin";
                encounter.character = "goblin";
                encounter.difficulty = GetRandomValue(1, 5);
                encounter.zelReward = encounter.difficulty * 10;
                break;
            case EncounterType::Qte:
                encounter.name = "Orc";
                encounter.character = "orc";
                encounter.difficulty = GetRandomValue(1, 5);
                encounter.zelReward = encounter.difficulty * 15;
                break;
            case EncounterType::Mash:
                encounter.name = "Troll";
                encounter.character = "troll";
                encounter.difficulty = GetRandomValue(1, 5);
                encounter.zelReward = encounter.difficulty * 20;
                break;
            default:
                break;
        }

        adventure.push_back(encounter);
    }
    return adventure;
}
