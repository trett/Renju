#include "simpleai.h"

#include <QDebug>
#include <QThread>

SimpleAi::SimpleAi(QObject *parent) : IPlayer(parent), m_rating(BOARD_SIZE, QVector<int>(BOARD_SIZE, 0))
{
}

QPair<int, QList<QSharedPointer<Dot>>> SimpleAi::generate(DOT_COLOR color) {
    // compute new rating
    Dot dot;
    for (int y = 0; y < m_rating.size(); y++) {
        for (int x = 0; x < m_rating.at(y).size(); x++) {
            if (Table::table.at(y).at(x) != 0) {
                continue;
            }
            dot.setX(x);
            dot.setY(y);
            dot.setColor(color);
            for (const Table::Direction &direction: QList<Table::Direction> { Table::X, Table::Y, Table::XY, Table::YX }) {
                m_rating[y][x] = std::max(m_rating[y][x],Table::getDotCountInRow(&dot, direction));
            }
        }
    }
    QList<QSharedPointer<Dot>> dotList;
    // search field with highest rating
    int max = -1;
    for (auto it = m_rating.begin(); it < m_rating.end(); it++) {
        auto temp = std::max_element(it->constBegin(), it->constEnd());
        if (*temp >= max) {
            max = *temp;
            QSharedPointer<Dot> dot = QSharedPointer<Dot>(new Dot);
            dot.data()->setY(std::distance(m_rating.begin(), it));
            dot.data()->setX(std::distance(it->constBegin(), temp));
            dotList << dot;
        }
    }
    // clear rating table
    std::for_each(m_rating.begin(), m_rating.end(), [](auto &it) { it.fill(0); } );
    return QPair<int, QList<QSharedPointer<Dot>>>(max, dotList);
}

Dot *SimpleAi::nextMove()
{
    auto selfMoves = generate(m_color);
    auto enemyMoves = generate(m_color == WHITE ? BLACK : WHITE);
#ifdef QT_DEBUG
    qDebug() << "AI moving";
    qDebug() << "Self rating" << selfMoves.first << "\r\nEnemy rating" << enemyMoves.first;
#endif
    Dot *dot = new Dot(this);
    // TODO: choose best move from generate
    if (selfMoves.first >= enemyMoves.first) {
        dot->setX(selfMoves.second.last().data()->x());
        dot->setY(selfMoves.second.last().data()->y());
    } else {
        dot->setX(enemyMoves.second.last().data()->x());
        dot->setY(enemyMoves.second.last().data()->y());
    }
    dot->setColor(m_color);
    // humanize
    QThread::msleep(qrand() % 300 + 400);
    return dot;
}
