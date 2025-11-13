#include "GameWindow.h"
#include <ctime>
#include <cstdlib>
#include <iostream>

GameWindow::GameWindow(int rows, int cols) : rows(rows), cols(cols) {
    loadTextures();
    createBoard();
    placeMines(50);
    calculateAdjacency();

    window.create(sf::VideoMode({static_cast<unsigned>(cols * 32),
                                 static_cast<unsigned>(rows * 32)}),
                                 "Minesweeper Game", sf::Style::Close);
}

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
}

void GameWindow::createBoard() {
    board.resize(rows, std::vector<Tile>(cols, Tile(tileHidden, {0, 0})));
    for (int r = 0; r < rows; ++r)
        for (int c = 0; c < cols; ++c)
            board[r][c] = Tile(tileHidden, {float(c * 32), float(r * 32)});
}

void GameWindow::placeMines(int mineCount) {
    srand(static_cast<unsigned>(time(nullptr)));
    int placed = 0;
    while (placed < mineCount) {
        int r = rand() % rows;
        int c = rand() % cols;
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
            for (int dr = -1; dr <= 1; ++dr)
                for (int dc = -1; dc <= 1; ++dc) {
                    int nr = r + dr, nc = c + dc;
                    if (nr >= 0 && nr < rows && nc >= 0 && nc < cols &&
                        board[nr][nc].getMine()) count++;
                }
            board[r][c].setAdjacentMines(count);
        }
    }
}

void GameWindow::run() {
    while (window.isOpen()) {
        while (auto e = window.pollEvent()) {
            if (e->is<sf::Event::Closed>()) {
                window.close();
            }

            if (auto m = e->getIf<sf::Event::MouseButtonPressed>()) {
                int x = m->position.x / 32;
                int y = m->position.y / 32;
                if (x >= 0 && x < cols && y >= 0 && y < rows) {
                    if (m->button == sf::Mouse::Button::Left)
                        board[y][x].reveal(tileRevealed, numberPtrs, mineTex);
                    else if (m->button == sf::Mouse::Button::Right)
                        board[y][x].toggleFlag(flagTex, tileHidden);
                }
            }
        }

        window.clear(sf::Color(180, 180, 180));
        for (auto& row : board)
            for (auto& tile : row)
                tile.draw(window);
        window.display();
    }
}
