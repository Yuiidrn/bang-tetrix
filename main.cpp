#include "game.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Start *menu = new Start();
    menu->initGame();
    menu->show();
    return a.exec();
}
