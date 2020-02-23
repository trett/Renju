#ifndef DOT_H
#define DOT_H

#include <QObject>

class Dot : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int x READ x WRITE setX)
    Q_PROPERTY(int y READ y WRITE setY)
    Q_PROPERTY(int color READ color WRITE setColor)

public:
    explicit Dot(QObject *parent = nullptr);

    int x() const;
    int y() const;
    int color() const;
    void setX(const int x);
    void setY(const int y);
    void setColor(const int color);

private:
    int m_x;
    int m_y;
    int m_color;
};
Q_DECLARE_METATYPE(Dot*)
#endif // DOT_H
