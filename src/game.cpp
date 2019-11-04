#include "game.h"

#include <QVariant>

Game::Game(QObject *parent): QObject(parent)
{
    gameBoard = new GameBoard();
    QObject::connect(this, &Game::paint, gameBoard, &GameBoard::paintDot);
    memset(table, 0, sizeof(table[0][0]) * BOARD_SIZE * BOARD_SIZE);
    pl1 = new Player(BLACK);
    pl2 = new Player(WHITE);
    pl1->canMove = true;
    currentPlayer = pl1;
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
    if (hasWinner()) {
        // TODO: emit(win)
    }
    Dot *dot = qobject_cast<Dot*>(v.value<QObject*>());
    int *field = &table[dot->x()][dot->y()];
    if (*field != NONE) {
        return;
    }
    dot->setColor(currentPlayer->color);
    *field = currentPlayer->color;
    emit(paint(dot));
    changePlayer();
}

void Game::changePlayer() {
    Player *previous = currentPlayer;
    previous->canMove = false;
    currentPlayer = previous == pl1 ? pl2 : pl1;
    currentPlayer->canMove = true;
}

bool Game::hasWinner() {
    return false;
}
