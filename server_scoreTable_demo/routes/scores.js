const express = require('express');
const router = express.Router();
const Score = require('../models/score');
const logger = require('../utils/logger');
const { apiKeyAuth } = require('../middlewares/auth');

/**
 * @route   GET /api/scores/world
 * @desc    获取世界排名
 * @access  Public
 */
router.get('/world', async (req, res) => {
  try {
    // 获取请求参数，用于分页
    const limit = parseInt(req.query.limit) || 100;
    const page = parseInt(req.query.page) || 1;
    const skip = (page - 1) * limit;
    
    logger.info(`获取世界排名: 页码=${page}, 每页记录数=${limit}`);
    
    // 聚合查询：对每个玩家只保留最高分记录，并按分数排序
    const worldRanking = await Score.aggregate([
      {
        $addFields: {
          combo: { $ifNull: ["$combo", 0] }
        }
      },
      {
        $sort: { score: -1, date: -1 } // 按分数降序，相同分数按日期降序
      },
      {
        $group: {
          _id: "$playerName",
          score: { $first: "$score" },
          combo: { $first: "$combo" },
          date: { $first: "$date" },
          playerName: { $first: "$playerName" }
        }
      },
      {
        $sort: { score: -1, date: -1 } // 再次按分数降序排序
      },
      {
        $skip: skip
      },
      {
        $limit: limit
      },
      {
        $project: {
          _id: 0,
          playerName: 1,
          score: 1,
          combo: 1,
          date: 1
        }
      }
    ]);
    
    // 获取总记录数
    const total = await Score.aggregate([
      {
        $group: {
          _id: "$playerName"
        }
      },
      {
        $count: "total"
      }
    ]);
    
    const totalCount = total.length > 0 ? total[0].total : 0;
    
    logger.info(`世界排名查询完成: 返回${worldRanking.length}条记录, 总记录数=${totalCount}`);
    
    res.json({
      success: true,
      count: worldRanking.length,
      total: totalCount,
      page,
      pages: Math.ceil(totalCount / limit),
      data: worldRanking
    });
  } catch (err) {
    logger.error(`获取世界排名失败: ${err.message}`);
    res.status(500).json({ 
      success: false, 
      message: '获取世界排名失败',
      error: err.message
    });
  }
});

/**
 * @route   GET /api/scores/personal/:clientId
 * @desc    获取特定客户端的个人历史记录
 * @access  Public
 */
router.get('/personal/:clientId', async (req, res) => {
  try {
    const { clientId } = req.params;
    const limit = parseInt(req.query.limit) || 50;
    
    const personalScores = await Score.find({ clientId })
      .sort({ score: -1 })
      .limit(limit)
      .select('playerName score combo date -_id');
    
    res.json({
      success: true,
      count: personalScores.length,
      data: personalScores
    });
  } catch (err) {
    console.error('获取个人历史记录失败:', err);
    res.status(500).json({ 
      success: false, 
      message: '获取个人历史记录失败',
      error: err.message
    });
  }
});

/**
 * @route   POST /api/scores
 * @desc    添加新的分数记录
 * @access  Public
 */
router.post('/', async (req, res) => {
  try {
    const { playerName, score, combo, clientId, date } = req.body;
    
    // 验证必要的字段
    if (!playerName || score === undefined || !clientId) {
      return res.status(400).json({
        success: false,
        message: '缺少必要的字段'
      });
    }
    
    // 创建新的分数记录
    const newScore = await Score.create({
      playerName,
      score,
      combo: combo !== undefined ? combo : 0,
      clientId,
      date: date || undefined
    });
    
    res.status(201).json({
      success: true,
      data: newScore
    });
  } catch (err) {
    console.error('添加分数记录失败:', err);
    
    // 检查是否是验证错误
    if (err.name === 'ValidationError') {
      const messages = Object.values(err.errors).map(val => val.message);
      return res.status(400).json({
        success: false,
        message: messages.join(', ')
      });
    }
    
    res.status(500).json({ 
      success: false, 
      message: '添加分数记录失败',
      error: err.message
    });
  }
});

/**
 * @route   POST /api/scores/batch
 * @desc    批量添加分数记录（用于初始同步）
 * @access  Public
 */
router.post('/batch', async (req, res) => {
  try {
    const { scores, clientId } = req.body;
    
    if (!Array.isArray(scores) || !clientId) {
      return res.status(400).json({
        success: false,
        message: '请提供有效的分数数组和客户端ID'
      });
    }
    
    // 为每个分数记录添加clientId
    const formattedScores = scores.map(score => ({
      ...score,
      clientId
    }));
    
    // 批量插入
    const result = await Score.insertMany(formattedScores);
    
    res.status(201).json({
      success: true,
      count: result.length,
      message: `成功添加${result.length}条分数记录`
    });
  } catch (err) {
    console.error('批量添加分数记录失败:', err);
    res.status(500).json({ 
      success: false, 
      message: '批量添加分数记录失败',
      error: err.message
    });
  }
});

/**
 * @route   DELETE /api/scores/:id
 * @desc    删除指定ID的分数记录
 * @access  Protected (需要API密钥)
 */
router.delete('/:id', apiKeyAuth, async (req, res) => {
  try {
    const { id } = req.params;
    const deletedScore = await Score.findByIdAndDelete(id);
    
    if (!deletedScore) {
      return res.status(404).json({
        success: false,
        message: '未找到指定ID的分数记录'
      });
    }
    
    logger.info(`删除分数记录成功: ${id}`);
    res.json({
      success: true,
      message: '分数记录已成功删除'
    });
  } catch (err) {
    logger.error(`删除分数记录失败: ${err.message}`);
    res.status(500).json({ 
      success: false, 
      message: '删除分数记录失败',
      error: err.message
    });
  }
});

/**
 * @route   GET /api/scores/status
 * @desc    获取服务器状态
 * @access  Public
 */
router.get('/status', async (req, res) => {
  try {
    const totalScores = await Score.countDocuments();
    const uniquePlayers = await Score.distinct('playerName');
    
    res.json({
      success: true,
      status: 'online',
      totalScores,
      uniquePlayers: uniquePlayers.length,
      serverTime: new Date()
    });
  } catch (err) {
    console.error('获取服务器状态失败:', err);
    res.status(500).json({ 
      success: false, 
      message: '获取服务器状态失败',
      error: err.message
    });
  }
});

module.exports = router; 