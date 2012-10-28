#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QPixmap>
#include "GameWidget.h"
#include <iostream>
#define MIN 0
#define MAX 14
GameWidget::GameWidget(QWidget *parent):QWidget(parent),pix(600,600)
{
    column=15;
    rows=15;
    x=x1=0;
    y=y1=0;
    counter=1;
    this->setStyleSheet("background: #FFDEAD; border: 5px solid black;");
    this->setFixedSize(600,600);
    QColor color(0,0,0,0);
    pix.fill(color);
    QBuffer buffer(&array);
    buffer.open(QIODevice::WriteOnly);
    pix.save(&buffer,"PNG");
}
GameWidget::~GameWidget()
{
    qDebug("ia umer");
    this->DebugInConsole();
    for (int i=0; i<column;++i){
        delete []table[i];
    }
    delete [] table;
}

void GameWidget::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing,true);
    for (int i=0; i<16; ++i){
        p.drawLine(20,i*40+20,580,i*40+20);
        p.drawLine(i*40+20,20,i*40+20,580);
    }
    p.drawPixmap(0,0,pix);
    p.end();
}
void GameWidget::mousePressEvent(QMouseEvent *pe)
{
    pointX = ((qRound((double((pe->x()-20))/40)))*40)+20;
    pointY = ((qRound((double((pe->y()-20))/40)))*40)+20;
    //qDebug()<<pointX<<"    "<<pointY<<"\n"<<pe->x()<<"   "<<pe->y();
    x=(pointX-20)/40;
    y=(pointY-20)/40;
    if (table[x][y]!=1 && table[x][y]!=2){
    this->moveFirst();
    this->conversionMove();
    this->checkTable(x,y);
    counter++;
    this->moveSecond();
    this->conversionMove();
    this->checkTable(x1,y1);
    counter++;
}
    else qDebug("Impossible");
}

void GameWidget::clearTable()
{
    pix.loadFromData(array);
    table = new int *[column];
    for (int i=0;i<column;++i){
        table[i]= new int[rows];
    }
    for (int j=0;j<column;++j){
        for(int k=0;k<rows;++k){
            table[j][k]=0;
            //qDebug()<<cell [j][k];
        }
    }
}

void GameWidget::moveFirst()
{
    QPainter p2(&pix);
    p2.setRenderHint(QPainter::Antialiasing,true);
    p2.setBrush(QBrush(Qt::black));
    p2.drawEllipse(QRect(pointX-10,pointY-10,20,20));
    this->repaint();
}
void GameWidget::moveSecond()
{
    QPainter p3(&pix);
    p3.setRenderHint(QPainter::Antialiasing,true);
    p3.setBrush(QBrush(Qt::white));
    int cx,cy;
    generator();
    if (table[x1][y1]!=1 && table[x1][y1]!=2){
    cx=x1*40+10;
    cy=y1*40+10;// пока рандомно
    }
    else generator();
    p3.drawEllipse(QRect(cx,cy,20,20));
    //qDebug()<<x1<<"         "<<y1;
    this->repaint();

}
void GameWidget::conversionMove()
{   
    bool odd = !!(counter & 1);
    // qDebug()<<cellX<<"  "<<cellY;
    if (odd==true){
//        x=(pointX-20)/40;
//        y=(pointY-20)/40;
        table[x][y]=1;
    }
    else {
        table[x1][y1]=2;
    }
}
void GameWidget::DebugInConsole()
{
    for (int j=0;j<column;++j){
        for(int k=0;k<rows;++k){
            //            qDebug()<<cell [j][k];
            std::cout<<table[k][j];
            if(k>=14) std::cout<<"\n";
        }
    }
}
void GameWidget::checkTable(int dx,int dy)
{
    if (checkDst(dx, dy, 1, 0, 1) >= 5 || checkDst(dx, dy, 0, 1, 1) >= 5
            || checkDst(dx, dy, 1, 1, 1) >= 5  || checkDst(dx, dy, 1, -1, 1) >= 5)
        qDebug()<<"Black Wins!";
    else if (checkDst(dx, dy, 1, 0, 2) >= 5 || checkDst(dx, dy, 0, 1, 2) >= 5
             || checkDst(dx, dy, 1, 1, 2) >= 5  || checkDst(dx, dy, 1, -1, 2) >= 5)
        qDebug()<<"White Wins!";
    else
        qDebug("Continue game");

}

int GameWidget::checkDst(int mx, int my, int dx, int dy, int a)
{
    int cx = mx, cy = my;
    int c = 0;
    while (cx >= MIN && cx <= MAX && cy >= MIN && cy <= MAX && table[cx][cy] == a) {
        ++c;
        cx += dx;
        cy += dy;
    }
    dx = -dx;
    dy = -dy;
    cx = x + dx;
    cy = y + dy;
    while (cx >= MIN && cx <= MAX && cy >= MIN && cy <= MAX && table[cx][cy] == a ) {
        ++c;
        cx += dx;
        cy += dy;
    }
    return c;
}
void GameWidget::generator()
{
    x1=rand()%15;
    y1=rand()%15;
}
