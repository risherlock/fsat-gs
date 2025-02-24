#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QComboBox>
#include <QSerialPort>
#include <QMainWindow>

#include "satconfig.h"
#include "qcgaugewidget.h"

typedef enum                // Different control modes
{
    CTRL_MODE_MOTOR_OMEGA,  // Motor angular rate control
    CTRL_MODE_SAT_OMEGA,    // Satellite angular rate control
    CTRL_MODE_SAT_YAW,      // Satellite angular position control
    CTRL_MODE_IDLE,         // Disable control
} ctrl_mode_t;

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

    void on_lineEdit_kpm_editingFinished();

    void on_lineEdit_kim_editingFinished();

    void on_pushButton_mw_clicked();

private:
    Ui::MainWindow *ui;

    void transmit_telecommand(const tcommand_t cmd, const double value);
    void init_compass_gauge(void);
    void init_angular_rate_gauge(void);
    void init_inclinometer_gauge(void);
    void populate_serial_ports(QComboBox *cb);
    void populate_telemetry(const telemetry_t &t);

    // Set point trackers
    double spm = 0.0, spw = 0.0, spy = 0.0; // Set points

    // PID params: Motor angular rate
    double kpm = SAT_GAINS_MOTOR_OMEGA_KP;
    double kim = SAT_GAINS_MOTOR_OMEGA_KI;

    // PID params: Satellite angular rate
    double kpw = SAT_GAINS_SAT_OMEGA_KP;
    double kiw = SAT_GAINS_SAT_OMEGA_KP;

    // PID params: Satellite yaw
    double kpy = SAT_GAINS_SAT_YAW_KP;
    double kiy = SAT_GAINS_SAT_YAW_KI;
    double kdy = SAT_GAINS_SAT_YAW_KD;

    QcGaugeWidget *mAttitudeGauge;
    QcNeedleItem *mAttitudeNeedle;
    QcAttitudeMeter *mAttMeter;
    QcGaugeWidget *mCompassGauge;
    QcNeedleItem *mCompassNeedle;
    QcGaugeWidget *mSpeedGauge;
    QcNeedleItem *mSpeedNeedle;
    QSerialPort serial;
    QLineEdit *last_edited = nullptr;
    QMap<QLineEdit*, QString> pid_params;
    QMap<QLineEdit*, tcommand_t> pid_params_id;

    QByteArray rxbt;
};
#endif // MAINWINDOW_H
