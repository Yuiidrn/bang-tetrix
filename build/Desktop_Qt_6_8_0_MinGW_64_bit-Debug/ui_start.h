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
        bg->raise();
        begin->raise();

        retranslateUi(Start);

        begin->setDefault(false);


        QMetaObject::connectSlotsByName(Start);
    } // setupUi

    void retranslateUi(QWidget *Start)
    {
        Start->setWindowTitle(QCoreApplication::translate("Start", "Form", nullptr));
        begin->setText(QString());
        bg->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class Start: public Ui_Start {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_START_H
