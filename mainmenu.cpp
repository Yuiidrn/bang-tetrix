#include "mainmenu.h"
#include "game.h"
#include "ui_mainmenu.h"
#include "gameoverdialog.h"
#include <QApplication>
#include <QScreen>
#include <QDateTime>
#include <QDir>
#include <QFile>

Mainmenu::Mainmenu(QWidget *parent) :
    QWidget(parent)
    , ui(new Ui::Mainmenu)
    , scoreTable(nullptr)
    , scoreManager(nullptr)
    , shouldShowGameOver(false)
{
    ui->setupUi(this);
    int defaultWidth = 1200, defaultHeight = 675;
    this->setWindowTitle("BanG_Tetrix!");
    this->resize(defaultWidth, defaultHeight);
    //直接通过ui指针访问图片进行尺寸修改（可视化编程而不使用paintEvent（因直接套的非ui设计的主体代码，在游戏主体中没用上））
    ui->bg->setFixedSize(defaultWidth, defaultHeight);

    //界面切换槽函数（也可使用窗口类自带的keyPressEvent()）
    connect(ui->begin, &QPushButton::clicked, this, [=](){switchToGame();}); //槽函数需要lambda表达式封装！

    // 创建分数管理器
    scoreManager = new ScoreManager(this);

    // 加载本地存储的积分数据
    if (scoreManager->loadLocalScores()) {
        qDebug() << "成功从本地存储加载分数数据";
    } else {
        qDebug() << "从本地存储加载分数数据失败";
    }

    // 连接ScoreManager的信号和槽
    connect(scoreManager, &ScoreManager::worldRankingsUpdated, this, &Mainmenu::onWorldRankingsUpdated);
    connect(scoreManager, &ScoreManager::networkError, this, &Mainmenu::onNetworkError);
    connect(scoreManager, &ScoreManager::syncCompleted, this, &Mainmenu::onSyncCompleted);
    connect(scoreManager, &ScoreManager::scoreUploadCompleted, this, &Mainmenu::onScoreUploadCompleted);

    // 创建排行榜但不显示
    scoreTable = new ScoreTable(this);
    scoreTable->hide();

    // 加载数据到排行榜
    loadScoreData();

    // 连接排行榜关闭信号
    connect(scoreTable, &ScoreTable::typeChanged, this, &Mainmenu::onScoreTableTypeChanged);
    connect(scoreTable, &ScoreTable::closed, this, &Mainmenu::onScoreTableClosed);

    // 尝试与服务器同步
    // 设置服务器URL（如果有的话，http://yourApi.com/api/scores）
    scoreManager->setServerUrl("http://8.138.243.128:3000/api/scores");
    scoreManager->syncWithServer();
}
Mainmenu::~Mainmenu(){     // 保存积分数据
    if (scoreManager) {
        scoreManager->saveLocalScores();
    }

    delete ui;
}

void Mainmenu::initGame(){
    game = new GameWidget();
}

void Mainmenu::switchToGame()
{
    this->hide();
    this->game->initMenu(this); //为game的主菜单指针赋值！！
    this->game->show();
    this->game->InitGame();
}

void Mainmenu::loadScoreData()
{
    if (scoreTable) {
        // 获取个人历史记录
        QList<GameScore> personalHistory = scoreManager->getPersonalHistoryScores();
        scoreTable->setPersonalHistoryData(personalHistory);

        // 获取世界玩家排名
        QList<GameScore> worldPlayers = scoreManager->getWorldRankingScores();
        scoreTable->setWorldPlayersData(worldPlayers);
    }
}

void Mainmenu::on_showScoreTableButton_clicked()
{
    showScoreTable();
}

void Mainmenu::showScoreTable()
{
    if (!scoreTable) {
        scoreTable = new ScoreTable();
        scoreTable->setWindowTitle("分数排行榜");

        // 加载分数数据
        loadScoreData();

        connect(scoreTable, &ScoreTable::typeChanged, this, &Mainmenu::onScoreTableTypeChanged); // 连接类型变化信号
        connect(scoreTable, &ScoreTable::closed, this, &Mainmenu::onScoreTableClosed);           // 连接关闭信号
    }

    // 居中显示在主界面上
    QPoint center = this->mapToGlobal(this->rect().center());
    scoreTable->move(center.x() - scoreTable->width() / 2, center.y() - scoreTable->height() / 2);

    // 使用新的方法显示排行榜并安装全局事件过滤器
    scoreTable->showAndInstallFilter();
}

void Mainmenu::closeScoreTable()
{
    if (scoreTable && scoreTable->isVisible()) {
        scoreTable->hide();
    }
}

void Mainmenu::onScoreTableTypeChanged(ScoreTableType newType)
{
    qDebug() << "排行榜类型已更改为: " << (newType == PERSONAL_HISTORY ? "个人历史记录" : "世界玩家排名");

    // 当排行榜类型改变时，更新数据
    loadScoreData();
}

// 显示游戏结束界面
void Mainmenu::showGameOverDialog()
{
    // 创建游戏结束对话框
    GameOverDialog dialog(this);

    // 设置背景图片（如果存在）
    if (QFile(":/imgs/img/ui/inputbg.jpg").exists()) {
        dialog.setBackgroundImage(":/imgs/img/ui/inputbg.jpg");
    }

    // 显示对话框并等待用户选择
    dialog.exec();

    // 处理用户选择的按钮
    GameOverDialog::GameOverResult result = dialog.getResult();

    if (result == GameOverDialog::Restart) {
        game->InitGame();     // 调用重新开始游戏的方法
    } else if (result == GameOverDialog::MainMenu) {
        game->goToMainMenu();  // 调用返回主菜单的方法
    } else {
        QApplication::quit();  // 退出游戏
    }
}

void Mainmenu::scoreRecord(int end_score, int end_combo)
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
    loadScoreData();

    // 设置标记，表示排行榜关闭后需要显示游戏结束界面
    shouldShowGameOver = true;
    
    // 显示排行榜
    showScoreTable();
}

bool Mainmenu::checkIfNewRecord(int score)
{
    // 检查是否创造了个人历史记录最高分
    const QList<GameScore>& personalData = scoreManager->getPersonalHistoryScores();
    if (personalData.isEmpty() || score > personalData.first().score) {
        return true;
    }

    return false;
}

QString Mainmenu::getPlayerNameInput(int score, int combo, bool isNewRecord)
{
    ScoreInputDialog dialog(this);

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

// 新增网络相关槽函数实现
void Mainmenu::onWorldRankingsUpdated()
{
    qDebug() << "世界排名已更新";

    // 当世界排名更新时刷新数据
    loadScoreData();

    // 如果排行榜当前处于世界玩家模式，则更新界面
    if (scoreTable && scoreTable->isVisible() &&
        scoreTable->getCurrentType() == WORLD_PLAYERS) {
        scoreTable->updateTableDisplay();
    }
}

void Mainmenu::onNetworkError(const QString &errorMessage)
{
    // 显示网络错误
    QMessageBox::warning(this, "网络错误", errorMessage);
}

void Mainmenu::onSyncCompleted(bool success)
{
    if (success) {
        qDebug() << "与服务器同步成功";
    } else {
        qDebug() << "与服务器同步失败";
    }
}

void Mainmenu::onScoreUploadCompleted(bool success)
{
    if (success) {
        qDebug() << "分数上传成功";
    } else {
        qDebug() << "分数上传失败";
    }
}

// 添加排行榜关闭的槽函数
void Mainmenu::onScoreTableClosed()
{
    // 如果需要显示游戏结束界面
    if (shouldShowGameOver) {
        shouldShowGameOver = false;  // 重置标记
        showGameOverDialog();        // 显示游戏结束界面
    }
}

