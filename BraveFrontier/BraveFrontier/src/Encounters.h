#pragma once
#include <string>
#include <vector>

enum class EncounterType
{
    Circle = 0, // combat : curseur sur un cercle, ESPACE dans les zones
    Qte = 1,    // suite de touches à presser à temps
    Mash = 2,   // marteler ESPACE avant la fin du chrono
    Npc = 3,    // dialogue et soin, pas de mini-jeu
    Boss = 4    // les mini-jeux enchaînés en phases
};

struct Encounter
{
    EncounterType type = EncounterType::Circle;
    std::string name = "???";
    std::string character = "goblin";
    int difficulty = 1;
    int zelReward = 0;
    int zelCostPercent = 0;            // PNJ : part de la bourse retiree, en % (prix paye ou vol)
    int heal = 0;                      // PNJ : points de vie rendus
    std::vector<std::string> dialogue; // PNJ : une chaîne par page de dialogue
};

std::vector<Encounter> CreateAdventure();
