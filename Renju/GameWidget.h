#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H
#include <QWidget>
#include <QPainter>
#include <QPixmap>
#include <QBuffer>
#include <QVector>

class GameWidget : public QWidget
{
    Q_OBJECT

public:
    GameWidget(QWidget* parent = 0);
    virtual ~GameWidget();
    void clearTable();
    bool cmpFirst;
    bool soundOff;
    void  rmMove();
    QString status;
protected:
    virtual void mousePressEvent(QMouseEvent *pe);

private:
    int counter;
    void game();
    void paintEvent(QPaintEvent *event);
    int checkTable(int dx, int dy, int color);
    void moveFirst();
    void moveSecond();
    QPixmap pix;
    int pointX,pointY,x,y,x1,y1;
    int **table;
    int **rating;
    QByteArray array,array2;
    void conversionMove(int cx, int cy, int cl);
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
    void audio();
signals:
    void enblRm(bool);
    void sendStatus(QString str);
    void sendEndOfGame(QString str);
public slots:
    void cmpFirstMove();
};

#endif // GAMEWIDGET_H
