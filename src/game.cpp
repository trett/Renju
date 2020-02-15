#include "game.h"

#include "renju.h"

Game::Game(GameBoard *parent) : QObject(parent), m_gameBoard(parent)
{
    QObject::connect(this, &Game::paint, m_gameBoard, &GameBoard::paintDot);
    QObject::connect(this, &Game::showWin, m_gameBoard, &GameBoard::showWin);
    QObject::connect(m_gameBoard, &GameBoard::colorChanged, this, &Game::setPlayersColors);

    m_pl = QSharedPointer<HumanPlayer>(new HumanPlayer(parent));
    m_pl_ai = QSharedPointer<SimpleAi>(new SimpleAi(parent));

    QObject::connect(m_pl.data(), &IPlayer::move, this, &Game::nextMove);
    QObject::connect(m_pl_ai.data(), &IPlayer::move, this, &Game::nextMove);

    m_currentPlayer = m_pl.data();
    m_pl.data()->m_canMove = true;
}

void Game::setPlayersColors(int humanChoosenColor)
{
    // TODO: black always move first
    DOT_COLOR color = static_cast<DOT_COLOR>(humanChoosenColor);
    m_pl.data()->m_color = static_cast<DOT_COLOR>(color);
    m_pl_ai.data()->m_color = color == WHITE ? BLACK : WHITE;
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
    emit(paint(&nextDot));
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

