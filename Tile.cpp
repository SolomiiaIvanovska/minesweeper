#include "Tile.h"
#include <iostream>

Tile::Tile(sf::Texture& hiddenTex, sf::Vector2f pos)
    : sprite(hiddenTex)   // <-- initialize sprite with the texture here!
{
    sprite.setPosition(pos);
}


void Tile::reveal(sf::Texture& revealedTex, sf::Texture* numberTextures[8], sf::Texture& mineTex) {
    if (isFlagged || isRevealed) return;
    isRevealed = true;

    if (hasMine) {
        sprite.setTexture(mineTex);
    } else if (adjacentMines > 0) {
        sprite.setTexture(*numberTextures[adjacentMines - 1]);
    } else {
        sprite.setTexture(revealedTex);
    }
}

void Tile::toggleFlag(sf::Texture& flagTex, sf::Texture& hiddenTex) {
    if (isRevealed) return;
    isFlagged = !isFlagged;
    sprite.setTexture(isFlagged ? flagTex : hiddenTex);
}

void Tile::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

