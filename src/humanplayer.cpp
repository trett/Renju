#include "humanplayer.h"
#include "gameboard.h"
#include "renju.h"

#include <QVariant>

using namespace Renju;

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
    debug("Human moving");
    m_nextMove = qvariant_cast<Dot*>(dot);
    // check field is empty
    if (Table::table.at(m_nextMove->y()).at(m_nextMove->x()) != 0) {
        debug("Invalid move");
        return;
    }
    m_nextMove->setColor(m_color);
    emit move();
}
