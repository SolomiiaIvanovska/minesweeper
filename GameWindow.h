#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Tile.h"

class GameWindow {
private:
    sf::RenderWindow window;
    int rows, cols;
    std::vector<std::vector<Tile>> board;

    sf::Texture tileHidden;
    sf::Texture tileRevealed;
    sf::Texture flagTex;
    sf::Texture mineTex;
    sf::Texture numberTextures[8];
    sf::Texture* numberPtrs[8];

    void loadTextures();
    void createBoard();
    void placeMines(int mineCount);
    void calculateAdjacency();

public:
    GameWindow(int rows = 10, int cols = 10);
    void run();
};
