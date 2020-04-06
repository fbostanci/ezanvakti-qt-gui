#ifndef QTEZANVAKTI_H
#define QTEZANVAKTI_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QAction>
#include <QMenu>

QT_BEGIN_NAMESPACE
namespace Ui { class QtEzanvakti; }
QT_END_NAMESPACE

class QtEzanvakti : public QMainWindow
{
    Q_OBJECT

public:
    QtEzanvakti(QWidget *parent = nullptr);
    ~QtEzanvakti();

private slots:
    void slot_zamanlayici();
    void ZamaniGuncelle();
    void createActions();
    
private:
    Ui::QtEzanvakti *ui;
    QTimer *zamanlayici;
    QTimer *bir_saniye;
    QStringList vakitler;
    void createTrayIcon();
    void vakitleriAl();
    void vakitleriYaz();
    void konumuYaz();
    
    QAction *gizle;
    QAction *goster;
    QAction *cikis;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
};
#endif // QTEZANVAKTI_H
