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

private:
    Ui::Renju *ui;
    QLabel lbl;
};

#endif // CROSS_H
