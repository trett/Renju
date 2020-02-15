#ifndef IPLAYER_H
#define IPLAYER_H

#include "dot.h"
#include "gameboard.h"
#include "renju.h"

#include <QObject>

class IPlayer : public QObject {
    Q_OBJECT
public:
    IPlayer(GameBoard *parent = nullptr)
    {
        Q_UNUSED(parent)
        m_canMove = false;
    }
    bool m_canMove;
    Renju::DOT_COLOR m_color;
    Dot m_nextMove;

signals:
    void move(const Dot *dot);
};

#endif // IPLAYER_H
