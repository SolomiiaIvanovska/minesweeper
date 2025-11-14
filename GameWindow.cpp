"GameWindow.h"
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

GameWindow::GameWindow(const std::string& playerName)
    : rows(16)
    , cols(25)
    , totalMines(50)
    , playerName(playerName)
    , uiFont()
    , mineCounterText(uiFont, "", 24)     // SFML 3: needs Font in ctor
    , leaderboardButtonText(uiFont, "", 16)
    , placeholderTexture()
    , faceSprite(placeholderTexture)
    , pauseButton(placeholderTexture)
{
    // 1️⃣ Read config BEFORE anything (overwrites rows/cols/mines if file ok)
    readConfig();

    // 2️⃣ Load textures
    loadTextures();

    // 3️⃣ Create window using updated rows/cols
    window.create(
        sf::VideoMode(
            { static_cast<unsigned>(cols * 32),
              static_cast<unsigned>(rows * 32 + static_cast<unsigned>(topOffset)) }),
        "Minesweeper Game",
        sf::Style::Close
    );

    // 4️⃣ Load font
    if (!uiFont.openFromFile("assets/arial.ttf")) {
        std::cerr << "Error loading arial.ttf for GameWindow!\n";
    }

    // 5️⃣ Mine counter text
    mineCounterText.setFont(uiFont);
    mineCounterText.setCharacterSize(24);
    mineCounterText.setFillColor(sf::Color::Red);
    mineCounterText.setPosition(sf::Vector2f(10.f, 10.f));

    // 6️⃣ Leaderboard button (rectangle + text)
    leaderboardButtonRect.setSize(sf::Vector2f(150.f, 30.f));
    leaderboardButtonRect.setFillColor(sf::Color(200, 200, 200));
    leaderboardButtonRect.setOutlineColor(sf::Color::Black);
    leaderboardButtonRect.setOutlineThickness(2.f);
    leaderboardButtonRect.setPosition(sf::Vector2f(10.f, 40.f));

    leaderboardButtonText.setFont(uiFont);
    leaderboardButtonText.setCharacterSize(16);
    leaderboardButtonText.setFillColor(sf::Color::Black);
    leaderboardButtonText.setString("Leaderboard");

    {
        sf::FloatRect lb = leaderboardButtonText.getLocalBounds();
        leaderboardButtonText.setOrigin(sf::Vector2f(
            lb.size.x / 2.f,
            lb.size.y / 2.f
        ));
        leaderboardButtonText.setPosition(sf::Vector2f(
            leaderboardButtonRect.getPosition().x + leaderboardButtonRect.getSize().x / 2.f,
            leaderboardButtonRect.getPosition().y + leaderboardButtonRect.getSize().y / 2.f - 4.f
        ));
    }

    // 7️⃣ Face sprite
    faceSprite.setTexture(faceHappyTex, true);
    faceSprite.setScale(sf::Vector2f(1.5f, 1.5f));
    {
        sf::FloatRect faceBounds = faceSprite.getLocalBounds();
        faceSprite.setOrigin(sf::Vector2f(
            faceBounds.size.x / 2.f,
            faceBounds.size.y / 2.f
        ));
        faceSprite.setPosition(sf::Vector2f(
            static_cast<float>(window.getSize().x) / 2.f,
            topOffset / 2.f
        ));
    }

    // 8️⃣ Pause button (top right)
    pauseButton.setTexture(pauseTex, true);
    pauseButton.setScale(sf::Vector2f(1.5f, 1.5f));
    pauseButton.setPosition(sf::Vector2f(
        static_cast<float>(window.getSize().x) - 110.f,
        2.f
    ));

    // 9️⃣ Board + mines
    createBoard();
    placeMines();
    calculateAdjacency();

    // 🔟 Timer + state
    elapsedSeconds = 0;
    finalTime      = 0;
    paused         = false;
    state          = GameState::Playing;
    flagsPlaced    = 0;

    gameClock.restart();

    loadLeaderboard();
    updateInfoText();
}

// ---------------------------------------------------------
// Config: read config.cfg (format: COLUMNS,ROWS,MINES)
// Example: 25,16,50
// ---------------------------------------------------------
void GameWindow::readConfig() {
    std::ifstream file("config.cfg");
    if (!file.is_open()) {
        std::cout << "Could not open config.cfg. Using defaults: 25x16, 50 mines\n";
        rows = 16;
        cols = 25;
        totalMines = 50;
        return;
    }

    int c, r, m;
    if (!(file >> c >> r >> m)) {
        std::cout << "Error reading config.cfg. Using defaults: 25x16, 50 mines\n";
        rows = 16;
        cols = 25;
        totalMines = 50;
        return;
    }

    // Config format: COLS ROWS MINES
    cols = c;
    rows = r;
    totalMines = m;

    std::cout << "Config Loaded: "
              << cols << "x" << rows
              << " Mines: " << totalMines << "\n";
}


// ---------------------------------------------------------
// Textures
// ---------------------------------------------------------
void GameWindow::loadTextures() {
    if (!tileHidden.loadFromFile("assets/tile_hidden.png") ||
        !tileRevealed.loadFromFile("assets/tile_revealed.png") ||
        !flagTex.loadFromFile("assets/flag.png") ||
        !mineTex.loadFromFile("assets/mine.png")) {
        std::cerr << "Error loading tile textures!\n";
    }

    for (int i = 0; i < 8; ++i) {
        std::string filename = "assets/number_" + std::to_string(i + 1) + ".png";
        if (!numberTextures[i].loadFromFile(filename))
            std::cerr << "Error loading " << filename << "\n";
        numberPtrs[i] = &numberTextures[i];
    }

    if (!faceHappyTex.loadFromFile("assets/face_happy.png"))
        std::cerr << "Error loading face_happy.png\n";
    if (!faceWinTex.loadFromFile("assets/face_win.png"))
        std::cerr << "Error loading face_win.png\n";
    if (!faceLoseTex.loadFromFile("assets/face_lose.png"))
        std::cerr << "Error loading face_lose.png\n";

    if (!pauseTex.loadFromFile("assets/pause.png"))
        std::cerr << "Error loading pause.png\n";
    if (!playTex.loadFromFile("assets/play.png"))
        std::cerr << "Error loading play.png\n";
}

// ---------------------------------------------------------
// Board + mines
// ---------------------------------------------------------
void GameWindow::createBoard() {
    board.clear();
    board.resize(rows);
    for (int r = 0; r < rows; ++r) {
        board[r].reserve(cols);
        for (int c = 0; c < cols; ++c) {
            float x = static_cast<float>(c * 32);
            float y = topOffset + static_cast<float>(r * 32);
            board[r].emplace_back(tileHidden, sf::Vector2f(x, y));
        }
    }
}

void GameWindow::placeMines() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    int placed = 0;
    while (placed < totalMines) {
        int r = std::rand() % rows;
        int c = std::rand() % cols;
        if (!board[r][c].getMine()) {
            board[r][c].setMine(true);
            placed++;
        }
    }
}

void GameWindow::calculateAdjacency() {
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            if (board[r][c].getMine()) continue;
            int count = 0;
            for (int dr = -1; dr <= 1; ++dr) {
                for (int dc = -1; dc <= 1; ++dc) {
                    if (dr == 0 && dc == 0) continue;
                    int nr = r + dr;
                    int nc = c + dc;
                    if (nr >= 0 && nr < rows &&
                        nc >= 0 && nc < cols &&
                        board[nr][nc].getMine()) {
                        count++;
                    }
                }
            }
            board[r][c].setAdjacentMines(count);
        }
    }
}

// ---------------------------------------------------------
// Reveal logic
// ---------------------------------------------------------
void GameWindow::revealTile(int r, int c) {
    if (r < 0 || r >= rows || c < 0 || c >= cols) return;

    Tile& tile = board[r][c];

    if (tile.getRevealed() || tile.getFlagged()) return;

    tile.reveal(tileRevealed, numberPtrs, mineTex);

    if (!tile.getMine() && tile.getAdjacentMines() == 0) {
        for (int dr = -1; dr <= 1; ++dr) {
            for (int dc = -1; dc <= 1; ++dc) {
                if (dr == 0 && dc == 0) continue;
                revealTile(r + dr, c + dc);
            }
        }
    }
}

void GameWindow::revealAllMines() {
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            if (board[r][c].getMine()) {
                board[r][c].revealMine(mineTex);
            }
        }
    }
}

int GameWindow::countRevealedSafeTiles() const {
    int count = 0;
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            if (board[r][c].getRevealed() && !board[r][c].getMine()) {
                count++;
            }
        }
    }
    return count;
}

void GameWindow::autoFlagAllMines() {
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            Tile& tile = board[r][c];
            if (tile.getMine() && !tile.getFlagged()) {
                int delta = tile.toggleFlag(flagTex, tileHidden);
                flagsPlaced += delta;
            }
        }
    }
    updateInfoText();
}

// ---------------------------------------------------------
// Timer / UI helpers
// ---------------------------------------------------------
int GameWindow::getCurrentTimeSeconds() const {
    if (state == GameState::Won || state == GameState::Lost) {
        return finalTime;
    }
    if (paused) {
        return elapsedSeconds;
    }
    return elapsedSeconds + static_cast<int>(gameClock.getElapsedTime().asSeconds());
}

void GameWindow::updateInfoText() {
    int remaining = totalMines - flagsPlaced;
    int timeSec   = getCurrentTimeSeconds();
    mineCounterText.setString(
        "Mines: " + std::to_string(remaining) +
        "   Time: " + std::to_string(timeSec)
    );
}

// ---------------------------------------------------------
// Victory / reset
// ---------------------------------------------------------
void GameWindow::checkVictory() {
    int safeTiles = rows * cols - totalMines;
    if (state == GameState::Playing &&
        countRevealedSafeTiles() == safeTiles) {

        finalTime = getCurrentTimeSeconds();
        state     = GameState::Won;
        paused    = false;
        faceSprite.setTexture(faceWinTex, true);
        autoFlagAllMines();

        updateLeaderboardOnWin();
        updateInfoText();
    }
}

void GameWindow::reset() {
    state          = GameState::Playing;
    flagsPlaced    = 0;
    paused         = false;
    elapsedSeconds = 0;
    finalTime      = 0;
    gameClock.restart();

    createBoard();
    placeMines();
    calculateAdjacency();

    faceSprite.setTexture(faceHappyTex, true);
    updateInfoText();
}

// ---------------------------------------------------------
// Tile-show helpers for pause / leaderboard
// (these require Tile to have showAsTempRevealed / refreshSprite)
// ---------------------------------------------------------
void GameWindow::showAllTilesAsRevealed() {
    for (int r = 0; r < rows; ++r)
        for (int c = 0; c < cols; ++c)
            board[r][c].showAsTempRevealed(tileRevealed);
}

void GameWindow::restoreAllTileSprites() {
    for (int r = 0; r < rows; ++r)
        for (int c = 0; c < cols; ++c)
            board[r][c].refreshSprite(
                tileHidden, tileRevealed, numberPtrs, mineTex, flagTex
            );
}

// ---------------------------------------------------------
// Leaderboard file I/O
// ---------------------------------------------------------
void GameWindow::loadLeaderboard() {
    leaderboard.clear();
    std::ifstream file("leaderboard.txt");
    if (!file.is_open()) return;

    int time;
    std::string name;
    while (file >> time >> name) {
        leaderboard.emplace_back(time, name);
        if (leaderboard.size() == 5) break;
    }

    std::sort(leaderboard.begin(), leaderboard.end(),
              [](const auto& a, const auto& b) { return a.first < b.first; });
}

void GameWindow::saveLeaderboard() {
    std::ofstream file("leaderboard.txt");
    if (!file.is_open()) return;

    int count = 0;
    for (const auto& entry : leaderboard) {
        file << entry.first << " " << entry.second << "\n";
        if (++count == 5) break;
    }
}

void GameWindow::updateLeaderboardOnWin() {
    int timeSec   = finalTime;
    lastWinTime   = timeSec;
    lastWinName   = playerName;

    leaderboard.emplace_back(timeSec, playerName);
    std::sort(leaderboard.begin(), leaderboard.end(),
              [](const auto& a, const auto& b) { return a.first < b.first; });
    if (leaderboard.size() > 5) {
        leaderboard.resize(5);
    }

    saveLeaderboard();
}

// ---------------------------------------------------------
// Leaderboard popup window
// ---------------------------------------------------------
void GameWindow::showLeaderboardWindow() {
    // Pause game & show all tiles revealed
    bool wasPaused   = paused;
    int  timeBefore  = getCurrentTimeSeconds();
    paused           = true;
    elapsedSeconds   = timeBefore; // freeze
    showAllTilesAsRevealed();
    updateInfoText();

    sf::RenderWindow lbWindow(
        sf::VideoMode({400u, 300u}),
        "Leaderboard",
        sf::Style::Titlebar | sf::Style::Close
    );

    while (lbWindow.isOpen()) {
        while (auto e = lbWindow.pollEvent()) {
            if (e->is<sf::Event::Closed>()) {
                lbWindow.close();
            }
            if (auto key = e->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::Escape) {
                    lbWindow.close();
                }
            }
        }

        lbWindow.clear(sf::Color(30, 30, 30));

        sf::Text title(uiFont, "Top 5 Leaders", 22);
        title.setFillColor(sf::Color::Yellow);
        title.setPosition(sf::Vector2f(110.f, 10.f));
        lbWindow.draw(title);

        for (std::size_t i = 0; i < leaderboard.size() && i < 5; ++i) {
            const auto& entry = leaderboard[i];
            sf::Text t(uiFont, "", 18);
            t.setFillColor(sf::Color::White);

            bool mark = (entry.first == lastWinTime &&
                         entry.second == lastWinName);

            std::string line = std::to_string(i + 1) + ". " +
                               entry.second + " - " +
                               std::to_string(entry.first) + "s";
            if (mark) line += " *";

            t.setString(line);
            t.setPosition(sf::Vector2f(40.f, 60.f + static_cast<float>(i) * 30.f));
            lbWindow.draw(t);
        }

        lbWindow.display();
    }

    // Restore tiles to prior textures
    restoreAllTileSprites();

    // Restore pause state
    paused = wasPaused;
    if (!paused && state == GameState::Playing) {
        gameClock.restart();
    }
    updateInfoText();
}

// ---------------------------------------------------------
// Main loop
// ---------------------------------------------------------
void GameWindow::run() {
    while (window.isOpen()) {
        while (auto e = window.pollEvent()) {
            if (e->is<sf::Event::Closed>()) {
                window.close();
            }

            if (auto m = e->getIf<sf::Event::MouseButtonPressed>()) {
                int mx = m->position.x;
                int my = m->position.y;
                sf::Vector2f mousePoint(static_cast<float>(mx), static_cast<float>(my));

                if (m->button == sf::Mouse::Button::Left) {

                    // Face click: reset
                    if (faceSprite.getGlobalBounds().contains(mousePoint)) {
                        reset();
                        continue;
                    }

                    // Pause / play button
                    if (pauseButton.getGlobalBounds().contains(mousePoint)) {
                        if (state == GameState::Playing) {
                            if (!paused) {
                                // going into pause
                                elapsedSeconds = getCurrentTimeSeconds();
                                paused = true;
                                pauseButton.setTexture(playTex, true);
                                showAllTilesAsRevealed();
                            } else {
                                // leaving pause
                                paused = false;
                                pauseButton.setTexture(pauseTex, true);
                                gameClock.restart();
                                restoreAllTileSprites();
                            }
                            updateInfoText();
                        }
                        continue;
                    }

                    // Leaderboard button
                    if (leaderboardButtonRect.getGlobalBounds().contains(mousePoint)) {
                        showLeaderboardWindow();
                        continue;
                    }

                    // If paused or not playing, ignore clicks on board
                    if (paused || state != GameState::Playing) {
                        continue;
                    }

                    if (my >= static_cast<int>(topOffset)) {
                        int c = mx / 32;
                        int r = (my - static_cast<int>(topOffset)) / 32;
                        if (c >= 0 && c < cols && r >= 0 && r < rows) {
                            Tile& tile = board[r][c];

                            if (tile.getFlagged() || tile.getRevealed()) {
                                // Do nothing
                            } else if (tile.getMine()) {
                                finalTime = getCurrentTimeSeconds();
                                state     = GameState::Lost;
                                paused    = false;
                                faceSprite.setTexture(faceLoseTex, true);
                                revealAllMines();
                                updateInfoText();
                            } else {
                                revealTile(r, c);
                                checkVictory();
                                updateInfoText();
                            }
                        }
                    }
                }
                else if (m->button == sf::Mouse::Button::Right) {
                    if (paused || state != GameState::Playing) {
                        continue;
                    }
                    if (my >= static_cast<int>(topOffset)) {
                        int c = mx / 32;
                        int r = (my - static_cast<int>(topOffset)) / 32;
                        if (c >= 0 && c < cols && r >= 0 && r < rows) {
                            int delta = board[r][c].toggleFlag(flagTex, tileHidden);
                            if (delta != 0) {
                                flagsPlaced += delta;
                                updateInfoText();
                            }
                        }
                    }
                }
            }
        }

        // Update timer text each frame if game is running and not paused
        if (!paused && state == GameState::Playing) {
            updateInfoText();
        }

        window.clear(sf::Color(180, 180, 180));

        // Draw board
        for (auto& row : board) {
            for (auto& tile : row) {
                tile.draw(window);
            }
        }

        // Draw UI
        window.draw(faceSprite);
        window.draw(pauseButton);
        window.draw(leaderboardButtonRect);
        window.draw(leaderboardButtonText);
        window.draw(mineCounterText);

        window.display();
    }
}
