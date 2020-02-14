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
    void colorChanged(int color);

public slots:
    void paintDot(const Dot *dot);
    void showWin(const int color);
    void onMouseClicked(const QVariant &v);
    void onColorChoosed(const QVariant &color);
};

#endif // GAMEBOARD_H
