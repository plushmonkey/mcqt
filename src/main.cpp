#include "MainWindow.h"
#include <QApplication>
#include <mclib/block/Block.h>
#include "Settings.h"

int main(int argc, char *argv[])
{
    Settings::GetInstance();
    mc::block::BlockRegistry::GetInstance()->RegisterVanillaBlocks();
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
