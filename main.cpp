#include "qtezanvakti.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QtEzanvakti w;
    w.show();
    return a.exec();
}
