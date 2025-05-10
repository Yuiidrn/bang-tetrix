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
    //直接通过ui指针访问图片进行尺寸修改（不使用paintEvent的可视化编程（因直接套的非ui设计的主题代码，在游戏主体中没用上））
    ui->bg->setFixedSize(defaultWidth, defaultHeight);
    ui->begin->setStyleSheet("border:none;");

    //界面切换槽函数
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
