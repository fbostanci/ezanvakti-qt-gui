#include "qtezanvakti.h"
#include <QApplication>
#include <QSharedMemory>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Tekil örnek kontrolü için QSharedMemory kullanımı
    QSharedMemory sharedMemory("EzanvaktiQtInstance");

    // Paylaşılan bellek oluşturulmaya çalışılıyor
    if (!sharedMemory.create(1)) {
        QMessageBox::information(nullptr, "Bilgi", "Uygulamanın başka bir örneği zaten çalışıyor.");
        return -1;
    }

    QtEzanvakti mainWindow;
    mainWindow.show();

    return app.exec();
}
