#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QPixmap>
#include "GameWidget.h"
#include <iostream>
#include <QMessageBox>
#include <QtGui>
#if defined Q_WS_WIN
#include <QSound>
#elif defined Q_WS_X11
#include <QProcess>
#endif

#define MIN 0
#define MAX 14
#define COLUMN 15
#define ROWS 15

GameWidget::GameWidget(QWidget *parent):QWidget(parent),pix(470,470)
{
    x=x1=0;
    y=y1=0;
    counter=0;
    this->setAttribute(Qt::WA_StyledBackground);
    this->setStyleSheet("background: #FFDEAD; border: 2px solid black; border-radius: 5px");
    this->setFixedSize(460,460);
    QColor color(0,0,0,0);
    pix.fill(color);
    QBuffer buffer(&array);
    buffer.open(QIODevice::WriteOnly);
    pix.save(&buffer,"PNG");

}
GameWidget::~GameWidget()
{
    //this->DebugInConsole();
    for (int i=0; i<COLUMN;++i){
        delete []table[i];
        delete []rating[i];
    }
    delete [] table;
    delete [] rating;
}

void GameWidget::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing,true);
    for (int i=0; i<16; ++i){
        p.drawLine(20,i*30+20,440,i*30+20);
        p.drawLine(i*30+20,20,i*30+20,440);
    }
    p.setBrush(QBrush(Qt::black));
    p.drawEllipse(225,225,10,10);
    p.drawEllipse(105,105,10,10);
    p.drawEllipse(345,105,10,10);
    p.drawEllipse(105,345,10,10);
    p.drawEllipse(345,345,10,10);
    p.drawPixmap(0,0,pix);
    p.end();
}
void GameWidget::mousePressEvent(QMouseEvent *pe)
{
    if (pe->x()>=10 && pe->y()>=10 && pe->x()<=450 && pe->y()<=450){
        pointX = ((qRound((double((pe->x()-20))/30)))*30)+20;
        pointY = ((qRound((double((pe->y()-20))/30)))*30)+20;
        x=(pointX-20)/30;
        y=(pointY-20)/30;
        game();
    }
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
        }
    }
    win=false;
    counter=0;
}

void GameWidget::moveFirst()
{
    audio();
    QPainter p2(&pix);
    p2.setRenderHint(QPainter::Antialiasing,true);
    if(cmpFirst==true) p2.setBrush(QBrush(Qt::white));
    else p2.setBrush(QBrush(Qt::black));
    p2.drawEllipse(QRect(pointX-13,pointY-13,26,26));
    this->repaint();
}
void GameWidget::moveSecond()
{
    generator();
    if (table[x1][y1]!=1 && table[x1][y1]!=2){
        QPainter p3(&pix);
        p3.setRenderHint(QPainter::Antialiasing,true);
        if(cmpFirst==true) p3.setBrush(QBrush(Qt::black));
        else p3.setBrush(QBrush(Qt::white));
        p3.drawEllipse(QRect(x1*30+7,y1*30+7,26,26));
        this->repaint();
    }
    else moveSecond();
}
void GameWidget::conversionMove(int cx, int cy, int cl)
{   
    if (cl==1){
        table[cx][cy]=1;
    }
    else if(cl==2){
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
    std::cout<<"\n \n";
    for (int j=0;j<15;++j){
        for(int k=0;k<15;++k){
            std::cout<<table[k][j]<<" ";
            if(k>=14) std::cout<<"\n";
        }
    }
}
int GameWidget::checkTable(int dx,int dy,int color)
{
    if (checkDst(dx, dy, 1, 0, color) >= 5 || checkDst(dx, dy, 0, 1, color) >= 5 ||
            checkDst(dx, dy, 1, 1, color) >= 5  || checkDst(dx, dy, 1, -1, color) >= 5){
        if (color==1){
            showWin("black");

        }
        else if (color==2){
            showWin("white");
        }
        return 1;
    }
    else return 0;
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

            if (checkDst(i,j,0,1,0)>=5) rating[i][j]++;
            if (checkDst(i,j,1,0,0)>=5) rating[i][j]++;
            if (checkDst(i,j,1,1,0)>=5) rating[i][j]++;
            if (checkDst(i,j,1,-1,0)>=5) rating[i][j]++;

            // для белых

            if (checkRatingH(i,j,2)==1) rating[i][j]++;
            if (checkRatingH(i,j,2)==2) rating[i][j]+=3;
            if (checkRatingH(i,j,2)==3) rating[i][j]+=15;
            if (checkRatingH(i,j,2)==4) rating[i][j]+=50;  //35
            if (checkRatingH(i,j,2)>=6) rating[i][j]+=30;  //16

            if (checkRatingV(i,j,2)==1) rating[i][j]++;
            if (checkRatingV(i,j,2)==2) rating[i][j]+=3;
            if (checkRatingV(i,j,2)==3) rating[i][j]+=15;
            if (checkRatingV(i,j,2)==4) rating[i][j]+=50;
            if (checkRatingV(i,j,2)>=6) rating[i][j]+=30;

            if (checkRatingD1(i,j,2)==1) rating[i][j]++;
            if (checkRatingD1(i,j,2)==2) rating[i][j]+=3;
            if (checkRatingD1(i,j,2)==3) rating[i][j]+=15;
            if (checkRatingD1(i,j,2)==4) rating[i][j]+=50;
            if (checkRatingD1(i,j,2)>=6) rating[i][j]+=30;

            if (checkRatingD2(i,j,2)==1) rating[i][j]++;
            if (checkRatingD2(i,j,2)==2) rating[i][j]+=3;
            if (checkRatingD2(i,j,2)==3) rating[i][j]+=15;
            if (checkRatingD2(i,j,2)==4) rating[i][j]+=50;
            if (checkRatingD2(i,j,2)>=6) rating[i][j]+=30;

            // для черных

            if (checkRatingH(i,j,1)==1) rating[i][j]++;
            if (checkRatingH(i,j,1)==2) rating[i][j]+=3;
            if (checkRatingH(i,j,1)==3) rating[i][j]+=15;
            if (checkRatingH(i,j,1)==4) rating[i][j]+=49; //34
            if (checkRatingH(i,j,1)>=6) rating[i][j]+=29; //15

            if (checkRatingV(i,j,1)==1) rating[i][j]++;
            if (checkRatingV(i,j,1)==2) rating[i][j]+=3;
            if (checkRatingV(i,j,1)==3) rating[i][j]+=15;
            if (checkRatingV(i,j,1)==4) rating[i][j]+=49;
            if (checkRatingV(i,j,1)>=6) rating[i][j]+=29;

            if (checkRatingD1(i,j,1)==1) rating[i][j]++;
            if (checkRatingD1(i,j,1)==2) rating[i][j]+=3;
            if (checkRatingD1(i,j,1)==3) rating[i][j]+=15;
            if (checkRatingD1(i,j,1)==4) rating[i][j]+=49;
            if (checkRatingD1(i,j,1)>=6) rating[i][j]+=29;

            if (checkRatingD2(i,j,1)==1) rating[i][j]++;
            if (checkRatingD2(i,j,1)==2) rating[i][j]+=3;
            if (checkRatingD2(i,j,1)==3) rating[i][j]+=15;
            if (checkRatingD2(i,j,1)==4) rating[i][j]+=49;
            if (checkRatingD2(i,j,1)>=6) rating[i][j]+=29;
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
    qDebug()<<max;
    clearRating();
}

void GameWidget::showWin(QString str)
{
//    QMessageBox *msgBox = new QMessageBox(this);
//    msgBox->setStyleSheet();
//    msgBox->setTextFormat(Qt::RichText);
//    msgBox->setIcon(QMessageBox::Information);
//    if (str=="black")
//        msgBox->setText("<FONT SIZE=15>You Win!</FONT>");
//    else if (str=="white")
//        msgBox->setText("<FONT SIZE=15>You loose!</FONT>");
//    msgBox->exec();
    emit sendEndOfGame(str);
}

int GameWidget::checkRatingH(int mx, int my, int a)
{
    int cx = mx, cy = my;
    int c = 0, i = 1;
    if(table[mx][my]==0){
        while (cx+i <= MAX && table[cx+i][cy] == a && i<5) {
            ++c;
            ++i;
            if(cx+i<MAX)
                if(c==3 && table[cx+i][cy]==0) c=6;
        }
        i=1;
        while (cx-i >= MIN && table[cx-i][cy] == a && i<5 ) {
            ++c;
            i++;
            if (cx-i>MIN)
                if(c==3 && table[cx-i][cy]==0) c=6;
        }
    }
    return c;
}

int GameWidget::checkRatingV(int mx, int my, int a)
{
    int cx = mx, cy = my;
    int c = 0, i = 1;
    if(table[mx][my]==0){
        while (cy+i <= MAX && table[cx][cy+i] == a && i<5) {
            ++c;
            ++i;
            if(cy+i<=MAX)
                if(c==3 && table[cx][cy+i]==0) c=6;
        }
        i=1;
        while (cy-i >= MIN && table[cx][cy-i] == a && i<5 ) {
            ++c;
            i++;
            if(cx-i>=MIN)
                if(c==3 && table[cx][cy-i]==0) c=6;
        }
    }
    return c;
}

int GameWidget::checkRatingD1(int mx, int my, int a)
{
    int cx = mx, cy = my;
    int c = 0, i = 1;
    if(table[mx][my]==0){
        while (cx+i <= MAX && cy+i <= MAX && table[cx+i][cy+i] == a && i<5) {
            ++c;
            ++i;
            if(cx+i<=MAX && cy+i<=MAX)
                if(c==3 && table[cx+i][cy+i]==0) c=6;
        }
        i=1;
        while (cx-i>= MIN && cy-i >= MIN && table[cx-i][cy-i] == a && i<5 ) {
            ++c;
            i++;
            if(cx-i>= MIN && cy-i >= MIN)
                if(c==3 && table[cx-i][cy-i]==0) c=6;
        }
    }
    return c;
}

int GameWidget::checkRatingD2(int mx, int my, int a)
{
    int cx = mx, cy = my;
    int c = 0, i = 1;
    if(table[mx][my]==0){
        while (cx+i <= MAX && cy-i >= MIN && table[cx+i][cy-i] == a && i<5) {
            ++c;
            ++i;
            if (cx+i <= MAX && cy-i >= MIN)
                if(c==3 && table[cx+i][cy-i]==0) c=6;
        }
        i=1;
        while (cx-i>= MIN && cy+i <= MAX && table[cx-i][cy+i] == a && i<5 ) {
            ++c;
            i++;
            if (cx-i>= MIN && cy+i <= MAX)
                if(c==3 && table[cx-i][cy+i]==0) c=6;
        }
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

void GameWidget::game()
{
    QBuffer buffer2(&array2);
    buffer2.open(QIODevice::WriteOnly);
    pix.save(&buffer2,"PNG");
    if (table[x][y]!=1 && table[x][y]!=2 && win!=true){
        this->moveFirst();
        this->conversionMove(x,y,1);
        if (checkTable(x,y,1)!=1) {
            counter++;
            this->moveSecond();
            this->conversionMove(x1,y1,2);
            if(checkTable(x1,y1,2)==1) win=true;
            counter++;
            emit enblRm(true);
            status = "Move: "+(QString::number(counter))+"      Black: "+(QString::number(x))\
                    +"-"+(QString::number(y))+"         White: "\
                    +(QString::number(x1))+"-"+(QString::number(y1));
            emit sendStatus(status);
        }
        else win=true;
    }
    else status="Invalid move!";
    emit sendStatus(status);
}

void GameWidget::audio()
{
    if(soundOff==true){
#if defined Q_WS_WIN
        QSound::play("click.wav")
        #elif defined Q_WS_X11
        QProcess::startDetached("aplay -q click.wav");
#endif
    }
}

void GameWidget::rmMove()
{
    pix.loadFromData(array2);
    counter+=-2;
    table[x][y]=0;
    table[x1][y1]=0;
    this->update();
}

void GameWidget::cmpFirstMove()
{
    x1=y1=7;
    table[x1][y1]=0;
    conversionMove(x1,y1,2);
    counter++;
    QPainter p4(&pix);
    p4.setRenderHint(QPainter::Antialiasing,true);
    p4.setBrush(QBrush(Qt::black));
    p4.drawEllipse(QRect(x1*30+7,y1*30+7,26,26));
    this->repaint();
}
