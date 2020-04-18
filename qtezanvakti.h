#ifndef QTEZANVAKTI_H
#define QTEZANVAKTI_H

#include <QMainWindow>
#include <QProcess>
#include <QSystemTrayIcon>
#include <QAction>
#include <QMenu>
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

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void slot_zamanlayici();
    void createActions();
    void durumDegisti(QMediaPlayer::State state);
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
    QMediaPlayer *oynatici;
    QProcess *bash;

    QAction *gizle;
    QAction *goster;
    QAction *cikis;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    QStringList vakitler;
    QString svakit, svakit_adi;

    void baslat();
    void createTrayIcon();
    void zamaniGuncelle();
    void siradakiVakitGoster();
    void birSaniyedeGuncelle();
    void birDakikadaGuncelle();
    void ezvDenetle();
    void birSaatteGuncelle();
    void birGundeGuncelle();
    void ilkGuncelleme();

    void vakitleriAl();
    void vakitleriYaz();
    void vakitleriSec();
    void konumuYaz();
    void renkleriSifirla();
    void bildirimGonder(QString bildirim);
};
#endif // QTEZANVAKTI_H
