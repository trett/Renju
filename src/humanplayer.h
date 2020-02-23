#ifndef PLAYER_H
#define PLAYER_H

#include "i_player.h"

#include <QObject>

class HumanPlayer : public IPlayer
{
public:
    HumanPlayer(QObject *parent);
    Dot *nextMove();

public slots:
    void onMouseClicked(const QVariant &dot);
private:
    Dot *m_nextMove = nullptr;
};

#endif // PLAYER_H
