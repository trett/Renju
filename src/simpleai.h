#ifndef SIMPLEAI_H
#define SIMPLEAI_H

#include "dot.h"
#include "i_player.h"
#include "renju.h"

#include <QVector>

using namespace Renju;

class SimpleAi : public IPlayer
{
public:
    SimpleAi(GameBoard *parent, DOT_COLOR color);
    void onPlayerChange();

private:
    QPair<int, QList<QSharedPointer<Dot>>> generate(DOT_COLOR color);
    QVector<QVector<int>> m_rating;
};
#endif // SIMPLEAI_H
