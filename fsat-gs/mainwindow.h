#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QComboBox>
#include <QSerialPort>
#include <QMainWindow>

#include "qcgaugewidget.h"

typedef struct
{
    float a[3]; // Accelerometer [m/s^2]
    float m[3]; // Magnetometer [uT]
    float g[3]; // Gyroscope [deg/s]
    float e[3]; // Euler angles (yaw, pitch, roll) [deg]
    float b;    // Battery voltage [V]
    float w;    // Motor angular rate [deg/s]
} telemetry_t;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pusb_btconn_clicked();
    void on_push_btrefresh_clicked();
    void on_hslider_motor_sliderMoved(int position);
    void on_hslider_satrate_sliderMoved(int position);
    void on_hslider_satyaw_sliderMoved(int position);
    void handleReadyRead();

private:
    Ui::MainWindow *ui;

    void init_compass_gauge(void);
    void init_angular_rate_gauge(void);
    void init_inclinometer_gauge(void);
    void populate_serial_ports(QComboBox *cb);
    void populate_telemetry(const telemetry_t &t);

    QcGaugeWidget * mAttitudeGauge;
    QcNeedleItem * mAttitudeNeedle;
    QcAttitudeMeter *mAttMeter;
    QcGaugeWidget *mCompassGauge;
    QcNeedleItem *mCompassNeedle;
    QcGaugeWidget *mSpeedGauge;
    QcNeedleItem *mSpeedNeedle;
    QSerialPort serial;

    QByteArray rxbt;
};
#endif // MAINWINDOW_H
