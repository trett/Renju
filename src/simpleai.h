#ifndef SIMPLEAI_H
#define SIMPLEAI_H

#include "i_player.h"
#include "renju.h"

#include <QVector>

using namespace Renju;

class SimpleAi : public IPlayer
{
public:
    SimpleAi(QObject *parent);
    Dot *nextMove();
private:
    QPair<int, QVector<QSharedPointer<Dot>>> generate(DOT_COLOR color);
    Dot* correctMove(const QVector<QSharedPointer<Dot>> &dots);
};
#endif // SIMPLEAI_H
