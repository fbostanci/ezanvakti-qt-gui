//--------------------------------------------------------------------------+
//                                                                          *
//                 "Ezanvakti için Qt arayüz uygulaması"                    *
//              Copyright(C) 2020, FB <ironic{at}yaani.com>                 *
//             https://gitlab.com/fbostanci/ezanvakti-qt-gui                *
//                          Ezanvakti-qt-gui                                *
//                              GPL v3                                      *
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

#include <QProcess>
#include <QPixmap>
#include <QDate>
#include <QTimer>
#include <QTime>
#include <QDebug>
#include <QRegularExpression>

QtEzanvakti::QtEzanvakti(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::QtEzanvakti)
{
    ui->setupUi(this);
    QPixmap pm(":/images/ezanvakti96.png");
    ui->label_ezv->setPixmap(pm);
    ui->label_ezv->setScaledContents(true);

    createActions();
    createTrayIcon();
    bir_saniye = new QTimer(this);
    connect(bir_saniye, SIGNAL(timeout()), this, SLOT(ZamaniGuncelle()));
    bir_saniye->start(1000);
    vakitleriYaz();
    konumuYaz();

    zamanlayici = new QTimer(this);
    zamanlayici->setInterval(1000);
    zamanlayici->setSingleShot(false);
    connect(zamanlayici, SIGNAL(timeout()), this, SLOT(slot_zamanlayici()));
    zamanlayici->start();

}

QtEzanvakti::~QtEzanvakti()
{
    delete ui;
}

void QtEzanvakti::ZamaniGuncelle()
{
    QDate tarih = QDate::currentDate();
    QTime saat = QTime::currentTime();
    QString tarihStr = tarih.toString("dd.MM.yyyy");
    QString saatStr = saat.toString("hh:mm:ss");
    // arayüz bugünün tarihi
    ui->label_st->setText(tarihStr);
    ui->label_ss->setText(saatStr);
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
    QIcon icon(":/images/ezanvakti96.png");
    trayIcon->setIcon(icon);
    trayIcon->setToolTip("Ezanvakti-qt");
    QString ileti = "Sistem tepsisinde başlatıldı";
    trayIcon->show();
    trayIcon->showMessage("Ezanvakti-qt", ileti, QSystemTrayIcon::Information, 1000);
}

void QtEzanvakti::vakitleriAl()
{
    QProcess bash;
    bash.start("bash", QStringList()<<"-c"<<"ezanvakti --qt v");
    bash.waitForFinished();
    QString output = bash.readAllStandardOutput();
    output = output.trimmed();
    vakitler = output.split(QRegularExpression("\\s+"));
    //sabah vakitler.at(0)
    // güneş vakitler.at(1)
    // öğle vakitler.at(2)
    // ikindi vakitler.at(3)
    // akşam vakitler.at(4)
    // yatsı vakitler.at(5)
    // kv_gunes vakitler.at(6)
    // kv_ogle vakitler.at(7)
    // kv_aksam vakitler.at(8)
}

void QtEzanvakti::vakitleriYaz()
{
    vakitleriAl();
    ui->label_sv->setText(vakitler.at(0));
    ui->label_gv->setText(vakitler.at(1));
    ui->label_ov->setText(vakitler.at(2));
    ui->label_iv->setText(vakitler.at(3));
    ui->label_av->setText(vakitler.at(4));
    ui->label_yv->setText(vakitler.at(5));
    QString kerahat1, kerahat2, kerahat3;
    QString kerahat4, kerahat5;

    kerahat1 =tr("%1 ~ %2 arası").arg(vakitler.at(0)).arg(vakitler.at(1));
    kerahat2 =tr("%1 ~ %2 arası").arg(vakitler.at(1)).arg(vakitler.at(6));
    kerahat3 =tr("%1 ~ %2 arası").arg(vakitler.at(7)).arg(vakitler.at(2));
    kerahat4 =tr("%1 ~ %2 arası").arg(vakitler.at(3)).arg(vakitler.at(8));
    kerahat5 =tr("%1 ~ %2 arası").arg(vakitler.at(8)).arg(vakitler.at(4));

    ui->label_kv1->setText(kerahat1);
    ui->label_kv2->setText(kerahat2);
    ui->label_kv3->setText(kerahat3);
    ui->label_kv4->setText(kerahat4);
    ui->label_kv5->setText(kerahat5);
}

void QtEzanvakti::konumuYaz()
{
    QProcess bash;
    bash.start("bash", QStringList()<<"-c"<<"ezanvakti --qt k");
    bash.waitForFinished();

    QString output = bash.readAllStandardOutput();
    output = output.trimmed();
    QStringList konum = output.split(QRegularExpression("\\+"));

    ui->label_ul->setText(konum.at(0));
    ui->label_il->setText(konum.at(1));
}

void QtEzanvakti::slot_zamanlayici()
{
    QString sabah, gunes,ogle;
    QString  ikindi, aksam, yatsi;
    QString kv_gunes, kv_ogle, kv_aksam;

    QTime simdikiSaatS =QTime::currentTime();
    QString simdikiSaat = simdikiSaatS.toString("hh:mm");
    QString simdikiSaatY = simdikiSaatS.toString("hh:mm:ss");

    sabah = vakitler.at(0);
    gunes = vakitler.at(1);
    ogle = vakitler.at(2);
    ikindi = vakitler.at(3);
    aksam = vakitler.at(4);
    yatsi = vakitler.at(5);
    kv_gunes = vakitler.at(6);
    kv_ogle = vakitler.at(7);
    kv_aksam = vakitler.at(8);


    if (simdikiSaat < sabah )
    {
        ui->label_mv->setText("Şimdi Yatsı Vakti");
        ui->label_mv->setStyleSheet("color: green;");
        ui->label_y->setStyleSheet("color: green;");
        ui->label_yv->setStyleSheet("color: green;");

    } else if (simdikiSaat >= sabah && simdikiSaat < gunes) {
        ui->label_mv->setText("Şimdi Kerahat Vakti 1");
        ui->label_mv->setStyleSheet("color: red;");

    } else if (simdikiSaat == gunes) {
        ui->label_mv->setText("Güneş Doğuş Vakti");
        ui->label_g->setStyleSheet("color: green;");
        ui->label_gv->setStyleSheet("color: green;");

    } else if (simdikiSaat > gunes && simdikiSaat <= kv_gunes) {
        ui->label_mv->setText("Şimdi Kerahat Vakti 2");
        ui->label_mv->setStyleSheet("color: red;");
        ui->label_k2->setStyleSheet("color: red;");
        ui->label_kv2->setStyleSheet("color: red;");

    } else if (simdikiSaat > kv_gunes && simdikiSaat < kv_ogle) {
        ui->label_mv->setText("Şimdi Kuşluk Vakti");

    } else if (simdikiSaat < ogle && simdikiSaat >= kv_ogle) {
        ui->label_mv->setText("Şimdi Kerahat Vakti 3");
        ui->label_mv->setStyleSheet("color: red;");
        ui->label_k3->setStyleSheet("color: red;");
        ui->label_kv3->setStyleSheet("color: red;");

    } else if (simdikiSaat >= ogle && simdikiSaat < ikindi) {
        ui->label_mv->setText("Şimdi Öğle Vakti");
        ui->label_mv->setStyleSheet("color: green;");
        ui->label_o->setStyleSheet("color: green;");
        ui->label_ov->setStyleSheet("color: green;");

    } else if (simdikiSaat >=ikindi && simdikiSaat < kv_aksam) {
        ui->label_mv->setText("Şimdi Kerahat Vakti 4");
        ui->label_mv->setStyleSheet("color: red;");
        ui->label_k4->setStyleSheet("color: red;");
        ui->label_kv4->setStyleSheet("color: red;");

    } else if (simdikiSaat < aksam && simdikiSaat >= kv_aksam) {
        ui->label_mv->setText("Şimdi Kerahat Vakti 5");
        ui->label_mv->setStyleSheet("color: red;");
        ui->label_k5->setStyleSheet("color: red;");
        ui->label_kv5->setStyleSheet("color: red;");

    } else if (simdikiSaat >= aksam && simdikiSaat < yatsi) {
        ui->label_mv->setText("Şimdi Akşam Vakti");
        ui->label_mv->setStyleSheet("color: green;");
        ui->label_a->setStyleSheet("color: green;");
        ui->label_av->setStyleSheet("color: green;");

    } else if (simdikiSaat < "24:00") {
        ui->label_mv->setText("Şimdi Yatsı Vakti");
        ui->label_mv->setStyleSheet("color: green;");
        ui->label_y->setStyleSheet("color: green;");
        ui->label_yv->setStyleSheet("color: green;");

    } else if (simdikiSaatY == "00:00:00") {
        qDebug() << "vakitler güncelleniyor";
        vakitleriAl();

    }
}
