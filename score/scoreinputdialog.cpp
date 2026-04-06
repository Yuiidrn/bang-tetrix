#include "scoreinputdialog.h"
#include <QFrame>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QApplication>
#include <QScreen>
#include <QFontDatabase>
#include <QFile>
#include <QEvent>

namespace {

constexpr int kPanelRadius = 18;

void addLabelShadow(QLabel *label, const QColor &color, int blur, int offsetY = 1)
{
    auto *fx = new QGraphicsDropShadowEffect(label);
    fx->setBlurRadius(blur);
    fx->setColor(color);
    fx->setOffset(0, offsetY);
    label->setGraphicsEffect(fx);
}

const QString kLineEditStyleNormal = QStringLiteral(
    "QLineEdit {"
    "    background-color: rgba(255, 245, 248, 0.76);"
    "    border: 1px solid rgba(255, 255, 255, 0.45);"
    "    border-radius: 14px;"
    "    padding: 11px 18px;"
    "    color: #2d1f2a;"
    "    font-size: 13pt;"
    "    selection-background-color: #db2777;"
    "    selection-color: #ffffff;"
    "}"
    "QLineEdit:focus {"
    "    border: 2px solid rgba(244, 114, 182, 0.92);"
    "    background-color: rgba(255, 255, 255, 0.95);"
    "}"
    "QLineEdit:disabled {"
    "    background-color: rgba(245, 245, 245, 0.65);"
    "    color: #888888;"
    "}"
);

const QString kLineEditStyleEmptyError = QStringLiteral(
    "QLineEdit {"
    "    background-color: rgba(255, 240, 242, 0.88);"
    "    border: 2px solid rgba(248, 113, 113, 0.75);"
    "    border-radius: 14px;"
    "    padding: 11px 18px;"
    "    color: #3d2834;"
    "    font-size: 13pt;"
    "    selection-background-color: #db2777;"
    "    selection-color: #ffffff;"
    "}"
    "QLineEdit:focus {"
    "    border: 2px solid rgba(244, 114, 182, 0.92);"
    "    background-color: rgba(255, 255, 255, 0.95);"
    "}"
);

} // namespace

void ScoreInputDialog::applyNameInputStyle(bool emptyError)
{
    nameInput->setStyleSheet(emptyError ? kLineEditStyleEmptyError : kLineEditStyleNormal);
}

ScoreInputDialog::ScoreInputDialog(QWidget *parent)
    : QDialog(parent), trophyLabel(nullptr), playerScore(0), maxCombo(0), isNewHighScore(false)
{
    // 设置窗口属性
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setModal(true);

    // 初始化主布局（边距与区块间距，适配海报类背景）
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(26, 28, 26, 32);
    mainLayout->setSpacing(0);

    auto *titleLayout = new QHBoxLayout();
    titleLayout->setSpacing(10);

    headerLabel = new QLabel(QStringLiteral("游戏结束"), this);
    headerLabel->setFont(QFont(QStringLiteral("微软雅黑"), 20, QFont::Bold));
    headerLabel->setAlignment(Qt::AlignCenter);
    headerLabel->setStyleSheet(QStringLiteral("color: #fff8fc; background: transparent;"));

    trophyLabel = new QLabel(this);
    trophyLabel->setFixedSize(40, 40);
    trophyLabel->setScaledContents(true);
    trophyLabel->hide();

    titleLayout->addStretch();
    titleLayout->addWidget(trophyLabel);
    titleLayout->addWidget(headerLabel);
    titleLayout->addStretch();

    addLabelShadow(headerLabel, QColor(0, 0, 0, 168), 14, 2);

    auto *statsCard = new QFrame(this);
    statsCard->setObjectName(QStringLiteral("ScoreStatsCard"));
    statsCard->setStyleSheet(
        QStringLiteral("QFrame#ScoreStatsCard {"
                       " background-color: rgba(0, 0, 0, 0.26);"
                       " border-radius: %1px;"
                       " border: 1px solid rgba(255, 255, 255, 0.14);"
                       "}")
            .arg(kPanelRadius));

    auto *statsLayout = new QVBoxLayout(statsCard);
    statsLayout->setContentsMargins(20, 18, 20, 18);
    statsLayout->setSpacing(10);
    statsLayout->setAlignment(Qt::AlignCenter);

    scoreLabel = new QLabel(QStringLiteral("分数: 0"), this);
    scoreLabel->setFont(QFont(QStringLiteral("微软雅黑"), 24, QFont::Bold));
    scoreLabel->setAlignment(Qt::AlignCenter);
    scoreLabel->setStyleSheet(QStringLiteral("color: #fff1b5; background: transparent;"));

    comboLabel = new QLabel(QStringLiteral("最大连击: 0"), this);
    comboLabel->setFont(QFont(QStringLiteral("微软雅黑"), 17, QFont::DemiBold));
    comboLabel->setAlignment(Qt::AlignCenter);
    comboLabel->setStyleSheet(QStringLiteral("color: #ffd8e8; background: transparent;"));

    statsLayout->addWidget(scoreLabel);
    statsLayout->addWidget(comboLabel);

    addLabelShadow(scoreLabel, QColor(50, 5, 30, 210), 12, 2);
    addLabelShadow(comboLabel, QColor(40, 5, 25, 185), 10, 1);

    auto *inputSection = new QFrame(this);
    inputSection->setObjectName(QStringLiteral("ScoreInputSection"));
    inputSection->setStyleSheet(
        QStringLiteral("QFrame#ScoreInputSection {"
                       " background-color: rgba(0, 0, 0, 0.2);"
                       " border-radius: %1px;"
                       " border: 1px solid rgba(255, 210, 230, 0.16);"
                       "}")
            .arg(kPanelRadius));

    auto *inputVBox = new QVBoxLayout(inputSection);
    inputVBox->setContentsMargins(18, 16, 18, 16);
    inputVBox->setSpacing(12);

    inputPromptLabel = new QLabel(QStringLiteral("请输入您的用户名:"), this);
    inputPromptLabel->setFont(QFont(QStringLiteral("微软雅黑"), 11, QFont::DemiBold));
    inputPromptLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    inputPromptLabel->setStyleSheet(
        QStringLiteral("QLabel { color: rgba(255,105,180, 0.92); background: transparent; "
                       "letter-spacing: 1px; }"));

    nameInput = new QLineEdit(this);
    nameInput->setMinimumHeight(48);
    nameInput->setFont(QFont(QStringLiteral("微软雅黑"), 12));
    applyNameInputStyle(false);
    nameInput->setMaxLength(20);

    auto *inputGlow = new QGraphicsDropShadowEffect(nameInput);
    inputGlow->setBlurRadius(22);
    inputGlow->setColor(QColor(236, 72, 153, 72));
    inputGlow->setOffset(0, 5);
    nameInput->setGraphicsEffect(inputGlow);

    nameInput->installEventFilter(this);

    inputVBox->addWidget(inputPromptLabel);
    inputVBox->addWidget(nameInput);

    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(18);

    okButton = new QPushButton(QStringLiteral("确认"), this);
    okButton->setMinimumSize(120, 40);
    okButton->setFont(QFont("微软雅黑", 12));
    okButton->setStyleSheet(
        "QPushButton {"
        "    color: #ffffff;"
        "    border: none;"
        "    border-radius: 12px;"
        "    padding: 8px 18px;"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "        stop:0 #f9a8d4, stop:1 #db2777);"
        "}"
        "QPushButton:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "        stop:0 #fbcfe8, stop:1 #be185d);"
        "}"
        "QPushButton:pressed {"
        "    background: #9d174d;"
        "}"
        "QPushButton:disabled {"
        "    background: #d4d4d8;"
        "    color: #71717a;"
        "}"
    );

    // 添加取消按钮
    cancelButton = new QPushButton(QStringLiteral("取消"), this);
    cancelButton->setMinimumSize(120, 40);
    cancelButton->setFont(QFont("微软雅黑", 12));
    cancelButton->setStyleSheet(
        "QPushButton {"
        "    color: #ffffff;"
        "    border: none;"
        "    border-radius: 12px;"
        "    padding: 8px 18px;"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "        stop:0 #fda4af, stop:1 #e11d48);"
        "}"
        "QPushButton:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "        stop:0 #fecdd3, stop:1 #be123c);"
        "}"
        "QPushButton:pressed {"
        "    background: #9f1239;"
        "}"
    );

    // 组装按钮布局
    buttonLayout->addStretch();
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(okButton);
    buttonLayout->addStretch();

    mainLayout->addLayout(titleLayout);
    mainLayout->addSpacing(14);
    mainLayout->addWidget(statsCard);
    mainLayout->addSpacing(16);
    mainLayout->addWidget(inputSection);
    mainLayout->addSpacing(18);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addStretch(1);

    auto *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(28);
    shadowEffect->setColor(QColor(20, 5, 12, 150));
    shadowEffect->setOffset(0, 6);
    setGraphicsEffect(shadowEffect);

    setMinimumSize(440, 390);
    resize(440, 390);

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

    if (isNewHighScore) {
        headerLabel->setText(QStringLiteral("恭喜！新高分！"));
        headerLabel->setStyleSheet(QStringLiteral("color: #ffe8a8; background: transparent;"));
        trophyLabel->show();
        inputPromptLabel->setText(QStringLiteral("创造了新高分！请输入您的用户名:"));
    } else {
        headerLabel->setText(QStringLiteral("游戏结束"));
        headerLabel->setStyleSheet(QStringLiteral("color: #fff8fc; background: transparent;"));
        trophyLabel->hide();
        inputPromptLabel->setText(QStringLiteral("请输入您的用户名:"));
    }
}

void ScoreInputDialog::setMaxCombo(int combo)
{
    maxCombo = combo;
    comboLabel->setText(QString("最大连击: %1").arg(combo));
}

QString ScoreInputDialog::getPlayerName() const { return nameInput->text(); }

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

void ScoreInputDialog::drawBusyBackgroundOverlay(QPainter &painter, const QRect &rect)
{
    // 竖向酒红渐变：与 Pico 系海报红框、暖色画面统一，中间略透保留细节
    const QPoint topMid(rect.center().x(), rect.top());
    const QPoint botMid(rect.center().x(), rect.bottom());
    QLinearGradient wash(topMid, botMid);
    wash.setColorAt(0.0, QColor(240,248,255, 130));
    wash.setColorAt(1.0, QColor(240,248,255, 110));
    painter.fillRect(rect, wash);

    // 暗角：边缘再压一层，减轻四角杂讯抢字
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

void ScoreInputDialog::drawDefaultBackground(QPainter &painter, const QRect &rect)
{
    QLinearGradient gradient(rect.topLeft(), rect.bottomRight());

    if (isNewHighScore) {
        gradient.setColorAt(0, QColor(75, 75, 75));
        gradient.setColorAt(0.3, QColor(100, 80, 0));
        gradient.setColorAt(0.7, QColor(50, 50, 50));
        gradient.setColorAt(1, QColor(30, 30, 30));
    } else {
        gradient.setColorAt(0, QColor(59, 89, 152));
        gradient.setColorAt(0.5, QColor(41, 62, 105));
        gradient.setColorAt(1, QColor(23, 35, 60));
    }

    painter.setBrush(gradient);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect, kPanelRadius, kPanelRadius);
}

void ScoreInputDialog::checkInput()
{
    // 检查输入是否为空，更新确认按钮状态
    bool isEmpty = nameInput->text().trimmed().isEmpty();
    okButton->setEnabled(!isEmpty);

    // 如果输入框失去焦点且为空，显示提示
    if (isEmpty && !nameInput->hasFocus()) {
        applyNameInputStyle(true);
        inputPromptLabel->setText(QStringLiteral("<span style='color:#fecaca;'>用户名不能为空</span>"));
    } else {
        applyNameInputStyle(false);
        if (isNewHighScore) {
            inputPromptLabel->setText(QStringLiteral("创造了新高分！请输入您的用户名:"));
        } else {
            inputPromptLabel->setText(QStringLiteral("请输入您的用户名:"));
        }
    }
}

bool ScoreInputDialog::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == nameInput
        && (event->type() == QEvent::FocusOut || event->type() == QEvent::FocusIn)) {
        checkInput();
    }
    return QDialog::eventFilter(watched, event);
}
