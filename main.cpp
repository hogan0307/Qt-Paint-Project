#include "frmmainwnd.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    frmMainWnd w;
    w.show();
    return a.exec();
}
