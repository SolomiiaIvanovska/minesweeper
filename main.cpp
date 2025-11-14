#include "WelcomeWindow.hpp"
#include "GameWindow.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

// Read config.cfg
// bool loadConfig(int& rows, int& cols, int& mines)
// {
//     rows = 16;
//     cols = 25;
//     mines = 50;
//
//     std::ifstream in("config.cfg");
//     if (!in.is_open()) {
//         std::cerr << "Could not open config.cfg, using defaults.\n";
//         return false;
//     }
//
//     std::stringstream buffer;
//     buffer << in.rdbuf();
//     std::string s = buffer.str();
//     for (char& ch : s) {
//         if (ch == ',' || ch == '\n' || ch == '\r' || ch == '\t')
//             ch = ' ';
//     }
//
//     std::stringstream ss(s);
//     int c, r, m;
//     if (!(ss >> c >> r >> m)) {
//         std::cerr << "Error parsing config.cfg, using defaults.\n";
//         return false;
//     }
//
//     cols = c;
//     rows = r;
//     mines = m;
//     return true;
// }

int main()
{
    int rows, cols, mines;
    // loadConfig(rows, cols, mines);

    // Show welcome window
    WelcomeWindow welcome;

    if (!welcome.run())
        return 0;

    std::string playerName = welcome.getPlayerName();
    std::cout << "Player: " << playerName << "\n";
    // std::cout << "Config => " << cols << "x" << rows << " mines: " << mines << "\n";

    // GameWindow constructor DOES NOT need rows/cols/mines
    GameWindow game(playerName);
    game.run();

    return 0;
}


