/*Copyright (C) <2012>  <trett>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "renju.h"
#include "ui_renju.h"
#include <QPainter>
#include <QtGui>

Renju::Renju(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Renju)
{
    ui->setupUi(this);
	setFixedSize(480, 515);
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
	QObject::connect(&gw, SIGNAL(sendStatus(QString)), ui->statusBar, SLOT(showMessage(QString)));
	QObject::connect(&gw, SIGNAL(sendEndOfGame(QString)), this, SLOT(endOfGame(QString)));
    QMessageBox *msg = new QMessageBox(this);
    msg->setText("Choose your color");
    QPushButton *b_button = msg->addButton(tr("black"), QMessageBox::ActionRole);
    QPushButton *w_button = msg->addButton(tr("white"), QMessageBox::ActionRole);
    msg->exec();
	if(msg->clickedButton() == w_button){
        gw.cmpFirstMove();
		gw.cmpFirst = true;
    }
	else if(msg->clickedButton() == b_button) gw.cmpFirst = false;
}

void Renju::on_action_4_triggered()
{
	close();
}

void Renju::on_action_3_triggered()
{
    if (ui->action_3->isChecked()) gw.soundOff = true;
    else gw.soundOff = false;
}

void Renju::on_action_2_triggered()
{
	QObject::connect(&gw, SIGNAL(enblRm(bool)), ui->action_2, SLOT(setEnabled(bool)));
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
	else if (str == "white")
        msgBox->setText("<FONT SIZE=15>You loose!</FONT>");
    msgBox->exec();
	QObject::disconnect(&gw, SIGNAL(sendEndOfGame(QString)), this, SLOT(endOfGame(QString)));
}
