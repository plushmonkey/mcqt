#include "mainwindow.h"
#include <QApplication>
#include "mclib/Block.h"

int main(int argc, char *argv[])
{
    Minecraft::BlockRegistry::GetInstance()->RegisterVanillaBlocks();
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
