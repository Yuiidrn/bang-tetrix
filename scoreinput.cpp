#include "scoreinput.h"
#include "mainmenu.h"
#include "game.h"

ScoreInput::ScoreInput(QWidget *parent)
{
    menu = new Mainmenu();
    scoreTable = new ScoreTable();
    scoreManager = new ScoreManager();

    shouldShowGameOver = false;
}

void ScoreInput::initMenu(Mainmenu* &gameInstance_menu){
    menu = gameInstance_menu;
    scoreTable = menu->getScoreTable();
    scoreManager = menu->getScoreManager();
    //信号广播者必须是连接当前游戏的具体实例! 不是放上面刚创的实例去connect!
    connect(scoreTable, &ScoreTable::closed, this, &ScoreInput::onScoreTableClosed);
    connect(scoreManager, &ScoreManager::scoreUploadCompleted, this, &ScoreInput::onScoreUploadCompleted);
}

void ScoreInput::scoreRecord(int end_score, int end_combo)
{
    // 录入分数分数
    int score = end_score;
    int combo = end_combo;

    // 检查是否创造了新高记录
    bool isNewRecord = checkIfNewRecord(score);

    // 显示分数结算对话框并获取玩家名称，若是新高记录则提示用户
    QString playerName = getPlayerNameInput(score, combo, isNewRecord);

    // 如果玩家取消输入，不添加成绩，并直接显示游戏结束界面
    if (playerName.isEmpty()) {
        showGameOverDialog();
        return;
    }

    // 使用ScoreManager添加分数到本地
    scoreManager->addPersonalScore(playerName, score);
    scoreManager->addWorldPlayerScore(playerName, score);

    // 上传分数到服务器
    scoreManager->uploadScore(playerName, score);

    // 重新加载积分数据到表格
    menu->loadScoreData();

    // 设置标记，表示排行榜关闭后需要显示游戏结束界面
    shouldShowGameOver = true;

    // 显示排行榜
    menu->showScoreTable();
}

bool ScoreInput::checkIfNewRecord(int score)
{
    // 检查是否创造了个人历史记录最高分
    const QList<GameScore>& personalData = scoreManager->getPersonalHistoryScores();
    if (personalData.isEmpty() || score > personalData.first().score) {
        return true;
    }

    return false;
}

QString ScoreInput::getPlayerNameInput(int score, int combo, bool isNewRecord)
{
    ScoreInputDialog dialog(menu);

    QString defaultName = "新手工作人员" + QString::number(QRandomGenerator::global()->bounded(1000, 10000));
    dialog.setDefaultName(defaultName);  //设置默认用户名

    dialog.setScore(score, isNewRecord); //设置分数并判断是否为新高分
    dialog.setMaxCombo(combo);           //设置连击数

    // 设置背景图片（如果图片存在）
    if (QFile(":/imgs/img/ui/inputbg.jpg").exists()) {
        dialog.setBackgroundImage(":/imgs/img/ui/inputbg.jpg");
    }

    // 显示对话框并等待用户输入
    if (dialog.exec() == QDialog::Accepted) {
        return dialog.getPlayerName();
    } else {
        return QString();
    }
}

// 添加排行榜关闭的槽函数
void ScoreInput::onScoreTableClosed()
{
    // 如果需要显示游戏结束界面
    if (shouldShowGameOver) {
        shouldShowGameOver = false;  // 重置标记
        showGameOverDialog();        // 显示游戏结束界面
    }
}

// 显示游戏结束界面
void ScoreInput::showGameOverDialog()
{
    // 创建游戏结束对话框
    GameOverDialog dialog(menu);

    // 设置背景图片（如果存在）
    if (QFile(":/imgs/img/ui/inputbg.jpg").exists()) {
        dialog.setBackgroundImage(":/imgs/img/ui/inputbg.jpg");
    }

    // 显示对话框并等待用户选择
    dialog.exec();

    // 处理用户选择的按钮
    GameOverDialog::GameOverResult result = dialog.getResult();

    if (result == GameOverDialog::Restart) {
        menu->game->InitGame();      // 调用重新开始游戏的方法
    } else if (result == GameOverDialog::MainMenu) {
        menu->game->goToMainMenu();  // 调用返回主菜单的方法
    } else {
        QApplication::quit();  // 退出游戏
    }
}

void ScoreInput::onScoreUploadCompleted(bool success)
{
    if (success) {
        qDebug() << "分数上传成功";
    } else {
        qDebug() << "分数上传失败";
    }
}
