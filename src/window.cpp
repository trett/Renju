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

#include "ui_window.h"
#include "window.h"

#include <QPainter>
#include <QtGui>
#include <QPushButton>
#include <QMessageBox>

Window::Window(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Window)
{
	ui->setupUi(this);
	setFixedSize(480, 515);
	ui->centralWidget->setLayout(&v_layout);
	QPixmap pix(":/icons/splash.jpg");
	lbl.setPixmap(pix);
	v_layout.addWidget(&lbl);
	QObject::connect(ui->newGameAction, SIGNAL(triggered()), this, SLOT(onNewGame()));
	QObject::connect(ui->cancelLastMoveAction, SIGNAL(triggered()), this, SLOT(onCancelLastMove()));
	QObject::connect(ui->exitAction, SIGNAL(triggered()), this, SLOT(onExit()));
	gameBoard.clearTable();
}

Window::~Window()
{
	delete ui;
}

void Window::onNewGame()
{
    QObject::connect(&gameBoard, SIGNAL(sendStatus(QString)), ui->statusBar, SLOT(showMessage(QString)));
    QObject::connect(&gameBoard, SIGNAL(sendEndOfGame(QString)), this, SLOT(endOfGame(QString)));
    lbl.close();
    if (ui->soundAction->isChecked()) gameBoard.soundBox = true;
    else gameBoard.soundBox = false;
    gameBoard.clearTable();
    v_layout.addWidget(&gameBoard);
    gameBoard.update();
    QMessageBox *msg = new QMessageBox(this);
    msg->setText("Choose your color");
    QPushButton *b_button = msg->addButton(tr("black"), QMessageBox::ActionRole);
    QPushButton *w_button = msg->addButton(tr("white"), QMessageBox::ActionRole);
    msg->exec();
    if(msg->clickedButton() == w_button){
        gameBoard.cmpFirstMove();
        gameBoard.cmpFirst = true;
    }
    else if(msg->clickedButton() == b_button) gameBoard.cmpFirst = false;
}

void Window::onExit()
{
    close();
}

void Window::onSoundCheck()
{
    if (ui->soundAction->isChecked()) gameBoard.soundBox = true;
    else gameBoard.soundBox = false;
}

void Window::onCancelLastMove()
{
    QObject::connect(&gameBoard, SIGNAL(enblRm(bool)), ui->cancelLastMoveAction, SLOT(setEnabled(bool)));
    gameBoard.rmMove();
    ui->cancelLastMoveAction->setEnabled(false);
}

void Window::endOfGame(QString str)
{
    QMessageBox *msgBox = new QMessageBox(this);
    msgBox->setTextFormat(Qt::RichText);
    msgBox->setIcon(QMessageBox::Information);
    if (str == "black")
        msgBox->setText("You Win!");
    else if (str == "white")
        msgBox->setText("You loose!");
    msgBox->exec();
    QObject::disconnect(&gameBoard, SIGNAL(sendEndOfGame(QString)), this, SLOT(endOfGame(QString)));
}
