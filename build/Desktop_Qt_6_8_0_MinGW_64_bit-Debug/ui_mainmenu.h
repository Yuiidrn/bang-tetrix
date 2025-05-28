/********************************************************************************
** Form generated from reading UI file 'mainmenu.ui'
**
** Created by: Qt User Interface Compiler version 6.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINMENU_H
#define UI_MAINMENU_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Mainmenu
{
public:
    QPushButton *begin;
    QLabel *bg;
    QPushButton *showScoreTableButton;

    void setupUi(QWidget *Mainmenu)
    {
        if (Mainmenu->objectName().isEmpty())
            Mainmenu->setObjectName("Mainmenu");
        Mainmenu->resize(1200, 675);
        begin = new QPushButton(Mainmenu);
        begin->setObjectName("begin");
        begin->setEnabled(true);
        begin->setGeometry(QRect(190, 250, 161, 51));
        begin->setMouseTracking(true);
        begin->setToolTipDuration(-8);
        begin->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: #4CAF50;\n"
"    color: white;\n"
"    font:  16pt \"\345\215\216\346\226\207\345\275\251\344\272\221\";\n"
"    border-radius: 10px;\n"
"    padding: 8px 16px;\n"
"}\n"
"QPushButton:hover {\n"
"    background-color: #45a049;\n"
"}\n"
"QPushButton:pressed {\n"
"    background-color: #3d8b40;\n"
"}"));
#if QT_CONFIG(shortcut)
        begin->setShortcut(QString::fromUtf8(""));
#endif // QT_CONFIG(shortcut)
        begin->setCheckable(false);
        begin->setAutoDefault(false);
        begin->setFlat(false);
        bg = new QLabel(Mainmenu);
        bg->setObjectName("bg");
        bg->setGeometry(QRect(0, 0, 1200, 675));
        bg->setPixmap(QPixmap(QString::fromUtf8(":/imgs/img/ui/welcome.png")));
        showScoreTableButton = new QPushButton(Mainmenu);
        showScoreTableButton->setObjectName("showScoreTableButton");
        showScoreTableButton->setGeometry(QRect(190, 310, 161, 51));
        showScoreTableButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: #2196F3;\n"
"    color: white;\n"
"    font:  16pt \"\345\215\216\346\226\207\345\275\251\344\272\221\";\n"
"    border-radius: 10px;\n"
"    padding: 8px 16px;\n"
"}\n"
"QPushButton:hover {\n"
"    background-color: #1e88e5;\n"
"}\n"
"QPushButton:pressed {\n"
"    background-color: #1976d2;\n"
"}"));
        bg->raise();
        begin->raise();
        showScoreTableButton->raise();

        retranslateUi(Mainmenu);

        begin->setDefault(false);


        QMetaObject::connectSlotsByName(Mainmenu);
    } // setupUi

    void retranslateUi(QWidget *Mainmenu)
    {
        Mainmenu->setWindowTitle(QCoreApplication::translate("Mainmenu", "Form", nullptr));
        begin->setText(QCoreApplication::translate("Mainmenu", "\345\274\200\345\247\213\346\270\270\346\210\217", nullptr));
        bg->setText(QString());
#if QT_CONFIG(tooltip)
        showScoreTableButton->setToolTip(QCoreApplication::translate("Mainmenu", "<html><head/><body><p>\351\230\277\346\235\276\345\244\247</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        showScoreTableButton->setText(QCoreApplication::translate("Mainmenu", "\346\216\222\350\241\214\346\246\234", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Mainmenu: public Ui_Mainmenu {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINMENU_H
