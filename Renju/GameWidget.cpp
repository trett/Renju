#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QPixmap>
#include "GameWidget.h"
#include <iostream>
#include <QMessageBox>

#define MIN 0
#define MAX 14
#define COLUMN 15
#define ROWS 15

GameWidget::GameWidget(QWidget *parent):QWidget(parent),pix(600,600)
{
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
    // qDebug("ia umer");
    this->DebugInConsole();
    for (int i=0; i<COLUMN;++i){
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
    // qDebug()<<table[x][y];
    if (table[x][y]!=1 && table[x][y]!=2){

        this->moveFirst();
        this->conversionMove(x,y);
        //qDebug()<<"x y"<<x<<y;
        this->checkTable(x,y,1);
        counter++;


        this->moveSecond();
        this->conversionMove(x1,y1);
        //qDebug()<<"x1 y1"<<x1<<y1;
        this->checkTable(x1,y1,2);
        counter++;
    }
    else qDebug("Impossible");
}

void GameWidget::clearTable()
{
    pix.loadFromData(array);
    table = new int *[COLUMN];
    rating = new int *[COLUMN];
    for (int i=0;i<COLUMN;++i){
        table[i]= new int[ROWS];
        rating[i] = new int[ROWS];
    }
    for (int j=0;j<COLUMN;++j){
        for(int k=0;k<ROWS;++k){
            table[j][k]=0;
            rating[j][k]=0;
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

    generator();
    if (table[x1][y1]!=1 && table[x1][y1]!=2){
        QPainter p3(&pix);
        p3.setRenderHint(QPainter::Antialiasing,true);
        p3.setBrush(QBrush(Qt::white));
        p3.drawEllipse(QRect(x1*40+10,y1*40+10,20,20));
        this->repaint();
    }
    else moveSecond();
}
void GameWidget::conversionMove(int cx, int cy)
{   
    odd = !!(counter & 1);
    if (odd==true){
        table[cx][cy]=1;
    }
    else {
        table[cx][cy]=2;
    }
}
void GameWidget::DebugInConsole()
{
    for (int j=0;j<15;++j){
        for(int k=0;k<15;++k){
            std::cout<<rating[k][j]<<" ";
            if(k>=14) std::cout<<"\n";
        }
    }
    //    std::cout<<"\n \n";
    //    for (int j=0;j<15;++j){
    //        for(int k=0;k<15;++k){
    //            std::cout<<table[k][j]<<" ";
    //            if(k>=14) std::cout<<"\n";
    //        }
    //    }
}
void GameWidget::checkTable(int dx,int dy,int color)
{
    //    if (odd=true){
    //        color=1;
    if (checkDst(dx, dy, 1, 0, color) >= 5 || checkDst(dx, dy, 0, 1, color) >= 5 || checkDst(dx, dy, 1, 1, color) >= 5  || checkDst(dx, dy, 1, -1, color) >= 5){
        if (color==1){
            showWin("black");
        }
        else if (color==2){
            showWin("white");
        }
    }
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
    cx = mx + dx;
    cy = my + dy;
    while (cx >= MIN && cx <= MAX && cy >= MIN && cy <= MAX && table[cx][cy] == a ) {
        ++c;
        cx += dx;
        cy += dy;
    }
    return c;
}
void GameWidget::generator()
{
    for (int j=0;j<COLUMN;++j){
        for(int k=0;k<ROWS;++k){
            rating[j][k]=0;
        }
    }

    for (int i=0; i<15; ++i){
        for (int j=0;j<15;++j){

            //для пустых полей

            if (checkDst(i,j,0,1,0)>5) rating[i][j]++;
            if (checkDst(i,j,1,0,0)>=5) rating[i][j]++;
            if (checkDst(i,j,1,1,0)>=5) rating[i][j]++;
            if (checkDst(i,j,1,-1,0)>=5) rating[i][j]++;

            // для белых

            if (checkRatingH(i,j,2)==1) rating[i][j]+=2;
            if (checkRatingH(i,j,2)==2) rating[i][j]+=3;
            if (checkRatingH(i,j,2)==3) rating[i][j]+=5;
            if (checkRatingH(i,j,2)==4) rating[i][j]+=21;

            if (checkRatingV(i,j,2)==1) rating[i][j]+=2;
            if (checkRatingV(i,j,2)==2) rating[i][j]+=3;
            if (checkRatingV(i,j,2)==3) rating[i][j]+=5;
            if (checkRatingV(i,j,2)==4) rating[i][j]+=21;

            if (checkRatingD1(i,j,2)==1) rating[i][j]+=2;
            if (checkRatingD1(i,j,2)==2) rating[i][j]+=3;
            if (checkRatingD1(i,j,2)==3) rating[i][j]+=5;
            if (checkRatingD1(i,j,2)==4) rating[i][j]+=21;

            if (checkRatingD2(i,j,2)==1) rating[i][j]+=2;
            if (checkRatingD2(i,j,2)==2) rating[i][j]+=3;
            if (checkRatingD2(i,j,2)==3) rating[i][j]+=5;
            if (checkRatingD2(i,j,2)==4) rating[i][j]+=21;

            // для черных

            if (checkRatingH(i,j,1)==1) rating[i][j]+=2;
            if (checkRatingH(i,j,1)==2) rating[i][j]+=3;
            if (checkRatingH(i,j,1)==3) rating[i][j]+=10;
            if (checkRatingH(i,j,1)==4) rating[i][j]+=20;

            if (checkRatingV(i,j,1)==1) rating[i][j]+=2;
            if (checkRatingV(i,j,1)==2) rating[i][j]+=3;
            if (checkRatingV(i,j,1)==3) rating[i][j]+=10;
            if (checkRatingV(i,j,1)==4) rating[i][j]+=20;

            if (checkRatingD1(i,j,1)==1) rating[i][j]+=2;
            if (checkRatingD1(i,j,1)==2) rating[i][j]+=3;
            if (checkRatingD1(i,j,1)==3) rating[i][j]+=10;
            if (checkRatingD1(i,j,1)==4) rating[i][j]+=20;

            if (checkRatingD2(i,j,1)==1) rating[i][j]+=2;
            if (checkRatingD2(i,j,1)==2) rating[i][j]+=3;
            if (checkRatingD2(i,j,1)==3) rating[i][j]+=10;
            if (checkRatingD2(i,j,1)==4) rating[i][j]+=20;
        }
    }
    // выбор большего рейтинга
    int cx=0;
    int cy=0;
    int max=0;
    for (int j=0; j<15;++j){
        for (int k=0; k<15; ++k){
            if(max<rating[j][k] && table[j][k]==0){
                max=rating[j][k];
                cx=j;
                cy=k;
            }
        }
    }
    x1=cx;
    y1=cy;
    clearRating();
    qDebug()<<cx<<cy;
}

void GameWidget::showWin(QString str)
{
    QMessageBox msgBox;
    if (str=="black")
        msgBox.setText("<FONT COLOR=green SIZE=20>You win!</FONT>");
    else if (str=="white")
        msgBox.setText("<FONT COLOR=green SIZE=20>You loose!</FONT>");
    msgBox.exec();
}

int GameWidget::checkRatingH(int mx, int my, int a)
{
    int cx = mx, cy = my;
    int c = 0, i = 1;
    while (cx >= MIN && cx+i <= MAX && cy >= MIN && cy <= MAX && table[cx+i][cy] == a && i<5) {
        ++c;
        ++i;
    }
    i=1;
    while (cx-i >= MIN && cx <= MAX && cy >= MIN && cy <= MAX && table[cx-i][cy] == a && i<5 ) {
        ++c;
        i++;
    }
    return c;

}

int GameWidget::checkRatingV(int mx, int my, int a)
{
    int cx = mx, cy = my;
    int c = 0, i = 1;
    while (cx >= MIN && cx+i <= MAX && cy >= MIN && cy+i <= MAX && table[cx][cy+i] == a && i<5) {
        ++c;
        ++i;
    }
    i=1;
    while (cx>= MIN && cx <= MAX && cy-i >= MIN && cy <= MAX && table[cx][cy-i] == a && i<5 ) {
        ++c;
        i++;
    }
    return c;
}

int GameWidget::checkRatingD1(int mx, int my, int a)
{
    int cx = mx, cy = my;
    int c = 0, i = 1;
    while (cx >= MIN && cx+i <= MAX && cy >= MIN && cy+i <= MAX && table[cx+i][cy+i] == a && i<5) {
        ++c;
        ++i;
    }
    i=1;
    while (cx-i>= MIN && cx <= MAX && cy-i >= MIN && cy <= MAX && table[cx-i][cy-i] == a && i<5 ) {
        ++c;
        i++;
    }
    return c;
}

int GameWidget::checkRatingD2(int mx, int my, int a)
{
    int cx = mx, cy = my;
    int c = 0, i = 1;
    while (cx >= MIN && cx+i <= MAX && cy-i >= MIN && cy <= MAX && table[cx+i][cy-i] == a && i<5) {
        ++c;
        ++i;
    }
    i=1;
    while (cx-i>= MIN && cx <= MAX && cy-i >= MIN && cy+i <= MAX && table[cx-i][cy+i] == a && i<5 ) {
        ++c;
        i++;
    }
    return c;
}
void GameWidget::clearRating()
{
    for (int j=0; j<COLUMN; ++j){
        for (int k=0; k<ROWS; ++k){
            rating[j][k]=0;
        }
    }
}
