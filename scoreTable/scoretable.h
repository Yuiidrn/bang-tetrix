#ifndef SCORETABLE_H
#define SCORETABLE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTableWidget>
#include <QPushButton>
#include <QKeyEvent>
#include <QList>
#include <QPair>
#include <QString>
#include <QHideEvent>
#include <QDateTime>
#include <QStandardItem>
#include <QEvent>

// 排行榜类型枚举
enum ScoreTableType {
    PERSONAL_HISTORY, // 个人历史记录
    WORLD_PLAYERS     // 世界玩家排名
};

// 游戏成绩结构体
struct GameScore {
    QString playerName;    // 玩家名称
    QString date;          // 游戏日期
    int score;             // 得分
    
    GameScore(const QString& player, const QString& gameDate, int gameScore)
        : playerName(player), date(gameDate), score(gameScore) {}
};

class ScoreTable : public QWidget
{
    Q_OBJECT

public:
    explicit ScoreTable(QWidget *parent = nullptr);
    ~ScoreTable();

    // 设置积分数据
    void setPersonalHistoryData(const QList<GameScore>& data);
    void setWorldPlayersData(const QList<GameScore>& data);
    
    // 获取当前积分数据
    const QList<GameScore>& getPersonalHistoryData() const { return personalHistoryData; }
    const QList<GameScore>& getWorldPlayersData() const { return worldPlayersData; }

    void addPersonalScore(const QString& playerName, int score);    // 添加新的个人游戏成绩
    void addWorldPlayerScore(const QString& playerName, int score); // 添加新的世界玩家成绩
    void switchTableType(ScoreTableType type);                      // 切换排行榜类型
    ScoreTableType getCurrentType() const { return currentType; }   // 获取当前显示的排行榜类型
    void updateTableDisplay();              // 更新表格显示
    void showAndInstallFilter();            // 显示排行榜并安装全局事件过滤器

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QVBoxLayout *mainLayout;
    QLabel *titleLabel;
    QTableWidget *tableWidget;
    QLabel *escHintLabel;
    
    // 排行榜类型切换区域
    QHBoxLayout *switchLayout;
    QPushButton *personalHistoryButton;
    QPushButton *worldPlayersButton;
    
    // 数据存储
    QList<GameScore> personalHistoryData;
    QList<GameScore> worldPlayersData;
    
    // 当前排行榜类型
    ScoreTableType currentType;

    void updateButtonStyles();                  // 设置按钮样式
    void sortScoreData(QList<GameScore>& data); // 按分数排序数据

signals:
    void closed();
    void typeChanged(ScoreTableType newType);
};

#endif // SCORETABLE_H 
