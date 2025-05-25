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

    // 加载本地存储的积分数据
    bool loadLocalScores();
    
    // 保存积分数据到本地
    bool saveLocalScores();
    
    // 获取个人历史记录
    QList<GameScore> getPersonalHistoryScores() const;
    
    // 获取世界排名记录
    QList<GameScore> getWorldRankingScores() const;
    
    // 添加新的个人历史记录
    void addPersonalScore(const QString &playerName, int score);
    
    // 添加/更新世界玩家记录
    void addWorldPlayerScore(const QString &playerName, int score);
    
    // 设置应用程序数据目录
    void setDataDirectory(const QString &dirPath);
    
    // 新增服务器通信方法
    void syncWithServer();
    void fetchWorldRankings();
    void uploadScore(const QString &playerName, int score);
    void batchUploadScores();
    
    // 设置服务器URL
    void setServerUrl(const QString &url);
    
    // 获取客户端ID
    QString getClientId() const;

    // 检查服务器连接
    void checkServerConnection();

signals:
    // 新增信号
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
    
    // 新增成员
    QNetworkAccessManager *networkManager;
    QString serverUrl;
    QString clientId;
    bool isOnline;
    
    // 将GameScore列表转换为JSON数组
    QJsonArray scoresToJson(const QList<GameScore> &scores);
    
    // 从JSON数组解析GameScore列表
    QList<GameScore> scoresFromJson(const QJsonArray &jsonArray);
    
    // 确保数据目录存在
    bool ensureDataDirectoryExists();
    
    // 按分数排序数据
    void sortScoreData(QList<GameScore> &data);

    // 新增私有方法
    void generateClientId();
    void saveClientId();
    void loadClientId();
    void processWorldRankingsReply(QNetworkReply *reply);
    void processScoreUploadReply(QNetworkReply *reply);
    void processSyncReply(QNetworkReply *reply);
};

#endif // SCOREMANAGER_H 
