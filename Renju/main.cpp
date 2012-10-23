#include <QtGui/QApplication>
#include "renju.h"
#include "GameWidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Renju w;    
    w.show();    
    return a.exec();
}
