#include "scoreinputdialog.h"
#include <QPainter>
#include <QPixmap>
#include <QApplication>
#include <QScreen>
#include <QFontDatabase>
#include <QFile>

ScoreInputDialog::ScoreInputDialog(QWidget *parent)
    : QDialog(parent), playerScore(0), isNewHighScore(false), trophyLabel(nullptr)
{
    // 设置窗口属性
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setModal(true);
    
    // 初始化主布局
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(15);
    
    // 创建标题布局
    QHBoxLayout *titleLayout = new QHBoxLayout();
    
    // 添加标题标签
    headerLabel = new QLabel("游戏结束", this);
    QFont headerFont("微软雅黑", 18, QFont::Bold);
    headerLabel->setFont(headerFont);
    headerLabel->setAlignment(Qt::AlignCenter);
    headerLabel->setStyleSheet("color: #FFFFFF; background-color: transparent;");
    
    // 创建奖杯图标标签(最初隐藏)
    trophyLabel = new QLabel(this);
    trophyLabel->setFixedSize(32, 32);
    trophyLabel->setScaledContents(true);
    trophyLabel->hide();  // 默认隐藏，只有新高分时才显示
    
    // 将标题和奖杯添加到标题布局
    titleLayout->addStretch();
    titleLayout->addWidget(trophyLabel);
    titleLayout->addWidget(headerLabel);
    titleLayout->addStretch();
    
    // 添加分数标签
    scoreLabel = new QLabel("分数: 0", this);
    QFont scoreFont("微软雅黑", 22, QFont::Bold);
    scoreLabel->setFont(scoreFont);
    scoreLabel->setAlignment(Qt::AlignCenter);
    scoreLabel->setStyleSheet("color: #FFD700; background-color: transparent;");
    
    // 添加输入提示标签
    inputPromptLabel = new QLabel("请输入您的用户名:", this);
    QFont promptFont("微软雅黑", 12);
    inputPromptLabel->setFont(promptFont);
    inputPromptLabel->setStyleSheet("color: #FFFFFF; background-color: transparent;");
    
    // 添加用户名输入框
    nameInput = new QLineEdit(this);
    nameInput->setMinimumHeight(40);
    nameInput->setFont(QFont("微软雅黑", 12));
    nameInput->setStyleSheet(
        "QLineEdit {"
        "    background-color: rgba(255, 255, 255, 220);"
        "    border-radius: 10px;"
        "    padding: 5px 10px;"
        "    color: #333333;"
        "    border: 1px solid #CCCCCC;"
        "}"
        "QLineEdit:focus {"
        "    border: 2px solid #4CAF50;"
        "}"
    );
    nameInput->setMaxLength(20);
    
    // 添加按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(15);
    
    // 添加确认按钮
    okButton = new QPushButton("确认", this);
    okButton->setMinimumSize(120, 40);
    okButton->setFont(QFont("微软雅黑", 12));
    okButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #4CAF50;"
        "    color: white;"
        "    border-radius: 10px;"
        "    padding: 5px 15px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #45a049;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #3d8b40;"
        "}"
        "QPushButton:disabled {"
        "    background-color: #CCCCCC;"
        "    color: #888888;"
        "}"
    );
    
    // 添加取消按钮
    cancelButton = new QPushButton("取消", this);
    cancelButton->setMinimumSize(120, 40);
    cancelButton->setFont(QFont("微软雅黑", 12));
    cancelButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #F44336;"
        "    color: white;"
        "    border-radius: 10px;"
        "    padding: 5px 15px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #d32f2f;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #b71c1c;"
        "}"
    );
    
    // 组装按钮布局
    buttonLayout->addStretch();
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(okButton);
    buttonLayout->addStretch();
    
    // 组装主布局
    mainLayout->addLayout(titleLayout);
    mainLayout->addWidget(scoreLabel);
    mainLayout->addSpacing(20);
    mainLayout->addWidget(inputPromptLabel);
    mainLayout->addWidget(nameInput);
    mainLayout->addSpacing(20);
    mainLayout->addLayout(buttonLayout);
    
    // 添加窗口阴影效果
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(20);
    shadowEffect->setColor(QColor(0, 0, 0, 180));
    shadowEffect->setOffset(0, 0);
    this->setGraphicsEffect(shadowEffect);
    
    // 设置固定大小
    setMinimumSize(400, 350);
    resize(400, 350);
    
    // 连接信号和槽
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(nameInput, &QLineEdit::textChanged, this, &ScoreInputDialog::checkInput);
    
    // 将对话框移到屏幕中央
    QRect screenGeometry = QApplication::primaryScreen()->geometry();
    int x = (screenGeometry.width() - this->width()) / 2;
    int y = (screenGeometry.height() - this->height()) / 2;
    move(x, y);
    
    // 初始化背景图片路径为空
    backgroundImagePath = "";
    
    // 设置奖杯图标
    setupTrophyIcon();
}

ScoreInputDialog::~ScoreInputDialog()
{
}

void ScoreInputDialog::setupTrophyIcon()
{
    QFile file(":/imgs/img/ui/trophy.png");
    if (file.exists()) {
        QPixmap trophyPixmap(":/imgs/img/ui/trophy.png");
        trophyLabel->setPixmap(trophyPixmap);
    } else {
        // 如果找不到资源中的图片，则创建一个简单的奖杯图标
        QPixmap trophyPixmap(32, 32);
        trophyPixmap.fill(Qt::transparent);
        
        QPainter painter(&trophyPixmap);
        painter.setRenderHint(QPainter::Antialiasing);
        
        // 绘制简单的金色奖杯
        QColor goldColor(255, 215, 0);
        painter.setPen(Qt::NoPen);
        painter.setBrush(goldColor);
        
        // 杯身
        painter.drawRoundedRect(8, 4, 16, 20, 3, 3);
        
        // 把手
        painter.drawEllipse(3, 10, 6, 8);
        painter.drawEllipse(23, 10, 6, 8);
        
        // 底座
        painter.setBrush(QColor(180, 160, 0));
        painter.drawRoundedRect(10, 24, 12, 4, 1, 1);
        
        trophyLabel->setPixmap(trophyPixmap);
    }
}

void ScoreInputDialog::setScore(int score, bool isNewRecord)
{
    playerScore = score;
    isNewHighScore = isNewRecord;
    
    // 设置分数标签
    scoreLabel->setText(QString("分数: %1").arg(score));
    
    // 如果是新高分，更新标题和样式，并显示奖杯
    if (isNewHighScore) {
        headerLabel->setText("恭喜！新高分！");
        headerLabel->setStyleSheet("color: #FFD700; background-color: transparent;");
        
        // 显示奖杯图标
        trophyLabel->show();
        
        // 更新输入提示
        inputPromptLabel->setText("创造了新高分！请输入您的用户名:");
    } else {
        headerLabel->setText("游戏结束");
        headerLabel->setStyleSheet("color: #FFFFFF; background-color: transparent;");
        
        // 隐藏奖杯图标
        trophyLabel->hide();
        
        // 更新输入提示
        inputPromptLabel->setText("请输入您的用户名:");
    }
}

QString ScoreInputDialog::getPlayerName() const
{
    return nameInput->text();
}

void ScoreInputDialog::setDefaultName(const QString &name)
{
    nameInput->setText(name);
    nameInput->selectAll();
}

void ScoreInputDialog::setBackgroundImage(const QString &imagePath)
{
    backgroundImagePath = imagePath;
    update(); // 重绘窗口
}

void ScoreInputDialog::keyPressEvent(QKeyEvent *event)
{
    // 如果按下Esc键，则关闭对话框（相当于点击取消）
    if (event->key() == Qt::Key_Escape) {
        reject();
    }
    // 如果按下Enter键，且用户名不为空，则接受对话框（相当于点击确认）
    else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        if (!nameInput->text().isEmpty()) {
            accept();
        }
    }
    else {
        QDialog::keyPressEvent(event);
    }
}

void ScoreInputDialog::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 绘制圆角矩形作为背景
    QRect rect = this->rect();
    
    // 如果有背景图片，则绘制图片
    if (!backgroundImagePath.isEmpty()) {
        QPixmap pixmap(backgroundImagePath);
        if (!pixmap.isNull()) {
            pixmap = pixmap.scaled(rect.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            painter.drawPixmap(rect, pixmap);
            
            // 在背景图片上方绘制半透明黑色遮罩，以确保文字清晰可读
            painter.setBrush(QColor(0, 0, 0, 180));
            painter.setPen(Qt::NoPen);
            painter.drawRoundedRect(rect, 15, 15);
        } else {
            // 如果加载图片失败，回退到默认背景
            drawDefaultBackground(painter, rect);
        }
    } else {
        // 如果没有设置背景图片，绘制默认背景
        drawDefaultBackground(painter, rect);
    }
    
    // 调用父类方法绘制其他内容
    QDialog::paintEvent(event);
}

void ScoreInputDialog::drawDefaultBackground(QPainter &painter, const QRect &rect)
{
    // 创建垂直渐变作为默认背景
    QLinearGradient gradient(rect.topLeft(), rect.bottomRight());
    
    if (isNewHighScore) {
        // 新高分时使用更加华丽的渐变
        gradient.setColorAt(0, QColor(75, 75, 75));
        gradient.setColorAt(0.3, QColor(100, 80, 0));
        gradient.setColorAt(0.7, QColor(50, 50, 50));
        gradient.setColorAt(1, QColor(30, 30, 30));
    } else {
        // 普通分数时使用蓝色到深蓝的渐变
        gradient.setColorAt(0, QColor(59, 89, 152));
        gradient.setColorAt(0.5, QColor(41, 62, 105));
        gradient.setColorAt(1, QColor(23, 35, 60));
    }
    
    painter.setBrush(gradient);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect, 15, 15);
}

void ScoreInputDialog::checkInput()
{
    // 检查输入是否为空，更新确认按钮状态
    okButton->setEnabled(!nameInput->text().isEmpty());
} 
