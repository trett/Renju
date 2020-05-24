#include "simpleai.h"

#include <QSharedPointer>
#include <QThread>
#include <QMap>

SimpleAi::SimpleAi(QObject *parent) : IPlayer(parent), m_model(BOARD_SIZE, QVector<int>(BOARD_SIZE, 0))
{
}

int SimpleAi::generate(DOT_COLOR color, int depth, int alpha, int beta) {
    DOT_COLOR opColor = color ? BLACK : WHITE;
    if (depth == 0) {
        return calculate();
    }
    auto moves = getAllMoves(color);
    foreach (QSharedPointer<Dot> move, moves) {
        // make move
        m_model[move->y()][move->x()] = color;
        int tmp = -generate(opColor, Table::history.size() == 1 ? 0 : depth - 1, -beta, -alpha);
        // unmake move
        m_model[move->y()][move->x()] = NONE;
        if (tmp >= alpha) {
            alpha = tmp;
            m_tmpDot = QSharedPointer<Dot>(new Dot);
            m_tmpDot->setX(move->x());
            m_tmpDot->setY(move->y());
        }
    }
    return alpha;
}

QVector<QSharedPointer<Dot> > SimpleAi::getAllMoves(DOT_COLOR color)
{
    auto getRate = [](DOT_COLOR color, int x, int y, QMap<int, QVector<QSharedPointer<Dot>>> &map){
        QSharedPointer<Dot> dot(new Dot);
        dot->setX(x);
        dot->setY(y);
        dot->setColor(color);
        int rate = -1;
        for (const Table::Direction &direction: QList<Table::Direction> { Table::X, Table::Y, Table::XY, Table::YX }) {
            rate = std::max(rate, Table::getDotCountInRow(dot.data(), direction));
        }
        map[rate].append(dot);
    };
    QMap<int, QVector<QSharedPointer<Dot>>> dotsMap;
    QMap<int, QVector<QSharedPointer<Dot>>> opDotsMap;
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (Table::table.at(y).at(x) != 0) {
                continue;
            }
            getRate(color, x, y, dotsMap);
            getRate(color == WHITE ? BLACK : WHITE, x, y, opDotsMap);
        }
    }

    QVector<QSharedPointer<Dot>> res;
    auto m = color == m_color ?
                dotsMap.lastKey() >= opDotsMap.lastKey() ?
                    &dotsMap : &opDotsMap : dotsMap.lastKey() > opDotsMap.lastKey() ?
                        &dotsMap : &opDotsMap;
    if (m->lastKey() < 3 || m->lastKey() >= 5) {
        res << m->last();
    } else {
        QMapIterator<int, QVector<QSharedPointer<Dot>>> it(*m);
        while(it.hasNext()) {
            auto val = it.next();
            if (val.key() > 2) {
                res << val.value();
            }
        }
    }
    return res;
}

Dot *SimpleAi::nextMove()
{
    Dot *dot = new Dot;
    debug("AI moving");
    // first move
    if (Table::history.empty()) {
        dot->setX(7);
        dot->setY(7);
        dot->setColor(m_color);
        return dot;
    }
    // copy Table to model
    std::copy(Table::table.constBegin(), Table::table.constEnd(), m_model.begin());
    generate(m_color, DEPTH, -5000, -5000);
    dot->setX(m_tmpDot->x());
    dot->setY(m_tmpDot->y());
    dot->setColor(m_color);
    // clear model
    std::for_each(m_model.begin(), m_model.end(), [](auto &it) { it.fill(0); } );
    // humanize
    QThread::msleep(qrand() % 200 + 300);
    return dot;
}

int SimpleAi::calculate()
{
    // calculate how much possibilities to increase continuous dots
    QMap<int, int> opMap;
    QMap<int, int> selfMap;
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (m_model.at(y).at(x) != 0) {
                continue;
            }
            Dot dot;
            dot.setX(x);
            dot.setY(y);
            // opponent
            dot.setColor(m_color == WHITE ? BLACK : WHITE);
            int tmp = 0;
            for (const Table::Direction &direction: QList<Table::Direction> { Table::X, Table::Y, Table::XY, Table::YX }) {
                tmp = std::max(tmp, Table::getDotCountInRow(&dot, direction));
            }
            opMap[tmp]++;
            // self
            dot.setColor(m_color);
            tmp = 0;
            for (const Table::Direction &direction: QList<Table::Direction> { Table::X, Table::Y, Table::XY, Table::YX }) {
                tmp = std::max(tmp, Table::getDotCountInRow(&dot, direction));
            }
            selfMap[tmp]++;
        }
    }
    int opScore = opMap.lastKey() * 1000 + opMap.last();
    int selfScore = selfMap.lastKey() * 1000 + selfMap.last();
    if (opScore >= 5000) {
        return -MAX;
    }
    if (selfScore >= 5000) {
        return MAX;
    }
    if (selfScore >= opScore) {
        return selfScore - opScore;
    }
    return opScore - selfScore;
}
