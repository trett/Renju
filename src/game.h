#ifndef GAME_H
#define GAME_H

#include "player.h"
#include "gameboard.h"

#include <QObject>

class Game : public QObject
{
	Q_OBJECT
public:
	enum DOT_COLOR {
		NONE = 0,
		BLACK = -1,
		WHITE = 1
	};

	explicit Game(QObject *parent = nullptr);
	~Game();

	static const int BOARD_SIZE = 15;
	int table[BOARD_SIZE][BOARD_SIZE];
	int totalMoves;

	GameBoard *gameBoard;
	Player *pl1;
	Player *pl2;
	Player *currentPlayer;

	void changePlayer();
	bool hasWinner();
signals:
	void paint(const Dot *dot);
public slots:
	void nextMove(const QVariant &v);
};

#endif // GAME_H
