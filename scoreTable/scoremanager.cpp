#include "scoremanager.h"
#include <QStandardPaths>
#include <algorithm>
#include <QDebug>
#include <QUrlQuery>

ScoreManager::ScoreManager(QObject *parent)
    : QObject(parent), isOnline(false)
{
    // 设置默认数据目录为应用程序数据位置
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    setDataDirectory(appDataPath + "/BangTetrix");
    
    // 初始化网络管理器
    networkManager = new QNetworkAccessManager(this);
    
    // 设置默认服务器URL(在start.cpp中set改！)
    serverUrl = "-1";
    
    // 加载或生成客户端ID
    loadClientId();
}

ScoreManager::~ScoreManager()
{
    // 析构时自动保存数据
    saveLocalScores();
}

void ScoreManager::setDataDirectory(const QString &dirPath)
{
    dataDirectory = dirPath;
    personalScoresFile = dataDirectory + "/personal_scores.json";
    worldRankingFile = dataDirectory + "/world_ranking.json";
    
    // 确保数据目录存在
    ensureDataDirectoryExists();
}

bool ScoreManager::ensureDataDirectoryExists()
{
    QDir dir(dataDirectory);
    if (!dir.exists()) {
        return dir.mkpath(".");
    }
    return true;
}

bool ScoreManager::loadLocalScores()
{
    // 加载个人历史记录
    QFile personalFile(personalScoresFile);
    if (personalFile.exists() && personalFile.open(QIODevice::ReadOnly)) {
        QByteArray data = personalFile.readAll();
        personalFile.close();
        
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isArray()) {
            personalScores = scoresFromJson(doc.array());
            // 排序数据
            sortScoreData(personalScores);
        }
    }
    
    // 加载世界排名记录
    QFile worldFile(worldRankingFile);
    if (worldFile.exists() && worldFile.open(QIODevice::ReadOnly)) {
        QByteArray data = worldFile.readAll();
        worldFile.close();
        
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isArray()) {
            worldRankingScores = scoresFromJson(doc.array());
            // 排序数据
            sortScoreData(worldRankingScores);
        }
    }
    
    return true;
}

bool ScoreManager::saveLocalScores()
{
    // 确保数据目录存在
    if (!ensureDataDirectoryExists()) {
        qDebug() << "Failed to create data directory:" << dataDirectory;
        return false;
    }
    
    // 保存个人历史记录
    QFile personalFile(personalScoresFile);
    if (personalFile.open(QIODevice::WriteOnly)) {
        QJsonArray jsonArray = scoresToJson(personalScores);
        QJsonDocument doc(jsonArray);
        personalFile.write(doc.toJson());
        personalFile.close();
    } else {
        qDebug() << "Failed to open personal scores file for writing:" << personalScoresFile;
        return false;
    }
    
    // 保存世界排名记录
    QFile worldFile(worldRankingFile);
    if (worldFile.open(QIODevice::WriteOnly)) {
        QJsonArray jsonArray = scoresToJson(worldRankingScores);
        QJsonDocument doc(jsonArray);
        worldFile.write(doc.toJson());
        worldFile.close();
    } else {
        qDebug() << "Failed to open world ranking file for writing:" << worldRankingFile;
        return false;
    }
    
    return true;
}

QList<GameScore> ScoreManager::getPersonalHistoryScores() const
{
    return personalScores;
}

QList<GameScore> ScoreManager::getWorldRankingScores() const
{
    return worldRankingScores;
}

void ScoreManager::addPersonalScore(const QString &playerName, int score)
{
    // 使用当前时间作为游戏日期
    QString currentDate = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm");
    
    // 创建新的游戏成绩记录并添加到个人历史数据中
    GameScore newScore(playerName, currentDate, score);
    personalScores.append(newScore);
    
    // 按分数排序
    sortScoreData(personalScores);
    
    // 保存数据
    saveLocalScores();
}

void ScoreManager::addWorldPlayerScore(const QString &playerName, int score)
{
    // 使用当前时间作为游戏日期
    QString currentDate = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm");
    
    // 检查该玩家是否已存在于世界玩家数据中
    bool playerExists = false;
    for (int i = 0; i < worldRankingScores.size(); i++) {
        if (worldRankingScores[i].playerName == playerName) {
            // 仅当新分数更高时才更新
            if (worldRankingScores[i].score < score) {
                worldRankingScores[i].score = score;
                worldRankingScores[i].date = currentDate;
            }
            playerExists = true;
            break;
        }
    }
    
    // 如果玩家不存在，添加新记录
    if (!playerExists) {
        GameScore newScore(playerName, currentDate, score);
        worldRankingScores.append(newScore);
    }
    
    // 按分数排序
    sortScoreData(worldRankingScores);
    
    // 保存数据
    saveLocalScores();
}

void ScoreManager::sortScoreData(QList<GameScore> &data)
{
    // 按分数从高到低排序
    std::sort(data.begin(), data.end(), [](const GameScore& a, const GameScore& b) {
        return a.score > b.score;
    });
}

QJsonArray ScoreManager::scoresToJson(const QList<GameScore> &scores)
{
    QJsonArray jsonArray;
    
    for (const auto &score : scores) {
        QJsonObject scoreObj;
        scoreObj["playerName"] = score.playerName;
        scoreObj["date"] = score.date;
        scoreObj["score"] = score.score;
        
        jsonArray.append(scoreObj);
    }
    
    return jsonArray;
}

QList<GameScore> ScoreManager::scoresFromJson(const QJsonArray &jsonArray)
{
    QList<GameScore> scores;
    
    for (const auto &value : jsonArray) {
        if (!value.isObject()) {
            continue;
        }
        
        QJsonObject obj = value.toObject();
        QString playerName = obj["playerName"].toString();
        QString date = obj["date"].toString();
        int score = obj["score"].toInt();
        
        scores.append(GameScore(playerName, date, score));
    }
    
    return scores;
}

//  服务器URL设置
void ScoreManager::setServerUrl(const QString &url)
{
    serverUrl = url;
}

//  获取客户端ID
QString ScoreManager::getClientId() const
{
    return clientId;
}

//  与服务器同步
void ScoreManager::syncWithServer()
{
    if (serverUrl.isEmpty()) {
        emit networkError("服务器URL未设置");
        return;
    }
    
    // 先尝试更新世界排名
    fetchWorldRankings();
    
    // 然后上传本地分数
    batchUploadScores();
}

//  获取世界排名
void ScoreManager::fetchWorldRankings()
{
    if (serverUrl.isEmpty()) {
        emit networkError("服务器URL未设置");
        return;
    }
    
    QUrl url(serverUrl + "/world");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QNetworkReply *reply = networkManager->get(request);
    
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        processWorldRankingsReply(reply);
        reply->deleteLater();
    });
}

//  上传单个分数
void ScoreManager::uploadScore(const QString &playerName, int score)
{
    if (serverUrl.isEmpty()) {
        emit networkError("服务器URL未设置");
        return;
    }
    
    QUrl url(serverUrl);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QJsonObject scoreObj;
    scoreObj["playerName"] = playerName;
    scoreObj["score"] = score;
    scoreObj["clientId"] = clientId;
    scoreObj["date"] = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm");
    
    QJsonDocument doc(scoreObj);
    QByteArray data = doc.toJson();
    
    QNetworkReply *reply = networkManager->post(request, data);
    
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        processScoreUploadReply(reply);
        reply->deleteLater();
    });
}

//  批量上传分数
void ScoreManager::batchUploadScores()
{
    if (serverUrl.isEmpty()) {
        emit networkError("服务器URL未设置");
        return;
    }
    
    QUrl url(serverUrl + "/batch");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QJsonObject uploadObj;
    uploadObj["clientId"] = clientId;
    uploadObj["scores"] = scoresToJson(personalScores);
    
    QJsonDocument doc(uploadObj);
    QByteArray data = doc.toJson();
    
    QNetworkReply *reply = networkManager->post(request, data);
    
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        processSyncReply(reply);
        reply->deleteLater();
    });
}

//  生成客户端ID
void ScoreManager::generateClientId()
{
    // 生成全局唯一的客户端ID
    clientId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    saveClientId();
}

//  保存客户端ID
void ScoreManager::saveClientId()
{
    QSettings settings;
    settings.setValue("clientId", clientId);
}

//  加载客户端ID
void ScoreManager::loadClientId()
{
    QSettings settings;
    clientId = settings.value("clientId").toString();
    
    // 如果没有客户端ID，则生成一个
    if (clientId.isEmpty()) {
        generateClientId();
    }
}

//  处理世界排名响应
void ScoreManager::processWorldRankingsReply(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "获取世界排名失败:" << reply->errorString();
        emit networkError("获取世界排名失败: " + reply->errorString());
        return;
    }
    
    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    
    if (!doc.isObject()) {
        emit networkError("服务器返回的数据格式无效");
        return;
    }
    
    QJsonObject obj = doc.object();
    
    if (!obj["success"].toBool()) {
        emit networkError("服务器返回错误: " + obj["message"].toString());
        return;
    }
    
    QJsonArray rankingsArray = obj["data"].toArray();
    worldRankingScores = scoresFromJson(rankingsArray);
    
    // 刷新数据后发出信号
    emit worldRankingsUpdated();
}

// 处理分数上传响应
void ScoreManager::processScoreUploadReply(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "上传分数失败:" << reply->errorString();
        emit networkError("上传分数失败: " + reply->errorString());
        emit scoreUploadCompleted(false);
        return;
    }
    
    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    
    if (!doc.isObject()) {
        emit networkError("服务器返回的数据格式无效");
        emit scoreUploadCompleted(false);
        return;
    }
    
    QJsonObject obj = doc.object();
    
    if (!obj["success"].toBool()) {
        emit networkError("服务器返回错误: " + obj["message"].toString());
        emit scoreUploadCompleted(false);
        return;
    }
    
    // 上传成功，尝试更新世界排名
    fetchWorldRankings();
    
    emit scoreUploadCompleted(true);
}

//  处理同步响应
void ScoreManager::processSyncReply(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "同步数据失败:" << reply->errorString();
        emit networkError("同步数据失败: " + reply->errorString());
        emit syncCompleted(false);
        return;
    }
    
    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    
    if (!doc.isObject()) {
        emit networkError("服务器返回的数据格式无效");
        emit syncCompleted(false);
        return;
    }
    
    QJsonObject obj = doc.object();
    
    if (!obj["success"].toBool()) {
        emit networkError("服务器返回错误: " + obj["message"].toString());
        emit syncCompleted(false);
        return;
    }
    
    qDebug() << "成功同步了" << obj["count"].toInt() << "条记录";
    
    // 同步成功，尝试更新世界排名
    fetchWorldRankings();
    
    emit syncCompleted(true);
} 

// 在scoremanager.cpp中添加检查方法
void ScoreManager::checkServerConnection()
{
    QNetworkRequest request(QUrl(serverUrl + "/status"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = networkManager->get(request);

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            emit serverConnectionStatusChanged(true, "服务器连接正常");
        } else {
            emit serverConnectionStatusChanged(false, "无法连接到服务器: " + reply->errorString());
        }
        reply->deleteLater();
    });
}
