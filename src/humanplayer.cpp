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
    Dot *clicked = qvariant_cast<Dot*>(dot);
    if (!clicked) {
        return;
    }
    if (clicked->y() < 0 || clicked->y() >= Renju::BOARD_SIZE ||
        clicked->x() < 0 || clicked->x() >= Renju::BOARD_SIZE) {
        debug("Click out of bounds");
        return;
    }
    // check field is empty
    if (Table::table.at(clicked->y()).at(clicked->x()) != 0) {
        debug("Invalid move");
        return;
    }
    // clicked is a QML-owned object reused across clicks; clone it so the
    // move we hand off (and that ends up in Table::history) is a distinct,
    // C++-owned object rather than an alias to that mutable instance.
    m_nextMove = new Dot();
    m_nextMove->setX(clicked->x());
    m_nextMove->setY(clicked->y());
    m_nextMove->setColor(m_color);
    emit move();
}
