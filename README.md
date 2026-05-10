# BanG Tetrix! — "邦邦消消乐～ 大份! ～"

启发于 *BanG Dream! Girls Band Party!☆PICO ～ OHMORI ～* Episode 9，基于 Qt 6.8.0 开发的类俄罗斯方块消除游戏。

## 项目结构

```
bang-tetrix/
├── main.cpp                  # 程序入口
├── ui/                       # 游戏主界面
│   ├── game.h / .cpp         # GameWidget：游戏核心循环、绘制、输入
│   ├── mainmenu.h / .cpp     # 主菜单
│   └── gameoverdialog.h/.cpp # 游戏结束对话框（重开/菜单/退出）
├── gameplay/                 # 游戏逻辑
│   ├── blockInfo.h           # Block_info 类、Band_name 枚举、角色表
│   ├── backgroundInfo.h      # 游戏区域常量（AREA_ROW/COL）、方向向量
│   ├── blockLogic.cpp        # CreateBlock / ResetBlock / ConvertStable / IsCollide
│   ├── blockMatch.cpp        # BlockCheck — 连通块匹配消除（含递归连击）
│   └── blockMove.cpp         # BlockTranslate — 下落/移动/旋转
├── score/                    # 分数与排行榜
│   ├── scoreinput.h / .cpp   # ScoreInput — 游戏结束→分数录入→排行榜流程编排
│   ├── scoreinputdialog.h/.cpp# 自定义玩家名输入对话框（展示分数/连击数）
│   └── scoreTable/           # 排行榜系统
│       ├── scoretable.h/.cpp # ScoreTable 组件 + GameScore 结构体
│       └── scoremanager.h/.cpp # 本地 JSON 持久化 + 服务端 HTTP 同步
├── img/                      # 图片资源（角色、UI、物品）
├── sound/                    # 音效资源（SE、BGM）
├── legacy/                   # 未加入 .pro 的旧源文件（参考用）
└── bang-tetrix.pro           # Qt 项目文件
```

## 核心特性

| 特性 | 说明 |
|------|------|
| **乐队角色系统** | 每个方块代表一名角色，同一乐队 5 名角色全部消除即为一次连击 |
| **连通块消除** | 相邻同乐队角色 ≥ 10 个触发消除，连锁反应产生连击 Combo |
| **连击计分** | 得分 = 10^match_count，`MaxCombo` 记录本局最大连击数 |
| **排行榜** | 个人历史记录 / 世界玩家排名，展示分数、日期、最大连击数 |
| **服务器同步** | 启动/游戏结束时自动同步本地与远程 MongoDB 数据库 |
| **固定出场序列** | （录制用）可开启固定角色出场顺序：saki → soyo → mutsumi → uika |

## 构建与运行

### 环境要求
- Qt 6.8.0+（MinGW 64-bit）
- C++17 编译器

### 构建步骤
```bash
# Qt Creator 方式
# 1. 打开 bang-tetrix.pro
# 2. 选择 Kit: Desktop Qt 6.8.0 MinGW 64-bit
# 3. Ctrl+B 构建，Ctrl+R 运行

# 命令行方式
mkdir -p build/Release
qmake bang-tetrix.pro -spec win32-g++ "CONFIG+=release"
mingw32-make -j4
```

### 启用固定出场序列（录制视频用）
```cpp
// 在 game.h 中设置，默认关闭
void setFixedSpawnEnabled(bool enabled);
// 游戏初始化时调用
game->setFixedSpawnEnabled(true);
```

## 排行榜与后端

排行榜支持本地存储（`%APPDATA%/BangTetrix/`）和远程服务器同步。

配套后端项目位于 `../ScoreTable_demo/server_scoreTable_demo/`：
- Node.js + Express + MongoDB
- RESTful API：提交分数 / 世界排名 / 个人历史 / 批量同步
- MongoDB 文档字段：`{ playerName, score, combo, date, clientId }`

## 操作说明

| 按键 | 操作 |
|------|------|
| ← → | 左右移动 |
| ↑ | 旋转 |
| ↓ | 加速下落 |
| Space | 直接落下 |
| Esc | 关闭排行榜 |
