#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include "dot.h"

#include <QObject>

class GameBoard : public QObject
{
    Q_OBJECT
public:
    explicit GameBoard(QObject *parent);
    QObject *m_board;
signals:
    void dotFromBoard(const Dot *dot);
public slots:
    void paintDot(const Dot *dot);
    void showWin(const int color);
    void onMouseClicked(const QVariant &v);
};

#endif // GAMEBOARD_H
