#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QComboBox>
#include <QSerialPort>
#include <QMainWindow>

#include "qcgaugewidget.h"

// Enum used on fsat-ctrl too!
typedef enum
{
    // Motor PID
    TCMD_KPM,
    TCMD_KIM,
    TCMD_KDM,
    TCMD_TFM,
    TCMD_BM,
    TCMD_CM,

    // Satellite rate PID
    TCMD_KPW,
    TCMD_KIW,
    TCMD_KDW,
    TCMD_TFW,
    TCMD_BW,
    TCMD_CW,

    // Satellite yaw PID
    TCMD_KPY,
    TCMD_KIY,
    TCMD_KDY,
    TCMD_TFY,
    TCMD_BY,
    TCMD_CY,

    // ctrl_mode_t
    TCMD_CTRL_MODE,

    // Set points
    TCMD_SP_MW,
    TCMD_SP_SW,
    TCMD_SP_SY,

    // Must be at last!
    TCMD_LENGTH
} tcommand_t;

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

    void on_push_set_spm_clicked();

    void on_push_set_spy_clicked();

    void on_push_set_spw_clicked();

private:
    Ui::MainWindow *ui;

    void transmit_telecommand(const tcommand_t cmd, const double value);
    void init_compass_gauge(void);
    void init_angular_rate_gauge(void);
    void init_inclinometer_gauge(void);
    void populate_serial_ports(QComboBox *cb);
    void populate_telemetry(const telemetry_t &t);

    double spm = 0.0, spw = 0.0, spy = 0.0; // Set points
    double kpm = 0.0, kim = 0.0, kdm = 0.0; // PID params: Motor angular rate
    double kpw = 0.0, kiw = 0.0, kdw = 0.0; // PID params: Satellite angular rate
    double kpy = 0.0, kiy = 0.0, kdy = 0.0; // PID params: Satellite yaw

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
