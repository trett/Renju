#include "game.h"

#include "renju.h"

Game::Game(GameBoard *parent) : QObject(parent), m_gameBoard(parent)
{
    QObject::connect(this, &Game::paint, m_gameBoard, &GameBoard::paintDot);
    QObject::connect(this, &Game::showWin, m_gameBoard, &GameBoard::showWin);
    m_pl = QSharedPointer<HumanPlayer>(new HumanPlayer(parent));
    // TODO: choose color
    m_pl.data()->m_color = BLACK;
    m_pl_ai = QSharedPointer<SimpleAi>(new SimpleAi(parent, WHITE));
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
    Table::table[dot->y()][dot->x()] = m_currentPlayer->m_color;
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
    for (const Table::Direction &direction: QList<Table::Direction> { Table::X, Table::Y, Table::XY, Table::YX }) {
        if (Table::getDotCountInRow(dot, direction) > 4) {
            return true;
        }
    }
    return false;
}

