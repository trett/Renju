#ifndef IPLAYER_H
#define IPLAYER_H

#include "dot.h"
#include "renju.h"

#include <QObject>

class IPlayer : public QObject {

    Q_OBJECT
public:
    IPlayer(QObject *parent = nullptr)
    {
        Q_UNUSED(parent)
        m_canMove = false;
    }
    bool m_canMove;

    Renju::DOT_COLOR m_color;
    virtual Dot *nextMove() = 0;

public slots:
    virtual void onMouseClicked(const QVariant &dot) {
        Q_UNUSED(dot)
    }

signals:
    void move();
};

#endif // IPLAYER_H
