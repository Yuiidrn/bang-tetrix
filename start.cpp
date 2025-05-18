#include "start.h"
#include "game.h"
#include "ui_start.h"
#include <QApplication>
#include <QScreen>
#include <QDateTime>
#include <QDir>

Start::Start(QWidget *parent) :
    QWidget(parent)
    , ui(new Ui::Start)
    , scoreTable(nullptr)
    , scoreManager(nullptr)
{
    ui->setupUi(this);
    int defaultWidth = 1200, defaultHeight = 675;
    this->setWindowTitle("BanG_Tetrix!");
    this->resize(defaultWidth, defaultHeight);
    //直接通过ui指针访问图片进行尺寸修改（可视化编程而不使用paintEvent（因直接套的非ui设计的主体代码，在游戏主体中没用上））
    ui->bg->setFixedSize(defaultWidth, defaultHeight);
    ui->begin->setStyleSheet("border:none;");

    //界面切换槽函数（也可使用窗口类自带的keyPressEvent()）
    connect(ui->begin, &QPushButton::clicked, this, [=](){switchToGame();}); //槽函数也需要lambda表达式封装！

    // 创建分数管理器
    scoreManager = new ScoreManager(this);

    // 加载本地存储的积分数据
    if (scoreManager->loadLocalScores()) {
        qDebug() << "成功从本地存储加载分数数据";
    } else {
        qDebug() << "从本地存储加载分数数据失败";
    }

    // 连接ScoreManager的信号和槽
    connect(scoreManager, &ScoreManager::worldRankingsUpdated, this, &Start::onWorldRankingsUpdated);
    connect(scoreManager, &ScoreManager::networkError, this, &Start::onNetworkError);
    connect(scoreManager, &ScoreManager::syncCompleted, this, &Start::onSyncCompleted);
    connect(scoreManager, &ScoreManager::scoreUploadCompleted, this, &Start::onScoreUploadCompleted);

    // 创建排行榜但不显示
    scoreTable = new ScoreTable(this);
    scoreTable->hide();

    // 加载数据到排行榜
    loadScoreData();

    // 连接排行榜关闭信号
    connect(scoreTable, &ScoreTable::typeChanged, this, &Start::onScoreTableTypeChanged);

    // 尝试与服务器同步
    // 设置服务器URL (可以根据需要修改)
    scoreManager->setServerUrl("http://localhost:3000/api/scores");
    scoreManager->syncWithServer();
}
Start::~Start(){     // 保存积分数据
    if (scoreManager) {
        scoreManager->saveLocalScores();
    }

    delete ui;}

void Start::initGame(){
    game = new GameWidget();
}

void Start::switchToGame()
{
    this->hide();
    this->game->initMenu(this); //为game的主菜单指针赋值！！
    this->game->show();
    this->game->InitGame();
}

void Start::loadScoreData()
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

void Start::on_showScoreTableButton_clicked()
{
    showScoreTable();
}

void Start::showScoreTable()
{
    if (!scoreTable) {
        scoreTable = new ScoreTable();
        scoreTable->setWindowTitle("分数排行榜");

        // 加载分数数据
        loadScoreData();

        // 连接类型变化信号
        connect(scoreTable, &ScoreTable::typeChanged, this, &Start::onScoreTableTypeChanged);
    }

    // 居中显示在主界面上
    QPoint center = this->mapToGlobal(this->rect().center());
    scoreTable->move(center.x() - scoreTable->width() / 2, center.y() - scoreTable->height() / 2);

    scoreTable->show();
}

void Start::closeScoreTable()
{
    if (scoreTable && scoreTable->isVisible()) {
        scoreTable->hide();
    }
}

void Start::onScoreTableTypeChanged(ScoreTableType newType)
{
    // 使用参数避免警告
    qDebug() << "排行榜类型已更改为: " << (newType == PERSONAL_HISTORY ? "个人历史记录" : "世界玩家排名");

    // 当排行榜类型改变时，更新数据
    loadScoreData();
}

void Start::scoreRecord(int end_score)
{
    // 录入分数分数
    int score = end_score;

    // 检查是否创造了新高记录
    bool isNewRecord = checkIfNewRecord(score);

    // 显示分数结算对话框并获取玩家名称，若是新高记录则提示用户
    QString playerName = getPlayerNameInput(score, isNewRecord);

    // 如果玩家取消输入，不添加成绩
    if (playerName.isEmpty()) {
        return;
    }

    // 使用ScoreManager添加分数到本地
    scoreManager->addPersonalScore(playerName, score);
    scoreManager->addWorldPlayerScore(playerName, score);

    // 上传分数到服务器
    scoreManager->uploadScore(playerName, score);

    // 重新加载积分数据到表格
    loadScoreData();

    // 自动显示排行榜
    showScoreTable();
}

bool Start::checkIfNewRecord(int score)
{
    // 检查是否创造了个人历史记录最高分
    const QList<GameScore>& personalData = scoreManager->getPersonalHistoryScores();
    if (personalData.isEmpty() || score > personalData.first().score) {
        return true;
    }

    return false;
}

QString Start::getPlayerNameInput(int score, bool isNewRecord)
{
    // 创建自定义对话框
    ScoreInputDialog dialog(this);

    // 设置默认用户名
    QString defaultName = "玩家" + QString::number(QRandomGenerator::global()->bounded(1000, 10000));
    dialog.setDefaultName(defaultName);

    // 设置分数和是否为新高分
    dialog.setScore(score, isNewRecord);

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
void Start::onWorldRankingsUpdated()
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

void Start::onNetworkError(const QString &errorMessage)
{
    // 显示网络错误
    QMessageBox::warning(this, "网络错误", errorMessage);
}

void Start::onSyncCompleted(bool success)
{
    if (success) {
        qDebug() << "与服务器同步成功";
        // 可以添加UI反馈，比如状态栏消息等
    } else {
        qDebug() << "与服务器同步失败";
    }
}

void Start::onScoreUploadCompleted(bool success)
{
    if (success) {
        qDebug() << "分数上传成功";
        // 可以添加UI反馈，比如状态栏消息等
    } else {
        qDebug() << "分数上传失败";
    }
}
