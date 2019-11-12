#include "gameboard.h"
#include "dot.h"

#include <QVariant>
#ifdef QT_DEBUG
#include <QDebug>
#endif

GameBoard::GameBoard(QObject *parent) : QObject (parent), m_board(parent)
{
}

void GameBoard::paintDot(const Dot *dot) {
#ifdef QT_DEBUG
    qDebug() << "x:" << dot->x() << " y: " << dot->y();
#endif
    QMetaObject::invokeMethod(m_board, "paintDot", Q_ARG(QVariant, QVariant::fromValue<Dot*>(const_cast<Dot*>(dot))));
}

void GameBoard::showWin(const Player *pl) {
#ifdef QT_DEBUG
    qDebug() << "winner is " << pl->m_color;
#endif
    QMetaObject::invokeMethod(m_board, "showWin", Q_ARG(QVariant, QVariant::fromValue<int>(pl->m_color)));
}
