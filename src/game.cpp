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
    int x = dot->x();
    int y = dot->y();

    int xx, yy, counter;
    auto moveNext = [dot](int x, int y, auto m_table, int counter) {
        return counter != 5 && x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE && m_table[x][y] == dot->color();
    };

    // x
    //  x
    //   x
    for (xx = x + 1, yy = y + 1, counter = 1; moveNext(xx, yy, m_table, counter); ++xx, ++yy) {
        counter++;
    }
    for (xx = x - 1, yy = y - 1; moveNext(xx, yy, m_table, counter); --xx, --yy) {
        counter++;
    }
    if (counter > 4) {
        return true;
    }

    //   x
    //  x
    // x
    for (xx = x - 1, yy = y + 1, counter = 1; moveNext(xx, yy, m_table, counter); --xx, ++yy) {
        counter++;
    }
    for (xx = x + 1, yy = y - 1; moveNext(xx, yy, m_table, counter); ++xx, --yy) {
        counter++;
    }
    if (counter > 4) {
        return true;
    }

    // xxx
    for (xx = x - 1, counter = 1; moveNext(xx, y, m_table, counter); --xx) {
        counter++;
    }
    for (xx = x + 1; moveNext(xx, y, m_table, counter); ++xx) {
        counter++;
    }
    if (counter > 4) {
        return true;
    }

    // x
    // x
    // x
    for (yy = y - 1, counter = 1; moveNext(x, yy, m_table, counter); --yy) {
        counter++;
    }
    for (yy = y + 1; moveNext(x, yy, m_table, counter); ++yy) {
        counter++;
    }
    if (counter > 4) {
        return true;
    }
    return false;
}
