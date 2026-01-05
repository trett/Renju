#ifndef SIMPLEAI_H
#define SIMPLEAI_H

#include "i_player.h"
#include "renju.h"

#include <QVector>
#include <QSharedPointer>

using namespace Renju;

/**
 * @brief NegaMax with alpha-beta impl
 */
class SimpleAi : public IPlayer
{
public:
    SimpleAi(QObject *parent);
    Dot *nextMove();
    QVector<QVector<int>> m_model;
private:
    DOT_COLOR m_enemyColor = NONE;
    int generate(DOT_COLOR color, int depth, int alpha, int beta);
    QVector<QSharedPointer<Dot>> getAllMoves(DOT_COLOR color);
    int calculate();
    int evaluatePattern(int x, int y, DOT_COLOR color, const Table::Direction &direction);
    bool isOpenThree(int x, int y, DOT_COLOR color, const Table::Direction &direction);
    bool isOpenFour(int x, int y, DOT_COLOR color, const Table::Direction &direction);
    bool isSplitThree(int x, int y, DOT_COLOR color, const Table::Direction &direction);
    bool blocksExistingFour(int x, int y, DOT_COLOR color, const Table::Direction &direction);
    int countPatterns(DOT_COLOR color);
    QSharedPointer<Dot> m_tmpDot;
    const int DEPTH = 6;
    const int MAX = 6000;
};
#endif // SIMPLEAI_H
