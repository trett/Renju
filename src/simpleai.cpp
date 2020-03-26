#include "simpleai.h"

#include <QSharedPointer>
#include <QThread>
#include <QMap>

SimpleAi::SimpleAi(QObject *parent) : IPlayer(parent)
{
}

QPair<int, QVector<QSharedPointer<Dot>>> SimpleAi::generate(DOT_COLOR color) {
    // compute new rating
    QMap<int, QVector<QSharedPointer<Dot>>> dotMap;
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (Table::table.at(y).at(x) != 0) {
                continue;
            }
            QSharedPointer<Dot> dot(new Dot);
            dot->setX(x);
            dot->setY(y);
            dot->setColor(color);
            int rate = -1;
            for (const Table::Direction &direction: QList<Table::Direction> { Table::X, Table::Y, Table::XY, Table::YX }) {
                rate = std::max(rate, Table::getDotCountInRow(dot.data(), direction));
            }
            dotMap[rate].append(dot);
        }
    }
    return qMakePair(dotMap.lastKey(), dotMap.last());
}

Dot *SimpleAi::nextMove()
{
    Dot *dot = new Dot();
    auto enemyMoves = generate(m_color == WHITE ? BLACK : WHITE);
    // check enemy has dotes
    if (enemyMoves.first > 1) {
        auto selfMoves = generate(m_color);
        debug("AI moving");
        debug("Self rating", selfMoves.first, "\r\nEnemy rating", enemyMoves.first);
        auto corrected = selfMoves.first >= enemyMoves.first ? correctMove(selfMoves.second) : correctMove(enemyMoves.second);
        dot->setX(corrected->x());
        dot->setY(corrected->y());
    } else {
        // or it's first move and we set default values
        dot->setX(7);
        dot->setY(7);
    }
    dot->setColor(m_color);
    // humanize
    QThread::msleep(qrand() % 200 + 300);
    return dot;
}

Dot *SimpleAi::correctMove(const QVector<QSharedPointer<Dot>> &dots)
{
    Dot temp;
    QMap<int, Dot*> result;
    foreach (auto dot, dots) {
        int rating = 0;
        for (const Table::Direction &direction: QList<Table::Direction> { Table::X, Table::Y, Table::XY, Table::YX }) {
            temp.setX(dot->x());
            temp.setY(dot->y());
            temp.setColor(NONE);
            int emptyRows = Table::getDotCountInRow(&temp, direction);
            if (emptyRows > 4) {
                rating += emptyRows;
            }
        }
        result.insert(rating, dot.data());
    }
    return result.last();
}
