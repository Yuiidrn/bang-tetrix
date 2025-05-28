#ifndef MAINMENU_H
#define MAINMENU_H

#include "./scoreTable/scoretable.h"
#include "./scoreTable/scoremanager.h"
#include <QWidget>
#include <QScreen>
#include <QApplication>
#include <QMessageBox>
#include <QRandomGenerator>

namespace Ui {class Mainmenu;}

class GameWidget; //前置声明（作空载体，用于承载在.cpp文件中才引入game.h中的Widget，从而避免头文件相互引用）
//说人话就是在.cpp中才引用需要的.h，而另一边又不直接引用.cpp文件，所以就不存在循环引用的问题咯

class Mainmenu : public QWidget
{
    Q_OBJECT

public:
    explicit Mainmenu(QWidget *parent = nullptr);

    void initGame(); //加载游戏窗口
    ScoreTable *getScoreTable() const {return scoreTable; }
    ScoreManager *getScoreManager()const {return scoreManager; }

    void showScoreTable();      // 显示排行榜
    void loadScoreData();       // 加载数据
    void closeScoreTable();     // 关闭分数排行榜
    ~ Mainmenu();

public:
    GameWidget *game;

private slots:
    void switchToGame();  //跳转至游戏窗口

    void on_showScoreTableButton_clicked();
    void onScoreTableTypeChanged(ScoreTableType newType);

    // 网络相关槽函数
    void onWorldRankingsUpdated();
    void onNetworkError(const QString &errorMessage);
    void onSyncCompleted(bool success);

private:
    Ui::Mainmenu *ui;
    ScoreTable *scoreTable;
    ScoreManager *scoreManager;

};

#endif // MAINMENU_H
