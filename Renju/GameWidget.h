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
    int cellX,cellY;
    int counter;
protected:
    virtual void mousePressEvent(QMouseEvent *pe);

private:
    void paintEvent(QPaintEvent *event);
    QPixmap pix;
    int pointX;
    int pointY;
    int **cell;
    int column;
    int rows;
    QByteArray array;
    void conversionMove();
    void DebugInConsole();
};

#endif // GAMEWIDGET_H
