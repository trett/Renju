#ifndef PLAYER_H
#define PLAYER_H

#include "i_player.h"

#include <QObject>

class HumanPlayer : public IPlayer
{
public:
    explicit HumanPlayer(QObject *parent);
    Dot *nextMove() override;

public slots:
    void onMouseClicked(const QVariant &dot) override;
private:
    Dot *m_nextMove = nullptr;
};

#endif // PLAYER_H
