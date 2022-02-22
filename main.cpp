#include "MapleCalc.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFont global("Microsoft Yahei UI");
    global.setPointSize(9);
    a.setFont(global);

    MapleCalc w;
    w.show();
    return a.exec();
}
