# BanG Tetrix 排行榜服务器

这是 BanG Tetrix 游戏的在线排行榜服务器，用于同步和管理所有玩家的游戏分数。

## 功能特点

- 接收并存储玩家游戏分数
- 提供世界玩家排名API
- 支持获取个人历史记录
- 支持批量上传分数（初始同步）
- RESTful API设计
- MongoDB数据存储

## 技术栈

- Node.js & Express
- MongoDB & Mongoose
- Docker容器化支持

## 安装和启动

### 前提条件

- Node.js 14+ 
- MongoDB 4.4+
- npm 或 yarn

### 本地开发

1. 克隆仓库
```
git clone <仓库地址>
cd server_scoreTable_demo
```

2. 安装依赖
```
npm install
```

3. 设置环境变量
创建`.env`文件并配置:
```
PORT=3000
MONGODB_URI=mongodb://localhost:27017/bangTetrixDB
NODE_ENV=development
```

4. 启动服务器
```
npm run dev
```

### 使用Docker启动

1. 使用Docker Compose启动服务:
```
docker-compose up -d
```

## API文档

### 获取世界排名

```
GET /api/scores/world
```

参数:
- `page`: 页码 (默认: 1)
- `limit`: 每页记录数 (默认: 100)

响应:
```json
{
  "success": true,
  "count": 10,
  "total": 120,
  "page": 1,
  "pages": 12,
  "data": [
    {
      "playerName": "玩家1",
      "score": 9850,
      "combo": 5,
      "date": "2023-05-15 20:30"
    },
    // ...更多记录
  ]
}
```

### 获取个人历史记录

```
GET /api/scores/personal/:clientId
```

参数:
- `clientId`: 客户端唯一标识

响应:
```json
{
  "success": true,
  "count": 5,
  "data": [
    {
      "playerName": "玩家1",
      "score": 9850,
      "combo": 5,
      "date": "2023-05-15 20:30"
    },
    // ...更多记录
  ]
}
```

### 提交新分数

```
POST /api/scores
```

请求体:
```json
{
  "playerName": "玩家名称",
  "score": 8500,
  "combo": 5,          // 最大连击数，可选，默认为0
  "clientId": "客户端ID",
  "date": "2023-05-20 14:30" // 可选
}
```

### 批量提交分数

```
POST /api/scores/batch
```

请求体:
```json
{
  "clientId": "客户端ID",
  "scores": [
    {
      "playerName": "玩家1",
      "score": 8500,
      "combo": 5,
      "date": "2023-05-15 20:30"
    },
    // ...更多记录
  ]
}
```

### 获取服务器状态

```
GET /api/scores/status
```

响应:
```json
{
  "success": true,
  "status": "online",
  "totalScores": 1250,
  "uniquePlayers": 320,
  "serverTime": "2023-05-21T14:23:45.123Z"
}
```

## 与客户端集成

客户端需要实现以下功能:

1. 生成并保存唯一的客户端ID
2. 在游戏结束时提交分数到服务器
3. 启动时同步本地和服务器数据
4. 定期从服务器获取世界排名数据

## 许可

[MIT许可](LICENSE) 