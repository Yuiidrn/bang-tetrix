#include "scoretable.h"
#include <QHeaderView>
#include <QFont>
#include <QColor>
#include <QGraphicsDropShadowEffect>
#include <QDateTime>
#include <algorithm>
#include <QApplication>

ScoreTable::ScoreTable(QWidget *parent)
    : QWidget(parent), currentType(PERSONAL_HISTORY)
{
    // 设置窗口为无边框，但不使用Popup类型以避免点击外部自动关闭
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    
    // 创建主布局
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);
    
    // 添加标题
    titleLabel = new QLabel("个人历史记录排行榜", this);
    QFont titleFont("微软雅黑", 16, QFont::Bold);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    
    // 创建排行榜类型切换按钮
    switchLayout = new QHBoxLayout();
    switchLayout->setSpacing(10);
    
    personalHistoryButton = new QPushButton("个人历史记录", this);
    worldPlayersButton = new QPushButton("世界玩家排名", this);
    
    QFont buttonFont("微软雅黑", 10);
    personalHistoryButton->setFont(buttonFont);
    worldPlayersButton->setFont(buttonFont);
    
    personalHistoryButton->setMinimumSize(150, 35);
    worldPlayersButton->setMinimumSize(150, 35);
    
    // 设置按钮初始样式
    QString activeButtonStyle = 
        "QPushButton {"
        "    background-color: #4CAF50;"
        "    color: white;"
        "    border-radius: 5px;"
        "    padding: 5px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #45a049;"
        "}";
    
    QString inactiveButtonStyle = 
        "QPushButton {"
        "    background-color: #f0f0f0;"
        "    color: #333;"
        "    border-radius: 5px;"
        "    border: 1px solid #ddd;"
        "    padding: 5px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #e0e0e0;"
        "}";
    
    personalHistoryButton->setStyleSheet(activeButtonStyle);
    worldPlayersButton->setStyleSheet(inactiveButtonStyle);
    
    // 连接按钮信号
    connect(personalHistoryButton, &QPushButton::clicked, [this]() {
        switchTableType(PERSONAL_HISTORY);
    });
    
    connect(worldPlayersButton, &QPushButton::clicked, [this]() {
        switchTableType(WORLD_PLAYERS);
    });
    
    switchLayout->addStretch();
    switchLayout->addWidget(personalHistoryButton);
    switchLayout->addWidget(worldPlayersButton);
    switchLayout->addStretch();
    
    mainLayout->addLayout(switchLayout);
    
    // 创建表格
    tableWidget = new QTableWidget(0, 4, this);
    tableWidget->setHorizontalHeaderLabels({"排名", "用户名", "游戏日期", "得分"});
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setAlternatingRowColors(true);
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->verticalHeader()->setVisible(false);
    tableWidget->setShowGrid(true);
    tableWidget->setStyleSheet(
        "QTableWidget {"
        "    background-color: rgba(255, 255, 255, 240);"
        "    border-radius: 10px;"
        "    border: 1px solid #ddd;"
        "}"
        "QTableWidget::item {"
        "    padding: 5px;"
        "}"
        "QTableWidget::item:selected {"
        "    background-color: #b8e6ff;"
        "}"
        "QHeaderView::section {"
        "    background-color: #f0f0f0;"
        "    padding: 5px;"
        "    font-weight: bold;"
        "    border: none;"
        "    border-bottom: 1px solid #ddd;"
        "}"
    );
    
    // 添加表格阴影效果
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(15);
    shadowEffect->setColor(QColor(0, 0, 0, 80));
    shadowEffect->setOffset(0, 0);
    tableWidget->setGraphicsEffect(shadowEffect);
    
    mainLayout->addWidget(tableWidget);
    
    // 添加按ESC退出提示
    escHintLabel = new QLabel("按 [Esc] 键关闭排行榜", this);
    escHintLabel->setAlignment(Qt::AlignCenter);
    QFont hintFont("微软雅黑", 9);
    escHintLabel->setFont(hintFont);
    escHintLabel->setStyleSheet("color: #555;");
    mainLayout->addWidget(escHintLabel);
    
    // 设置窗口整体样式
    setStyleSheet(
        "ScoreTable {"
        "    background-color: rgba(245, 245, 245, 240);"
        "    border-radius: 15px;"
        "}"
    );
    
    // 设置固定大小
    setMinimumSize(500, 500);
}

ScoreTable::~ScoreTable()
{
}

void ScoreTable::setPersonalHistoryData(const QList<GameScore>& data)
{
    personalHistoryData = data;
    if (currentType == PERSONAL_HISTORY) {
        updateTableDisplay();
    }
}

void ScoreTable::setWorldPlayersData(const QList<GameScore>& data)
{
    worldPlayersData = data;
    if (currentType == WORLD_PLAYERS) {
        updateTableDisplay();
    }
}

void ScoreTable::addPersonalScore(const QString& playerName, int score)
{
    // 使用当前时间作为游戏日期
    QString currentDate = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm");
    
    // 创建新的游戏成绩记录并添加到个人历史数据中
    GameScore newScore(playerName, currentDate, score);
    personalHistoryData.append(newScore);
    
    // 按分数排序
    sortScoreData(personalHistoryData);
    
    // 如果当前显示的是个人历史记录，则更新显示
    if (currentType == PERSONAL_HISTORY) {
        updateTableDisplay();
    }
}

void ScoreTable::addWorldPlayerScore(const QString& playerName, int score)
{
    // 使用当前时间作为游戏日期
    QString currentDate = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm");
    
    // 检查该玩家是否已存在于世界玩家数据中
    bool playerExists = false;
    for (int i = 0; i < worldPlayersData.size(); i++) {
        if (worldPlayersData[i].playerName == playerName) {
            // 仅当新分数更高时才更新
            if (worldPlayersData[i].score < score) {
                worldPlayersData[i].score = score;
                worldPlayersData[i].date = currentDate;
            }
            playerExists = true;
            break;
        }
    }
    
    // 如果玩家不存在，添加新记录
    if (!playerExists) {
        GameScore newScore(playerName, currentDate, score);
        worldPlayersData.append(newScore);
    }
    
    // 按分数排序
    sortScoreData(worldPlayersData);
    
    // 如果当前显示的是世界玩家排名，则更新显示
    if (currentType == WORLD_PLAYERS) {
        updateTableDisplay();
    }
}

void ScoreTable::sortScoreData(QList<GameScore>& data)
{
    // 按分数从高到低排序
    std::sort(data.begin(), data.end(), [](const GameScore& a, const GameScore& b) {
        return a.score > b.score;
    });
}

void ScoreTable::switchTableType(ScoreTableType type)
{
    if (currentType != type) {
        currentType = type;
        
        // 更新标题
        if (type == PERSONAL_HISTORY) {
            titleLabel->setText("个人历史记录排行榜");
        } else {
            titleLabel->setText("世界玩家排名榜");
        }
        
        // 更新按钮样式
        updateButtonStyles();
        
        // 更新表格数据
        updateTableDisplay();
        
        // 发送类型改变信号
        emit typeChanged(currentType);
    }
}

void ScoreTable::updateButtonStyles()
{
    QString activeButtonStyle = 
        "QPushButton {"
        "    background-color: #4CAF50;"
        "    color: white;"
        "    border-radius: 5px;"
        "    padding: 5px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #45a049;"
        "}";
    
    QString inactiveButtonStyle = 
        "QPushButton {"
        "    background-color: #f0f0f0;"
        "    color: #333;"
        "    border-radius: 5px;"
        "    border: 1px solid #ddd;"
        "    padding: 5px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #e0e0e0;"
        "}";
    
    if (currentType == PERSONAL_HISTORY) {
        personalHistoryButton->setStyleSheet(activeButtonStyle);
        worldPlayersButton->setStyleSheet(inactiveButtonStyle);
    } else {
        personalHistoryButton->setStyleSheet(inactiveButtonStyle);
        worldPlayersButton->setStyleSheet(activeButtonStyle);
    }
}

void ScoreTable::updateTableDisplay()
{
    tableWidget->setRowCount(0);
    
    const QList<GameScore>& data = 
        (currentType == PERSONAL_HISTORY) ? personalHistoryData : worldPlayersData;
    
    if (currentType == PERSONAL_HISTORY) {
        // 个人历史记录显示四列：排名、用户名、游戏日期、得分
        tableWidget->setColumnCount(4);
        tableWidget->setHorizontalHeaderLabels({"排名", "用户名", "游戏日期", "得分"});
    } else {
        // 世界玩家排名显示三列：排名、玩家名称、得分
        tableWidget->setColumnCount(3);
        tableWidget->setHorizontalHeaderLabels({"排名", "玩家名称", "得分"});
    }
    
    for (int i = 0; i < data.size(); ++i) {
        tableWidget->insertRow(i);
        
        // 排名
        QTableWidgetItem *rankItem = new QTableWidgetItem(QString::number(i + 1));
        rankItem->setTextAlignment(Qt::AlignCenter);
        
        // 用户名/玩家名称
        QTableWidgetItem *nameItem = new QTableWidgetItem(data[i].playerName);
        
        // 日期 (仅个人历史记录显示)
        QTableWidgetItem *dateItem = nullptr;
        if (currentType == PERSONAL_HISTORY) {
            dateItem = new QTableWidgetItem(data[i].date);
        }
        
        // 分数
        QTableWidgetItem *scoreItem = new QTableWidgetItem(QString::number(data[i].score));
        scoreItem->setTextAlignment(Qt::AlignCenter);
        
        // 设置前三名的颜色
        if (i == 0) {
            rankItem->setForeground(QColor("#FF9800"));  // 金色
            rankItem->setFont(QFont("微软雅黑", 10, QFont::Bold));
        } else if (i == 1) {
            rankItem->setForeground(QColor("#9E9E9E"));  // 银色
            rankItem->setFont(QFont("微软雅黑", 10, QFont::Bold));
        } else if (i == 2) {
            rankItem->setForeground(QColor("#CD7F32"));  // 铜色
            rankItem->setFont(QFont("微软雅黑", 10, QFont::Bold));
        }
        
        tableWidget->setItem(i, 0, rankItem);
        tableWidget->setItem(i, 1, nameItem);
        
        if (currentType == PERSONAL_HISTORY) {
            tableWidget->setItem(i, 2, dateItem);
            tableWidget->setItem(i, 3, scoreItem);
        } else {
            tableWidget->setItem(i, 2, scoreItem);
        }
    }
}

void ScoreTable::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        emit closed();
        hide();
    } else {
        QWidget::keyPressEvent(event);
    }
}

void ScoreTable::hideEvent(QHideEvent *event)
{
    // 当窗口隐藏时，移除全局事件过滤器
    qApp->removeEventFilter(this);

    // 无论何种原因导致窗口隐藏，都发送关闭信号
    emit closed();
    QWidget::hideEvent(event);
}
void ScoreTable::showAndInstallFilter()
{
    // 显示窗口
    show();

    // 安装全局事件过滤器
    qApp->installEventFilter(this);
}

bool ScoreTable::eventFilter(QObject *watched, QEvent *event)
{
    // 如果是鼠标按下事件
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

        // 如果启用了点击外部关闭功能

        // 获取鼠标在屏幕上的坐标
        QPoint globalPos = mouseEvent->globalPosition().toPoint();

        // 将全局坐标转换为窗口坐标
        QPoint localPos = mapFromGlobal(globalPos);

        // 检查点击是否在窗口区域外
        if (!rect().contains(localPos)) {
            // 如果点击在窗口外部，隐藏窗口
            hide();
            return true; // 事件已处理
        }
    }

    // 其他事件交给默认处理
    return QWidget::eventFilter(watched, event);
}
