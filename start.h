#ifndef START_H
#define START_H

#include <QWidget>
#include <QScreen>
#include <QApplication>
#include "./scoreTable/scoretable.h"
#include "./scoreTable/scoreinputdialog.h"
#include "./scoreTable/scoremanager.h"
#include <QMessageBox>
#include <QRandomGenerator>

namespace Ui {class Start;}

class GameWidget; //前置声明（作空载体，用于承载在.cpp文件中才引入game.h中的Widget，从而避免头文件相互引用）
//说人话就是在.cpp才引用需要的.h，而另一边又不直接引用.cpp文件，所以就不存在循环引用的问题咯

class Start : public QWidget
{
    Q_OBJECT

public:
    explicit Start(QWidget *parent = nullptr);
    //无参构造
    void initGame(); //加载游戏窗口

    ~Start();

public:
    GameWidget *game;

public slots:
    void scoreRecord(int end_score, int end_combo);
    
    // 显示游戏结束界面
    void showGameOverDialog();

private slots:
    void switchToGame();

    void on_showScoreTableButton_clicked();
    void onScoreTableTypeChanged(ScoreTableType newType);
    
    // 处理排行榜关闭的槽函数
    void onScoreTableClosed();

    // 新增网络相关槽函数
    void onWorldRankingsUpdated();
    void onNetworkError(const QString &errorMessage);
    void onSyncCompleted(bool success);
    void onScoreUploadCompleted(bool success);

private:
    Ui::Start *ui;
    ScoreTable *scoreTable;
    ScoreManager *scoreManager;
    
    // 标记是否需要显示游戏结束界面
    bool shouldShowGameOver;

    // 显示排行榜
    void showScoreTable();

    // 加载数据
    void loadScoreData();

    // 生成随机分数（模拟游戏结束）
    int generateRandomScore();

    // 检查是否创造了新高记录
    bool checkIfNewRecord(int score);

    // 显示自定义玩家名称输入对话框
    QString getPlayerNameInput(int score, int combo, bool isNewRecord);

    // 关闭分数排行榜
    void closeScoreTable();

};

#endif // START_H
