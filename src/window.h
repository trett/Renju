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

#pragma once

#include "gameboard.h"

#include <QMainWindow>
#include <QVBoxLayout>
#include <QtGui>
#include <QLabel>

namespace Ui {
class Window;
}

class Window : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit Window(QWidget *parent = 0);
    ~Window();
    GameBoard gameBoard;
    QVBoxLayout v_layout;

private slots:
    void onNewGame();
    void onCancelLastMove();
    void onSoundCheck();
    void onExit();
    void endOfGame(QString str);

private:
    Ui::Window *ui;
    QLabel lbl;
};

