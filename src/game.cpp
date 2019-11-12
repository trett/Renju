#include "game.h"

#include <QPointer>
#include <QSharedPointer>
#include <QVariant>

Game::Game(GameBoard *parent): QObject(parent), m_gameBoard(parent), m_table(BOARD_SIZE, QVector<int>(BOARD_SIZE, 0))
{
    QObject::connect(this, &Game::paint, m_gameBoard, &GameBoard::paintDot);
    QObject::connect(this, &Game::showWin, m_gameBoard, &GameBoard::showWin);
    m_pl1.m_color = BLACK;
    m_pl2.m_color = WHITE;
    m_pl1.m_canMove = true;
    m_currentPlayer = &m_pl1;
}

void Game::nextMove(const QVariant &v) {
    if (!m_currentPlayer->m_canMove) {
        // TODO: Error
    }
    Dot *dot = qobject_cast<Dot*>(v.value<QObject*>());
    int *field = &m_table[dot->x()][dot->y()];
    if (*field != NONE) {
        return;
    }
    dot->setColor(m_currentPlayer->m_color);
    *field = m_currentPlayer->m_color;
    emit(paint(dot));
    if (hasWinner(dot)) {
        emit(showWin(m_currentPlayer));
        return;
    }
    m_history.push_back(dot);
    changePlayer();
}

void Game::changePlayer() {
    Player *previous = m_currentPlayer;
    previous->m_canMove = false;
    m_currentPlayer = previous == &m_pl1 ? &m_pl2 : &m_pl1;
    m_currentPlayer->m_canMove = true;
}

bool Game::hasWinner(Dot *dot) {
    int counter = 1;
    int x = dot->x();
    int y = dot->y();

    // x
    //  x
    //   x
    for (int xx = x + 1, yy = y + 1; m_table[xx][yy] == dot->color() && counter != 5; ++xx, ++yy) {
        counter++;
    }
    for (int xx = x - 1, yy = y - 1; m_table[xx][yy] == dot->color() && counter != 5; --xx, --yy) {
        counter++;
    }
    if (counter > 4) {
        return true;
    }

    //   x
    //  x
    // x
    counter = 1;
    for (int xx = x - 1, yy = y + 1; m_table[xx][yy] == dot->color() && counter != 5; --xx, ++yy) {
        counter++;
    }
    for (int xx = x + 1, yy = y - 1; m_table[xx][yy] == dot->color() && counter != 5; ++xx, --yy) {
        counter++;
    }
    if (counter > 4) {
        return true;
    }

    // xxx
    counter = 1;
    for (int xx = x - 1; m_table[xx][y] == dot->color() && counter != 5; --xx) {
        counter++;
    }
    for (int xx = x + 1; m_table[xx][y] == dot->color() && counter != 5; ++xx) {
        counter++;
    }
    if (counter > 4) {
        return true;
    }

    // x
    // x
    // x
    counter = 1;
    for (int yy = y - 1; m_table[x][yy] == dot->color() && counter != 5; --yy) {
        counter++;
    }
    for (int yy = y + 1; m_table[x][yy] == dot->color() && counter != 5; ++yy) {
        counter++;
    }
    if (counter > 4) {
        return true;
    }
    return false;
}
