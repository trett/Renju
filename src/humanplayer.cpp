#include "humanplayer.h"

#include "gameboard.h"

#include <QDebug>

HumanPlayer::HumanPlayer(QObject *parent) : IPlayer(parent)
{
    QObject::connect(GameBoard::board(), SIGNAL(mouseClicked(QVariant)), this, SLOT(onMouseClicked(QVariant)));
}

Dot *HumanPlayer::nextMove()
{
    return m_nextMove;
}

void HumanPlayer::onMouseClicked(const QVariant &dot)
{
#ifdef QT_DEBUG
    qDebug() << "Human moving";
#endif
    m_nextMove = qvariant_cast<Dot*>(dot);
    m_nextMove->setColor(m_color);
    emit move();
}
