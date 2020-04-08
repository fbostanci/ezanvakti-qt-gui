#ifndef QTEZANVAKTI_H
#define QTEZANVAKTI_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QAction>
#include <QMenu>
#include <QMultimedia>
#include <QMediaPlayer>

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
    void on_pushButton_ba_clicked();
    void on_pushButton_bh_clicked();
    void on_pushButton_bb_clicked();
    void on_pushButton_sv_clicked();
    void on_pushButton_ik_clicked();
    void on_pushButton_kv_clicked();
    void on_pushButton_ed_clicked();
    void on_pushButton_kd_clicked();
    void on_pushButton_ki_clicked();
    void on_pushButton_ei_clicked();

    
private:
    Ui::QtEzanvakti *ui;
    QTimer *zamanlayici;
    QTimer *bir_saniye;
    QMediaPlayer *oynatici;

    QStringList vakitler;
    void createTrayIcon();
    void vakitleriAl();
    void vakitleriYaz();
    void konumuYaz();
    void renkleriSifirla();
    void bildirimGonder(QString bildirim);
    
    QAction *gizle;
    QAction *goster;
    QAction *cikis;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
};
#endif // QTEZANVAKTI_H
