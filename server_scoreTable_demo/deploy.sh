#!/bin/bash

# 服务器部署脚本
echo "开始部署 BanG Tetrix 排行榜服务器..."

# 确保目录存在
mkdir -p logs/pm2

# 安装依赖
echo "安装依赖..."
npm install --production

# 复制生产环境配置文件
echo "配置生产环境变量..."
cp .env.production .env

# 设置API密钥 (如果没有设置过)
if grep -q "your_secure_api_key_here" .env; then
  # 生成随机API密钥
  API_KEY=$(openssl rand -hex 16)
  # 替换配置文件中的占位符
  sed -i "s/your_secure_api_key_here/$API_KEY/" .env
  echo "生成了新的API密钥"
fi

# 检查MongoDB配置
echo "检查MongoDB配置..."
if [ -f "/etc/mongod.conf" ]; then
  # 检查MongoDB是否已启用身份验证和远程访问
  if ! grep -q "authorization: enabled" /etc/mongod.conf || ! grep -q "bindIp: 0.0.0.0" /etc/mongod.conf; then
    echo "MongoDB需要配置以启用身份验证和远程访问"
    echo "请以root用户运行: sudo bash mongodb_config.sh"
    echo "然后运行: mongo < mongodb_setup.js"
  else
    echo "MongoDB已正确配置"
  fi
else
  echo "未找到MongoDB配置文件，请确认MongoDB已正确安装"
fi

# 检查防火墙设置
echo "检查防火墙设置..."
if command -v ufw &> /dev/null; then
  # Ubuntu/Debian
  echo "检测到UFW防火墙"
  echo "请确保以下端口已开放:"
  echo "  - 3000 (API服务器)"
  echo "  - 27017 (MongoDB，仅在需要远程访问数据库时)"
  echo "可以使用以下命令开放端口:"
  echo "  sudo ufw allow 3000/tcp"
  echo "  sudo ufw allow 27017/tcp"
elif command -v firewall-cmd &> /dev/null; then
  # CentOS/RHEL
  echo "检测到Firewalld防火墙"
  echo "请确保以下端口已开放:"
  echo "  - 3000 (API服务器)"
  echo "  - 27017 (MongoDB，仅在需要远程访问数据库时)"
  echo "可以使用以下命令开放端口:"
  echo "  sudo firewall-cmd --permanent --add-port=3000/tcp"
  echo "  sudo firewall-cmd --permanent --add-port=27017/tcp"
  echo "  sudo firewall-cmd --reload"
fi

# 检查PM2是否安装
if ! command -v pm2 &> /dev/null; then
  echo "安装PM2..."
  npm install -g pm2
fi

# 启动应用
echo "启动应用..."
pm2 start ecosystem.config.js --env production

# 保存PM2配置
pm2 save

# 设置开机自启
echo "设置PM2开机自启..."
pm2 startup

# 获取服务器IP
SERVER_IP=$(grep SERVER_IP .env | cut -d '=' -f2)
if [ -z "$SERVER_IP" ]; then
  SERVER_IP=$(hostname -I | awk '{print $1}')
fi

echo "部署完成！服务器正在运行"
echo "API地址: http://${SERVER_IP}:3000/api/scores"
echo "客户端连接URL: http://${SERVER_IP}:3000/api/scores"
echo ""
echo "请在Qt客户端中修改连接URL:"
echo "scoreManager->setServerUrl(\"http://${SERVER_IP}:3000/api/scores\");"

# 显示日志
echo "显示实时日志 (按Ctrl+C退出)..."
pm2 logs