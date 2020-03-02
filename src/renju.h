#ifndef RENJU_H
#define RENJU_H

#include "dot.h"

#include <QList>
#include <QVector>

namespace Renju {

enum DOT_COLOR {
    NONE = 0,
    BLACK = -1,
    WHITE = 1
};

static const int BOARD_SIZE = 16;
};

using namespace Renju;
class Table
{
public:
    Table() = default;
    static QVector<QVector<int>> table;
    using Direction = QList<QPair<char, char>>;
    static const Direction XY;
    static const Direction YX;
    static const Direction X;
    static const Direction Y;

    int static getDotCountInRow(const Dot *dot, const Direction &direction);
    void static clear();
};
#endif // RENJU_H
