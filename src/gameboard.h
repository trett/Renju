#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include "dot.h"
#include "player.h"

#include <QObject>

class GameBoard : public QObject
{
	Q_OBJECT
public:
	explicit GameBoard(QObject *parent = nullptr);
	QObject *board;
public slots:
	void paintDot(const Dot *dot);
	void showWin(const Player *pl);
};

#endif // GAMEBOARD_H
