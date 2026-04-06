#include "gameoverdialog.h"
#include <QPainter>
#include <QPixmap>
#include <QApplication>
#include <QScreen>
#include <QGraphicsDropShadowEffect>

GameOverDialog::GameOverDialog(QWidget *parent)
    : QDialog(parent), result(Quit)
{
    // 设置窗口属性
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setModal(true);
    
    // 初始化主布局
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(15);
    
    // 添加标题标签
    headerLabel = new QLabel("游戏结束", this);
    QFont headerFont("微软雅黑", 24, QFont::Bold);
    headerLabel->setFont(headerFont);
    headerLabel->setAlignment(Qt::AlignCenter);
    headerLabel->setStyleSheet("color: #FFFFFF; background-color: transparent;");
    
    // 添加提示标签
    promptLabel = new QLabel("请选择后续操作", this);
    QFont promptFont("微软雅黑", 16);
    promptLabel->setFont(promptFont);
    promptLabel->setAlignment(Qt::AlignCenter);
    promptLabel->setStyleSheet("color: #EEEEEE; background-color: transparent;");
    
    // 创建按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(20);
    
    // 创建三个按钮
    restartButton = new QPushButton("重新开始", this);
    mainMenuButton = new QPushButton("返回主菜单", this);
    quitButton = new QPushButton("退出游戏", this);
    
    // 设置按钮样式
    setupStyles();
    
    // 组装按钮布局
    buttonLayout->addWidget(restartButton);
    buttonLayout->addWidget(mainMenuButton);
    buttonLayout->addWidget(quitButton);
    
    // 组装主布局
    mainLayout->addWidget(headerLabel);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(promptLabel);
    mainLayout->addSpacing(20);
    mainLayout->addLayout(buttonLayout);
    
    // 添加窗口阴影效果
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(20);
    shadowEffect->setColor(QColor(0, 0, 0, 180));
    shadowEffect->setOffset(0, 0);
    this->setGraphicsEffect(shadowEffect);
    
    // 设置固定大小
    setMinimumSize(400, 250);
    resize(400, 250);
    
    // 连接信号和槽
    connect(restartButton, &QPushButton::clicked, [this]() {
        result = Restart;
        accept();
    });
    
    connect(mainMenuButton, &QPushButton::clicked, [this]() {
        result = MainMenu;
        accept();
    });
    
    connect(quitButton, &QPushButton::clicked, [this]() {
        result = Quit;
        reject();
    });
    
    // 将对话框移到屏幕中央
    QRect screenGeometry = QApplication::primaryScreen()->geometry();
    int x = (screenGeometry.width() - this->width()) / 2;
    int y = (screenGeometry.height() - this->height()) / 2;
    move(x, y);
    
    // 初始化背景图片路径为空
    backgroundImagePath = "";
}

GameOverDialog::~GameOverDialog()
{
}

void GameOverDialog::setBackgroundImage(const QString &imagePath)
{
    backgroundImagePath = imagePath;
    update(); // 重绘窗口
}

GameOverDialog::GameOverResult GameOverDialog::getResult() const
{
    return result;
}

void GameOverDialog::keyPressEvent(QKeyEvent *event)
{
    // 如果按下Esc键，则关闭对话框
    if (event->key() == Qt::Key_Escape) {
        result = Quit;
        reject();
    }
    // 如果按下Enter键，则默认"重新开始"操作
    else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        result = Restart;
        accept();
    }
    else {
        QDialog::keyPressEvent(event);
    }
}

void GameOverDialog::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 如果设置了背景图片，则使用图片，否则使用默认背景
    if (!backgroundImagePath.isEmpty() && QFile::exists(backgroundImagePath)) {
        QPixmap bgImage(backgroundImagePath);
        painter.drawPixmap(rect(), bgImage, bgImage.rect());
        
        // 添加半透明遮罩以确保文字清晰可见
        painter.setBrush(QColor(0, 0, 0, 180));
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(rect(), 15, 15);
    } else {
        drawDefaultBackground(painter, rect());
    }
    
    QDialog::paintEvent(event);
}

bool GameOverDialog::eventFilter(QObject *watched, QEvent *event)
{
    return QDialog::eventFilter(watched, event);
}

void GameOverDialog::drawDefaultBackground(QPainter &painter, const QRect &rect)
{
    // 创建渐变背景
    QLinearGradient gradient(rect.topLeft(), rect.bottomRight());
    gradient.setColorAt(0, QColor(45, 45, 62, 240));
    gradient.setColorAt(1, QColor(25, 25, 37, 240));
    
    painter.setPen(Qt::NoPen);
    painter.setBrush(gradient);
    
    // 绘制圆角矩形作为背景
    painter.drawRoundedRect(rect, 15, 15);
    
    // 绘制边框
    QPen borderPen(QColor(80, 80, 120, 150), 2);
    painter.setPen(borderPen);
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(rect.adjusted(1, 1, -1, -1), 15, 15);
}

void GameOverDialog::setupStyles()
{
    // 设置通用按钮样式
    QString buttonCommonStyle = 
        "QPushButton {"
        "    color: white;"
        "    border-radius: 10px;"
        "    padding: 8px 16px;"
        "    font: bold 14px '微软雅黑';"
        "}"
        "QPushButton:hover {"
        "    opacity: 0.9;"
        "}"
        "QPushButton:pressed {"
        "    opacity: 0.7;"
        "}";
    
    // 重新开始按钮 - 绿色
    restartButton->setStyleSheet(
        buttonCommonStyle +
        "QPushButton {"
        "    background-color: #4CAF50;"
        "}"
        "QPushButton:hover {"
        "    background-color: #45a049;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #3d8b40;"
        "}"
    );
    
    // 返回主菜单按钮 - 蓝色
    mainMenuButton->setStyleSheet(
        buttonCommonStyle +
        "QPushButton {"
        "    background-color: #2196F3;"
        "}"
        "QPushButton:hover {"
        "    background-color: #0b7dda;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #0a69b7;"
        "}"
    );
    
    // 退出游戏按钮 - 红色
    quitButton->setStyleSheet(
        buttonCommonStyle +
        "QPushButton {"
        "    background-color: #F44336;"
        "}"
        "QPushButton:hover {"
        "    background-color: #d32f2f;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #b71c1c;"
        "}"
    );
} 
