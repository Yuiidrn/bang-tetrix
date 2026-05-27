const logger = require('../utils/logger');

/**
 * API密钥认证中间件
 * 用于保护敏感API路由
 */
const apiKeyAuth = (req, res, next) => {
  // 从环境变量获取API密钥
  const API_KEY = process.env.API_KEY;
  
  // 如果未配置API密钥，跳过认证
  if (!API_KEY) {
    logger.warn('未配置API_KEY，跳过API认证');
    return next();
  }
  
  // 获取请求中的API密钥
  const requestApiKey = req.headers['x-api-key'] || req.query.api_key;
  
  // 检查API密钥是否有效
  if (!requestApiKey || requestApiKey !== API_KEY) {
    logger.warn(`无效的API密钥尝试: ${req.ip}, 路径: ${req.path}`);
    return res.status(401).json({
      success: false,
      message: '无效的API密钥'
    });
  }
  
  next();
};

module.exports = {
  apiKeyAuth
}; 