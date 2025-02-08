#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Power switch
    ui->push_power->setIcon(QIcon(":/icons/assets/pow_off.png"));

    // Bluetooth connection
    QPixmap pixmap_bt0(":/icons/assets/bt_dis.png");
    pixmap_bt0 = pixmap_bt0.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->label_bt->setPixmap(pixmap_bt0);

    // Battery levels
    QPixmap pixmap_batt(":/icons/assets/batt_what.png");
    pixmap_batt = pixmap_batt.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->label_batt->setPixmap(pixmap_batt);

    // Error
    QPixmap pixmap_error(":/icons/assets/error_no.png");
    pixmap_error = pixmap_error.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->label_err->setPixmap(pixmap_error);

    // Satellite switch
    QPixmap pixmap_satcon(":/icons/assets/sat_off.png");
    pixmap_satcon = pixmap_satcon.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->label_satcon->setPixmap(pixmap_satcon);

    mAttitudeGauge = new QcGaugeWidget;
    mAttitudeGauge = ui->widget;

    mAttitudeGauge->addBackground(99);
    QcBackgroundItem *bkg = mAttitudeGauge->addBackground(92);
    bkg->clearrColors();
    bkg->addColor(0.1,Qt::black);
    bkg->addColor(1.0,Qt::white);
    mAttMeter = mAttitudeGauge->addAttitudeMeter(88);

    mAttitudeNeedle = mAttitudeGauge->addNeedle(70);
    mAttitudeNeedle->setMinDegree(0);
    mAttitudeNeedle->setMaxDegree(180);
    mAttitudeNeedle->setValueRange(0,180);
    mAttitudeNeedle->setCurrentValue(90);
    mAttitudeNeedle->setColor(Qt::white);
    mAttitudeNeedle->setNeedle(QcNeedleItem::AttitudeMeterNeedle);
    mAttitudeGauge->addGlass(80);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_push_power_clicked()
{
    static bool flag = true;

    QPixmap pixmap_bt0(":/icons/assets/bt_dis.png");
    QPixmap pixmap_bt1(":/icons/assets/bt_con.png");
    pixmap_bt0 = pixmap_bt0.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    pixmap_bt1 = pixmap_bt1.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    if (flag)
    {
        ui->label_bt->setPixmap(pixmap_bt0);
        ui->push_power->setIcon(QIcon(":/icons/assets/pow_on.png"));
    }
    else
    {
        ui->label_bt->setPixmap(pixmap_bt1);
        ui->push_power->setIcon(QIcon(":/icons/assets/pow_off.png"));
    }

    flag = !flag;
}
