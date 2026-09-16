#include "Encounters.h"

#include <raylib.h>

namespace
{
    const int HealerWeightMax = 50;  
    const int WeirdoWeightMax = 100;
    const int ThiefWeight = 50;      

    void FillCombat(Encounter& encounter, EncounterType type)
    {
        encounter.type = type;
        encounter.difficulty = GetRandomValue(1, 5);

        switch (type)
        {
        case EncounterType::Qte:
            encounter.name = "Orc";
            encounter.character = "orc";
            encounter.zelReward = encounter.difficulty * 15;
            break;
        case EncounterType::Mash:
            encounter.name = "Troll";
            encounter.character = "troll";
            encounter.zelReward = encounter.difficulty * 20;
            break;
        case EncounterType::Circle:
        default:
            encounter.type = EncounterType::Circle;
            encounter.name = "Goblin";
            encounter.character = "goblin";
            encounter.zelReward = encounter.difficulty * 10;
            break;
        }
    }

    int PickWeighted(const int* weights, int count)
    {
        int total = 0;
        for (int i = 0; i < count; i++) total += weights[i];
        if (total <= 0) return -1;

        int roll = GetRandomValue(1, total);
        for (int i = 0; i < count; i++)
        {
            roll -= weights[i];
            if (roll <= 0) return i;
        }
        return count - 1;
    }

    void FillHealer(Encounter& encounter)
    {
        encounter.name = "Soigneur";
        encounter.character = "goblin";
        encounter.heal = GetRandomValue(1, 3);
        encounter.zelCostPercent = 0;
        encounter.dialogue = {
            "Salut, voyageur !",
            "Tu as l'air mal en point.",
            TextFormat("Tiens, %i PV, c'est cadeau.", encounter.heal)
        };
    }

    void FillWeirdo(Encounter& encounter)
    {
        encounter.name = "Type Louche";
        encounter.character = "merchant";
        encounter.heal = GetRandomValue(3, 5);
        encounter.zelCostPercent = GetRandomValue(20, 40);
        encounter.dialogue = {
            "Pssst... par ici.",
            TextFormat("%i PV contre %i%% de ta bourse.", encounter.heal, encounter.zelCostPercent),
            "Marche conclu, hehe..."
        };
    }

    void FillThief(Encounter& encounter)
    {
        encounter.name = "Voleur";
        encounter.character = "troll";
        encounter.zelCostPercent = GetRandomValue(20, 35);
        encounter.dialogue = {
            "Ta bourse ou la vie !",
            "Je prends ma part, merci.",
            "Bonne route quand meme !"
        };
    }
}

std::vector<Encounter> CreateAdventure()
{
    const int encounters = GetRandomValue(3, 6);

    std::vector<Encounter> adventure;
    for (int i = 0; i < encounters; i++)
    {
        Encounter encounter;
        const EncounterType type = static_cast<EncounterType>(GetRandomValue(0, 3));

        if (type == EncounterType::Npc) encounter.type = EncounterType::Npc;
        else FillCombat(encounter, type);

        adventure.push_back(encounter);
    }
    return adventure;
}

void ResolveNpcEncounter(Encounter& encounter, const Player& player)
{
    if (encounter.type != EncounterType::Npc) return;

    const int missing = player.maxHp - player.hp;
    const bool hurt = missing > 0;
    const bool rich = player.zel > 0;

    // Un soigneur n'a rien a soigner, un voleur n'a rien a voler : la rencontre devient un combat.
    if (!hurt && !rich)
    {
        FillCombat(encounter, static_cast<EncounterType>(GetRandomValue(0, 2)));
        return;
    }

    // Plus le joueur est bas en PV, plus le marchand prend le pas sur le soigneur gratuit.
    const int woundedPercent = missing * 100 / player.maxHp;

    const int weights[3] = {
        hurt ? (100 - woundedPercent) * HealerWeightMax / 100 : 0, // 0 = Soigneur
        hurt ? woundedPercent * WeirdoWeightMax / 100 : 0,         // 1 = Type Louche
        rich ? ThiefWeight : 0                                     // 2 = Voleur
    };

    switch (PickWeighted(weights, 3))
    {
    case 0: FillHealer(encounter); break;
    case 1: FillWeirdo(encounter); break;
    case 2: FillThief(encounter); break;
    default: FillCombat(encounter, EncounterType::Circle); break;
    }
}
