#ifndef GAME_H
#define GAME_H

#include "humanplayer.h"
#include "renju.h"
#include "simpleai.h"

#include <QObject>
#include <QSharedPointer>
#include <QVariant>
#include <QVector>

using namespace Renju;

class Controller : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Dot* nextMove READ nextMove NOTIFY nextMoveChanged)
    Q_PROPERTY(GameState state READ state)

public:
    explicit Controller(QObject *parent = nullptr);

    enum GameState {
        NONE, AI, HUMAN
    };
    Q_ENUM(GameState)

    Q_INVOKABLE void initGame(const QVariant &humanChoosenColor);
    Q_INVOKABLE bool checkWin(Dot *dot);
    Q_INVOKABLE void getNextMove();

    void changePlayer();
    Dot *nextMove() const;
    GameState state() const;

signals:
    void nextMoveChanged();

private:
    QObject *m_parent;
    HumanPlayer *m_pl = nullptr;
    SimpleAi *m_pl_ai = nullptr;
    IPlayer *m_currentPlayer = nullptr;
    Dot *m_nextMove = nullptr;
    QList<Dot*> m_history;
    GameState m_state = NONE;
};
#endif // GAME_H
