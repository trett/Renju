#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include "dot.h"

#include <QObject>

class GameBoard : public QObject
{
	Q_OBJECT
public:
	explicit GameBoard(QObject *parent = nullptr);
	QObject *board;
public slots:
	void paintDot(const Dot *dot);
};

#endif // GAMEBOARD_H
