#include "renju.h"
#include "ui_renju.h"
#include <QPainter>
#include <QtGui>

Renju::Renju(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Renju)
{
    ui->setupUi(this);
    this->setFixedSize(470,510);
    ui->centralWidget->setLayout(&v_layout);
}

Renju::~Renju()
{
    delete ui;
}

void Renju::on_action_triggered()
{
    gw.soundOff = true;
    gw.clearTable();
    v_layout.addWidget(&gw);
    gw.update();
    QObject::connect(&gw,SIGNAL(sendStatus(QString)),ui->statusBar,SLOT(showMessage(QString)));
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
