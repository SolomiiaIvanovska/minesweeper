#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "Tile.h"

class GameWindow {
private:
    sf::RenderWindow window;

    int rows;
    int cols;
    int totalMines;
    float topOffset = 100.f;

    std::string playerName;

    std::vector<std::vector<Tile>> board;

    // Tile textures
    sf::Texture tileHidden;
    sf::Texture tileRevealed;
    sf::Texture flagTex;
    sf::Texture mineTex;
    sf::Texture numberTextures[8];
    sf::Texture* numberPtrs[8];

    // UI
    sf::Font uiFont;
    sf::Text mineCounterText;          // Mines + time

    // Face textures + sprite
    sf::Texture faceHappyTex;
    sf::Texture faceWinTex;
    sf::Texture faceLoseTex;
    sf::Texture placeholderTexture;    // Used just to construct sprites (SFML 3)
    sf::Sprite faceSprite;

    // Timer + pause
    sf::Clock gameClock;
    int elapsedSeconds = 0;
    int finalTime = 0;
    bool paused = false;

    sf::Texture pauseTex;
    sf::Texture playTex;
    sf::Sprite pauseButton;

    // Leaderboard button
    sf::RectangleShape leaderboardButtonRect;
    sf::Text leaderboardButtonText;

    enum class GameState { Playing, Won, Lost };
    GameState state = GameState::Playing;
    int flagsPlaced = 0;

    // Leaderboard data: (time, name)
    std::vector<std::pair<int, std::string>> leaderboard;
    int         lastWinTime = -1;
    std::string lastWinName;

    // Config
    void readConfig();                  // read config.cfg to set rows/cols/mines

    // Board & game logic
    void loadTextures();
    void createBoard();
    void placeMines();
    void calculateAdjacency();

    void revealTile(int r, int c);
    void revealAllMines();
    void autoFlagAllMines();
    int  countRevealedSafeTiles() const;
    void checkVictory();

    void reset();

    // Timer / UI
    int  getCurrentTimeSeconds() const;
    void updateInfoText();

    // Pause & leaderboard tile helpers
    void showAllTilesAsRevealed();
    void restoreAllTileSprites();

    // Leaderboard helpers
    void loadLeaderboard();
    void saveLeaderboard();
    void updateLeaderboardOnWin();
    void showLeaderboardWindow();

public:
    explicit GameWindow(const std::string& playerName);

    void run();
};
