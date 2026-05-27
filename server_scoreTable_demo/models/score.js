const mongoose = require('mongoose');

// 定义分数记录的Schema
const scoreSchema = new mongoose.Schema({
  playerName: {
    type: String,
    required: [true, '玩家名称不能为空'],
    trim: true,
    maxlength: [50, '玩家名称不能超过50个字符']
  },
  score: {
    type: Number,
    required: [true, '分数不能为空'],
    min: [0, '分数不能为负数']
  },
  combo: {
    type: Number,
    default: 0,
    min: [0, '连击数不能为负数']
  },
  date: {
    type: String,
    default: () => new Date().toISOString().slice(0, 16).replace('T', ' ')
  },
  clientId: {
    type: String,
    required: [true, '客户端ID不能为空'],
    index: true
  },
  createdAt: {
    type: Date,
    default: Date.now
  }
});

// 索引用于优化查询性能
scoreSchema.index({ score: -1 }); // 按分数降序索引
scoreSchema.index({ playerName: 1, clientId: 1 }); // 复合索引，用于查找特定玩家和客户端的记录

// 创建模型
const Score = mongoose.model('Score', scoreSchema);

module.exports = Score; 