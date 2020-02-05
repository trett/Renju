#ifndef IPLAYER_H
#define IPLAYER_H

#include "dot.h"
#include "gameboard.h"

#include <QObject>

class IPlayer : public QObject {
    Q_OBJECT
public:
    IPlayer(GameBoard *parent = nullptr, const QVector<QVector<int>> *table = nullptr)
    {
        m_canMove = false;
        m_table = table;
    }
    bool m_canMove;
    int m_color;
    Dot m_nextMove;
    const QVector<QVector<int>> *m_table;
signals:
    void move(const Dot *dot);
};

#endif // IPLAYER_H
