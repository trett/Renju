#include "player.h"

Player::Player(int clr) : color(clr)
{
}

void Player::move() {
    if (!canMove) {
        return;
    }
    canMove = false;
}
