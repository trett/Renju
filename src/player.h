#ifndef PLAYER_H
#define PLAYER_H


class Player
{
public:
    Player(int color);
    bool canMove = false;
    int color;

    void move();
};

#endif // PLAYER_H
