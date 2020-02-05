#ifndef PLAYER_H
#define PLAYER_H

#include "i_player.h"

#include <QObject>

class HumanPlayer : public IPlayer
{
public:
    HumanPlayer(GameBoard *parent, const QVector<QVector<int>> *table);
private:
    GameBoard *m_gameBoard;
public slots:
    void getNextMove(const Dot *dot);
};

#endif // PLAYER_H
