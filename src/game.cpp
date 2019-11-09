#include "game.h"

#include <QSharedPointer>
#include <QVariant>

Game::Game(QObject *parent): QObject(parent)
{
    gameBoard = new QSharedPointer<GameBoard>(new GameBoard());
    QObject::connect(this, &Game::paint, gameBoard->data(), &GameBoard::paintDot);
    QObject::connect(this, &Game::showWin, gameBoard->data(), &GameBoard::showWin);
    memset(table, 0, sizeof(table[0][0]) * BOARD_SIZE * BOARD_SIZE);

    pl1 = new QSharedPointer<Player>(new Player(BLACK));
    pl2 = new QSharedPointer<Player>(new Player(WHITE));
    pl1->data()->canMove = true;
    currentPlayer = pl1->data();
}

Game::~Game() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        delete[] &table[i];
    }
}

void Game::nextMove(const QVariant &v) {
    if (!currentPlayer->canMove) {
        // TODO: Error
    }
    Dot *dot = qobject_cast<Dot*>(v.value<QObject*>());
    int *field = &table[dot->x()][dot->y()];
    if (*field != NONE) {
        return;
    }
    dot->setColor(currentPlayer->color);
    *field = currentPlayer->color;
    emit(paint(dot));
    if (hasWinner(dot)) {
        emit(showWin(currentPlayer));
        return;
    }
    changePlayer();
}

void Game::changePlayer() {
    Player *previous = currentPlayer;
    previous->canMove = false;
    currentPlayer = previous == pl1->data() ? pl2->data() : pl1->data();
    currentPlayer->canMove = true;
}

bool Game::hasWinner(Dot *dot) {
    int counter = 1;
    int x = dot->x();
    int y = dot->y();

    // x
    //  x
    //   x
    for (int xx = x + 1, yy = y + 1; table[xx][yy] == dot->color() && counter != 5; ++xx, ++yy) {
        counter++;
    }
    for (int xx = x - 1, yy = y - 1; table[xx][yy] == dot->color() && counter != 5; --xx, --yy) {
        counter++;
    }
    if (counter > 4) {
        return true;
    }

    //   x
    //  x
    // x
    counter = 1;
    for (int xx = x - 1, yy = y + 1; table[xx][yy] == dot->color() && counter != 5; --xx, ++yy) {
        counter++;
    }
    for (int xx = x + 1, yy = y - 1; table[xx][yy] == dot->color() && counter != 5; ++xx, --yy) {
        counter++;
    }
    if (counter > 4) {
        return true;
    }

    // xxx
    counter = 1;
    for (int xx = x - 1; table[xx][y] == dot->color() && counter != 5; --xx) {
        counter++;
    }
    for (int xx = x + 1; table[xx][y] == dot->color() && counter != 5; ++xx) {
        counter++;
    }
    if (counter > 4) {
        return true;
    }

    // x
    // x
    // x
    counter = 1;
    for (int yy = y - 1; table[x][yy] == dot->color() && counter != 5; --yy) {
        counter++;
    }
    for (int yy = y + 1; table[x][yy] == dot->color() && counter != 5; ++yy) {
        counter++;
    }
    if (counter > 4) {
        return true;
    }
    return false;
}
