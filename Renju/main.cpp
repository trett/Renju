#include <QtGui/QApplication>
#include "renju.h"
#include "GameWidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("qRenju");
    Renju w;
    QIcon icon(":/icons/game_white.ico");
    w.setWindowIcon(icon);
    w.show();
    return a.exec();
}
