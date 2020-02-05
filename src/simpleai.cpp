#include "simpleai.h"

#include <QDebug>


SimpleAi::SimpleAi(GameBoard *parent, int color, QVector<QVector<int>> *table) : IPlayer(parent, table), m_rating(16, QVector<int>(16, 0))
{
    m_color = color;
    // default first move
    m_nextMove.setX(7);
    m_nextMove.setY(7);
}

QPair<int, QSharedPointer<Dot>> SimpleAi::getNextMove(int color) {
    // compute new rating
    int xx, yy, counter;
    auto checkNext = [this, color] (int x, int y) {
        return x >= 0 && x < 16 && y >= 0 && y < 16 && m_table->at(y).at(x) == color;
    };
    for (int y = 0; y < m_rating.size(); y++) {
        for (int x = 0; x < m_rating.at(y).size(); x++) {
            if (m_table->at(y).at(x) != 0) {
                continue;
            }

            // x
            //  x
            //   x
            for (xx = x + 1, yy = y + 1, counter = 1; checkNext(xx, yy); ++xx, ++yy) {
                counter++;
            }
            for (xx = x - 1, yy = y - 1; checkNext(xx, yy); --xx, --yy) {
                counter++;
            }
            m_rating[y][x] = counter;
            //   x
            //  x
            // x
            for (xx = x - 1, yy = y + 1, counter = 1; checkNext(xx, yy); --xx, ++yy) {
                counter++;
            }
            for (xx = x + 1, yy = y - 1; checkNext(xx, yy); ++xx, --yy) {
                counter++;
            }
            m_rating[y][x] = std::max(m_rating[y][x], counter);

            // xxx
            for (xx = x - 1, counter = 1; checkNext(xx, y); --xx) {
                counter++;
            }
            for (xx = x + 1; checkNext(xx, y); ++xx) {
                counter++;
            }
            m_rating[y][x] = std::max(m_rating[y][x], counter);

            // x
            // x
            // x
            for (yy = y - 1, counter = 1; checkNext(x, yy); --yy) {
                counter++;
            }
            for (yy = y + 1; checkNext(x, yy); ++yy) {
                counter++;
            }
            m_rating[y][x] = std::max(m_rating[y][x], counter);
        }
    }
    // search field with highest rating
    QSharedPointer<Dot> dot = QSharedPointer<Dot>(new Dot);
    auto bestMoveCdts = QPair<int, QSharedPointer<Dot>>(-1, dot);
    for (auto it = m_rating.begin(); it < m_rating.end(); it++) {
        auto temp = std::max_element(it->constBegin(), it->constEnd());
        if (*temp > bestMoveCdts.first) {
            bestMoveCdts.first = *temp;
            dot.data()->setY(std::distance(m_rating.begin(), it));
            dot.data()->setX(std::distance(it->constBegin(), temp));
        }
    }
#ifdef QT_DEBUG
    std::for_each(m_rating.begin(), m_rating.end(), [](auto &it) { qDebug() << it; } );
    qDebug() << "x:" << bestMoveCdts.second.data()->x() << "y:" << bestMoveCdts.second.data()->y();
#endif
    // clear rating table
    std::for_each(m_rating.begin(), m_rating.end(), [](auto &it) { it.fill(0); } );
    return bestMoveCdts;
}

void SimpleAi::onPlayerChange()
{
    auto selfBstMove = getNextMove(m_color);
    auto enemyBestMove = getNextMove(m_color == -1 ? 1 : -1);
#ifdef QT_DEBUG
    qDebug() << "AI moving";
    qDebug() << "Self rating" << selfBstMove.first << "\r\nEnemy rating" << enemyBestMove.first;
#endif
    if (selfBstMove.first >= enemyBestMove.first) {
        m_nextMove.setX(selfBstMove.second.data()->x());
        m_nextMove.setY(selfBstMove.second.data()->y());
    } else {
        m_nextMove.setX(enemyBestMove.second.data()->x());
        m_nextMove.setY(enemyBestMove.second.data()->y());
    }
    emit(move(const_cast<Dot*>(&m_nextMove)));
}
