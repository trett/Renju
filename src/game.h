#ifndef GAME_H
#define GAME_H

#include "player.h"
#include "gameboard.h"

#include <QObject>
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
    Player m_pl1;
    Player m_pl2;
    Player *m_currentPlayer;
    QList<Dot*> m_history;

    void changePlayer();
    bool hasWinner(Dot *dot);
signals:
    void paint(const Dot *dot);
    void showWin(const Player *pl);
public slots:
    void nextMove(const QVariant &v);
};

#endif // GAME_H
