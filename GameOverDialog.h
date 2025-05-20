#ifndef GAMEOVERDIALOG_H
#define GAMEOVERDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QKeyEvent>
#include <QPainter>
#include <QPixmap>
#include <QApplication>
#include <QScreen>
#include <QFont>
#include <QFile>

class GameOverDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GameOverDialog(QWidget *parent = nullptr);
    ~GameOverDialog();
    
    // 设置背景图片
    void setBackgroundImage(const QString &imagePath);
    
    // 获取用户选择的结果
    enum GameOverResult {
        Restart,
        MainMenu,
        Quit
    };
    
    GameOverResult getResult() const;

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QVBoxLayout *mainLayout;
    QLabel *headerLabel;
    QLabel *promptLabel;
    QPushButton *restartButton;
    QPushButton *mainMenuButton;
    QPushButton *quitButton;
    
    QString backgroundImagePath;
    GameOverResult result;
    
    // 绘制默认背景
    void drawDefaultBackground(QPainter &painter, const QRect &rect);
    
    // 设置样式
    void setupStyles();
};

#endif // GAMEOVERDIALOG_H 