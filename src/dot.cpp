#include "dot.h"

Dot::Dot(QObject *parent) : QObject(parent), m_x(0), m_y(0), m_color(0)
{
}

int Dot::x() const {
    return m_x;
}

int Dot::y() const {
    return m_y;
}

int Dot::color() const {
    return m_color;
}

void Dot::setX(const int x) {
    m_x = x;
}

void Dot::setY(const int y) {
    m_y = y;
}

void Dot::setColor(const int color) {
    m_color = color;
}

