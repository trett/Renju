#include "player.h"

Player::Player()
{
}

void Player::move() {
    if (!m_canMove) {
        return;
    }
    m_canMove = false;
}
