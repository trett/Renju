#include "renju.h"

int Table::getDotCountInRow(const Dot* dot, const Direction &direction)
{
    int counter = 1;
    for (QPair<char, char> pair : direction) {
        int x = dot->x(), y = dot->y();
        bool match = false;
        do {
            switch (pair.first) {
            case '-':
                x--;
                break;
            case '+':
                x++;
                break;
            }
            switch (pair.second) {
            case '+':
                y++;
                break;
            case '-':
                y--;
                break;
            }
            match =  x >= 0 && x < Renju::BOARD_SIZE && y >= 0 && y < Renju::BOARD_SIZE && table.at(y).at(x) == dot->color();
            if (match) {
                counter++;
            }
        }  while (match);
    }
    return counter;
}

void Table::clear()
{
    std::for_each(table.begin(), table.end(), [](auto &it) { it.fill(0); } );
}

QVector<QVector<int>> Table::table(BOARD_SIZE, QVector<int>(BOARD_SIZE, 0));
using Direction = QList<QPair<char, char>>;
const Direction Table::XY = { qMakePair('+', '+'),  qMakePair('-', '-') };
const Direction Table::YX = { qMakePair('-', '+'),  qMakePair('+', '-') };
const Direction Table::X = { qMakePair('-', ' '),  qMakePair('+', ' ') };
const Direction Table::Y = { qMakePair(' ', '-'),  qMakePair(' ', '+') };
