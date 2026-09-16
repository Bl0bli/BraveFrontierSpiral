#pragma once
#include <memory>
#include <string>
#include <vector>
#include <raylib.h>
#include "Assets.h"
#include "Background.h"
#include "Character.h"
#include "Effects.h"
#include "Encounters.h"
#include "Player.h"
#include "minigames/MiniGame.h"

enum class GameState
{
    Title,
    Walking,
    Encounter,
    Dialogue,
    Result,  
    GameOver,
    Victory  
};

struct FloatingText
{
    Vector2 position{};
    std::string text;
    Color color = WHITE;
    float timer = 0.0f;
};

class Game
{
public:
    void Init();
    void Update(float dt);
    void Draw() const;
    void Unload();

private:
    void ChangeState(GameState newState);
    void UpdateTitle();
    void UpdateWalking(float dt);

    void ResetRun();
    void PrepareNextEnemy();
    void StartEncounter(const Encounter& encounter, bool isDebug);
    void UpdateEncounter(float dt);

    void HandleDebugKeys();
    void HandleCombatEvent(CombatEvent event);
    
    void FinishEncounter(bool won);
    void UpdateResult(float dt);
    void GoToNextEncounter();
    void UpdateEndScreen();
    
    void UpdateDialogue();
    
    void PlayMusic(Music& music);
    
    void SpawnFloatingText(Vector2 position, const std::string& text, Color color);

    void DrawCharacters() const;
    void DrawTitle() const;
    void DrawHud() const;
    void DrawResult() const;   
    void DrawEndScreen() const;
    void DrawDialogue() const;

    int _bestZel = 0;
    bool _newRecord = false;
    
    Assets _assets;
    
    Background _background;
    float _scroll = 0.0f;

    GameState _state = GameState::Title;
    float _stateTime = 0.0f;
    Character _hero;

    std::vector<Encounter> _adventure;
    int _encounterIndex = 0;
    Encounter _currentEncounter;
    Character _enemy;
    float _nextEncounterX = 0.0f;

    Player _player;
    std::unique_ptr<MiniGame> _miniGame;
    bool _isDebugEncounter = false;
    bool _lastEncounterWon = false;
    float _encounterEndTimer = 0.0f;
    
    std::string _resultText;
    Color _resultColor = WHITE;
    int _lastHp = 0;
    float _shakeTimer = 0.0f;
    
    bool _enemyFleeing = false;
    
    int _dialoguePage = 0;
    
    Music* _currentMusic = nullptr;
    
    Effects _effects;
    std::vector<FloatingText> _floatingTexts;
    float _hitStopTimer = 0.0f;
};
