# Bang Dream! Tetrix ～ OHMORI ～ ("邦邦消消乐～ 大份! ～ " in Chinese) inspired from BanG Dream! Girls Band Party!☆PICO ～ OHMORI ～ Episode 9.

## 项目简介
BanG_Tetrix! 启发于BanG Dream! Girls Band Party!☆PICO ～ OHMORI ～ Episode 9.并基于QT框架开发的类俄罗斯方块游戏，

## 项目结构
- `main.cpp`：程序入口
- `ui/`：`game`、`mainmenu`、`gameoverdialog` 及对应 `.ui`
- `gameplay/`：`blockLogic`、`blockMatch`、`blockMove`、`backgroundInfo.h`、`blockInfo.h`
- `score/`：分数录入（`scoreinput*`）
- `score/scoreTable/`：排行榜与 `ScoreManager`
- `legacy/`：未加入 `bang-tetrix.pro` 的旧源文件（仅供参考）

## 构建过程

### 1. 环境准备
确保已安装以下工具：
- [QT Creator](https://www.qt.io/download)（推荐版本：6.8.0）
- MinGW编译器（64位）

### 2. 打开项目
1. 打开QT Creator。
2. 选择 `File -> Open File or Project`。
3. 浏览到项目目录，选择 `bang-tetrix.pro` 文件并打开。

### 3. 配置项目
1. 在QT Creator左侧的 `Projects` 面板中，确保 `Kit` 选择为 `Desktop Qt 6.8.0 MinGW 64-bit`。
2. 检查 `Build Settings`，确保构建目录正确。

### 4. 构建项目
1. 点击 `Build -> Build Project "bang-tetrix"`，或按下 `Ctrl+B`。
2. 构建完成后，点击 `Run -> Run`，或按下 `Ctrl+R` 启动游戏。

### 5. 清理构建
如果需要重新构建，可以执行以下步骤：
1. 点击 `Build -> Clean All`。
2. 再次点击 `Build -> Build Project "bang-tetrix"`。

## 运行效果
- 游戏启动后，进入主菜单，可以选择开始游戏或查看排行榜。
- 游戏过程中，方块会不断下落，玩家可以通过键盘控制方块的移动和旋转。
- 游戏结束后，玩家可以输入姓名并查看排行榜。

## 贡献
欢迎提交Issue或Pull Request，共同改进项目！

## 许可证
本项目采用 MIT 许可证，详情请参阅 [LICENSE](LICENSE) 文件。
