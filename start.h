#ifndef START_H
#define START_H

#include <QWidget>
#include <QScreen>
#include <QApplication>

namespace Ui {class Start;}

class Widget; //前置声明（作空载体，用于承载在.cpp文件中才引入game.h中的Widget，从而避免头文件相互引用）
//——人话就是在.cpp才引用需要的.h，而另一边又不直接引用.cpp文件，所以就不存在循环引用的问题咯

class Start : public QWidget
{
    Q_OBJECT

public:
    explicit Start(QWidget *parent = nullptr);
    //无参构造
    void initGame(); //加载游戏窗口

    ~Start();

public:
    Widget *game;

private slots:
    void switchToGame();

private:
    Ui::Start *ui;  
};

#endif // START_H
