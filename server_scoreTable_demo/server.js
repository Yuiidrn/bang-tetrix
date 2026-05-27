// 导入必要的包
const express = require('express');
const mongoose = require('mongoose');
const cors = require('cors');
const dotenv = require('dotenv');
const helmet = require('helmet');
const morgan = require('morgan');
const compression = require('compression');
const bodyParser = require('body-parser');
const logger = require('./utils/logger');

// 导入路由（请求访问的根目录）
const scoresRoutes = require('./routes/scores');

// 配置环境变量
dotenv.config();

// 创建Express应用
const app = express();

// 获取服务器IP地址
const SERVER_IP = process.env.SERVER_IP || 'localhost';
const PORT = process.env.PORT || 3000;
const PUBLIC_URL = `http://${SERVER_IP}:${PORT}`;

// 应用中间件
app.use(cors());
app.use(helmet());
app.use(compression());
app.use(morgan('combined', { stream: logger.stream }));
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));

// 请求超时中间件（防止反向代理返回408前连接一直挂起）
app.use((req, res, next) => {
  res.setTimeout(25000, () => {
    logger.warn(`请求超时: ${req.method} ${req.originalUrl} from ${req.ip}`);
    res.status(408).json({
      success: false,
      message: '请求超时，请稍后重试'
    });
  });
  next();
});

// 连接MongoDB
mongoose.connect(process.env.MONGODB_URI, {
  useNewUrlParser: true,
  useUnifiedTopology: true,
  connectTimeoutMS: 10000,
  socketTimeoutMS: 30000,
  serverSelectionTimeoutMS: 5000,
})

// 基本路由
app.get('/', (req, res) => {
  res.json({ 
    message: 'BanG Tetrix排行榜API服务正在运行',
    publicUrl: PUBLIC_URL
  });
});

// 应用API路由
app.use('/api/scores', scoresRoutes);

// 错误处理中间件
app.use((err, req, res, next) => {
  logger.error(`服务器错误: ${err.stack}`);
  res.status(500).json({
    message: '服务器内部错误',
    error: process.env.NODE_ENV === 'development' ? err.message : {}
  });
});

// 未处理的Promise拒绝处理
process.on('unhandledRejection', (reason, promise) => {
  logger.error(`未处理的Promise拒绝: ${reason}`);
});

// 未捕获的异常处理
process.on('uncaughtException', (err) => {
  logger.error(`未捕获的异常: ${err.message}`);
  logger.error(err.stack);
  process.exit(1);
});

// 启动服务器
const server = app.listen(PORT, '0.0.0.0', () => {
  logger.info(`服务器正在运行在端口 ${PORT}`);
  logger.info(`环境: ${process.env.NODE_ENV}`);
  logger.info(`公网访问地址: ${PUBLIC_URL}`);
  logger.info(`API地址: ${PUBLIC_URL}/api/scores`);
});

// HTTP Server 超时设置（30秒，覆盖默认 0 无超时）
server.timeout = 30000;
server.keepAliveTimeout = 30000; 