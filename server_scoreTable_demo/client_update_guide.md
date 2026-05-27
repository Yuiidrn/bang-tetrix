# BanG Tetrix 客户端更新指南

为了让BanG Tetrix客户端能够连接到远程服务器，您需要更新客户端代码中的服务器URL。

## 更新步骤

### 1. 修改ScoreManager类

在您的Qt项目中，找到`scoremanager.cpp`文件，修改默认服务器URL：

```cpp
// 修改前
serverUrl = "http://localhost:3000/api/scores";

// 修改后
serverUrl = "http://8.138.243.127:3000/api/scores";
```

### 2. 修改Widget类

如果您在`widget.cpp`中也有设置服务器URL的代码，同样需要更新：

```cpp
// 修改前
scoreManager->setServerUrl("http://localhost:3000/api/scores");

// 修改后
scoreManager->setServerUrl("http://8.138.243.127:3000/api/scores");
```

### 3. 添加服务器连接状态检查

为了提高用户体验，建议在客户端添加服务器连接状态检查：

```cpp
// 在scoremanager.h中添加新的信号
signals:
    void serverConnectionStatusChanged(bool isConnected, const QString &message);

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
```

## MongoDB Compass连接指南

要使用MongoDB Compass连接到服务器上的MongoDB数据库：

1. 打开MongoDB Compass
2. 创建新连接
3. 输入连接字符串：
   ```
   mongodb://yuii:Dcm12345@8.138.243.127:27017/bTscores?authSource=admin
   ```
4. 点击"连接"

## 故障排除

如果无法连接到服务器，请检查：

1. 服务器防火墙是否已开放3000和27017端口
2. MongoDB是否已配置为允许远程访问
3. 服务器IP地址是否正确
4. 网络连接是否正常

如有任何问题，请查看服务器日志：
```bash
pm2 logs
``` 