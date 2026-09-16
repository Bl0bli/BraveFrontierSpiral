#include "MiniGame.h"

std::vector<CombatEvent> MiniGame::TakeEvents()
{
    std::vector<CombatEvent> events;
    events.swap(_events);
    _events.clear();
    return events;
}

void MiniGame::Emit(CombatEvent event)
{
    _events.push_back(event);
}
