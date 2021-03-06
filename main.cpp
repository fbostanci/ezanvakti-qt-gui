#include "qtezanvakti.h"

#include <QApplication>
#include <QSharedMemory>

int main(int argc, char *argv[])
{
    QSharedMemory _singular("EzanvaktiQtInstance");
    if(_singular.attach(QSharedMemory::ReadOnly))
    {
        _singular.detach();
        return -42;
    } else {
        _singular.create(1);
    }
    QApplication a(argc, argv);
    QtEzanvakti w;
    w.show();
    return a.exec();
}
