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
        encounter.type = static_cast<EncounterType>(GetRandomValue(0, 3));
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
            case EncounterType::Npc:
                {
                    int random = GetRandomValue(1, 3);
                    if (random == 1)
                    {
                        encounter.name = "Soigneur";
                        encounter.character = "goblin"; // placeholder en attendant un sprite de PNJ
                        encounter.heal = GetRandomValue(1, 3);
                        encounter.dialogue = {
                            "Salut, voyageur !",
                            "Tu as l'air mal en point.",
                            TextFormat("Tiens, %i PV, c'est cadeau.", encounter.heal)
                        };
                    }
                    else if (random == 2)
                    {
                        encounter.name = "Type Louche";
                        encounter.character = "orc"; // placeholder en attendant un sprite de PNJ
                        encounter.heal = GetRandomValue(3, 5);
                        encounter.zelCostPercent = GetRandomValue(20, 40);
                        encounter.dialogue = {
                            "Pssst... par ici.",
                            TextFormat("%i PV contre %i%% de ta bourse.", encounter.heal, encounter.zelCostPercent),
                            "Marche conclu, hehe..."
                        };
                    }
                    else
                    {
                        encounter.name = "Voleur";
                        encounter.character = "troll"; // placeholder en attendant un sprite de PNJ
                        encounter.zelCostPercent = GetRandomValue(20, 35);
                        encounter.dialogue = {
                            "Ta bourse ou la vie !",
                            "Je prends ma part, merci.",
                            "Bonne route quand meme !"
                        };
                    }
                    
                    break;
                }
            default:
                break;
        }

        adventure.push_back(encounter);
    }
    return adventure;
}
