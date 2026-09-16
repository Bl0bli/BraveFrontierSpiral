#pragma once
#include <vector>
#include "Player.h"

enum class MiniGameResult
{
    Running,
    Win,
    Lose
};

enum class CombatEvent
{
    HeroAttack, 
    HeroParry,  
    HeroHurt,   
    HeroDodge,  
    EnemyAttack,
    EnemyHurt,  
    EnemyFlee   
};

class MiniGame
{
public:
    virtual ~MiniGame() = default;

    virtual void Start(Player& player, int difficulty) = 0;
    virtual MiniGameResult Update(float dt) = 0;
    virtual void Draw() const = 0;

    std::vector<CombatEvent> TakeEvents();

protected:
    void Emit(CombatEvent event);

    Player* _player = nullptr;
    int _difficulty = 1;

private:
    std::vector<CombatEvent> _events;
};
