#include "game.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Mainmenu *menu = new Mainmenu();
    menu->initGame();
    menu->show();
    return a.exec();
}
