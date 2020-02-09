#include "gameboard.h"
#include "dot.h"

#include <QVariant>

#ifdef QT_DEBUG
#include <QDebug>
#endif

GameBoard::GameBoard(QObject *parent) : QObject (parent), m_board(parent)
{
    QObject::connect(parent, SIGNAL(mouseClicked(QVariant)), this, SLOT(onMouseClicked(QVariant)));
}

void GameBoard::paintDot(const Dot *dot) {
#ifdef QT_DEBUG
    qDebug() << "Painting dot" << "x:" << dot->x() << " y:" << dot->y();
#endif
    Dot tmp;
    tmp.setX(dot->x());
    tmp.setY(dot->y());
    tmp.setColor(dot->color());
    QMetaObject::invokeMethod(m_board, "paintDot", Q_ARG(QVariant, QVariant::fromValue<Dot*>(&tmp)));
}

void GameBoard::showWin(const int color) {
#ifdef QT_DEBUG
    qDebug() << "Winner is" << color;
#endif
    QMetaObject::invokeMethod(m_board, "showWin", Q_ARG(QVariant, QVariant::fromValue<int>(color)));
}

void GameBoard::onMouseClicked(const QVariant &v)
{
    emit(dotFromBoard(qobject_cast<Dot*>(v.value<QObject*>())));
}
