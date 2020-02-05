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
    enum DOT_COLOR {
        NONE = 0,
        BLACK = -1,
        WHITE = 1
    };

    explicit Game(GameBoard *gameBoard);
    GameBoard *m_gameBoard;
private:
    static const int BOARD_SIZE = 16;
    QVector<QVector<int>> m_table;
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
