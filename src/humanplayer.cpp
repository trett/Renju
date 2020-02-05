#include "humanplayer.h"

#include <QDebug>

HumanPlayer::HumanPlayer(GameBoard *parent, const QVector<QVector<int>> *table) : IPlayer(parent, table)
{
    m_gameBoard = parent;
    QObject::connect(m_gameBoard, &GameBoard::dotFromBoard, this, &HumanPlayer::getNextMove);
}

void HumanPlayer::getNextMove(const Dot *dot)
{
#ifdef QT_DEBUG
    qDebug() << "Human moving";
#endif
    int field = m_table->at(dot->y()).at(dot->x());
    if (field != 0) {
        return;
    }
    m_nextMove.setX(dot->x());
    m_nextMove.setY(dot->y());
    emit(move(const_cast<Dot*>(&m_nextMove)));
}
