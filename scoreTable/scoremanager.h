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

    void setServerUrl(const QString &url);  // 设置服务器URL
    QString getClientId() const;            // 获取客户端ID
    void checkServerConnection();           // 检查服务器连接

signals:
    void worldRankingsUpdated();            // 当世界排名数据更新时触发
    void syncCompleted(bool success);       // 同步操作完成时触发，参数表示是否成功
    void scoreUploadCompleted(bool success);         // 分数上传完成时触发，参数表示是否成功
    void networkError(const QString &errorMessage);  // 发生网络错误时触发，包含错误信息
    void serverConnectionStatusChanged(bool isConnected, const QString &message);  // 服务器连接状态变化时触发

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

    void generateClientId();                    // 生成客户端唯一标识符
    void saveClientId();                        // 保存客户端ID到本地
    void loadClientId();                        // 从本地加载客户端ID
    void processWorldRankingsReply(QNetworkReply *reply);   // 处理获取世界排名的网络响应
    void processScoreUploadReply(QNetworkReply *reply);     // 处理分数上传的网络响应
    void processSyncReply(QNetworkReply *reply);            // 处理数据同步的网络响应
};

#endif // SCOREMANAGER_H 
