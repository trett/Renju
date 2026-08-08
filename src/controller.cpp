#include "controller.h"

#include <QtConcurrent/QtConcurrentRun>

Controller::Controller(QObject *parent) : QObject(parent)
{
}

void Controller::initGame(const QVariant &humanChoosenColor)
{
    debug("Init game");
    if (m_pl_hmn || m_pl_ai) {
        end();
    }
    m_pl_hmn = new HumanPlayer(this);
    m_pl_ai = new SimpleAi(this);
    QObject::connect(m_pl_hmn, &IPlayer::move, this, &Controller::getNextMove);

    auto color = static_cast<DOT_COLOR>(humanChoosenColor.toInt());
    m_pl_hmn->m_color = color;
    m_pl_ai->m_color = color == WHITE ? BLACK : WHITE;
    debug("Human color is", m_pl_hmn->m_color);
    debug("AI color is", m_pl_ai->m_color);

    m_moveNumber = 0;
    GameLogger::startGame(m_pl_hmn->m_color, m_pl_ai->m_color);

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

    // The search (SimpleAi::nextMove) runs on a worker thread, but all
    // mutation of shared state (Table::table/history, m_currentPlayer,
    // m_state, m_nextMove) happens back on the GUI thread in the
    // "finished" handler below, to avoid racing with the GUI thread
    // reading that same state (e.g. HumanPlayer::onMouseClicked).
    IPlayer *player = m_currentPlayer;
    const int token = ++m_moveToken;
    auto *watcher = new QFutureWatcher<Dot*>(this);
    m_watcher = watcher;
    connect(watcher, &QFutureWatcher<Dot*>::finished, this, [this, watcher, player, token]() {
        if (m_watcher == watcher) {
            m_watcher = nullptr;
        }
        watcher->deleteLater();

        Dot *move = watcher->result();
        if (token != m_moveToken) {
            // The game ended (or a new game started) while this move was
            // being computed; discard the stale result.
            delete move;
            return;
        }
        if (!move) {
            return;
        }

        m_nextMove = move;
        Table::table[move->y()][move->x()] = player->m_color;
        Table::history.push_back(move);

        ++m_moveNumber;
        const auto color = static_cast<DOT_COLOR>(move->color());
        if (player == m_pl_ai) {
            GameLogger::logMove(m_moveNumber, color, move->x(), move->y(), true,
                                 m_pl_ai->lastElapsedMs(), m_pl_ai->lastDepthReached(), m_pl_ai->lastScore());
        } else {
            GameLogger::logMove(m_moveNumber, color, move->x(), move->y(), false);
        }
        if (Table::checkWin(move)) {
            GameLogger::endGame(color);
        }

        changePlayer();
        emit nextMoveChanged();
    });
    watcher->setFuture(QtConcurrent::run([player]() { return player->nextMove(); }));
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
    return Table::checkWin(dot);
}

void Controller::end() {
    debug("End game");
    ++m_moveToken;
    if (m_watcher) {
        m_watcher->waitForFinished();
        m_watcher = nullptr;
    }
    // No-op if the game already ended cleanly (Table::checkWin() true in
    // getNextMove()'s finished handler already closed the log); otherwise
    // records that this game stopped short of a result (e.g. a new game
    // started, or the app quit, mid-game).
    GameLogger::abortGame();
    Table::clear();
    qDeleteAll(Table::history);
    Table::history.clear();
    if (m_pl_hmn) {
        m_pl_hmn->deleteLater();
        m_pl_hmn = nullptr;
    }
    if (m_pl_ai) {
        m_pl_ai->deleteLater();
        m_pl_ai = nullptr;
    }
    m_currentPlayer = nullptr;
    m_nextMove = nullptr;
    m_state = IDLE;
}

Dot *Controller::nextMove() const
{
    return m_nextMove;
}

Controller::GameState Controller::state() const
{
    return m_state;
}
