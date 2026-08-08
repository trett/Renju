#ifndef SIMPLEAI_H
#define SIMPLEAI_H

#include "i_player.h"
#include "renju.h"

#include <QElapsedTimer>
#include <QVector>
#include <QtGlobal>

using namespace Renju;

/**
 * @brief Iterative-deepening NegaMax with alpha-beta impl
 */
class SimpleAi : public IPlayer
{
public:
    explicit SimpleAi(QObject *parent);
    Dot *nextMove() override;

    // Stats from the most recent nextMove() call, for move logging/analysis.
    qint64 lastElapsedMs() const { return m_lastElapsedMs; }
    int lastDepthReached() const { return m_lastDepthReached; }
    int lastScore() const { return m_lastScore; }
private:
    // Lightweight candidate-move representation used internally during the
    // search. A full QObject-based Dot is heap-allocated for every
    // candidate at every node of the search tree, which is by far the
    // most expensive part of the algorithm; this struct avoids that.
    struct Move { int x; int y; };

    int generate(DOT_COLOR color, int depth, int alpha, int beta);
    QVector<Move> getAllMoves(DOT_COLOR color);
    int calculate(DOT_COLOR color);
    int countConsecutive(int x, int y, int dx, int dy, int color);
    // Shared by isOpenThree/isOpenFour/isDoubleOpenFour: the length of the
    // run of `color` through (x, y) along `direction`, and how many of its
    // two ends are open (empty), computed in one pass over the direction.
    void lineStats(int x, int y, DOT_COLOR color, const Table::Direction &direction, int &totalCount, int &openEnds);
    bool isWinningMove(int x, int y, DOT_COLOR color);
    bool isOpenThree(int x, int y, DOT_COLOR color, const Table::Direction &direction);
    bool isOpenFour(int x, int y, DOT_COLOR color, const Table::Direction &direction);
    bool isDoubleOpenFour(int x, int y, DOT_COLOR color, const Table::Direction &direction);
    bool isSplitThree(int x, int y, DOT_COLOR color, const Table::Direction &direction);
    bool completesFive(int x, int y, DOT_COLOR color, const Table::Direction &direction);
    int countPatterns(DOT_COLOR color);

    int m_board[BOARD_SIZE][BOARD_SIZE];
    bool m_hasBestMove = false;
    Move m_bestMove {};
    int m_rootDepth = 0;
    QElapsedTimer m_timer;
    qint64 m_lastElapsedMs = 0;
    int m_lastDepthReached = 0;
    int m_lastScore = 0;

    static constexpr int MAX_SEARCH_DEPTH = 8;
    static constexpr qint64 TIME_BUDGET_MS = 1500;
    static constexpr qint64 MIN_THINK_MS = 300;
    const int MAX = 6000;
};
#endif // SIMPLEAI_H
