#ifndef SCOREMANAGER_H
#define SCOREMANAGER_H

#include <QObject>
#include <QList>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSettings>
#include <QUuid>
#include "scoretable.h"

class ScoreManager : public QObject
{
    Q_OBJECT

public:
    explicit ScoreManager(QObject *parent = nullptr);
    ~ScoreManager();

    void setDataDirectory(const QString &dirPath);          // 设置应用程序数据目录
    bool loadLocalScores();                                 // 加载本地存储的积分数据
    bool saveLocalScores();                                 // 保存积分数据到本地
    QList<GameScore> getPersonalHistoryScores() const;          // 获取个人历史记录
    QList<GameScore> getWorldRankingScores() const;             // 获取世界排名记录
    void addPersonalScore(const QString &playerName, int score);    // 添加新的个人历史记录
    void addWorldPlayerScore(const QString &playerName, int score); // 添加/更新世界玩家记录
    
    void syncWithServer();                  // 同步本地数据到服务器
    void fetchWorldRankings();              // 从服务器获取世界排名数据
    void uploadScore(const QString &playerName, int score);  // 上传单个分数到服务器
    void batchUploadScores();               // 批量上传所有未同步的分数记录

    void setServerUrl(const QString &url); // 设置服务器URL
    QString getClientId() const;           // 获取客户端ID
    void checkServerConnection();          // 检查服务器连接

signals:
    void worldRankingsUpdated();
    void syncCompleted(bool success);
    void scoreUploadCompleted(bool success);
    void networkError(const QString &errorMessage);
    void serverConnectionStatusChanged(bool isConnected, const QString &message);

private:
    QList<GameScore> personalScores;    // 个人历史记录
    QList<GameScore> worldRankingScores; // 世界排名记录

    QString dataDirectory;              // 数据存储目录
    QString personalScoresFile;         // 个人记录文件路径
    QString worldRankingFile;           // 世界排名文件路径

    QNetworkAccessManager *networkManager;
    QString serverUrl;
    QString clientId;
    bool isOnline;

    QJsonArray scoresToJson(const QList<GameScore> &scores);      // 将GameScore列表转换为JSON数组
    QList<GameScore> scoresFromJson(const QJsonArray &jsonArray); // 从JSON数组解析GameScore列表
    bool ensureDataDirectoryExists();           // 确保数据目录存在
    void sortScoreData(QList<GameScore> &data); // 按分数排序数据

    void generateClientId();
    void saveClientId();
    void loadClientId();
    void processWorldRankingsReply(QNetworkReply *reply);
    void processScoreUploadReply(QNetworkReply *reply);
    void processSyncReply(QNetworkReply *reply);
};

#endif // SCOREMANAGER_H 
