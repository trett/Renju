#include "humanplayer.h"

#include "renju.h"

#include <QDebug>

HumanPlayer::HumanPlayer(GameBoard *parent) : IPlayer(parent)
{
    m_gameBoard = parent;
    QObject::connect(m_gameBoard, &GameBoard::dotFromBoard, this, &HumanPlayer::getNextMove);
}

void HumanPlayer::getNextMove(const Dot *dot)
{
#ifdef QT_DEBUG
    qDebug() << "Human moving";
#endif
    int field = Table::table.at(dot->y()).at(dot->x());
    if (field != 0) {
        return;
    }
    m_nextMove.setX(dot->x());
    m_nextMove.setY(dot->y());
    emit(move(&m_nextMove));
}
