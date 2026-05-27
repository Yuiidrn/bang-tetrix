/**
 * 工具函数集合
 */

/**
 * 生成一个唯一的客户端ID
 * @returns {string} 唯一ID
 */
const generateClientId = () => {
  return 'cid_' + Date.now() + '_' + Math.random().toString(36).substr(2, 9);
};

/**
 * 格式化日期为"YYYY-MM-DD HH:MM"格式
 * @param {Date} date - 日期对象
 * @returns {string} 格式化的日期字符串
 */
const formatDate = (date) => {
  const d = date || new Date();
  const year = d.getFullYear();
  const month = String(d.getMonth() + 1).padStart(2, '0');
  const day = String(d.getDate()).padStart(2, '0');
  const hours = String(d.getHours()).padStart(2, '0');
  const minutes = String(d.getMinutes()).padStart(2, '0');
  
  return `${year}-${month}-${day} ${hours}:${minutes}`;
};

/**
 * 检查服务器连接状态
 * @param {Object} mongoose - Mongoose实例
 * @returns {boolean} 连接状态
 */
const checkDbConnection = (mongoose) => {
  return mongoose.connection.readyState === 1;
};

module.exports = {
  generateClientId,
  formatDate,
  checkDbConnection
}; 