#include "WelcomeWindow.hpp"

#include "GameWindow.h"
#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    WelcomeWindow welcome;

    if (welcome.run()) {
        std::string name = welcome.getPlayerName();
        std::cout << "Player: " << name << "\n";

        int rows = 16;
        int cols = 25;
        GameWindow game(rows, cols);
        game.run();
    }

    return 0;
}
