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

    void reveal(sf::Texture& revealedTex, sf::Texture* numberTextures[8], sf::Texture& mineTex);
    void toggleFlag(sf::Texture& flagTex, sf::Texture& hiddenTex);
    void draw(sf::RenderWindow& window);
};
