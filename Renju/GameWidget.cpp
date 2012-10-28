#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QPixmap>
#include "GameWidget.h"
#include <iostream>
#define MIN 0
#define MAX 14
GameWidget::GameWidget(QWidget *parent):QWidget(parent),pix(600,600),column(15),rows(15),x(0),y(0),
    counter(1),x1(0),y1(0)
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
    this->moveFirst();
    this->conversionMove();
    this->checkTable();
    counter++;
    this->moveSecond();
    this->conversionMove();
    counter++;

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
    x1=rand()%15;
    cx=x1*40+10; //
    y1=rand()%15;
    cy=y1*40+10; // пока рандомно
    p3.drawEllipse(QRect(cx,cy,20,20));
    //qDebug()<<x1<<"         "<<y1;
    this->repaint();

}
void GameWidget::conversionMove()
{

    bool odd = !!(counter & 1);
    // qDebug()<<cellX<<"  "<<cellY;
    if (odd==true){
        x=(pointX-20)/40;
        y=(pointY-20)/40;
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
void GameWidget::checkTable()
{
    //  qDebug()<<"x y"<<x<<""<<y;
    int fiveRows,cx,cy;
    fiveRows=0;
    cx=x;
    cy=y;
    while(cx>(x-5) && cx>=0){
        for (int i=0;i<5;++i){
            if(cx>=MIN && cx+i<=MAX){
                // qDebug()<<"x  "<<c+i;
                if (table[cx+i][y]==1)
                    fiveRows++;
            }
        }
        // qDebug()<<"Xrows "<<fiveRows;
        if (fiveRows==5){
            qDebug()<<"X You win";
            fiveRows=0;
        }
        else {fiveRows=0;}
        cx--;
    }
    while(cy>(y-5) && cy>=0){
        for (int i=0;i<5;++i){
            if(cy>=MIN && cy+i<=MAX){
                //               qDebug()<<"y "<<cy+i;
                if (table[x][cy+i]==1)
                    fiveRows++;
            }
        }
       // qDebug()<<"Yrows "<<fiveRows;
        if (fiveRows==5){
            qDebug()<<"Y You win";
            fiveRows=0;
        }
        else {fiveRows=0;}
        cy--;
    }
    cx=x;
    cy=y;
    while(cy>(y-5) && cx>(x-5)){
        for (int i=0;i<5;++i){
            if(cy>=MIN && cy+i<=MAX && cx>=MIN && cx+i<=MAX){
                //             qDebug()<<cx+i<<"  "<<cy+i;
                if (table[cx+i][cy+i]==1)
                    fiveRows++;
            }
        }
        //qDebug()<<"Arows "<<fiveRows;
        if (fiveRows==5){
            qDebug()<<"A You win";
            fiveRows=0;
        }
        else {fiveRows=0;}
        cy--;
        cx--;
    }
    cx=x;
    cy=y;
    while(cy<(y+5) && cx>(x-5)){
        for (int i=0;i<5;++i){
            if(cy>=MIN && cy+i<=MAX && cx>=MIN && cx+i<=MAX){
                // qDebug()<<cx+i<<"  "<<cy-i;
                if (table[cx+i][cy-i]==1)
                    fiveRows++;
            }
        }
        //qDebug()<<"Brows "<<fiveRows;
        if (fiveRows==5){
            qDebug()<<"B You win";
            fiveRows=0;
        }
        else {fiveRows=0;}
        cy++;
        cx--;
    }
}
