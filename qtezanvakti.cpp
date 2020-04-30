//--------------------------------------------------------------------------+
//                                                                          *
//                 "Ezanvakti için Qt arayüz uygulaması"                    *
//              Copyright(C) 2020, FB <ironic{at}yaani.com>                 *
//             https://gitlab.com/fbostanci/ezanvakti-qt-gui                *
//                      Ezanvakti-qt-gui v1.1                               *
//                            GPLv3                                         *
//                                                                          *
//--------------------------------------------------------------------------+
//                                                                          *
//    This program is free software: you can redistribute it and/or modify  *
//    it under the terms of the GNU General Public License as published by  *
//    the Free Software Foundation, either version 3 of the License, or     *
//    (at your option) any later version.                                   *
//                                                                          *
//    This program is distributed in the hope that it will be useful,       *
//    but WITHOUT ANY WARRANTY; without even the implied warranty of        *
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
//    GNU General Public License for more details.                          *
//                                                                          *
//    You should have received a copy of the GNU General Public License     *
//    along with this program.  If not, see <https://www.gnu.org/licenses/>.*
//                                                                          *
//--------------------------------------------------------------------------+

#include "qtezanvakti.h"
#include "ui_qtezanvakti.h"


#include <QPixmap>
#include <QDate>
#include <QTimer>
#include <QTime>
#include <QDebug>
#include <QCloseEvent>
#include <QRegularExpression>
#include <QStandardPaths>

QtEzanvakti::QtEzanvakti(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::QtEzanvakti)
{
    ui->setupUi(this);
    this->setWindowTitle("Ezanvakti Qt Arayüzü v1.1_0428_01");
    this->setWindowIcon(QIcon::fromTheme("ezanvakti"));
    this->setFixedWidth(552);
    this->setFixedHeight(352);

    QPixmap pm(":/images/ezanvakti96.png");
    ui->label_ezv->setPixmap(pm);

    ui->label_ezv->setScaledContents(true);
    ui->tabWidget->setCurrentIndex(0);
    //iptal düğmeleri etkisiz
    ui->pushButton_ki->setEnabled(false);
    ui->pushButton_ei->setEnabled(false);

    zamanlayici = new QTimer(this);
    zamanlayici->setInterval(10);
    zamanlayici->setTimerType(Qt::PreciseTimer);
    zamanlayici->setSingleShot(false);
    connect(zamanlayici, SIGNAL(timeout()), this, SLOT(slot_zamanlayici()));
    zamanlayici->start();

    oynatici = new QMediaPlayer(this);
    connect(oynatici, SIGNAL(stateChanged( QMediaPlayer::State )), this, SLOT(durumDegisti(QMediaPlayer::State)));
    oynatici->setVolume(ayarOku("SES=").toInt());

    bash = new QProcess(this);

    createActions();
    createTrayIcon();
    ilkGuncelleme();
}

QtEzanvakti::~QtEzanvakti()
{
    delete ui;
    delete oynatici;
    delete zamanlayici;
    delete bash;
    delete trayIcon;
    delete trayIconMenu;
    delete gizle;
    delete goster;
    delete cikis;
}

void QtEzanvakti::zamaniGuncelle()
{
    QDate tarih = QDate::currentDate();
    QTime saat = QTime::currentTime();
    QString tarihStr = tarih.toString("dd.MM.yyyy");
    QString saatStr = saat.toString("hh:mm:ss");
    // arayüz bugünün tarihi
    ui->label_st->setText(tarihStr);
    ui->label_ss->setText(saatStr);
}

QString QtEzanvakti::ayarOku(QString ayar)
{
    auto homePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString ayarD = (homePath + "/.config/ezanvakti/ayarlar");
    QFile inputFile(ayarD);
    if (inputFile.open(QIODevice::ReadOnly))
    {
        QTextStream in(&inputFile);
        while (!in.atEnd())
        {
            QString satir = in.readLine();
            QString tAyar(ayar);
            int knm = satir.indexOf(tAyar);
            if (knm >= 0)
            {
                QString donus = satir.mid(knm + tAyar.length());
                return donus.remove("'");
            }
        }
        inputFile.close();
    }
    return "null";
}

void QtEzanvakti::createActions()
{
    gizle = new QAction(QString("Gizle"), this);
    connect(gizle, SIGNAL(triggered()), this, SLOT(hide()));
    goster = new QAction(QString("Göster"), this);
    connect(goster, SIGNAL(triggered()), this, SLOT(show()));
    cikis = new QAction(QString("Çıkış"), this);
    connect(cikis, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void QtEzanvakti::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(goster);
    trayIconMenu->addAction(gizle);
    trayIconMenu->addAction(cikis);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(QIcon::fromTheme("ezanvakti"));
    trayIcon->setToolTip("Ezanvakti-qt");
    QString ileti = "Sistem tepsisinde başlatıldı";
    trayIcon->show();
    trayIcon->showMessage("Ezanvakti-qt", ileti, QSystemTrayIcon::Information, 1000);
}

void QtEzanvakti::closeEvent(QCloseEvent *event)
{
    if (trayIcon->isVisible())
    {
        this->hide();
        event->ignore();
    }
}

void QtEzanvakti::ezvDenetle()
{
    bash->start("bash", QStringList()<<"-c"<<"[[ -x $(which ezanvakti) ]] && { echo var ; }");
    bash->waitForFinished();
    QString output = bash->readAllStandardOutput();
    output = output.trimmed();

    if (QString::compare(output,"var") != 0)
    {
        qDebug() << "ezanvakti bulunamadı.";
        exit(1);
    }
}

QString QtEzanvakti::kerahatVakit(QString vakit, int kvsure)
{
    QTime saatS = QTime::fromString(vakit);
    saatS = saatS.addSecs(kvsure);
    QString saat = saatS.toString("hh:mm");

    return saat;
}

void QtEzanvakti::vakitleriAl()
{
   auto homePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
   QString ezanveri = (homePath + "/.config/ezanvakti/" + ayarOku("EZANVERI_ADI="));
   QDate tarih = QDate::currentDate();
   QString tarihStr = tarih.toString("dd.MM.yyyy");

   QFile inputFile(ezanveri);
   if (inputFile.open(QIODevice::ReadOnly))
   {
      QTextStream in(&inputFile);
      while (!in.atEnd())
      {
         QString line = in.readLine();
         QString tAyar(tarihStr);
         int pos = line.indexOf(tAyar);
         if (pos >= 0)
         {
            vakitler = line.split(QRegularExpression("\\s+"));
            vakitler << kerahatVakit(vakitler.at(2),2700);
            vakitler << kerahatVakit(vakitler.at(3),-2700);
            vakitler << kerahatVakit(vakitler.at(5),-2700);
         }
      }
      inputFile.close();
   }
   sabah = vakitler.at(1);
   gunes = vakitler.at(2);
   ogle = vakitler.at(3);
   ikindi = vakitler.at(4);
   aksam = vakitler.at(5);
   yatsi = vakitler.at(6);
   kv_gunes = vakitler.at(7);
   kv_ogle = vakitler.at(8);
   kv_aksam = vakitler.at(9);
}

void QtEzanvakti::vakitleriYaz()
{
    ui->label_sv->setText(sabah);
    ui->label_gv->setText(gunes);
    ui->label_ov->setText(ogle);
    ui->label_iv->setText(ikindi);
    ui->label_av->setText(aksam);
    ui->label_yv->setText(yatsi);
    QString kerahat1, kerahat2, kerahat3;
    QString kerahat4, kerahat5;

    kerahat1 =tr("%1 ~ %2 arası").arg(sabah).arg(gunes);
    kerahat2 =tr("%1 ~ %2 arası").arg(gunes).arg(kv_gunes);
    kerahat3 =tr("%1 ~ %2 arası").arg(kv_ogle).arg(ogle);
    kerahat4 =tr("%1 ~ %2 arası").arg(ikindi).arg(kv_aksam);
    kerahat5 =tr("%1 ~ %2 arası").arg(kv_aksam).arg(aksam);

    ui->label_kv1->setText(kerahat1);
    ui->label_kv2->setText(kerahat2);
    ui->label_kv3->setText(kerahat3);
    ui->label_kv4->setText(kerahat4);
    ui->label_kv5->setText(kerahat5);
}

void QtEzanvakti::konumuYaz()
{
    ui->label_ul->setText(ayarOku("ULKE="));
    ui->label_il->setText(ayarOku("ILCE="));
}

void QtEzanvakti::bildirimGonder(QString bildirim)
{
    QString komut;
    if(QString::compare(bildirim,"ayet") == 0)
        komut="ezanvakti --ayet -b";
    else if (QString::compare(bildirim,"hadis") == 0)
        komut="ezanvakti --hadis -b";
    else if (QString::compare(bildirim,"bilgi") == 0)
        komut="ezanvakti --bilgi -b";
    else if (QString::compare(bildirim,"vakit") == 0)
        komut="ezanvakti -vtb";
    else if (QString::compare(bildirim,"iftar") == 0)
        komut="ezanvakti --iftar -b";
     else if(QString::compare(bildirim,"kerahat") == 0)
        komut="ezanvakti -vkb";

    ezvDenetle();
    bash->start("bash", QStringList()<<"-c"<<komut);
    bash->waitForFinished();
}

void QtEzanvakti::on_pushButton_ba_clicked()
{
    bildirimGonder("ayet");
}
void QtEzanvakti::on_pushButton_bh_clicked()
{
    bildirimGonder("hadis");
}
void QtEzanvakti::on_pushButton_bb_clicked()
{
    bildirimGonder("bilgi");
}
void QtEzanvakti::on_pushButton_sv_clicked()
{
    bildirimGonder("vakit");
}
void QtEzanvakti::on_pushButton_ik_clicked()
{
    bildirimGonder("iftar");
}
void QtEzanvakti::on_pushButton_kv_clicked()
{
    bildirimGonder("kerahat");
}

void QtEzanvakti::on_pushButton_ki_clicked()
{
    on_pushButton_ei_clicked();
}

void QtEzanvakti::on_pushButton_ei_clicked()
{
    oynatici->stop();
    ui->pushButton_ki->setEnabled(false);
    ui->pushButton_kd->setEnabled(true);
    ui->pushButton_ed->setEnabled(true);
    ui->pushButton_ei->setEnabled(false);
}

void QtEzanvakti::on_pushButton_kd_clicked()
{
    ui->pushButton_ki->setEnabled(true);
    ui->pushButton_kd->setEnabled(false);
    ui->pushButton_ed->setEnabled(false);
    ui->pushButton_ei->setEnabled(false);

    QString sureG, okuyucu;
    sureG = ui->comboBox_su->currentText();
    QStringList sure = sureG.split(QRegularExpression("\\-"));
    okuyucu = ui->comboBox_ok->currentText();
    QString dinlet = tr("http://www.listen2quran.com/listen/%1/%2.mp3").arg(okuyucu).arg(sure.at(0));

    oynatici->setMedia(QUrl(dinlet));
    oynatici->play();
}

void QtEzanvakti::on_pushButton_ed_clicked()
{
    ui->pushButton_ki->setEnabled(false);
    ui->pushButton_kd->setEnabled(false);
    ui->pushButton_ed->setEnabled(false);
    ui->pushButton_ei->setEnabled(true);


    QString istenen = ui->comboBox_ez->currentText();
    if (QString::compare(istenen,"Sabah") == 0)
        oynatici->setMedia(QUrl::fromLocalFile(ayarOku("SABAH_EZANI=")));

    else if (QString::compare(istenen,"Öğle") == 0)
        oynatici->setMedia(QUrl::fromLocalFile(ayarOku("OGLE_EZANI=")));

    else if (QString::compare(istenen,"İkindi") == 0)
        oynatici->setMedia(QUrl::fromLocalFile(ayarOku("IKINDI_EZANI=")));

    else if (QString::compare(istenen,"Akşam") == 0)
        oynatici->setMedia(QUrl::fromLocalFile(ayarOku("AKSAM_EZANI=")));

    else if (QString::compare(istenen,"Yatsı") == 0)
        oynatici->setMedia(QUrl::fromLocalFile(ayarOku("YATSI_EZANI=")));

    else if (QString::compare(istenen,"Ezan Duası") == 0)
        oynatici->setMedia(QUrl::fromLocalFile(ayarOku("EZAN_DUASI=")));

    else if (QString::compare(istenen,"Cuma Selası") == 0)
        oynatici->setMedia(QUrl::fromLocalFile(ayarOku("CUMA_SELASI=")));

    oynatici->play();
}

void QtEzanvakti::durumDegisti(QMediaPlayer::State)
{
    if(oynatici->state() == QMediaPlayer::StoppedState)
    {
        ui->pushButton_ki->setEnabled(false);
        ui->pushButton_kd->setEnabled(true);
        ui->pushButton_ed->setEnabled(true);
        ui->pushButton_ei->setEnabled(false);
    }
}

void QtEzanvakti::renkleriSifirla() {
  ui->label_mv->setStyleSheet("");
  ui->label_s->setStyleSheet("");
  ui->label_sv->setStyleSheet("");
  ui->label_g->setStyleSheet("");
  ui->label_gv->setStyleSheet("");
  ui->label_o->setStyleSheet("");
  ui->label_ov->setStyleSheet("");
  ui->label_i->setStyleSheet("");
  ui->label_iv->setStyleSheet("");
  ui->label_a->setStyleSheet("");
  ui->label_av->setStyleSheet("");
  ui->label_y->setStyleSheet("");
  ui->label_yv->setStyleSheet("");

  ui->label_k1->setStyleSheet("");
  ui->label_kv1->setStyleSheet("");
  ui->label_k2->setStyleSheet("");
  ui->label_kv2->setStyleSheet("");
  ui->label_k3->setStyleSheet("");
  ui->label_kv3->setStyleSheet("");
  ui->label_k4->setStyleSheet("");
  ui->label_kv4->setStyleSheet("");
  ui->label_k5->setStyleSheet("");
  ui->label_kv5->setStyleSheet("");
}

void QtEzanvakti::vakitleriSec()
{
    QTime simdikiSaatS = QTime::currentTime();
    QString simdikiSaat = simdikiSaatS.toString("hh:mm");

    renkleriSifirla();
    if (simdikiSaat < sabah )
    {
        ui->label_mv->setText("Şimdi Yatsı Vakti");
        ui->label_mv->setStyleSheet("color: green;");
        ui->label_y->setStyleSheet("color: green;");
        ui->label_yv->setStyleSheet("color: green;");
        svakit_adi = "Sabah";
        svakit = sabah;

    } else if (simdikiSaat >= sabah && simdikiSaat < gunes) {
        ui->label_mv->setText("Şimdi Kerahat Vakti 1");
        ui->label_s->setStyleSheet("color: green;");
        ui->label_sv->setStyleSheet("color: green;");
        ui->label_mv->setStyleSheet("color: red;");
        ui->label_k1->setStyleSheet("color: red;");
        ui->label_kv1->setStyleSheet("color: red;");
        svakit_adi = "Öğle";
        svakit = ogle;

    } else if (simdikiSaat == gunes) {
        ui->label_mv->setText("Güneş Doğuş Vakti");
        ui->label_g->setStyleSheet("color: green;");
        ui->label_gv->setStyleSheet("color: green;");
        svakit_adi = "Öğle";
        svakit = ogle;

    } else if (simdikiSaat > gunes && simdikiSaat < kv_gunes) {
        ui->label_mv->setText("Şimdi Kerahat Vakti 2");
        ui->label_mv->setStyleSheet("color: red;");
        ui->label_k2->setStyleSheet("color: red;");
        ui->label_kv2->setStyleSheet("color: red;");
        svakit_adi = "Öğle";
        svakit = ogle;

    } else if (simdikiSaat >= kv_gunes && simdikiSaat < kv_ogle) {
        ui->label_mv->setText("Şimdi Kuşluk Vakti");
        svakit_adi = "Öğle";
        svakit = ogle;

    } else if (simdikiSaat < ogle && simdikiSaat >= kv_ogle) {
        ui->label_mv->setText("Şimdi Kerahat Vakti 3");
        ui->label_mv->setStyleSheet("color: red;");
        ui->label_k3->setStyleSheet("color: red;");
        ui->label_kv3->setStyleSheet("color: red;");
        svakit_adi = "Öğle";
        svakit = ogle;

    } else if (simdikiSaat >= ogle && simdikiSaat < ikindi) {
        ui->label_mv->setText("Şimdi Öğle Vakti");
        ui->label_mv->setStyleSheet("color: green;");
        ui->label_o->setStyleSheet("color: green;");
        ui->label_ov->setStyleSheet("color: green;");
        svakit_adi = "İkindi";
        svakit = ikindi;

    } else if (simdikiSaat >=ikindi && simdikiSaat < kv_aksam) {
        ui->label_mv->setText("Şimdi Kerahat Vakti 4");
        ui->label_mv->setStyleSheet("color: red;");
        ui->label_i->setStyleSheet("color: green;");
        ui->label_iv->setStyleSheet("color: green;");
        ui->label_k4->setStyleSheet("color: red;");
        ui->label_kv4->setStyleSheet("color: red;");
        svakit_adi = "Akşam";
        svakit = aksam;

    } else if (simdikiSaat < aksam && simdikiSaat >= kv_aksam) {
        ui->label_mv->setText("Şimdi Kerahat Vakti 5");
        ui->label_mv->setStyleSheet("color: red;");
        ui->label_k5->setStyleSheet("color: red;");
        ui->label_kv5->setStyleSheet("color: red;");
        svakit_adi = "Akşam";
        svakit = aksam;

    } else if (simdikiSaat >= aksam && simdikiSaat < yatsi) {
        ui->label_mv->setText("Şimdi Akşam Vakti");
        ui->label_mv->setStyleSheet("color: green;");
        ui->label_a->setStyleSheet("color: green;");
        ui->label_av->setStyleSheet("color: green;");
        svakit_adi = "Yatsı";
        svakit = yatsi;

    } else if (simdikiSaat < "24:00") {
        ui->label_mv->setText("Şimdi Yatsı Vakti");
        ui->label_mv->setStyleSheet("color: green;");
        ui->label_y->setStyleSheet("color: green;");
        ui->label_yv->setStyleSheet("color: green;");
        svakit_adi = "Yeni";
        svakit = "24:00"; //asla kullanilmayacak.
    }
}

void QtEzanvakti::siradakiVakitGoster()
{
    if (QString::compare(svakit_adi,"Yeni") == 0)
    {
        ui->label_np->setText("Sabah");
        ui->label_kp->setText("-- : --");
        trayIcon->setToolTip("Sabah vaktine kalan:\n -- : --");

    } else {
        ui->label_np->setText(svakit_adi);

        QTime a = QTime::currentTime();
        QTime b = QTime::fromString(svakit + ":00");
        int hedef_sure = a.secsTo(b);

        int saat = hedef_sure / 3600;
        int dakika = hedef_sure % 3600 / 60;
        int saniye = hedef_sure % 60;

        QString gerisayim = QString("%1 saat : %2 dakika : %3 saniye")
          .arg(saat, 2, 10, QChar('0'))
          .arg(dakika, 2, 10, QChar('0'))
          .arg(saniye, 2, 10, QChar('0'));

        ui->label_kp->setText(gerisayim);
        trayIcon->setToolTip(svakit_adi + " vaktine kalan:\n" + gerisayim);
    }
}

void QtEzanvakti::birSaniyedeGuncelle()
{
    zamaniGuncelle();
    siradakiVakitGoster();
}

void QtEzanvakti::birDakikadaGuncelle()
{
    vakitleriSec();
}

void QtEzanvakti::birGundeGuncelle()
{
    vakitleriAl();
    vakitleriYaz();
    vakitleriSec();
    konumuYaz();
}

void QtEzanvakti::ilkGuncelleme()
{
    vakitleriAl();
    vakitleriYaz();
    konumuYaz();
    birDakikadaGuncelle();
    //qDebug() << QTime::currentTime().toString("hh:mm:ss") << "ben ilk güncellemeyim";
}

void QtEzanvakti::slot_zamanlayici()
{
    int simdikiySaniye = (int)QTime::currentTime().msecsSinceStartOfDay()/100;
    if (simdikiySaniye%10 == 0) {
        birSaniyedeGuncelle();
        if (simdikiySaniye%600 == 0) {
            birDakikadaGuncelle();
            if (simdikiySaniye == 0) {
                birGundeGuncelle();
            }
        }
    }
}
