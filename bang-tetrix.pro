QT       += core gui
QT       += multimedia
QT       +=multimediawidgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0

win32: RC_ICONS = $$PWD/img/ui/bt.ico
macx: ICON = $$PWD/img/ui/bt.icns

INCLUDEPATH += $$PWD/ui $$PWD/gameplay $$PWD/score $$PWD/score/scoreTable

SOURCES += \
    main.cpp \
    ui/game.cpp \
    ui/mainmenu.cpp \
    ui/gameoverdialog.cpp \
    gameplay/blockLogic.cpp \
    gameplay/blockMatch.cpp \
    gameplay/blockMove.cpp \
    score/scoreinput.cpp \
    score/scoreinputdialog.cpp \
    score/scoreTable/scoremanager.cpp \
    score/scoreTable/scoretable.cpp

HEADERS += \
    gameplay/backgroundInfo.h \
    gameplay/blockInfo.h \
    ui/game.h \
    ui/gameoverdialog.h \
    ui/mainmenu.h \
    score/scoreinput.h \
    score/scoreinputdialog.h \
    score/scoreTable/scoremanager.h \
    score/scoreTable/scoretable.h

FORMS += \
    ui/game.ui \
    ui/mainmenu.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc
