#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H
#include <QWidget>
#include <QPainter>
#include <QPixmap>
#include <QBuffer>
#include <QVector>

class GameWidget : public QWidget
{
public:
    GameWidget(QWidget* parent = 0);
    virtual ~GameWidget();
    void clearTable();
    void moveFirst();
    void moveSecond();
    int counter;
    void checkTable(int dx, int dy);
protected:
    virtual void mousePressEvent(QMouseEvent *pe);

private:
    void paintEvent(QPaintEvent *event);
    QPixmap pix;
    int pointX,pointY,x,y,x1,y1;
    int **table;
    int column, rows;
    QByteArray array;
    void conversionMove();
    void DebugInConsole();
    int checkDst(int mx, int my, int dx, int dy, int a);
    void generator();
};

#endif // GAMEWIDGET_H
