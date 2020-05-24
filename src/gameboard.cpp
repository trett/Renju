#include "gameboard.h"

GameBoard::GameBoard(QObject *parent) : QObject (parent)
{
    m_board = parent;
}

QObject *GameBoard::board()
{
    return m_board;
}
QObject *GameBoard::m_board = nullptr;
