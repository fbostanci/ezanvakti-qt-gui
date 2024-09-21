#ifndef QTEZANVAKTI_H
#define QTEZANVAKTI_H

#include <QProcess>
#include <QMediaPlayer>
#include <QSystemTrayIcon>
#include <QMainWindow>


QT_BEGIN_NAMESPACE
namespace Ui { class QtEzanvakti; }
QT_END_NAMESPACE

class QtEzanvakti : public QMainWindow
{
    Q_OBJECT

public:
    explicit QtEzanvakti(QWidget *parent = nullptr);
    ~QtEzanvakti();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_pushButton_ba_clicked();
    void on_pushButton_bh_clicked();
    void on_pushButton_bb_clicked();
    void on_pushButton_sv_clicked();
    void on_pushButton_ik_clicked();
    void on_pushButton_mk_clicked();
    void on_pushButton_ei_clicked();
    void on_pushButton_ki_clicked();
    void on_pushButton_ed_clicked();
    void on_pushButton_kd_clicked();
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void durumDegisti(QMediaPlayer::MediaStatus);
    void slot_zamanlayici();

private:
    Ui::QtEzanvakti *ui;
    QTimer *zamanlayici;
    QMediaPlayer *oynatici;
    QAudioOutput *audioOutput;
    QProcess *bash;
    QTime *a, *b;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QAction *gizle;
    QAction *goster;
    QAction *cikis;

    void zamaniGuncelle();
    void vakitleriAl();
    void vakitleriYaz();
    void vakitleriSec();
    void siradakiVakitGoster();
    void ezvDenetle();
    void konumuYaz();
    void createActions();
    void createTrayIcon();
    void bildirimGonder(const QString &bildirim);
    void renkleriSifirla();
    void birSaniyedeGuncelle();
    void birGundeGuncelle();
    void ilkGuncelleme();

    QString ayarOku(const QString &ayar);
    QString kerahatVakit(const QString &vakit, int kvsure);
    QString sabah, gunes, ogle, ikindi, aksam, yatsi;
    QString kv_gunes, kv_ogle, kv_aksam, ysabah;
    QString svakit, svakit_adi;
    bool isGununBaslangici(int saniye) const;
    bool isVisible;
};

#endif // QTEZANVAKTI_H
