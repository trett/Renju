#ifndef PLAYER_H
#define PLAYER_H


class Player
{
public:
    Player();
    bool m_canMove = false;
    int m_color;

    void move();
};

#endif // PLAYER_H
