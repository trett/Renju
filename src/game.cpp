#include "game.h"

#include <QPointer>
#include <QVariant>

Game::Game(GameBoard *parent) : QObject(parent), m_gameBoard(parent), m_table(BOARD_SIZE, QVector<int>(BOARD_SIZE, 0))
{
    QObject::connect(this, &Game::paint, m_gameBoard, &GameBoard::paintDot);
    QObject::connect(this, &Game::showWin, m_gameBoard, &GameBoard::showWin);
    m_pl = QSharedPointer<HumanPlayer>(new HumanPlayer(parent, &m_table));
    // TODO: choose color
    m_pl.data()->m_color = BLACK;
    m_pl_ai = QSharedPointer<SimpleAi>(new SimpleAi(parent, WHITE, &m_table));
    m_pl.data()->m_canMove = true;
    QObject::connect(m_pl.data(), &IPlayer::move, this, &Game::nextMove);
    QObject::connect(m_pl_ai.data(), &IPlayer::move, this, &Game::nextMove);
    m_currentPlayer = m_pl.data();
}

void Game::nextMove(const Dot *dot) {
    if (!m_currentPlayer->m_canMove) {
        // TODO: Error
    }

    Dot nextDot;
    nextDot.setColor(m_currentPlayer->m_color);
    nextDot.setX(dot->x());
    nextDot.setY(dot->y());
    m_table[dot->y()][dot->x()] = m_currentPlayer->m_color;
    emit(paint(const_cast<Dot*>(&nextDot)));
    m_history.push_back(&nextDot);
    if (hasWinner(&nextDot)) {
        emit(showWin(m_currentPlayer->m_color));
        return;
    }
    changePlayer();
}

void Game::changePlayer()
{
    IPlayer *previous = m_currentPlayer;
    previous->m_canMove = false;
    if (m_currentPlayer == m_pl_ai.data()) {
        m_currentPlayer = m_pl.data();
    } else {
        m_currentPlayer = m_pl_ai.data();
    }
    m_currentPlayer->m_canMove = true;
    if (m_currentPlayer == m_pl_ai.data()) {
        m_pl_ai.data()->onPlayerChange();
    }
}

bool Game::hasWinner(Dot *dot) {
    int x = dot->x();
    int y = dot->y();

    int xx, yy, counter;
    auto moveNext = [dot](int x, int y, auto m_table, int counter) {
        return counter != 5 && x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE && m_table[y][x] == dot->color();
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
