#ifndef SIMPLEAI_H
#define SIMPLEAI_H

#include "dot.h"
#include "i_player.h"

#include <QVector>

class SimpleAi : public IPlayer
{
public:
    SimpleAi(GameBoard *parent, int color, QVector<QVector<int>> *table);
    void onPlayerChange();
private:
    QPair<int, QSharedPointer<Dot>> getNextMove(int color);
    QVector<QVector<int>> m_rating;
};
#endif // SIMPLEAI_H
