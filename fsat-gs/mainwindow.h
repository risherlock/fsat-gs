#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qcgaugewidget.h"

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
    void on_push_power_clicked();

private:
    Ui::MainWindow *ui;

    void init_compass_gauge(void);
    void init_angular_rate_gauge(void);
    void init_inclinometer_gauge(void);

    QcGaugeWidget * mAttitudeGauge;
    QcNeedleItem * mAttitudeNeedle;
    QcAttitudeMeter *mAttMeter;

    QcGaugeWidget * mCompassGauge;
    QcNeedleItem *mCompassNeedle;

    QcGaugeWidget * mSpeedGauge;
    QcNeedleItem *mSpeedNeedle;
};
#endif // MAINWINDOW_H
