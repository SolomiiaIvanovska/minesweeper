#include "Tile.h"

Tile::Tile(sf::Texture& hiddenTex, sf::Vector2f pos)
    : sprite(hiddenTex)
{
    sprite.setPosition(pos);
}

void Tile::reveal(sf::Texture& revealedTex,
                  sf::Texture* numberTextures[8],
                  sf::Texture& mineTex) {
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

void Tile::revealMine(sf::Texture& mineTex) {
    isRevealed = true;
    sprite.setTexture(mineTex);
}

int Tile::toggleFlag(sf::Texture& flagTex, sf::Texture& hiddenTex) {
    if (isRevealed) return 0;
    isFlagged = !isFlagged;
    sprite.setTexture(isFlagged ? flagTex : hiddenTex);
    return isFlagged ? 1 : -1;
}

void Tile::showAsTempRevealed(sf::Texture& revealedTex) {
    // Do NOT change isRevealed; just change appearance
    sprite.setTexture(revealedTex);
}

void Tile::refreshSprite(sf::Texture& hiddenTex,
                         sf::Texture& revealedTex,
                         sf::Texture* numberTextures[8],
                         sf::Texture& mineTex,
                         sf::Texture& flagTex) {
    if (isFlagged) {
        sprite.setTexture(flagTex);
    } else if (!isRevealed) {
        sprite.setTexture(hiddenTex);
    } else if (hasMine) {
        sprite.setTexture(mineTex);
    } else if (adjacentMines > 0) {
        sprite.setTexture(*numberTextures[adjacentMines - 1]);
    } else {
        sprite.setTexture(revealedTex);
    }
}

void Tile::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

