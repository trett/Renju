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
    gw.clearTable();
    v_layout.addWidget(&gw);
    gw.update();
}

void Renju::on_action_4_triggered()
{
    this->close();
}
