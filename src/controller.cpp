#include "controller.h"

#include <QtConcurrent/QtConcurrentRun>

Controller::Controller(QObject *parent) : QObject(parent)
{
}

void Controller::initGame(const QVariant &humanChoosenColor)
{
    m_pl = new HumanPlayer();
    m_pl_ai = new SimpleAi();

    QObject::connect(m_pl, &IPlayer::move, this, &Controller::getNextMove);

    // TODO: black always move first
    DOT_COLOR color = static_cast<DOT_COLOR>(humanChoosenColor.toInt());
    m_pl->m_color = static_cast<DOT_COLOR>(color);
    m_pl_ai->m_color = color == WHITE ? BLACK : WHITE;
    m_currentPlayer = m_pl;
    m_pl->m_canMove = true;
}

void Controller::getNextMove() {
    QtConcurrent::run([this]() {
        if (!m_currentPlayer->m_canMove) {
            return;
        }
        m_nextMove = m_currentPlayer->nextMove();
        Table::table[m_nextMove->y()][m_nextMove->x()] = m_currentPlayer->m_color;
        m_history.push_back(m_nextMove);
        changePlayer();
        emit(nextMoveChanged());
    });
}

void Controller::changePlayer()
{
    IPlayer *previous = m_currentPlayer;
    previous->m_canMove = false;
    if (m_currentPlayer == m_pl_ai) {
        m_currentPlayer = m_pl;
        m_state = HUMAN;
    } else {
        m_currentPlayer = m_pl_ai;
        m_state = AI;
    }
    m_currentPlayer->m_canMove = true;
}

bool Controller::checkWin(Dot *dot) {
    for (const Table::Direction &direction: QList<Table::Direction> { Table::X, Table::Y, Table::XY, Table::YX }) {
        if (Table::getDotCountInRow(dot, direction) > 4) {
            return true;
        }
    }
    return false;
}

Dot *Controller::nextMove() const
{
    return m_nextMove;
}

Controller::GameState Controller::state() const
{
    return m_state;
}

