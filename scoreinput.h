#ifndef SCOREINPUT_H
#define SCOREINPUT_H

#include "./scoreTable/scoretable.h"
#include "./scoreTable/scoremanager.h"
#include "gameoverdialog.h"
#include "scoreinputdialog.h"
#include <QObject>
#include <QWidget>
#include <QScreen>
#include <QApplication>
#include <QMessageBox>
#include <QRandomGenerator>

class GameWidget; //前置声明（作空载体，用于承载在.cpp文件中才引入game.h中的Widget，从而避免头文件相互引用）
class Mainmenu;   //同上

class ScoreInput: public QObject {
    Q_OBJECT

public:
    explicit ScoreInput(QWidget *parent = nullptr);

    void initMenu(Mainmenu* &gameInstance_menu);

public slots:
    void scoreRecord(int end_score, int end_combo);

    void showGameOverDialog();  // 显示游戏结束界面
    void onScoreTableClosed();  // 录入分数并关闭弹出排行榜后呼出游戏结束界面

    void onScoreUploadCompleted(bool success); //上传分数成功

private:
    Mainmenu *menu;
    ScoreTable *scoreTable;
    ScoreManager *scoreManager;

    bool shouldShowGameOver; // 标记是否需要显示游戏结束界面
    bool checkIfNewRecord(int score);   // 检查是否创造了新高记录
    QString getPlayerNameInput(int score, int combo, bool isNewRecord);     // 显示自定义玩家名称输入对话框

};

#endif // SCOREINPUT_H
