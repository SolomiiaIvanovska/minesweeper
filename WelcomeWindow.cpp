#include "WelcomeWindow.hpp"
#include <iostream>
#include <cctype>

// Helper: center text
void setText(sf::Text& text, float x, float y) {
    sf::FloatRect rect = text.getLocalBounds();
    text.setOrigin(sf::Vector2f(
        rect.position.x + rect.size.x / 2.0f,
        rect.position.y + rect.size.y / 2.0f));
    text.setPosition(sf::Vector2f(x, y));
}

WelcomeWindow::WelcomeWindow()
    : window(sf::VideoMode(sf::Vector2u(800, 600)), "Welcome")
    , background(bgTexture)
    , prompt(font)
    , inputText(font)
{
    window.setFramerateLimit(60);


    if (!bgTexture.loadFromFile("assets/background.png")) {
        std::cerr << "Error loading background.png\n";
    }

    if (!font.openFromFile("assets/arial.ttf")) {
        std::cerr << "Error loading arial.ttf\n";
    }


    prompt.setString("Please enter your name:");
    prompt.setCharacterSize(20);
    prompt.setStyle(sf::Text::Bold);
    prompt.setFillColor(sf::Color(250, 250, 255));
    setText(prompt, 400, 160 - 75);


    inputText.setString("");
    inputText.setCharacterSize(18);
    inputText.setStyle(sf::Text::Bold);
    inputText.setFillColor(sf::Color::Yellow);
    setText(inputText, 400, 300 - 45);


    inputBox.setSize(sf::Vector2f(400, 50));
    inputBox.setFillColor(sf::Color(50, 50, 50));
    inputBox.setOutlineColor(sf::Color::Blue);
    inputBox.setOutlineThickness(2);
    sf::FloatRect box = inputBox.getLocalBounds();
    inputBox.setOrigin(sf::Vector2f(
        box.position.x + box.size.x / 2.f,
        box.position.y + box.size.y / 2.f));
    inputBox.setPosition(sf::Vector2f(400, 300 - 45));


    cursor.setSize(sf::Vector2f(2, 24));
    cursor.setFillColor(sf::Color::White);
}

bool WelcomeWindow::run() {
    while (window.isOpen()) {
        while (std::optional<sf::Event> ev = window.pollEvent()) {
            if (ev->is<sf::Event::Closed>()) {
                window.close();
                return false;
            }

            if (auto txt = ev->getIf<sf::Event::TextEntered>()) {
                handleTextInput(*txt);
            }

            if (ev->is<sf::Event::KeyPressed>()) {
                if (auto key = ev->getIf<sf::Event::KeyPressed>()) {
                    if (key->code == sf::Keyboard::Key::Enter) {
                        if (!playerName.empty()) {
                            fadeOutAndClose();
                            return true;
                        } else {
                            std::cout << "Enter ignored: no name entered.\n";
                        }
                    }
                }
            }
        }
        updateCursor();
        draw();
    }
    return false;
}



void WelcomeWindow::handleTextInput(const sf::Event::TextEntered& ev) {
    if (ev.unicode == 8) {
        if (!playerName.empty()) playerName.pop_back();
    } else if (ev.unicode >= 32 && ev.unicode < 128 && playerName.length() < 10) {
        char c = static_cast<char>(ev.unicode);
        if (std::isalpha(c)) {
            if (playerName.empty())
                playerName += std::toupper(c);
            else
                playerName += std::tolower(c);
        }
    }
    inputText.setString(playerName + (showCursor ? "|" : ""));
    setText(inputText, 400, 300 - 45);
}

void WelcomeWindow::updateCursor() {
    if (cursorClock.getElapsedTime().asSeconds() > 0.5f) {
        showCursor = !showCursor;
        cursorClock.restart();
    }
    inputText.setString(playerName + (showCursor ? "|" : ""));
    setText(inputText, 400, 300 - 45);

    sf::FloatRect bounds = inputText.getLocalBounds();
    cursor.setPosition(sf::Vector2f(
        inputText.getPosition().x - bounds.size.x / 2.f + bounds.size.x + 5.f,
        inputText.getPosition().y - 12.f));
}

void WelcomeWindow::draw() {
    window.clear(sf::Color::Blue);
    window.draw(background);
    window.draw(prompt);
    window.draw(inputBox);
    window.draw(inputText);
    if (showCursor && playerName.length() < 10) window.draw(cursor);
    window.display();
}

void WelcomeWindow::fadeOutAndClose() {
    sf::RectangleShape fade(sf::Vector2f(800, 600));
    fade.setFillColor(sf::Color(0, 0, 0, 0));
    for (int a = 0; a <= 255; a += 8) {
        fade.setFillColor(sf::Color(0, 0, 0, a));
        window.clear();
        window.draw(background);
        window.draw(prompt);
        window.draw(inputBox);
        window.draw(inputText);
        window.draw(fade);
        window.display();
        sf::sleep(sf::milliseconds(16));
    }
    window.close();
}
