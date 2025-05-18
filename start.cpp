#include "start.h"
#include "game.h"
#include "ui_start.h"

Start::Start(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Start)
{
    ui->setupUi(this);
    int defaultWidth = 1200, defaultHeight = 675;
    this->setWindowTitle("BanG_Tetrix!");
    this->resize(defaultWidth, defaultHeight);
    //直接通过ui指针访问图片进行尺寸修改（可视化编程而不使用paintEvent（因直接套的非ui设计的主体代码，在游戏主体中没用上））
    ui->bg->setFixedSize(defaultWidth, defaultHeight);
    ui->begin->setStyleSheet("border:none;"); //ui类自带按钮

    //界面切换槽函数（也可使用窗口类自带的keyPressEvent()）
    connect(ui->begin, &QPushButton::clicked, this, [=](){switchToGame();}); //槽函数也需要lambda表达式封装！
}
Start::~Start(){ delete ui;}

void Start::initGame(){
    game = new Widget();
}

void Start::switchToGame()
{
    this->hide();
    this->game->initMenu(this); //为game的主菜单指针赋值！！
    this->game->show();
    this->game->InitGame();
}
