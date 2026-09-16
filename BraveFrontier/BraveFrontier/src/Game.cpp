
#include "Game.h"

#include <algorithm>
#include <cmath>

#include "Characters.h"
#include "Config.h"
#include "Save.h"
#include "Ui.h"
#include "minigames/CircleGame.h"
#include "minigames/MashGame.h"
#include "minigames/QteGame.h"
#include "minigames/TestGame.h"

namespace
{
    std::unique_ptr<MiniGame> CreateMiniGame(EncounterType type)
    {
        switch (type)
        {
        case EncounterType::Circle:
            return std::make_unique<CircleGame>();
        case EncounterType::Qte:
            return std::make_unique<QteGame>();
        case EncounterType::Mash:
            return std::make_unique<MashGame>();
        case EncounterType::Boss:
        default:
            return std::make_unique<TestGame>();
        }
    }

    const std::string BackgroundSkySheet = "assets/sprites/backgrounds/Background_0.png";
    const std::string BackgroundMountainsSheet = "assets/sprites/backgrounds/2.Mountains.png";
    const std::string BackgroundGroundSheet = "assets/sprites/backgrounds/4.Ground.png";

    const std::string MainFontFile = "assets/fonts/Press_Start_2P/PressStart2P-Regular.ttf";
    const int MainFontSize = 32;

    const float DialogueSpeed = 34.0f;
    const Rectangle DialogueBox = { 70.0f, 584.0f, Config::ScreenWidth - 140.0f, 124.0f };

    const char* StateName(GameState state)
    {
        switch (state)
        {
        case GameState::Title:     return "Title";
        case GameState::Walking:   return "Walking";
        case GameState::Encounter: return "Encounter";
        case GameState::Dialogue:  return "Dialogue";
        case GameState::Result:    return "Result";
        case GameState::GameOver:  return "GameOver";
        case GameState::Victory:   return "Victory";
        }
        return "?";
    }
}

void Game::Init()
{
    _assets.Init();
    
    _assets.LoadMainFont(MainFontFile, MainFontSize);
    SetTextureFilter(_assets.GetFont().texture, TEXTURE_FILTER_POINT);
    Ui::SetFont(_assets.GetFont());

    _hero.Setup(GetCharacterDef("hero"), _assets);
    _hero.position = { Config::HeroX, Config::GroundY }; // position = les pieds
    _hero.faceLeft = false;

    _enemy.Setup(GetCharacterDef("goblin"), _assets);
    _enemy.position = { Config::EnemyX, Config::GroundY };
    _enemy.faceLeft = true;

    _background.AddLayer(_assets.GetTexture(BackgroundSkySheet), 0.05f);
    _background.AddLayer(_assets.GetTexture(BackgroundMountainsSheet), 0.3f);
    _background.AddLayer(_assets.GetTexture(BackgroundGroundSheet), 1.0f);

    _bestZel = Save::LoadBestZel();
    
    TestZones();
}

void Game::ChangeState(GameState newState)
{
    _state = newState;
    _stateTime = 0.0f;

    switch (newState)
    {
    case GameState::Title:
        _hero.Play("idle");
        break;
    case GameState::Walking:
        _hero.Play("run");
        break;
    case GameState::GameOver:
    case GameState::Victory:
        if (_player.zel > _bestZel)
        {
            _bestZel = _player.zel;
            Save::SaveBestZel(_bestZel);
            _newRecord = true;
        }
        break;
    default:
        break;
    }
}

void Game::Update(float dt)
{
    _stateTime += dt;

    if (_state == GameState::Title || _state == GameState::Walking)
    {
        HandleDebugKeys();
    }

    switch (_state)
    {
    case GameState::Title: UpdateTitle(); break;
    case GameState::Walking: UpdateWalking(dt); break;
    case GameState::Encounter: UpdateEncounter(dt); break;
    case GameState::Dialogue: UpdateDialogue(dt); break;
    case GameState::Result: UpdateResult(dt); break;
    case GameState::GameOver:
    case GameState::Victory: UpdateEndScreen(); break;
    default: UpdateTitle(); break;
    }
    
    if (_player.hp < _lastHp) _shakeTimer = Config::ShakeDuration;
    _lastHp = _player.hp;
    if (_shakeTimer > 0.0f) _shakeTimer -= dt;

    if (_enemyFleeing)
    {
        _enemy.position.x += Config::EnemyFleeSpeed * dt;
    }

    _hero.Update(dt);
    _enemy.Update(dt);
}

void Game::UpdateTitle()
{
    if (IsKeyPressed(KEY_SPACE))
    {
        ResetRun();
        ChangeState(GameState::Walking);
    }
}

void Game::UpdateWalking(float dt)
{
    _scroll += Config::WalkSpeed * dt;
    if (_encounterIndex >= (int)_adventure.size()) return;
    
    _nextEncounterX -= Config::EnemySpeed * dt;

    _enemy.position.x = _nextEncounterX - _scroll;

    if (_enemy.position.x <= Config::EnemyX)
    {
        StartEncounter(_adventure[_encounterIndex], false);
    }
}

void Game::UpdateEncounter(float dt)
{
    if (_miniGame == nullptr) return;
    
    const MiniGameResult result = _miniGame->Update(dt);
    for (CombatEvent event : _miniGame->TakeEvents())
    {
        HandleCombatEvent(event);
    }
    
    if (result != MiniGameResult::Running)
    {
        _miniGame.reset();
        FinishEncounter(result == MiniGameResult::Win);
    }
}

void Game::HandleDebugKeys()
{
    Encounter debug;
    if (IsKeyPressed(KEY_F1))      debug = { .type = EncounterType::Circle, .name = "Debug Cercle", .character = "goblin", .difficulty = 2 };
    else if (IsKeyPressed(KEY_F2)) debug = { .type = EncounterType::Qte,    .name = "Debug QTE",    .character = "orc",    .difficulty = 2 };
    else if (IsKeyPressed(KEY_F3)) debug = { .type = EncounterType::Mash,   .name = "Debug Mash",   .character = "troll",  .difficulty = 2 };
    else if (IsKeyPressed(KEY_F4)) debug = { .type = EncounterType::Boss,   .name = "Debug Boss",   .character = "troll",  .difficulty = 3 };
    else if (IsKeyPressed(KEY_F5))
    {
        debug = { .type = EncounterType::Npc, .name = "Type Louche", .character = "orc", .difficulty = 1 };
        debug.heal = 4;
        debug.zelCostPercent = 30;
        debug.dialogue = { "Pssst... par ici.", "4 PV contre 30% de ta bourse.", "Marche conclu, hehe..." };
    }
    else return;

    _enemy.Setup(GetCharacterDef(debug.character), _assets);
    _enemy.position = { Config::EnemyX, Config::GroundY };
    _enemy.faceLeft = true;

    StartEncounter(debug, true);
}

void Game::HandleCombatEvent(CombatEvent event)
{
    switch (event)
    {
    case CombatEvent::HeroAttack:
        _hero.PlayOnce("attack", "idle");
        break;
    case CombatEvent::HeroParry:
        _hero.PlayOnce("attack", "idle");
        break;
    case CombatEvent::HeroHurt:
        _hero.PlayOnce("hurt", "idle");
        break;
    case CombatEvent::HeroDodge:
        _hero.PlayOnce("jump", "idle");
        break;
    case CombatEvent::EnemyAttack:
        _enemy.PlayOnce("attack", "idle");
        break;
    case CombatEvent::EnemyHurt:
        _enemy.PlayOnce("hurt", "idle");
        break;
    case CombatEvent::EnemyFlee:
        _enemy.faceLeft = false;
        _enemy.Play("run");
        break;
    }
}

void Game::FinishEncounter(bool won)
{
    _lastEncounterWon = won;

    if (won)
    {
        _player.zel += _currentEncounter.zelReward;
        _enemy.Kill();
        _resultText = TextFormat("Vous avez vaincu %s, +%i Zel !", _currentEncounter.name.c_str(), _currentEncounter.zelReward);
        _resultColor = GOLD;
    }
    else
    {
        if (_player.IsDead())
        {
            _hero.Kill();
            ChangeState(GameState::GameOver);
            return;
        }

        if (_currentEncounter.type == EncounterType::Mash)
        {
            HandleCombatEvent(CombatEvent::EnemyFlee);
            _enemyFleeing = true;
            _resultText = TextFormat("%s s'est enfui...", _currentEncounter.name.c_str());
            _resultColor = LIGHTGRAY;
        }
        else
        {
            _resultText = TextFormat("Vous avez perdu contre %s !", _currentEncounter.name.c_str());
            _resultColor = ORANGE;
        }
    }

    ChangeState(GameState::Result);
}

void Game::UpdateResult(float dt)
{
    if (_stateTime < Config::ResultDuration) return;
    if (_lastEncounterWon && !_enemy.IsDeathFinished()) return;
    if (_enemyFleeing && _enemy.position.x < Config::ScreenWidth + 200.0f) return;

    if (_isDebugEncounter)
    {
        _isDebugEncounter = false;
        PrepareNextEnemy();
        ChangeState(GameState::Walking);
        return;
    }

    GoToNextEncounter();
}

void Game::GoToNextEncounter()
{
    ++_encounterIndex;
    if (_encounterIndex >= (int)_adventure.size())
    {
        ChangeState(GameState::Victory);
        return;
    }
    PrepareNextEnemy();
    ChangeState(GameState::Walking);
}

void Game::UpdateEndScreen()
{
    if (_stateTime > Config::EndScreenInputDelay && IsKeyPressed(KEY_SPACE)) ChangeState(GameState::Title);
}

void Game::ResetRun()
{
    _player = Player{};
    _lastHp = _player.hp;
    _shakeTimer = 0.0f;
    _newRecord = false;

    _adventure = CreateAdventure();
    _encounterIndex = 0;
    _scroll = 0.0f;
    _hero.Setup(GetCharacterDef("hero"), _assets);
    _hero.position = { Config::HeroX, Config::GroundY };
    _hero.faceLeft = false;
    PrepareNextEnemy();
}

void Game::PrepareNextEnemy()
{
    if (_encounterIndex >= (int)_adventure.size()) return;
    
    const Encounter& encounter = _adventure[_encounterIndex];

    _enemy.Setup(GetCharacterDef(encounter.character), _assets);
    _enemy.faceLeft = true;
    _enemy.Play("run");
    _enemyFleeing = false;
    
    _nextEncounterX = _scroll + Config::EnemyX + Config::EncounterSpacing;
    
    _enemy.position = { _nextEncounterX - _scroll, Config::GroundY };
}

void Game::StartEncounter(const Encounter& encounter, bool isDebug)
{
    _currentEncounter = encounter;
    _isDebugEncounter = isDebug;
    _enemyFleeing = false;

    _enemy.Play("idle");
    _hero.Play("idle");
    _encounterEndTimer = 0.0f;

    if (encounter.type == EncounterType::Npc)
    {
        _miniGame.reset();
        _dialoguePage = 0;
        _pageTime = 0.0f;
        ChangeState(GameState::Dialogue);
        return;
    }

    _miniGame = CreateMiniGame(encounter.type);
    _miniGame->Start(_player, encounter.difficulty);
    _encounterEndTimer = 0.0f;

    ChangeState(GameState::Encounter);
}

void Game::UpdateDialogue(float dt)
{
    const std::vector<std::string>& pages = _currentEncounter.dialogue;
    if (_dialoguePage >= (int)pages.size())
    {
        FinishDialogue();
        return;
    }

    _pageTime += dt;

    const int total = (int)pages[_dialoguePage].size();
    const float fullPageTime = total / DialogueSpeed;

    if (!IsKeyPressed(KEY_SPACE)) return;

    if (_pageTime < fullPageTime)
    {
        _pageTime = fullPageTime;
        return;
    }

    ++_dialoguePage;
    _pageTime = 0.0f;
    if (_dialoguePage >= (int)pages.size()) FinishDialogue();
}

void Game::FinishDialogue()
{
    const int cost = _player.zel * _currentEncounter.zelCostPercent / 100;

    if (_currentEncounter.heal > 0) _player.Heal(_currentEncounter.heal);
    if (cost > 0) _player.zel = std::max(0, _player.zel - cost);

    _lastEncounterWon = false;
    _enemyFleeing = false;

    if (_currentEncounter.heal > 0 && cost > 0)
    {
        _resultText = TextFormat("+%i PV  /  -%i Zel", _currentEncounter.heal, cost);
        _resultColor = SKYBLUE;
    }
    else if (_currentEncounter.heal > 0)
    {
        _resultText = TextFormat("%s vous soigne : +%i PV", _currentEncounter.name.c_str(), _currentEncounter.heal);
        _resultColor = GREEN;
    }
    else if (cost > 0)
    {
        _resultText = TextFormat("%s vous vole %i Zel !", _currentEncounter.name.c_str(), cost);
        _resultColor = ORANGE;
    }
    else
    {
        _resultText = TextFormat("%s repart les mains vides...", _currentEncounter.name.c_str());
        _resultColor = LIGHTGRAY;
    }

    ChangeState(GameState::Result);
}

void Game::DrawDialogue() const
{
    const std::vector<std::string>& pages = _currentEncounter.dialogue;
    if (_dialoguePage >= (int)pages.size()) return;

    DrawRectangleRounded(DialogueBox, 0.12f, 8, Fade(BLACK, 0.85f));
    DrawRectangleRoundedLinesEx(DialogueBox, 0.12f, 8, 3.0f, RAYWHITE);

    Ui::DrawTextAt(_currentEncounter.name.c_str(), { DialogueBox.x + 30.0f, DialogueBox.y + 14.0f }, 20.0f, GOLD);

    const std::string& page = pages[_dialoguePage];
    const int shown = std::min((int)(_pageTime * DialogueSpeed), (int)page.size());
    const std::string visible = page.substr(0, shown);
    Ui::DrawTextAt(visible.c_str(), { DialogueBox.x + 30.0f, DialogueBox.y + 52.0f }, 20.0f, RAYWHITE);

    if (shown >= (int)page.size())
    {
        const float pulse = 0.5f + 0.5f * std::sin((float)GetTime() * 5.0f);
        Ui::DrawTextAt("[ESPACE]", { DialogueBox.x + DialogueBox.width - 170.0f, DialogueBox.y + 90.0f },
                       18.0f, Fade(WHITE, pulse));
    }

    Ui::DrawTextCentered(TextFormat("%i / %i", _dialoguePage + 1, (int)pages.size()),
                         DialogueBox.x + DialogueBox.width - 60.0f, DialogueBox.y + 16.0f, 16.0f, LIGHTGRAY);
}

void Game::Draw() const
{
    ClearBackground(DARKBLUE);
    Camera2D camera{};
    camera.zoom = 1.0f;
    if (_shakeTimer > 0.0f)
    {
        const int strength = (int)(12.0f * _shakeTimer / Config::ShakeDuration);
        camera.offset = { (float)GetRandomValue(-strength, strength), (float)GetRandomValue(-strength, strength) };
    }

    BeginMode2D(camera);

    _background.DrawBack(_scroll);
    DrawCharacters();
    _background.DrawFront(_scroll);

    switch (_state)
    {
        case GameState::Title: DrawTitle(); break;
        case GameState::Walking: DrawHud(); break;
        case GameState::Encounter:
            DrawRectangle(0, 0, Config::ScreenWidth, Config::ScreenHeight, Fade(BLACK, 0.45f));
            if (_miniGame != nullptr) _miniGame->Draw();
            DrawHud();
            break;
        case GameState::Dialogue:
            DrawHud();
            DrawDialogue();
            break;
        case GameState::Result: DrawResult(); break;
        case GameState::Victory: DrawEndScreen(); break;
        case GameState::GameOver: DrawEndScreen(); break;
        default: DrawTitle(); break;
    }

    EndMode2D();
    if (_shakeTimer > 0.0f)
    {
        DrawRectangle(0, 0, Config::ScreenWidth, Config::ScreenHeight,
                      Fade(RED, 0.3f * _shakeTimer / Config::ShakeDuration));
    }
}

void Game::DrawCharacters() const
{
    _enemy.Draw();
    _hero.Draw();
}

void Game::DrawTitle() const
{
    DrawRectangle(0, 0, Config::ScreenWidth, Config::ScreenHeight, Fade(BLACK, 0.5f)); //assombri

    const char* title = "Brave Spiral";
    const int titleSize = 60;
    Ui::DrawTextCentered(title, Config::ScreenWidth / 2, 180, titleSize, GOLD);
    
    const float pulse = 0.5f + 0.5f * std::sin((float)GetTime() * 4.0f);
    const char* hint = "ESPACE pour commencer";
    const int hintSize = 24;
    Ui::DrawTextCentered(hint, Config::ScreenWidth / 2, 280, hintSize, Fade(WHITE, pulse));

    // Meilleur score, charge depuis save.txt au demarrage
    if (_bestZel > 0)
    {
        Ui::DrawTextCentered(TextFormat("Meilleur score : %i Zel", _bestZel),
                             Config::ScreenWidth / 2.0f, 350.0f, 20.0f, GOLD);
    }
}

void Game::DrawHud() const
{
    for (int i = 0; i < _player.maxHp; i++)
    {
        const Color color = i < _player.hp ? RED : Fade(BLACK, 0.5f);
        DrawRectangleRounded({ 20.0f + i * 30.0f, 20.0f, 24.0f, 24.0f }, 0.4f, 4, color);
    }

    Ui::DrawTextAt(TextFormat("Zel : %i", _player.zel), { 20.0f, 54.0f }, 22.0f, GOLD);

    if (!_adventure.empty() && !_isDebugEncounter)
    {
        const int shown = std::min(_encounterIndex + 1, (int)_adventure.size());
        Ui::DrawTextCentered(TextFormat("Rencontre %i / %i", shown, (int)_adventure.size()),
                             Config::ScreenWidth - 160.0f, 20.0f, 20.0f, RAYWHITE);
    }

    DrawText(TextFormat("%s | %s | scroll %.0f", StateName(_state), _currentEncounter.name.c_str(), _scroll),
             10, Config::ScreenHeight - 26, 16, Fade(WHITE, 0.4f));
    DrawFPS(Config::ScreenWidth - 90, Config::ScreenHeight - 26);
}

void Game::DrawResult() const
{
    DrawHud();

    DrawRectangle(0, 186, Config::ScreenWidth, 56, Fade(BLACK, 0.6f));
    Ui::DrawTextCentered(_resultText.c_str(), Config::ScreenWidth / 2.0f, 200.0f, 30.0f, _resultColor);
}

void Game::DrawEndScreen() const
{
    const bool victory = _state == GameState::Victory;

    DrawRectangle(0, 0, Config::ScreenWidth, Config::ScreenHeight, Fade(BLACK, 0.7f));
    Ui::DrawTextCentered(victory ? "VICTOIRE !" : "GAME OVER", Config::ScreenWidth / 2.0f, 180.0f, 64.0f,
                         victory ? GOLD : RED);
    Ui::DrawTextCentered(victory ? "Grand Gaia est sauvee !" : "L'invocateur est tombe...",
                         Config::ScreenWidth / 2.0f, 290.0f, 24.0f, RAYWHITE);
    Ui::DrawTextCentered(TextFormat("Zel recoltes : %i", _player.zel), Config::ScreenWidth / 2.0f, 350.0f, 24.0f, GOLD);

    if (_newRecord)
    {
        const float recordPulse = 0.5f + 0.5f * std::sin((float)GetTime() * 8.0f);
        Ui::DrawTextCentered("NOUVEAU RECORD !", Config::ScreenWidth / 2.0f, 400.0f, 28.0f, Fade(GOLD, recordPulse));
    }
    else if (_bestZel > 0)
    {
        Ui::DrawTextCentered(TextFormat("Record : %i Zel", _bestZel), Config::ScreenWidth / 2.0f, 400.0f, 20.0f, LIGHTGRAY);
    }
    
    if (_stateTime > Config::EndScreenInputDelay)
    {
        const float pulse = 0.5f + 0.5f * std::sin((float)GetTime() * 4.0f);
        Ui::DrawTextCentered("ESPACE pour revenir au titre", Config::ScreenWidth / 2.0f, 460.0f, 22.0f, Fade(WHITE, pulse));
    }
}

void Game::Unload()
{
    _assets.UnloadAll();
}
