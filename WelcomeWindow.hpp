#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class WelcomeWindow {
public:
    WelcomeWindow();
    bool run();
    std::string getPlayerName() const { return playerName; }

private:
    sf::RenderWindow window;
    sf::Texture bgTexture;
    sf::Sprite background;
    sf::RectangleShape inputBox;
    sf::Text inputText;
    sf::Text prompt;
    sf::Font font;
    sf::RectangleShape cursor;
    sf::Clock cursorClock;

    std::string playerName;
    bool showCursor = true;

    void handleTextInput(const sf::Event::TextEntered& ev);

    void updateCursor();
    void draw();
    void fadeOutAndClose();
};
