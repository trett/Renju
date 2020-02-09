#include "simpleai.h"

#include <QDebug>


SimpleAi::SimpleAi(GameBoard *parent, DOT_COLOR color) : IPlayer(parent), m_rating(BOARD_SIZE, QVector<int>(BOARD_SIZE, 0))
{
    m_color = color;
    // default first move
    m_nextMove.setX(7);
    m_nextMove.setY(7);
}

QPair<int, QList<QSharedPointer<Dot>>> SimpleAi::generate(DOT_COLOR color) {
    // compute new rating
    for (int y = 0; y < m_rating.size(); y++) {
        for (int x = 0; x < m_rating.at(y).size(); x++) {
            if (Table::table.at(y).at(x) != 0) {
                continue;
            }
            Dot dot;
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

void SimpleAi::onPlayerChange()
{
    auto selfMoves = generate(m_color);
    auto enemyMoves = generate(m_color == WHITE ? BLACK : WHITE);
#ifdef QT_DEBUG
    qDebug() << "AI moving";
    qDebug() << "Self rating" << selfMoves.first << "\r\nEnemy rating" << enemyMoves.first;
#endif
    // TODO: choose best move from generate
    if (selfMoves.first >= enemyMoves.first) {
        m_nextMove.setX(selfMoves.second.last().data()->x());
        m_nextMove.setY(selfMoves.second.last().data()->y());
    } else {
        m_nextMove.setX(enemyMoves.second.last().data()->x());
        m_nextMove.setY(enemyMoves.second.last().data()->y());
    }
    emit(move(const_cast<Dot*>(&m_nextMove)));
}
