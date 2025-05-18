/********************************************************************************
** Form generated from reading UI file 'start.ui'
**
** Created by: Qt User Interface Compiler version 6.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_START_H
#define UI_START_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Start
{
public:
    QPushButton *begin;
    QLabel *bg;
    QPushButton *showScoreTableButton;

    void setupUi(QWidget *Start)
    {
        if (Start->objectName().isEmpty())
            Start->setObjectName("Start");
        Start->resize(1200, 675);
        begin = new QPushButton(Start);
        begin->setObjectName("begin");
        begin->setEnabled(true);
        begin->setGeometry(QRect(0, 0, 1200, 675));
        begin->setMouseTracking(false);
        begin->setToolTipDuration(-8);
        begin->setStyleSheet(QString::fromUtf8("ui->pushButton->setStyleSheet(\"QPushButton:hover { background-color: none; }\");\n"
""));
        begin->setCheckable(false);
        begin->setAutoDefault(false);
        begin->setFlat(true);
        bg = new QLabel(Start);
        bg->setObjectName("bg");
        bg->setGeometry(QRect(0, 0, 1200, 675));
        bg->setPixmap(QPixmap(QString::fromUtf8(":/imgs/img/ui/welcome.png")));
        showScoreTableButton = new QPushButton(Start);
        showScoreTableButton->setObjectName("showScoreTableButton");
        showScoreTableButton->setGeometry(QRect(80, 280, 209, 50));
        showScoreTableButton->setMinimumSize(QSize(200, 50));
        QFont font;
        font.setFamilies({QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
        font.setPointSize(12);
        font.setBold(true);
        showScoreTableButton->setFont(font);
        showScoreTableButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: #4CAF50;\n"
"    color: white;\n"
"    border-radius: 10px;\n"
"    padding: 8px 16px;\n"
"}\n"
"QPushButton:hover {\n"
"    background-color: #45a049;\n"
"}\n"
"QPushButton:pressed {\n"
"    background-color: #3d8b40;\n"
"}"));
        bg->raise();
        begin->raise();
        showScoreTableButton->raise();

        retranslateUi(Start);

        begin->setDefault(false);


        QMetaObject::connectSlotsByName(Start);
    } // setupUi

    void retranslateUi(QWidget *Start)
    {
        Start->setWindowTitle(QCoreApplication::translate("Start", "Form", nullptr));
        begin->setText(QString());
        bg->setText(QString());
        showScoreTableButton->setText(QCoreApplication::translate("Start", "\346\216\222\350\241\214\346\246\234", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Start: public Ui_Start {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_START_H
