#include "gameoverdialog.h"
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QApplication>
#include <QScreen>
#include <QGraphicsDropShadowEffect>

namespace {
constexpr int kPanelRadius = 18;
} // namespace

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
    
    auto *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(28);
    shadowEffect->setColor(QColor(20, 5, 12, 150));
    shadowEffect->setOffset(0, 6);
    setGraphicsEffect(shadowEffect);
    
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
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const QRect r = rect();
    QPainterPath clipPath;
    clipPath.addRoundedRect(r, kPanelRadius, kPanelRadius);
    painter.setClipPath(clipPath);

    if (!backgroundImagePath.isEmpty()) {
        const QPixmap pixmap(backgroundImagePath);
        if (!pixmap.isNull()) {
            const QPixmap scaled =
                pixmap.scaled(r.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            painter.drawPixmap(r, scaled);
            drawBusyBackgroundOverlay(painter, r);
        } else {
            painter.setClipping(false);
            drawDefaultBackground(painter, r);
        }
    } else {
        painter.setClipping(false);
        drawDefaultBackground(painter, r);
    }

    painter.setClipping(false);
    QDialog::paintEvent(event);
}

void GameOverDialog::drawBusyBackgroundOverlay(QPainter &painter, const QRect &rect)
{
    const QPoint topMid(rect.center().x(), rect.top());
    const QPoint botMid(rect.center().x(), rect.bottom());
    QLinearGradient wash(topMid, botMid);
    wash.setColorAt(0.0, QColor(88, 20, 48, 218));
    wash.setColorAt(0.4, QColor(48, 12, 32, 198));
    wash.setColorAt(1.0, QColor(22, 6, 16, 232));
    painter.fillRect(rect, wash);

    const qreal cx = rect.center().x();
    const qreal cy = rect.center().y() + rect.height() * 0.06;
    const qreal rad = qMax(rect.width(), rect.height()) * 0.78;
    QRadialGradient vig(cx, cy, rad);
    vig.setColorAt(0.0, QColor(248, 200, 220, 0));
    vig.setColorAt(0.38, QColor(0, 0, 0, 0));
    vig.setColorAt(1.0, QColor(0, 0, 0, 155));
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.fillRect(rect, vig);
}

bool GameOverDialog::eventFilter(QObject *watched, QEvent *event)
{
    return QDialog::eventFilter(watched, event);
}

void GameOverDialog::drawDefaultBackground(QPainter &painter, const QRect &rect)
{
    // 无海报图时与分数对话框普通态一致的深蓝渐变 + 同圆角
    QLinearGradient gradient(rect.topLeft(), rect.bottomRight());
    gradient.setColorAt(0, QColor(59, 89, 152));
    gradient.setColorAt(0.5, QColor(41, 62, 105));
    gradient.setColorAt(1, QColor(23, 35, 60));

    painter.setPen(Qt::NoPen);
    painter.setBrush(gradient);
    painter.drawRoundedRect(rect, kPanelRadius, kPanelRadius);

    QPen borderPen(QColor(255, 255, 255, 36), 1);
    painter.setPen(borderPen);
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(rect.adjusted(1, 1, -1, -1), kPanelRadius, kPanelRadius);
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
