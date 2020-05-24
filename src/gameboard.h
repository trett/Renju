
#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include "dot.h"
#include "renju.h"

#include <QObject>

using namespace Renju;

class GameBoard : public QObject
{
    Q_OBJECT

public:
    explicit GameBoard(QObject *parent);
    static QObject *board();

private:
    static QObject *m_board;
};
#endif // GAMEBOARD_H
