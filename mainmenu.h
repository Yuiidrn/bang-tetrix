#ifndef MAINMENU_H
#define MAINMENU_H

#include "./scoreTable/scoretable.h"
#include "./scoreTable/scoremanager.h"
#include "scoreinputdialog.h"
#include <QWidget>
#include <QScreen>
#include <QApplication>
#include <QMessageBox>
#include <QRandomGenerator>

namespace Ui {class Mainmenu;}

class GameWidget; //前置声明（作空载体，用于承载在.cpp文件中才引入game.h中的Widget，从而避免头文件相互引用）
//说人话就是在.cpp才引用需要的.h，而另一边又不直接引用.cpp文件，所以就不存在循环引用的问题咯

class Mainmenu : public QWidget
{
    Q_OBJECT

public:
    explicit Mainmenu(QWidget *parent = nullptr);
    //无参构造
    void initGame(); //加载游戏窗口

    ~ Mainmenu();

public:
    GameWidget *game;

public slots:
    void scoreRecord(int end_score, int end_combo);
    
    // 显示游戏结束界面
    void showGameOverDialog();

private slots:
    void switchToGame();  //跳转至游戏窗口

    void on_showScoreTableButton_clicked();
    void onScoreTableTypeChanged(ScoreTableType newType);
    
    // 处理排行榜关闭的槽函数
    void onScoreTableClosed();

    // 网络相关槽函数
    void onWorldRankingsUpdated();
    void onNetworkError(const QString &errorMessage);
    void onSyncCompleted(bool success);
    void onScoreUploadCompleted(bool success);

private:
    Ui::Mainmenu *ui;
    ScoreTable *scoreTable;
    ScoreManager *scoreManager;
    
    bool shouldShowGameOver; // 标记是否需要显示游戏结束界面

    //游戏结束信息输入相关函数
    void showScoreTable();              // 显示排行榜
    void loadScoreData();               // 加载数据
    bool checkIfNewRecord(int score);   // 检查是否创造了新高记录
    QString getPlayerNameInput(int score, int combo, bool isNewRecord);     // 显示自定义玩家名称输入对话框
    void closeScoreTable();             // 关闭分数排行榜

};

#endif // MAINMENU_H
