#include "game.h"

#include <QPointer>
#include <QSharedPointer>
#include <QVariant>

Game::Game(QObject *parent): QObject(parent), m_gameBoard(new QPointer<GameBoard>(new GameBoard())), m_history(new QList<Dot*>())
{
    QObject::connect(this, &Game::paint, m_gameBoard->data(), &GameBoard::paintDot);
    QObject::connect(this, &Game::showWin, m_gameBoard->data(), &GameBoard::showWin);
    memset(m_table, 0, sizeof(m_table[0][0]) * BOARD_SIZE * BOARD_SIZE);

    m_pl1 = new QSharedPointer<Player>(new Player(BLACK));
    m_pl2 = new QSharedPointer<Player>(new Player(WHITE));
    m_pl1->data()->canMove = true;
    m_currentPlayer = m_pl1->data();
}

Game::~Game() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        delete[] &m_table[i];
    }
}

void Game::nextMove(const QVariant &v) {
    if (!m_currentPlayer->canMove) {
        // TODO: Error
    }
    Dot *dot = qobject_cast<Dot*>(v.value<QObject*>());
    int *field = &m_table[dot->x()][dot->y()];
    if (*field != NONE) {
        return;
    }
    dot->setColor(m_currentPlayer->color);
    *field = m_currentPlayer->color;
    emit(paint(dot));
    if (hasWinner(dot)) {
        emit(showWin(m_currentPlayer));
        return;
    }
    m_history->push_back(dot);
    changePlayer();
}

void Game::changePlayer() {
    Player *previous = m_currentPlayer;
    previous->canMove = false;
    m_currentPlayer = previous == m_pl1->data() ? m_pl2->data() : m_pl1->data();
    m_currentPlayer->canMove = true;
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
