#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QPixmap>
#include "GameWidget.h"
#include <iostream>
GameWidget::GameWidget(QWidget *parent):QWidget(parent),pix(600,600),column(15),rows(15),cellX(0),cellY(0),counter(1)
{
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
        delete []cell[i];
    }
    delete [] cell;
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
    this->moveFirst();
    this->conversionMove();
    counter++;
}

void GameWidget::clearTable()
{
    pix.loadFromData(array);
    cell = new int *[column];
    for (int i=0;i<column;++i){
        cell[i]= new int[rows];
    }
    for (int j=0;j<column;++j){
        for(int k=0;k<rows;++k){
            cell[j][k]=0;
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

}
void GameWidget::conversionMove()
{
    cellX=(pointX-20)/40;
    cellY=(pointY-20)/40;
    qDebug()<<"counter"<<counter;
    bool odd = !!(counter & 1);
    qDebug()<<cellX<<"  "<<cellY;
    if (odd==true){
        cell[cellX][cellY]=1;
    }
    else {
        cell[cellX][cellY]=2;
    }
    qDebug()<<"odd"<<odd;
}
void GameWidget::DebugInConsole()
{
    for (int j=0;j<column;++j){
        for(int k=0;k<rows;++k){
//            qDebug()<<cell [j][k];
            std::cout<<cell[k][j];
            if(k>=14) std::cout<<"\n";
        }
    }
}
