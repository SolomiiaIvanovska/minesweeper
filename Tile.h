#pragma once
#include <SFML/Graphics.hpp>

class Tile {
private:
    sf::Sprite sprite;
    bool hasMine = false;
    bool isRevealed = false;
    bool isFlagged = false;
    int adjacentMines = 0;

public:
    Tile(sf::Texture& hiddenTex, sf::Vector2f pos);

    void setMine(bool mine) { hasMine = mine; }
    bool getMine() const { return hasMine; }

    void setAdjacentMines(int count) { adjacentMines = count; }
    int getAdjacentMines() const { return adjacentMines; }

    bool getRevealed() const { return isRevealed; }
    bool getFlagged() const { return isFlagged; }

    // Normal reveal (numbers / empty / mine)
    void reveal(sf::Texture& revealedTex,
                sf::Texture* numberTextures[8],
                sf::Texture& mineTex);

    // Used when the player loses to show all mines
    void revealMine(sf::Texture& mineTex);

    // Toggle flag; returns +1 if a flag was added, -1 if removed, 0 if no change
    int toggleFlag(sf::Texture& flagTex, sf::Texture& hiddenTex);

    // For pause/leaderboard: temporarily show as revealed gray tile
    void showAsTempRevealed(sf::Texture& revealedTex);

    // Restore sprite texture based on current state flags
    void refreshSprite(sf::Texture& hiddenTex,
                       sf::Texture& revealedTex,
                       sf::Texture* numberTextures[8],
                       sf::Texture& mineTex,
                       sf::Texture& flagTex);

    void draw(sf::RenderWindow& window);
};
