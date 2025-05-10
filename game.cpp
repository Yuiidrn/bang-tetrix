#include "game.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    // 根据用户主窗口尺寸实时调整窗口尺寸布局，影响背景和物块的适配度
    QScreen *screen = QGuiApplication::primaryScreen();
    // 获取主屏幕区域大小
    availableGeometry = screen->availableGeometry();  //声明后的二次定义也要带上数据类型（类比函数）！
    scaleUi = static_cast<qreal>(availableGeometry.width()) / defaultWidth; //获取比例
    this->resize(availableGeometry.width(), availableGeometry.height());

    BLOCK_SIZE = (availableGeometry.height() - upOffset) / AREA_ROW ; //分割窗口为指定行数
    lMARGIN *= scaleUi;
    uMARGIN *= scaleUi;
    this->setWindowTitle("BanG_Tetrix!");
}

inline void block_cpy(Block_info &dblock, Block_info &sblock) { dblock = sblock; }
inline void Sleep(unsigned int msec){
    QElapsedTimer timer;
    timer.start();
    while(timer.elapsed() < msec)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
  //在延迟过程中屏键入事件处理滞后 QEventLoop::ExcludeUserInputEvents
}

//图片动画切换逻辑（不跟场景一起重绘，直接定个label占住相应位置）
void Widget::onMTimeOut(){ //不能在时间槽函数里这里定义指针！！否则会画面会滞留在窗口上
    char cStrPath[100] = {'\0'};
    sprintf(cStrPath, ":/imgs/img/ui/marina-%d.png", currentIndex);
    QString path = cStrPath;
    QPixmap map = QPixmap(path);
    Marina->setPixmap(map);
    Marina->setGeometry(QRect(AREA_COL*BLOCK_SIZE + 1.25*lMARGIN, uMARGIN - 50 * scaleUi, map.width(),map.height()));
//    Marina->raise();
    Marina->show();
    currentIndex++;
    if( currentIndex > 3 ) {
        Mtimer->stop();
//        currentIndex = -1;
        path = ":/imgs/img/ui/marina-1.png";
        map = QPixmap(path);
        Marina->setPixmap(map); //下落过程中保持第一张图片
    }
}
void Widget::playStartMA(){ //动画定时器
    currentIndex = 1;
    Mtimer->start(); // 启动定时器
}

//界面及场景逻辑
void Widget::initMenu(Start *menu){ this->menu = menu; }
void Widget::goToMainMenu(){ this->hide(); menu->show(); }
void Widget::InitGame()
{
    ini_block = Block_info();  //空块，重置赋零值用
    iniPos_x = 4, iniPos_y = 0;  //设置生成位置坐标
    //清空场景
    for(int i = 0; i < AREA_ROW; i++)
        for(int j = 0; j < AREA_COL; j++)
            game_area[i][j] = Block_info(); //默认构造函数作初始化

    //剩余乐队及成员管理
    for(int i = 0; i < 3; i++)
        bandRest.insert(i); // bandRest = {0,1,2,3,4,5,6,7,8};

    for(int j = 0; j < CHAR_NUM; j++)
        ini_set.insert(j); // ini_set = {0, 1, 2, 3, 4, 5};

    for(auto &ss : charRest)    //场景外乐队剩余成员
        ss = ini_set;           //设置为满员

    //麻里奈动画设置：其他槽及信号链接设置 必须声明指针！！
    void(Widget::*MarinaAnimation0)() = &Widget::MarinaAnimation; //指明不带参信号指针
    void(Widget::*playStartMA0)() = &Widget::playStartMA;         //指明不带参槽指针
    connect(this, MarinaAnimation0, this, playStartMA0);
    Marina = new QLabel(this);

    //乐队背景音设置
    cur_bandMP = new QMediaPlayer(this);          //创建实例！！
    auto audioOutput = new QAudioOutput(this);    //配置*音频输出设备*
    cur_bandMP->setAudioOutput(audioOutput);
    connect(cur_bandMP, &QMediaPlayer::durationChanged, this, [=](){ //建立加载完毕自动获取时长链接
        m_MPdur = cur_bandMP->duration();
        // qDebug() << m_MPdur;
    });

    //音效预加载
    landEffect = new QSoundEffect();
    moveEffect = new QSoundEffect();
    landEffect->setSource(QUrl("qrc:/sounds/sound/land.wav")); landEffect->setVolume(1);
    moveEffect->setSource(QUrl("qrc:/sounds/sound/move.wav")); moveEffect->setVolume(0.2);

    //设置初始下落延迟速度和刷新率
    speed_ms = 25;
    fallingHeight = 3;
    refresh_ms = 16;     //1000ms / 60FPS = 16.67; 1000ms / 165FPS = 6.06

    //初始化随机数种子
    srand(time(0));
    //分数清0
    score=0;
    MaxCombo=0;
    //开始游戏
    StartGame();
    landEffect->play();
}
void Widget::StartGame()
{
    //改用定义实例计时器进行操作(方便后续的音效暂停)
    gameTimer = new QTimer(this);
    gameTimer->setInterval(speed_ms);
    refreshTimer = new QTimer(this);
    refreshTimer->setInterval(refresh_ms);
    Mtimer = new QTimer(this);
    Mtimer->setInterval(41);

    //定时刷新画面(paintEvent重绘)(lambda表达式绑定)
    connect(refreshTimer, &QTimer::timeout, this, [=](){update();});
    //定时方块下落
    connect(gameTimer, &QTimer::timeout, this, [=](){BlockTranslate(DOWN);});
    //麻里奈动画
    connect(Mtimer, SIGNAL(timeout()), this, SLOT(onMTimeOut()));

    //产生初始下一个方块
    CreateBlock(next_block);
    ResetBlock(); //产生方块

    gameTimer->start();
    refreshTimer->start();
}
void Widget::GameOver()
{
    gameTimer->stop();
    refreshTimer->stop();
    QMessageBox msgBox;
    msgBox.setWindowTitle("游戏结束");
    msgBox.setText("游戏结束！请选择您的操作：");
    msgBox.setInformativeText("您可以选择重新开始游戏或者返回主菜单。");

    // 添加按钮
    QPushButton *restartButton = msgBox.addButton("重新开始", QMessageBox::YesRole);
    QPushButton *mainMenuButton = msgBox.addButton("返回主菜单", QMessageBox::NoRole);
    msgBox.addButton("退出游戏", QMessageBox::RejectRole);  // 默认的退出按钮

    // 显示消息框
    msgBox.exec();
    // 处理用户选择的按钮
    if (msgBox.clickedButton() == restartButton) {
        InitGame();     // 调用重新开始游戏的方法
    } else if (msgBox.clickedButton() == mainMenuButton) {
        goToMainMenu();  // 调用返回主菜单的方法
    } else {
        // QApplication::quit();  // 退出游戏
    }
}

void Widget::paintEvent(QPaintEvent *event) //不一定非要用绘制函数！也可用原窗口上定义图形类并set的方式
{
    QPainter painter(this);
    //绘制游戏场景
    QImage bg = QImage(":/imgs/img/ui/bg.png");
    int titleH = style()->pixelMetric(QStyle::PM_TitleBarHeight);
    painter.drawImage(QRect(0, 0, availableGeometry.width(), availableGeometry.height() - titleH), bg);

    //绘制方块预告(next_block)
    QPixmap tImg = next_block.img; //下面也用得上
//    QPixmap marina = QPixmap(":/imgs/img/ui/marina-1.png");
    qreal scale = static_cast<qreal>(tImg.width()) / tImg.height();     //人物块缩放比例
    qreal ratioZoomX = 1.25, ratioHeight = next_block.belong != Item ? 2.5 : 1; //物体块不放大
    int uOff = 50 * scaleUi, halfBarrel = 77, centerPointX = AREA_COL*BLOCK_SIZE + ratioZoomX*lMARGIN + halfBarrel;
    int scalingImgWidth = qRound(scale * (BLOCK_SIZE*ratioHeight));
    painter.drawPixmap(QRect(centerPointX - scalingImgWidth/2, uMARGIN, scalingImgWidth, BLOCK_SIZE*ratioHeight), next_block.img);
                //通过设置QRect的宽高为格子宽高，以不失真地自动调整缩放图片以贴合格子宽高
//    painter.drawImage(QRect(centerPointX - halfBarrel, uMARGIN - uOff, marina.width() * scaleUi, marina.height() * scaleUi), marina);
    Marina->show(); //动画效果

    //绘制得分
    QColor color;
    color.setRgb(206, 37, 122);
    painter.setPen(color);
    painter.setFont(QFont("华文彩云",40));
    painter.drawText(QRect(AREA_COL*BLOCK_SIZE + 1.5*lMARGIN, 3.3*uMARGIN, 100, 100),Qt::AlignCenter,QString::number(score));
    //绘制最大连击数
    color.setRgb(71, 71, 71);
    painter.setPen(color);
    painter.setFont(QFont("华文琥珀",35));
    uOff = 3 * scaleUi;
    painter.drawText(QRect(AREA_COL*BLOCK_SIZE + 1.7*lMARGIN-uOff, 4.16*uMARGIN, 100, 100),Qt::AlignCenter,QString::number(MaxCombo));

    /* 绘制当前块(要实现平滑下落必须得单拎出来绘制：cur_block) */
    painter.save(); //通过移动转变坐标系来绘制（注意复原）
    tImg = cur_block.img;
    scale = static_cast<qreal>(tImg.width()) / tImg.height();
    // 根据缩放比例计算图片块宽度
    int Half_scalingImgWidth = cur_block.belong != Item ? qRound(scale * (BLOCK_SIZE*2) )/2 : qRound(scale*(BLOCK_SIZE))/2;
    painter.translate(lMARGIN + (cur_block.bp.pos_x+0.5)*(BLOCK_SIZE), cur_block.y); //画布原点默认先平移至当前质点(正立时！头顶)中心点(**当前块的y为实时坐标=>下落幅度即可以像素来衡量)
    // 注意额外的绕头旋转（左右侧）原点偏移处理
    switch (cur_block.dir) {  //再根据缩放后半图宽调整printPoint（只以屏幕左上角为准，提前确定好旋转点（需要纸面推算））
    case 1:
        painter.translate(BLOCK_SIZE/2, -Half_scalingImgWidth);  //腿朝左，头顶位置侧转至右侧，上拉角色图距离（1/2头发宽）
        break;
    case 2:
        painter.translate(Half_scalingImgWidth, 0);     //腿朝上，头顶位置不变，原点右拉角色图距离（1/2头发宽）即可
        break;
    case 3:
        painter.translate(-BLOCK_SIZE/2, Half_scalingImgWidth);  //腿朝右，头顶位置侧转至左侧，下拉角色图距离（1/2头发宽）
        break;
    default:
        painter.translate(-Half_scalingImgWidth, 0);    //腿朝下，头顶位置不变，原点左拉角色图距离（1/2头发宽）即可
        break;
    }
    painter.rotate(90*cur_block.dir); //旋转后调整printPoint
    painter.drawPixmap(QRect(0, 0, Half_scalingImgWidth*2, cur_block.belong != Item ? BLOCK_SIZE*2 : BLOCK_SIZE), tImg);
    painter.restore(); //复原至(0, 0)

    /* 绘制稳定块 */
    for(int i = 0; i < AREA_ROW; i++)
        for(int j = 0; j < AREA_COL; j++)
        {
            if(game_area[i][j].is_head && game_area[i][j].is_stable) // &&!game_area[i][j].is_stable
            {
                //通过移动转变坐标系来绘制（注意复原）
                painter.save();

                tImg = game_area[i][j].img;
                scale = static_cast<qreal>(tImg.width()) / tImg.height();
                // 根据缩放比例计算图片块宽度
                int Half_scalingImgWidth = game_area[i][j].belong != Item ? qRound(scale * (BLOCK_SIZE*2) )/2 : qRound(scale*(BLOCK_SIZE))/2;
                painter.translate(lMARGIN + (j+0.5)*(BLOCK_SIZE), i*BLOCK_SIZE); //画布原点默认先移动至上边界中心点(**稳定块的y坐标即用数组坐标)
                // 注意额外的绕头旋转原点偏移处理
                switch (game_area[i][j].dir) {  //再根据缩放后半图宽调整printPoint（只以屏幕左上角为准，提前确定好旋转点（需要纸面推算））
                case 1:
                    painter.translate(BLOCK_SIZE/2, BLOCK_SIZE/2 - Half_scalingImgWidth);  //腿朝左，原点平移至右中心点后上拉
                    break;
                case 2:
                    painter.translate(0 + Half_scalingImgWidth, BLOCK_SIZE);     //腿朝上，原点平移至下中心点后右拉
                    break;
                case 3:
                    painter.translate(-BLOCK_SIZE/2, BLOCK_SIZE/2 + Half_scalingImgWidth);   //腿朝右，原点平移至左中心点后下拉
                    break;
                default:
                    painter.translate(-Half_scalingImgWidth, 0);    //腿朝下，原点左拉
                    break;
                }
                painter.rotate(90*game_area[i][j].dir);     //旋转后再次根据具体图宽调整printPoint
                painter.drawPixmap(QRect(0, 0, Half_scalingImgWidth*2, game_area[i][j].belong != Item ? BLOCK_SIZE*2 : BLOCK_SIZE), tImg);

                painter.restore();
            }
        }
}
void Widget::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
    case Qt::Key_Up:
        moveEffect->play();
        BlockTranslate(UP);
        break;
    case Qt::Key_Down:
        BlockTranslate(DOWN);
        break;
    case Qt::Key_Left:
//        moveEffect->play();
        BlockTranslate(LEFT);
        break;
    case Qt::Key_Right:
//        moveEffect->play();
        BlockTranslate(RIGHT);
        break;
    case Qt::Key_Space:
        BlockTranslate(SPACE);
        break;
    default:
        break;
    }
}
bool Widget::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == this){ //监视对象为本窗口游戏界面
        //过滤用户输入事件
           if (event->type() == QEvent::MouseButtonPress ||
               event->type() == QEvent::MouseButtonRelease ||
               event->type() == QEvent::MouseButtonDblClick ||
               event->type() == QEvent::KeyPress ||
               event->type() == QEvent::KeyRelease ||
               event->type() == QEvent::Wheel ||
               event->type() == QEvent::Enter ||
               event->type() == QEvent::Leave ||
               event->type() == QEvent::HoverEnter ||
               event->type() == QEvent::HoverLeave ||
               event->type() == QEvent::HoverMove)
           {
               return true;
           }
           return false;
    }
}

Widget::~Widget() { delete ui; }
