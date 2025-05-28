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

    //--设置数据库服务器URL---（需要自行搭数据库和部署后端）
    scoreManager->setServerUrl("http://yourApi.com/api/scores");
    scoreManager->syncWithServer();      // 尝试与服务器同步

    // 创建排行榜但不显示
    scoreTable = new ScoreTable(this);
    scoreTable->hide();

    // 加载数据到排行榜
    loadScoreData();

    // 连接排行榜转换信号
    connect(scoreTable, &ScoreTable::typeChanged, this, &Mainmenu::onScoreTableTypeChanged);
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
    QMessageBox::warning(this, "网络错误", errorMessage); // 显示网络错误
}

void Mainmenu::onSyncCompleted(bool success)
{
    if (success) {
        qDebug() << "与服务器同步成功";
    } else {
        qDebug() << "与服务器同步失败";
    }
}

