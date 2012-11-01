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
    int checkTable(int dx, int dy, int color);
    void game();
protected:
    virtual void mousePressEvent(QMouseEvent *pe);

private:
    void paintEvent(QPaintEvent *event);
    QPixmap pix;
    int pointX,pointY,x,y,x1,y1;
    int **table;
    int **rating;
    QByteArray array;
    void conversionMove(int cx, int cy);
    void DebugInConsole();
    int checkDst(int mx, int my, int dx, int dy, int a);
    void generator();
    void showWin(QString str);
    int checkRatingH(int zx, int zy, int aq);
    int checkRatingV(int zx, int zy, int aq);
    int checkRatingD1(int zx, int zy, int aq);
    int checkRatingD2(int zx, int zy, int aq);
    bool win;
    void clearRating();
};

#endif // GAMEWIDGET_H
