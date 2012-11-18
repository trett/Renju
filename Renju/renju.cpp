#include "renju.h"
#include "ui_renju.h"
#include <QPainter>
#include <QtGui>

Renju::Renju(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Renju)
{
    ui->setupUi(this);
    this->setFixedSize(480,515);
    ui->centralWidget->setLayout(&v_layout);
    QPixmap pix(":/icons/renju2.jpg");
    lbl.setPixmap(pix);
    v_layout.addWidget(&lbl);
    gw.clearTable();
}

Renju::~Renju()
{
    delete ui;
}

void Renju::on_action_triggered()
{
    lbl.close();
    if (ui->action_3->isChecked()) gw.soundOff = true;
    else gw.soundOff = false;
    gw.clearTable();
    v_layout.addWidget(&gw);
    gw.update();
    QObject::connect(&gw,SIGNAL(sendStatus(QString)),ui->statusBar,SLOT(showMessage(QString)));
    QObject::connect(&gw,SIGNAL(sendEndOfGame(QString)),this,SLOT(endOfGame(QString)));
    QMessageBox *msg = new QMessageBox(this);
    msg->setText("Choose your color");
    QPushButton *b_button = msg->addButton(tr("black"), QMessageBox::ActionRole);
    QPushButton *w_button = msg->addButton(tr("white"), QMessageBox::ActionRole);
    msg->exec();
    if(msg->clickedButton()==w_button){
        gw.cmpFirstMove();
        gw.cmpFirst=true;
    }
    else if(msg->clickedButton()==b_button) gw.cmpFirst=false;
}

void Renju::on_action_4_triggered()
{
    this->close();
}

void Renju::on_action_3_triggered()
{
    if (ui->action_3->isChecked()) gw.soundOff = true;
    else gw.soundOff = false;
}

void Renju::on_action_2_triggered()
{
    QObject::connect(&gw,SIGNAL(enblRm(bool)),ui->action_2,SLOT(setEnabled(bool)));
    gw.rmMove();
    ui->action_2->setEnabled(false);
}

void Renju::endOfGame(QString str)
{
    QMessageBox *msgBox = new QMessageBox(this);
    msgBox->setTextFormat(Qt::RichText);
    msgBox->setIcon(QMessageBox::Information);
    if (str=="black")
        msgBox->setText("<FONT SIZE=15>You Win!</FONT>");
    else if (str=="white")
        msgBox->setText("<FONT SIZE=15>You loose!</FONT>");
    msgBox->exec();
    QObject::disconnect(&gw,SIGNAL(sendEndOfGame(QString)),this,SLOT(endOfGame(QString)));
}
