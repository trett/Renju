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

#ifndef RENJU_H
#define RENJU_H

#include <QMainWindow>
#include "GameWidget.h"
#include <QVBoxLayout>
#include <QtGui>
namespace Ui {
class Renju;
}

class Renju : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit Renju(QWidget *parent = 0);
    ~Renju();
     GameWidget gw;
     QVBoxLayout v_layout;

private slots:
    void on_action_triggered();
    void on_action_4_triggered();
    void on_action_3_triggered();
    void on_action_2_triggered();
    void endOfGame(QString str);

private:
    Ui::Renju *ui;
    QLabel lbl;
};

#endif // CROSS_H
