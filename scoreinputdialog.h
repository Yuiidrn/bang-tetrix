#ifndef SCOREINPUTDIALOG_H
#define SCOREINPUTDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QKeyEvent>
#include <QPainter>

class ScoreInputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ScoreInputDialog(QWidget *parent = nullptr);
    ~ScoreInputDialog();

    void setScore(int score, bool isNewRecord); // 设置分数和是否为新高分
    void setMaxCombo(int combo);                // 设置最大连击数
    QString getPlayerName() const;              // 获取输入的用户名
    void setDefaultName(const QString &name);   // 设置默认用户名
    void setBackgroundImage(const QString &imagePath); // 设置背景图片

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QVBoxLayout *mainLayout;
    QLabel *headerLabel;
    QLabel *scoreLabel;
    QLabel *comboLabel;
    QLabel *inputPromptLabel;
    QLineEdit *nameInput;
    QPushButton *okButton;
    QPushButton *cancelButton;
    
    // 奖杯图标标签
    QLabel *trophyLabel;
    
    QString backgroundImagePath;
    int playerScore;
    int maxCombo;
    bool isNewHighScore;
    
    // 绘制默认背景
    void drawDefaultBackground(QPainter &painter, const QRect &rect);

    // 设置奖杯图标
    void setupTrophyIcon();

private slots:
    void checkInput();
};

#endif // SCOREINPUTDIALOG_H 
