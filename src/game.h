#ifndef GAME_H
#define GAME_H

#include "gameboard.h"
#include "humanplayer.h"
#include "simpleai.h"

#include <QObject>
#include <QSharedPointer>
#include <QVector>

class Game : public QObject
{
    Q_OBJECT
public:
    explicit Game(GameBoard *parent);
    GameBoard *m_gameBoard;

private:
    QSharedPointer<HumanPlayer> m_pl;
    QSharedPointer<SimpleAi> m_pl_ai;
    IPlayer *m_currentPlayer;
    QList<Dot*> m_history;

    bool hasWinner(Dot *dot);

signals:
    void paint(const Dot *dot);
    void showWin(const int color);
    void playerChanged();

public slots:
    void nextMove(const Dot *dot);
    void changePlayer();
};

#endif // GAME_H
