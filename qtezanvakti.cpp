//--------------------------------------------------------------------------+
//                                                                          *
//                 "Ezanvakti için Qt arayüz uygulaması"                    *
//              Copyright(C) 2020, FB <ironic{at}yaani.com>                 *
//             https://gitlab.com/fbostanci/ezanvakti-qt-gui                *
//                      Ezanvakti-qt-gui v1.2                               *
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

#include <QTimer>
#include <QMediaPlayer>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QFile>
#include <QDateTime>
#include <QAudioOutput>
#include <QCloseEvent>
#include <QStandardPaths>

QtEzanvakti::QtEzanvakti(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::QtEzanvakti)
    , zamanlayici(new QTimer(this))
    , oynatici(new QMediaPlayer(this))
    , audioOutput(new QAudioOutput(this))
    , bash(new QProcess(this))
    , trayIcon(new QSystemTrayIcon(this))
    , trayIconMenu(new QMenu(this))
{
    ui->setupUi(this);


    this->setWindowTitle("Ezanvakti Qt Arayüzü");
    this->setWindowIcon(QIcon::fromTheme("ezanvakti"));
    this->setFixedSize(552, 352);

    QPixmap pm(":/images/ezanvakti96.png");
    ui->label_ezv->setPixmap(pm);

    ui->label_ezv->setScaledContents(true);
    ui->tabWidget->setCurrentIndex(0);
    //iptal düğmeleri etkisiz
    ui->pushButton_ki->setEnabled(false);
    ui->pushButton_ei->setEnabled(false);

    // Diğer kurulumlar
    connect(zamanlayici, &QTimer::timeout, this, &QtEzanvakti::slot_zamanlayici);
    zamanlayici->start(1000);
    connect(trayIcon, &QSystemTrayIcon::activated, this, &QtEzanvakti::trayIconActivated);
    connect(oynatici, &QMediaPlayer::mediaStatusChanged, this, &QtEzanvakti::durumDegisti);

    oynatici->setAudioOutput(audioOutput);
    audioOutput->setVolume(ayarOku("SES=").toInt());

    // Tray icon setup
    createActions();
    createTrayIcon();
    ilkGuncelleme();
}

QtEzanvakti::~QtEzanvakti()
{
    delete ui;
}

void QtEzanvakti::createActions()
{
    gizle = new QAction(QString("Gizle"), this);
    connect(gizle, &QAction::triggered, this, &QtEzanvakti::hide);

    goster = new QAction(QString("Göster"), this);
    connect(goster, &QAction::triggered, this, &QtEzanvakti::show);

    cikis = new QAction(QString("Çıkış"), this);
    connect(cikis, &QAction::triggered, qApp, &QCoreApplication::quit);
}

void QtEzanvakti::zamaniGuncelle()
{
    ui->label_st->setText(QDate::currentDate().toString("dd.MM.yyyy"));
    ui->label_ss->setText(QTime::currentTime().toString("hh:mm:ss"));
}

QString QtEzanvakti::ayarOku(const QString &ayar)
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
            if (satir.startsWith(ayar))
            {
                return satir.mid(ayar.length()).remove("'");
            }
        }
        inputFile.close();
    }
    return "null";
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
    trayIcon->show();
    trayIcon->showMessage("Ezanvakti-qt", tr("Sistem tepsisinde başlatıldı"), QSystemTrayIcon::Information, 1000);
}

void QtEzanvakti::closeEvent(QCloseEvent *event)
{
    if (trayIcon->isVisible())
    {
        hide();
        event->ignore();
    }
}

void QtEzanvakti::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::DoubleClick) {
        if (isVisible) {
            hide(); // Gizle
        } else {
            show(); // Göster
        }
        isVisible = !isVisible;
    } else if (reason == QSystemTrayIcon::Context) {
        trayIconMenu->exec(QCursor::pos());
    }
}

void QtEzanvakti::ezvDenetle()
{
    bash->start("bash", QStringList() << "-c" << "ezanvakti --qt v");

    if (!bash->waitForFinished()) {
        qWarning() << "Komut çalıştırma başarısız oldu:" << bash->errorString();
        exit(1);
    }

    QString output = bash->readAllStandardOutput().trimmed();

    if (output.isEmpty()) {
        qWarning() << "ezanvakti bulunamadı veya çıktı boş.";
        exit(1);
    } else {
        qDebug() << "ezanvakti başarıyla bulundu. Çıktı:" << output;
    }
}

QString QtEzanvakti::kerahatVakit(const QString &vakit, int kvsure)
{
    return QTime::fromString(vakit).addSecs(kvsure).toString("hh:mm");
}

void QtEzanvakti::vakitleriAl()
{
    auto homePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString ezanveri = homePath + "/.config/ezanvakti/" + ayarOku("EZANVERI_ADI=");
    QDate tarih = QDate::currentDate();
    QDate ytarih = tarih.addDays(1);

    QString ytarihStr = ytarih.toString("dd.MM.yyyy");
    QString tarihStr = tarih.toString("dd.MM.yyyy");
    int kerahat_suresi = ayarOku("KERAHAT_SURESI=").toInt() * 60;

    QFile inputFile(ezanveri);
    if (!inputFile.exists()) {
        qWarning() << "Dosya bulunamadı:" << ezanveri;
        return;
    }

    if (!inputFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Dosya açılamadı:" << inputFile.errorString();
        return;
    }

    QTextStream in(&inputFile);
    QStringList vakitler;

    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.contains(tarihStr)) {
            static const QRegularExpression vakitRegex("\\s+");
            vakitler = line.split(QRegularExpression(vakitRegex));
            if (vakitler.size() >= 6) {  // Örnek olarak 6 öğe kontrolü
                vakitler << kerahatVakit(vakitler.at(2), kerahat_suresi);
                vakitler << kerahatVakit(vakitler.at(3), -kerahat_suresi);
                vakitler << kerahatVakit(vakitler.at(5), -kerahat_suresi);
            }
        } else if (line.contains(ytarihStr)) {
            static const QRegularExpression yvakitRegex("\\s+");
            QStringList yvakitler = line.split(QRegularExpression(yvakitRegex));
            if (yvakitler.size() >= 2) {  // Örnek olarak 2 öğe kontrolü
                vakitler << yvakitler.at(1);
            }
        }
    }

    if (vakitler.size() >= 11) {  // Tüm gerekli öğeler mevcut mu kontrolü
        sabah = vakitler.at(1);
        gunes = vakitler.at(2);
        ogle = vakitler.at(3);
        ikindi = vakitler.at(4);
        aksam = vakitler.at(5);
        yatsi = vakitler.at(6);
        kv_gunes = vakitler.at(7);
        kv_ogle = vakitler.at(8);
        kv_aksam = vakitler.at(9);
        ysabah = vakitler.at(10);
    } else {
        qWarning() << "Vakitler listesi yeterli öğeye sahip değil.";
    }
}



void QtEzanvakti::vakitleriYaz()
{
    ui->label_sv->setText(sabah);
    ui->label_gv->setText(gunes);
    ui->label_ov->setText(ogle);
    ui->label_iv->setText(ikindi);
    ui->label_av->setText(aksam);
    ui->label_yv->setText(yatsi);

    ui->label_kv1->setText(tr("%1 ~ %2").arg(sabah, gunes));
    ui->label_kv2->setText(tr("%1 ~ %2").arg(gunes, kv_gunes));
    ui->label_kv3->setText(tr("%1 ~ %2").arg(kv_ogle, ogle));
    ui->label_kv4->setText(tr("%1 ~ %2").arg(ikindi, kv_aksam));
    ui->label_kv5->setText(tr("%1 ~ %2").arg(kv_aksam, aksam));
}

void QtEzanvakti::konumuYaz()
{
    ui->label_ul->setText(ayarOku("ULKE="));
    ui->label_il->setText(ayarOku("ILCE="));
}

void QtEzanvakti::bildirimGonder(const QString &bildirim)
{
    static const QMap<QString, QString> komutMap = {
        {"ayet", "ezanvakti --ayet -b"},
        {"hadis", "ezanvakti --hadis -b"},
        {"bilgi", "ezanvakti --bilgi -b"},
        {"vakit", "ezanvakti -vtb"},
        {"iftar", "ezanvakti --iftar -b"},
        {"imsak", "ezanvakti --imsak -b"}
    };

    QString komut = komutMap.value(bildirim, "");
    if (!komut.isEmpty())
    {
        bash->start("bash", QStringList() << "-c" << komut);
        if (!bash->waitForFinished()) {
            qWarning() << "Komut çalıştırma başarısız oldu:" << bash->errorString();
        }
    }
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

void QtEzanvakti::on_pushButton_mk_clicked()
{
    bildirimGonder("imsak");
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

void QtEzanvakti::on_pushButton_ed_clicked()
{
    oynatici->stop();
    // Butonların durumunu güncelle
    ui->pushButton_ki->setEnabled(false);
    ui->pushButton_kd->setEnabled(false);
    ui->pushButton_ed->setEnabled(false);
    ui->pushButton_ei->setEnabled(true);

    // Seçilen ezan türünü al
    const QString ezanTuru = ui->comboBox_ez->currentText();

    // Ezan türü ile ilişkilendirilmiş ayar anahtarını belirle
    static const QMap<QString, QString> ezanTuruMap = {
        {"Sabah", "SABAH_EZANI="},
        {"Öğle", "OGLE_EZANI="},
        {"İkindi", "IKINDI_EZANI="},
        {"Akşam", "AKSAM_EZANI="},
        {"Yatsı", "YATSI_EZANI="},
        {"Ezan Duası", "EZAN_DUASI="},
        {"Cuma Selası", "CUMA_SELASI="}
    };
    const QString ayarAnahtari = ezanTuruMap.value(ezanTuru, QString());
    if (ayarAnahtari.isEmpty()) {
        qWarning() << "Geçersiz ezan türü seçildi:" << ezanTuru;
        return;
    }

    // Medya dosyasının yolunu al ve medya oynatıcıyı ayarla
    const QString dosyaYolu = ayarOku(ayarAnahtari);
    if (dosyaYolu.isEmpty()) {
        qWarning() << "Medya dosyası bulunamadı:" << dosyaYolu;
        return;
    }

    // Medya dosyasını ayarla ve çal
    oynatici->setSource(QUrl::fromLocalFile(dosyaYolu));
    oynatici->play();
}
void QtEzanvakti::on_pushButton_kd_clicked()
{
    oynatici->stop();
    // UI düğme durumlarını güncelle
    ui->pushButton_ki->setEnabled(true);
    ui->pushButton_kd->setEnabled(false);
    ui->pushButton_ed->setEnabled(false);
    ui->pushButton_ei->setEnabled(false);

    // Seçilen sure ve okuyucuyu al
    QString sureSecimi = ui->comboBox_su->currentText();
    QStringList sureParca = sureSecimi.split(QRegularExpression("\\-"));

    // Seçilen okuyucuyu al
    const QString okuyucu = ui->comboBox_ok->currentText();

    static const QMap<QString, QString> okuyucuMap = {
        {"Saad el Ghamdi", "sa3d_al-ghaamidi/complete"},
        {"As Shatry", "abu_bakr_ash-shatri_tarawee7"},
        {"Ahmad el Ajmy", "ahmed_ibn_3ali_al-3ajamy"}
    };
    const QString okuyan = okuyucuMap.value(okuyucu, QString());

    // Ses akışı için URL'yi oluştur
    QString sesUrl = QStringLiteral("https://download.quranicaudio.com/quran/%1/%2.mp3")
                         .arg(okuyan, sureParca.first());

    // Medya oynatıcı hazır mı kontrol et
    if (!oynatici) {
        qWarning() << "Medya oynatıcı başlatılmamış.";
        return;
    }

    // Medyayı ayarla ve çalmaya başla
    oynatici->setSource(QUrl(sesUrl));
    oynatici->play();

    // Çalma eylemini günlüğe kaydet
    qDebug() << "Sure:" << sureParca.first() << "okuyucu:" << okuyucu << "URL:" << sesUrl;
}

void QtEzanvakti::renkleriSifirla()
{

    // QLabel işaretçilerini içeren bir QVector oluşturun
    QVector<QLabel*> labels = {
        ui->label_mv, ui->label_s, ui->label_sv, ui->label_g, ui->label_gv,
        ui->label_o, ui->label_ov, ui->label_i, ui->label_iv, ui->label_a,
        ui->label_av, ui->label_y, ui->label_yv, ui->label_k1, ui->label_kv1,
        ui->label_k2, ui->label_kv2, ui->label_k3, ui->label_kv3, ui->label_k4,
        ui->label_kv4, ui->label_k5, ui->label_kv5
    };

    // QLabel'lerin stil özelliklerini sıfırlayın
    for (QLabel* label : labels) {
        if (label) { // Null kontrolü
            label->setStyleSheet("");
        }
    }
}

void QtEzanvakti::vakitleriSec()
{
    QTime simdikiSaatS = QTime::currentTime();
    QString simdikiSaat = simdikiSaatS.toString("hh:mm");

    // Renkleri sıfırla
    renkleriSifirla();

    // Saat dilimlerini ve stilleri belirleyen bir fonksiyon
    auto setLabelProperties = [&](const QString& text, const QString& color, const QVector<QLabel*>& labels) {
        ui->label_mv->setText(text);
        ui->label_mv->setStyleSheet(QString("color: %1;").arg(color));
        for (QLabel* label : labels) {
            if (label) {
                label->setStyleSheet(QString("color: %1;").arg(color));
            }
        }
    };

    // Vakitleri denetle ve uygun olanı ayarla
    if (simdikiSaat < sabah) {
        setLabelProperties("Şimdi Yatsı Vakti", "green", { ui->label_y, ui->label_yv });
        svakit_adi = "Sabah";
        svakit = sabah;

    } else if (simdikiSaat >= sabah && simdikiSaat < gunes) {
        setLabelProperties("Şimdi Kerahat Vakti 1", "red", { ui->label_k1, ui->label_kv1 });
        setLabelProperties("Gün Doğumu", "green", { ui->label_s, ui->label_sv });
        svakit_adi = "Gün Doğumu";
        svakit = gunes;

    } else if (simdikiSaat == gunes) {
        setLabelProperties("Güneş Doğuş Vakti", "red", { ui->label_g, ui->label_gv, ui->label_k2, ui->label_kv2 });
        svakit_adi = "Öğle";
        svakit = ogle;

    } else if (simdikiSaat > gunes && simdikiSaat < kv_gunes) {
        setLabelProperties("Şimdi Kerahat Vakti 2", "red", { ui->label_k2, ui->label_kv2 });
        svakit_adi = "Öğle";
        svakit = ogle;

    } else if (simdikiSaat >= kv_gunes && simdikiSaat < kv_ogle) {
        setLabelProperties("Şimdi Kuşluk Vakti", "", {});
        svakit_adi = "Öğle";
        svakit = ogle;

    } else if (simdikiSaat >= kv_ogle && simdikiSaat < ogle) {
        setLabelProperties("Şimdi Kerahat Vakti 3", "red", { ui->label_k3, ui->label_kv3 });
        svakit_adi = "Öğle";
        svakit = ogle;

    } else if (simdikiSaat >= ogle && simdikiSaat < ikindi) {
        setLabelProperties("Şimdi Öğle Vakti", "green", { ui->label_o, ui->label_ov });
        svakit_adi = "İkindi";
        svakit = ikindi;

    } else if (simdikiSaat >= ikindi && simdikiSaat < kv_aksam) {
        setLabelProperties("Şimdi Kerahat Vakti 4", "red", { ui->label_i, ui->label_iv, ui->label_k4, ui->label_kv4 });
        svakit_adi = "Akşam";
        svakit = aksam;

    } else if (simdikiSaat >= kv_aksam && simdikiSaat < aksam) {
        setLabelProperties("Şimdi Kerahat Vakti 5", "red", { ui->label_k5, ui->label_kv5 });
        svakit_adi = "Akşam";
        svakit = aksam;

    } else if (simdikiSaat >= aksam && simdikiSaat < yatsi) {
        setLabelProperties("Şimdi Akşam Vakti", "green", { ui->label_a, ui->label_av });
        svakit_adi = "Yatsı";
        svakit = yatsi;

    } else {
        setLabelProperties("Şimdi Yatsı Vakti", "green", { ui->label_y, ui->label_yv });
        svakit_adi = "Yeni";
        svakit = "23:59:59";
    }
}

void QtEzanvakti::siradakiVakitGoster()
{
    // Hedef süre değişkeni
    int hedef_sure = 0;

    // 'Yeni' vaktine özel işleme
    if (svakit_adi == "Yeni")
    {
        // Sabah vaktini göster
        ui->label_np->setText("Sabah (" + ysabah + ")");
        svakit_adi = "(Yarın) Sabah";

        // Hedef sabah vaktinin saniye cinsinden hesaplanması
        static const QRegularExpression colonRegex("\\:");
        QStringList ysabahV = ysabah.split(QRegularExpression(colonRegex));
        int sabahSaat = ysabahV.at(0).toInt();
        int sabahDakika = ysabahV.at(1).toInt();
        int sabahToplamSaniye = (sabahSaat * 3600) + (sabahDakika * 60);

        // Hedef süreyi güncelle
        hedef_sure = sabahToplamSaniye - a->currentTime().secsTo(b->fromString(svakit)) - 1;
    }
    else
    {
        // Diğer vakitler için hedef sürenin hesaplanması
        ui->label_np->setText(svakit_adi);
        hedef_sure = a->currentTime().secsTo(b->fromString(svakit + ":00"));
    }

    // Süreyi formatla: saat, dakika ve saniye
    QString gerisayim = QString("%1 saat : %2 dakika : %3 saniye")
                            .arg(hedef_sure / 3600, 2, 10, QChar('0'))
                            .arg((hedef_sure % 3600) / 60, 2, 10, QChar('0'))
                            .arg(hedef_sure % 60, 2, 10, QChar('0'));

    // UI ve tray icon güncellemeleri
    ui->label_kp->setText(gerisayim);
    trayIcon->setToolTip(svakit_adi + " vaktine kalan:\n" + gerisayim);
}

void QtEzanvakti::durumDegisti(QMediaPlayer::MediaStatus durum)
{
    switch (durum)
    {
    case QMediaPlayer::EndOfMedia:
        qDebug() << "Ses durduruldu.";
        ui->pushButton_ki->setEnabled(false);
        ui->pushButton_kd->setEnabled(true);
        ui->pushButton_ed->setEnabled(true);
        ui->pushButton_ei->setEnabled(false);
        break;
    default:
        break;
    }
}

void QtEzanvakti::birSaniyedeGuncelle()
{
    zamaniGuncelle();
    vakitleriSec();
    siradakiVakitGoster();
}

void QtEzanvakti::birGundeGuncelle()
{
    ezvDenetle();
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
    //qDebug() << QTime::currentTime().toString("hh:mm:ss") << "ben ilk güncellemeyim";
}

void QtEzanvakti::slot_zamanlayici()
{
    QTime currentTime = QTime::currentTime();
    QTime startOfDay(0, 0);  // 00:00:00

    int simdikiySaniye = startOfDay.secsTo(currentTime);

    // Arayüz ve diğer bileşenleri saniyede bir güncelle
    birSaniyedeGuncelle();

    // Günün başlangıcında (00:00:00) tüm güncellemeleri yap
    if (isGununBaslangici(simdikiySaniye))
        birGundeGuncelle();
}

/**
 * Günün başlangıcında olup olmadığını kontrol eder.
 * @param saniye Şu anki saniye değeri.
 * @return Günün başlangıcı ise true, aksi takdirde false döner.
 */
bool QtEzanvakti::isGununBaslangici(int saniye) const
{
    // Günün başlangıcı 00:00:00 olduğunda saniye değeri 0 olur
    return saniye == 0;
}
