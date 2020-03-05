#include "controller.h"

#include <QtConcurrent/QtConcurrentRun>

Controller::Controller(QObject *parent) : QObject(parent), m_parent(parent)
{
}

void Controller::initGame(const QVariant &humanChoosenColor)
{
    debug("Init game");
    m_pl_hmn = new HumanPlayer(m_parent);
    m_pl_ai = new SimpleAi(m_parent);
    QObject::connect(m_pl_hmn, &IPlayer::move, this, &Controller::getNextMove);

    auto color = static_cast<DOT_COLOR>(humanChoosenColor.toInt());
    m_pl_hmn->m_color = color;
    m_pl_ai->m_color = color == WHITE ? BLACK : WHITE;
    debug("Human color is", m_pl_hmn->m_color);
    debug("AI color is", m_pl_ai->m_color);

    // inverse for first move
    if (color == BLACK) {
        m_currentPlayer = m_pl_ai;
    } else {
        m_currentPlayer = m_pl_hmn;
    }
    changePlayer();
    if (m_state == AI) {
        getNextMove();
    }
}

void Controller::getNextMove() {
    if (!m_currentPlayer->m_canMove) {
        return;
    }
    debug("Retrieving next move for", m_currentPlayer->m_color);
    QtConcurrent::run([this]() {
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
        m_currentPlayer = m_pl_hmn;
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

void Controller::end() {
    debug("End game");
    Table::clear();
    m_history.clear();
    m_pl_hmn->deleteLater();
    m_pl_ai->deleteLater();
}

Dot *Controller::nextMove() const
{
    return m_nextMove;
}

Controller::GameState Controller::state() const
{
    return m_state;
}
