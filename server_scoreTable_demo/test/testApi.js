/**
 * API测试脚本
 * 使用：node test/testApi.js
 */

const http = require('http');
const { generateClientId } = require('../utils/helpers');

// 配置
const API_HOST = 'localhost';
const API_PORT = 3000;
const API_BASE = '/api/scores';

// 生成测试用的客户端ID
const testClientId = generateClientId();
console.log(`测试客户端ID: ${testClientId}`);

// 辅助函数：发送HTTP请求
const sendRequest = (method, path, data = null) => {
  return new Promise((resolve, reject) => {
    const options = {
      hostname: API_HOST,
      port: API_PORT,
      path: `${API_BASE}${path}`,
      method: method,
      headers: {
        'Content-Type': 'application/json'
      }
    };

    const req = http.request(options, (res) => {
      let responseData = '';
      
      res.on('data', (chunk) => {
        responseData += chunk;
      });
      
      res.on('end', () => {
        try {
          const parsedData = JSON.parse(responseData);
          resolve({ statusCode: res.statusCode, data: parsedData });
        } catch (e) {
          reject(new Error(`解析响应失败: ${e.message}`));
        }
      });
    });
    
    req.on('error', (error) => {
      reject(error);
    });
    
    if (data) {
      req.write(JSON.stringify(data));
    }
    
    req.end();
  });
};

// 测试函数
const runTests = async () => {
  try {
    console.log('开始API测试...');
    
    // 测试1: 检查服务器状态
    console.log('\n测试1: 检查服务器状态');
    const statusResponse = await sendRequest('GET', '/status');
    console.log(`状态码: ${statusResponse.statusCode}`);
    console.log('响应数据:', statusResponse.data);
    
    // 测试2: 添加单个分数
    console.log('\n测试2: 添加单个分数');
    const scoreData = {
      playerName: '测试玩家',
      score: 8500,
      combo: 5,
      clientId: testClientId,
      date: new Date().toISOString().slice(0, 16).replace('T', ' ')
    };
    
    const addScoreResponse = await sendRequest('POST', '', scoreData);
    console.log(`状态码: ${addScoreResponse.statusCode}`);
    console.log('响应数据:', addScoreResponse.data);
    
    // 测试3: 批量添加分数
    console.log('\n测试3: 批量添加分数');
    const batchScoreData = {
      clientId: testClientId,
      scores: [
        { playerName: '测试玩家1', score: 7500, combo: 3, date: '2023-05-10 14:30' },
        { playerName: '测试玩家2', score: 9200, combo: 7, date: '2023-05-11 15:45' },
        { playerName: '测试玩家3', score: 6800, combo: 2, date: '2023-05-12 10:15' }
      ]
    };
    
    const batchAddResponse = await sendRequest('POST', '/batch', batchScoreData);
    console.log(`状态码: ${batchAddResponse.statusCode}`);
    console.log('响应数据:', batchAddResponse.data);
    
    // 测试4: 获取个人历史记录
    console.log('\n测试4: 获取个人历史记录');
    const personalResponse = await sendRequest('GET', `/personal/${testClientId}`);
    console.log(`状态码: ${personalResponse.statusCode}`);
    console.log('响应数据:', personalResponse.data);
    
    // 测试5: 获取世界排名
    console.log('\n测试5: 获取世界排名');
    const worldResponse = await sendRequest('GET', '/world');
    console.log(`状态码: ${worldResponse.statusCode}`);
    console.log('响应数据:', worldResponse.data);
    
    console.log('\n所有测试完成！');
  } catch (error) {
    console.error('测试过程中出错:', error);
  }
};

// 运行测试
runTests(); 