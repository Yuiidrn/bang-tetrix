#ifndef GAME_H
#define GAME_H

#include "start.h"
#include "backgroundInfo.h"
#include "blockInfo.h"
#include "gameoverdialog.h"
#include <QWidget>
#include <QApplication>
#include <QtMultimedia>
#include <QTimer>
#include <QTime>
#include <QLabel>
#include <QPainter>
#include <time.h>
#include <QKeyEvent>
#include <QMessageBox>
#include <QPushButton>
#include <QDebug>
#include <QQueue>
#include <QScreen>
#include <QApplication>
#include <QStyle>
#include <QPixmap>
#include <QLabel>
#include <QtAlgorithms>
#include <math.h>
#include <QPair>
using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class GameWidget : public QWidget
{
    Q_OBJECT
signals:
    void MarinaAnimation();
public slots:
    void playStartMA();
    void onMTimeOut();

public:
    /*界面*/
    void initMenu(Start *menu);   //分配菜单界面实例
    void InitGame();  //游戏初始化
    void StartGame(); //开始游戏
    void GameOver();  //游戏结束

    /*物块逻辑*/
    void ResetBlock(); //重置人物块（准确来说是进行交替？）
    void BlockTranslate(Direction dir);          //人物块平移
    void BlockRotate(Block_info &head_block);    //人物块旋转
    void CreateBlock(Block_info &head_block);    //产生人物块（千万注意非数组定义时是引用传值！！）
    void ConvertStable(int x, int pos_y, Block_info &cpy_block);    //转换为稳定方块
    bool IsCollide(Block_info check_block, Direction dir); //判断是否会碰撞
    int BlockCheck();           //匹配相消检查，并作整体下移

    /*其他*/
    void onPlayingChanged();    //音效切换
    void goToMainMenu();        //界面切换

public:
    GameWidget(QWidget *parent = nullptr);
    ~GameWidget();

    void paintEvent(QPaintEvent *event); //场景刷新    
    void keyPressEvent(QKeyEvent *event); //键盘响应
    bool eventFilter(QObject *watched, QEvent *event) override; //事件过滤器重写

private:
    Ui::Widget *ui;
    //主菜单
    Start *menu;

    //场景参数
    Block_info game_area[AREA_ROW][AREA_COL]; //场景区域：belong = 0 即为空方格
    QRect availableGeometry;    //屏幕矩阵信息
    qreal scaleUi;

    int BLOCK_SIZE;  //物块大小
    int lMARGIN=510; //场景左边距
    int rMARGIN=250; //场景右边距
    int uMARGIN=200; //场景上边距

    int score;      //游戏分数
    int MaxCombo;   //最大连击数
    QTimer *gameTimer;     //下落间隔定时器
    int fallingHeight;     //下落位移
    QTimer *refreshTimer;  //刷新定时器
    double speed_ms;   //下落时间间隔
    double refresh_ms; //刷新时间间隔

    //当前人物块信息    
    Block_info cur_block;   //当前方块形状    
    Block_info next_block;  //下一个方块形状
    Block_info ini_block;  //空块，重置赋零值用
    int iniPos_x;   //生成位置坐标
    int iniPos_y;

    //剩余人物信息
    QSet<int> bandRest;   //非空剩余乐队可在消块逻辑触发时直接补进相应乐队编号即可形成动态维护！（初始时均为非空）
    QSet<int> charRest[BAND_NUM], ini_set; //各乐队游戏场景外成员编号（ini_set = {0, 1, 2, 3, 4}(作成员消块后恢复用)）

    //音效
    QSoundEffect *landEffect;
    QSoundEffect *moveEffect;
    //乐队音效
    QMediaPlayer *cur_bandMP;
    int m_MPdur;
    QSoundEffect *cur_bandSE;
    QTimer *SEtimer;  //音效定时器（模仿图片切换的方式进行播放停滞）

    //麻里奈动画
    QLabel *Marina;
    QVector<QPixmap> Marinas;   // 存储多张图片
    int currentIndex;           // 当前显示的图片索引
    QTimer *Mtimer;             // 定时器用于切换图片
    
    // 游戏状态标志
    bool isGameOver;
};
#endif // GAME_H
