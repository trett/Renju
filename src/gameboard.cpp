#include "gameboard.h"
#include "dot.h"

#include <QVariant>
#ifdef QT_DEBUG
#include <QDebug>
#endif

GameBoard::GameBoard(QObject *parent) : QObject (parent)
{
}

void GameBoard::paintDot(const Dot *dot) {
#ifdef QT_DEBUG
    qDebug() << "x:" << dot->x() << " y: " << dot->y();
#endif
    QMetaObject::invokeMethod(board, "paintDot", Q_ARG(QVariant, QVariant::fromValue<Dot*>((Dot *)dot)));
}
