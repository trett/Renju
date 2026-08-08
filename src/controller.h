#ifndef GAME_H
#define GAME_H

#include "gamelogger.h"
#include "humanplayer.h"
#include "renju.h"
#include "simpleai.h"

#include <QFutureWatcher>
#include <QObject>
#include <QVariant>

using namespace Renju;

class Controller : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Dot* nextMove READ nextMove NOTIFY nextMoveChanged)
    Q_PROPERTY(GameState state READ state)

public:
    explicit Controller(QObject *parent = nullptr);

    enum GameState {
        IDLE, AI, HUMAN
    };
    Q_ENUM(GameState)

    Q_INVOKABLE void initGame(const QVariant &humanChoosenColor);
    Q_INVOKABLE bool checkWin(Dot *dot);
    Q_INVOKABLE void getNextMove();
    Q_INVOKABLE void end();

    void changePlayer();
    Dot *nextMove() const;
    GameState state() const;

signals:
    void nextMoveChanged();

private:
    HumanPlayer *m_pl_hmn = nullptr;
    SimpleAi *m_pl_ai = nullptr;
    IPlayer *m_currentPlayer = nullptr;
    Dot *m_nextMove = nullptr;
    GameState m_state = IDLE;
    QFutureWatcher<Dot*> *m_watcher = nullptr;
    int m_moveToken = 0;
    int m_moveNumber = 0;
};
#endif // GAME_H
