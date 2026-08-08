#include "gameboard.h"

GameBoard::GameBoard(QObject *parent) : QObject (parent)
{
    m_board = parent;
}

GameBoard::~GameBoard()
{
    m_board = nullptr;
}

QObject *GameBoard::board()
{
    return m_board;
}
QObject *GameBoard::m_board = nullptr;
