#include <QSerialPortInfo>
#include <QDebug>

#include "satconfig.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#define TCMD_START_CHAR '$'
#define TCMD_DELIMITER  ':'
#define TCMD_STOP_CHAR  '#'

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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

    // AHRS instruments
    init_compass_gauge();
    init_angular_rate_gauge();
    init_inclinometer_gauge();

    // Graph colour and width
    QPen setPointPen(QColor(0, 114, 189));
    QPen feedbackPen(QColor(217, 83, 25));
    setPointPen.setWidth(2);
    feedbackPen.setWidth(2);

    // Display default values
    ui->lineEdit_kpm->setText(QString::number(SAT_GAINS_MOTOR_OMEGA_KP));
    ui->lineEdit_kim->setText(QString::number(SAT_GAINS_MOTOR_OMEGA_KI));
    ui->lineEdit_kpw->setText(QString::number(SAT_GAINS_SAT_OMEGA_KP));
    ui->lineEdit_kid->setText(QString::number(SAT_GAINS_SAT_OMEGA_KI));
    ui->lineEdit_kpy->setText(QString::number(SAT_GAINS_SAT_YAW_KP));
    ui->lineEdit_kiy->setText(QString::number(SAT_GAINS_SAT_YAW_KI));
    ui->lineEdit_kdy->setText(QString::number(SAT_GAINS_SAT_YAW_KD));

    // Store the values to line edit in the QMap
    pid_params[ui->lineEdit_kpm] = ui->lineEdit_kpm->text();
    pid_params[ui->lineEdit_kim] = ui->lineEdit_kim->text();
    pid_params[ui->lineEdit_kpw] = ui->lineEdit_kpw->text();
    pid_params[ui->lineEdit_kid] = ui->lineEdit_kid->text();
    pid_params[ui->lineEdit_kpy] = ui->lineEdit_kpy->text();
    pid_params[ui->lineEdit_kiy] = ui->lineEdit_kiy->text();
    pid_params[ui->lineEdit_kdy] = ui->lineEdit_kdy->text();
    pid_params_id[ui->lineEdit_kpm] = TCMD_KPM;
    pid_params_id[ui->lineEdit_kim] = TCMD_KIM;
    pid_params_id[ui->lineEdit_kpw] = TCMD_KPW;
    pid_params_id[ui->lineEdit_kid] = TCMD_KIW;
    pid_params_id[ui->lineEdit_kpy] = TCMD_KPY;
    pid_params_id[ui->lineEdit_kiy] = TCMD_KIY;
    pid_params_id[ui->lineEdit_kdy] = TCMD_KDY;

    // Lambda function to track changes and reset unconfirmed fields
    auto track_line_edit = [this](QLineEdit *current)
    {
        // Reset previous field if another is edited
        if (last_edited && last_edited != current)
        {
            last_edited->setText(pid_params[last_edited]);
        }
        last_edited = current;
    };

    auto process_button_click = [this]()
    {
        if (last_edited)
        {
            // Log the confirmed value of the last edited field
            qDebug() << "Confirmed value of last edited field:" << last_edited->text();

            // Update the PID parameters map with the last edited value
            pid_params[last_edited] = last_edited->text();

            // Convert the value to double and send the command
            const double value = last_edited->text().toDouble();
            tcommand_t cmd = pid_params_id[last_edited];
            transmit_telecommand(cmd, value);
        }

        // Reset the last edited tracker
        last_edited = nullptr;
    };

    // Connect each line edit to the common lambda
    connect(ui->lineEdit_kpm, &QLineEdit::editingFinished, this, [this, track_line_edit](){track_line_edit(ui->lineEdit_kpm);});
    connect(ui->lineEdit_kim, &QLineEdit::editingFinished, this, [this, track_line_edit](){track_line_edit(ui->lineEdit_kim);});
    connect(ui->lineEdit_kpw, &QLineEdit::editingFinished, this, [this, track_line_edit](){track_line_edit(ui->lineEdit_kpw);});
    connect(ui->lineEdit_kid, &QLineEdit::editingFinished, this, [this, track_line_edit](){track_line_edit(ui->lineEdit_kid);});
    connect(ui->lineEdit_kpy, &QLineEdit::editingFinished, this, [this, track_line_edit](){track_line_edit(ui->lineEdit_kpy);});
    connect(ui->lineEdit_kiy, &QLineEdit::editingFinished, this, [this, track_line_edit](){track_line_edit(ui->lineEdit_kiy);});
    connect(ui->lineEdit_kdy, &QLineEdit::editingFinished, this, [this, track_line_edit](){track_line_edit(ui->lineEdit_kdy);});

    // Connect each button to the common lambda
    connect(ui->push_pidm, &QPushButton::clicked, this, process_button_click);
    connect(ui->push_pidw, &QPushButton::clicked, this, process_button_click);
    connect(ui->push_pidy, &QPushButton::clicked, this, process_button_click);

    // Labels on motor angular rate
    ui->widget_plot_motor_rate->xAxis->setLabel("Time [s]");
    ui->widget_plot_motor_rate->yAxis->setLabel("[°/s]");
    ui->widget_plot_motor_rate->xAxis->setLabelFont(QFont("Courier New", 12));
    ui->widget_plot_motor_rate->yAxis->setLabelFont(QFont("Courier New", 12));
    ui->widget_plot_motor_rate->xAxis->setLabelColor(Qt::blue);
    ui->widget_plot_motor_rate->yAxis->setLabelColor(Qt::blue);
    ui->widget_plot_motor_rate->addGraph(); // Graph 0 -> Set Point
    ui->widget_plot_motor_rate->addGraph(); // Graph 1 -> Feedback
    ui->widget_plot_motor_rate->graph(0)->setName("Set Point");
    ui->widget_plot_motor_rate->graph(1)->setName("Feedback");
    ui->widget_plot_motor_rate->legend->setVisible(true);
    ui->widget_plot_motor_rate->graph(0)->setPen(setPointPen);
    ui->widget_plot_motor_rate->graph(1)->setPen(feedbackPen);;
    ui->widget_plot_motor_rate->replot();

    // Labels on satellite angular rate
    ui->widget_plot_sat_rate->xAxis->setLabel("Time [s]");
    ui->widget_plot_sat_rate->yAxis->setLabel("[°/s]");
    ui->widget_plot_sat_rate->xAxis->setLabelFont(QFont("Courier New", 12));
    ui->widget_plot_sat_rate->yAxis->setLabelFont(QFont("Courier New", 12));
    ui->widget_plot_sat_rate->xAxis->setLabelColor(Qt::blue);
    ui->widget_plot_sat_rate->yAxis->setLabelColor(Qt::blue);
    ui->widget_plot_sat_rate->addGraph(); // Graph 0 -> Set Point
    ui->widget_plot_sat_rate->addGraph(); // Graph 1 -> Feedback
    ui->widget_plot_sat_rate->graph(0)->setName("Set Point");
    ui->widget_plot_sat_rate->graph(1)->setName("Feedback");
    ui->widget_plot_sat_rate->legend->setVisible(true);
    ui->widget_plot_sat_rate->graph(0)->setPen(setPointPen);
    ui->widget_plot_sat_rate->graph(1)->setPen(feedbackPen);
    ui->widget_plot_sat_rate->replot();

    // Labels on satellite yaw
    ui->widget_plot_yaw->xAxis->setLabel("Time [s]");
    ui->widget_plot_yaw->yAxis->setLabel("[°]");
    ui->widget_plot_yaw->xAxis->setLabelFont(QFont("Courier New", 12));
    ui->widget_plot_yaw->yAxis->setLabelFont(QFont("Courier New", 12));
    ui->widget_plot_yaw->xAxis->setLabelColor(Qt::blue);
    ui->widget_plot_yaw->yAxis->setLabelColor(Qt::blue);
    ui->widget_plot_yaw->addGraph(); // Graph 0 -> Set Point
    ui->widget_plot_yaw->graph(0)->setPen(QPen(Qt::red));
    ui->widget_plot_yaw->addGraph(); // Graph 1 -> Feedback
    ui->widget_plot_yaw->graph(0)->setName("Set Point");
    ui->widget_plot_yaw->graph(1)->setName("Feedback");
    ui->widget_plot_yaw->legend->setVisible(true);
    ui->widget_plot_yaw->graph(1)->setPen(QPen(Qt::green));
    ui->widget_plot_yaw->graph(0)->setPen(setPointPen);
    ui->widget_plot_yaw->graph(1)->setPen(feedbackPen);
    ui->widget_plot_yaw->replot();

    ui->comboBox_btbaud->addItem("115200", 115200);
    ui->comboBox_btbaud->addItem("9600", 9600);
    ui->comboBox_btbaud->addItem("19200", 19200);
    ui->comboBox_btbaud->addItem("38400", 38400);
    ui->comboBox_btbaud->addItem("57600", 57600);
    populate_serial_ports(ui->comboBox_btport);
}

void MainWindow::init_compass_gauge(void)
{
    mCompassGauge =  new QcGaugeWidget;
    mCompassGauge = ui->widget_compass;
    mCompassGauge->addBackground(99);

    QcBackgroundItem *bkg1 = mCompassGauge->addBackground(92);
    bkg1->clearrColors();
    bkg1->addColor(0.1,Qt::black);
    bkg1->addColor(1.0,Qt::white);

    QcBackgroundItem *bkg2 = mCompassGauge->addBackground(88);
    bkg2->clearrColors();
    bkg2->addColor(0.1,Qt::white);
    bkg2->addColor(1.0,Qt::black);

    QcLabelItem *w = mCompassGauge->addLabel(80);
    w->setText("W");
    w->setAngle(0);
    w->setColor(Qt::white);

    QcLabelItem *n = mCompassGauge->addLabel(80);
    n->setText("N");
    n->setAngle(90);
    n->setColor(Qt::white);

    QcLabelItem *e = mCompassGauge->addLabel(80);
    e->setText("E");
    e->setAngle(180);
    e->setColor(Qt::white);

    QcLabelItem *s = mCompassGauge->addLabel(80);
    s->setText("S");
    s->setAngle(270);
    s->setColor(Qt::white);

    QcDegreesItem *deg = mCompassGauge->addDegrees(70);
    deg->setStep(5);
    deg->setMaxDegree(270);
    deg->setMinDegree(-75);
    deg->setColor(Qt::white);

    mCompassNeedle = mCompassGauge->addNeedle(60);
    mCompassNeedle->setNeedle(QcNeedleItem::CompassNeedle);
    mCompassNeedle->setValueRange(0,360);
    mCompassNeedle->setMaxDegree(360);
    mCompassNeedle->setMinDegree(0);
    mCompassGauge->addBackground(7);
    mCompassGauge->addGlass(88);
}

void MainWindow::init_angular_rate_gauge(void)
{
    mSpeedGauge = new QcGaugeWidget;
    mSpeedGauge = ui->widget_angular_rate;

    mSpeedGauge->addBackground(99);
    QcBackgroundItem *bkg1 = mSpeedGauge->addBackground(92);
    bkg1->clearrColors();
    bkg1->addColor(0.1,Qt::black);
    bkg1->addColor(1.0,Qt::white);

    QcBackgroundItem *bkg2 = mSpeedGauge->addBackground(88);
    bkg2->clearrColors();
    bkg2->addColor(0.1,Qt::gray);
    bkg2->addColor(1.0,Qt::darkGray);

    mSpeedGauge->addArc(55);
    mSpeedGauge->addDegrees(65)->setValueRange(0,80);
    mSpeedGauge->addColorBand(50);

    mSpeedGauge->addValues(80)->setValueRange(0,80);

    mSpeedGauge->addLabel(70)->setText("Km/h");
    QcLabelItem *lab = mSpeedGauge->addLabel(40);
    lab->setText("0");
    mSpeedNeedle = mSpeedGauge->addNeedle(60);
    mSpeedNeedle->setLabel(lab);
    mSpeedNeedle->setColor(Qt::white);
    mSpeedNeedle->setValueRange(0,80);
    mSpeedGauge->addBackground(7);
    mSpeedGauge->addGlass(88);
}

void MainWindow::init_inclinometer_gauge(void)
{
    mAttitudeGauge = new QcGaugeWidget;
    mAttitudeGauge = ui->widget_inclinometer;
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

// Accidental attempt to open some port hangs the software.
// How to detect and not populate such ports?
void MainWindow::populate_serial_ports(QComboBox *cb)
{
    cb->clear();

    if(serial.isOpen())
    {
        serial.close();
    }

    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    std::reverse(ports.begin(), ports.end());

    foreach (const QSerialPortInfo &port, ports)
    {
        cb->addItem(port.portName());
    }

    if (cb->count() == 0)
    {
        cb->addItem("No Ports");
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pusb_btconn_clicked()
{
    // Close if open and vice versa
    if (!serial.isOpen())
    {
        serial.setPortName(ui->comboBox_btport->currentText());
        serial.setBaudRate(ui->comboBox_btbaud->currentData().toInt());
        serial.setDataBits(QSerialPort::Data8);
        serial.setParity(QSerialPort::NoParity);
        serial.setStopBits(QSerialPort::OneStop);
        serial.setFlowControl(QSerialPort::NoFlowControl);

        if (serial.open(QIODevice::ReadWrite))
        {
            connect(&serial, &QSerialPort::readyRead, this, &MainWindow::handleReadyRead);
            QPixmap pixmap_btconn(":/icons/assets/bt_con.png");
            ui->label_bt->setPixmap(pixmap_btconn);
            ui->label_bt_3->setPixmap(pixmap_btconn);
            ui->label_bt_4->setPixmap(pixmap_btconn);
            ui->label_bt_5->setPixmap(pixmap_btconn);
            ui->label_bt_6->setPixmap(pixmap_btconn);
            ui->pusb_btconn->setText("DISCONNECT");
        }
        else // Failed attempt to open serial
        {
            QPixmap pixmap_btdis(":/icons/assets/bt_dis.png");
            ui->label_bt->setPixmap(pixmap_btdis);
            ui->label_bt_3->setPixmap(pixmap_btdis);
            ui->label_bt_4->setPixmap(pixmap_btdis);
            ui->label_bt_5->setPixmap(pixmap_btdis);
            ui->label_bt_6->setPixmap(pixmap_btdis);

            QPixmap pixmap_batt(":/icons/assets/batt_what.png");
            ui->label_batt->setPixmap(pixmap_batt);
            ui->label_batt_3->setPixmap(pixmap_batt);
            ui->label_batt_4->setPixmap(pixmap_batt);
            ui->label_batt_5->setPixmap(pixmap_batt);
            ui->label_batt_6->setPixmap(pixmap_batt);
            ui->pusb_btconn->setText("CONNECT");
        }
    }
    else
    {
        serial.close();

        QPixmap pixmap_btdis(":/icons/assets/bt_dis.png");
        ui->label_bt->setPixmap(pixmap_btdis);
        ui->label_bt_3->setPixmap(pixmap_btdis);
        ui->label_bt_4->setPixmap(pixmap_btdis);
        ui->label_bt_5->setPixmap(pixmap_btdis);
        ui->label_bt_6->setPixmap(pixmap_btdis);

        QPixmap pixmap_batt(":/icons/assets/batt_what.png");
        ui->label_batt->setPixmap(pixmap_batt);
        ui->label_batt_3->setPixmap(pixmap_batt);
        ui->label_batt_4->setPixmap(pixmap_batt);
        ui->label_batt_5->setPixmap(pixmap_batt);
        ui->label_batt_6->setPixmap(pixmap_batt);
        ui->pusb_btconn->setText("CONNECT");
    }
}

void MainWindow::transmit_telecommand(const tcommand_t cmd, const double value)
{
    // Convert the cmd to a string
    std::ostringstream cmd_stream;
    cmd_stream << static_cast<int>(cmd);
    std::string cmd_str = cmd_stream.str();

    // Construct telecommand
    std::string packet;
    packet += TCMD_START_CHAR;
    packet += cmd_str;
    packet += TCMD_DELIMITER;
    packet += std::to_string(value);
    packet += TCMD_STOP_CHAR;
    packet += std::string("\0");

    // Transmit telecommand without null character
    qDebug() << packet << ", " << packet.length();
    serial.write(packet.c_str(), packet.length());
    serial.flush();
}

void MainWindow::on_push_btrefresh_clicked()
{
    populate_serial_ports(ui->comboBox_btport);
}

void MainWindow::on_hslider_motor_sliderMoved(int position)
{
    ui->lineEdit_motor->setText(QString::number(position));
}

void MainWindow::on_hslider_satrate_sliderMoved(int position)
{
    ui->lineEdit_satrate->setText(QString::number(position));
}

void MainWindow::on_hslider_satyaw_sliderMoved(int position)
{
    ui->lineEdit_satyaw->setText(QString::number(position));
}

void MainWindow::populate_telemetry(const telemetry_t &t)
{
    // AHRS instruments
    mCompassNeedle->setCurrentValue(t.e[0]);
    mAttMeter->setCurrentPitch(t.e[1]);
    mAttMeter->setCurrentRoll(t.e[2]);
    mAttitudeNeedle->setCurrentValue(90-t.e[2]);
    mSpeedNeedle->setCurrentValue(t.g[2]);

    // AHRS IMU/compass measurements
    ui->label_ax->setText(QString::number(t.a[0]));
    ui->label_ay->setText(QString::number(t.a[1]));
    ui->label_az->setText(QString::number(t.a[2]));
    ui->label_gx->setText(QString::number(t.g[0]));
    ui->label_gy->setText(QString::number(t.g[1]));
    ui->label_gz->setText(QString::number(t.g[2]));
    ui->label_mx->setText(QString::number(t.m[0]));
    ui->label_my->setText(QString::number(t.m[1]));
    ui->label_mz->setText(QString::number(t.m[2]));

    // AHRS Euler angles and angular rate
    ui->label_yaw->setText(QString::number(t.e[0]));
    ui->label_pitch->setText(QString::number(t.e[1]));
    ui->label_roll->setText(QString::number(t.e[2]));
    ui->label_satrate->setText(QString::number(t.g[2]));

    // Set points and feedback (process variable) plots
    static QVector<double> time, msp, mpv, wsp, wpv, ysp, ypv;
    static int count;

    msp.append(spm);
    wsp.append(spw);
    ysp.append(spy);
    mpv.append((double)t.w);
    wpv.append((double)t.g[2]);
    ypv.append((double)t.e[0]);
    time.append((double)count++);

    if (time.size() > 250)
    {
        time.removeFirst();
        msp.removeFirst();
        mpv.removeFirst();
        wsp.removeFirst();
        wpv.removeFirst();
        ysp.removeFirst();
        ypv.removeFirst();
    }

    // Plot motor angular rate
    ui->widget_plot_motor_rate->graph(0)->setData(time, msp);
    ui->widget_plot_motor_rate->graph(1)->setData(time, mpv);
    ui->widget_plot_motor_rate->xAxis->setRange(time.first(), time.last());

    QCustomPlot *plot = ui->widget_plot_motor_rate;
    QCPGraph *graph1 = plot->graph(0);
    QCPGraph *graph2 = plot->graph(1);
    graph1->rescaleValueAxis(true);
    graph2->rescaleValueAxis(true);

    ui->widget_plot_motor_rate->replot();

    // Plot satellite angular rate
    ui->widget_plot_sat_rate->graph(0)->setData(time, wsp);
    ui->widget_plot_sat_rate->graph(1)->setData(time, wpv);
    ui->widget_plot_sat_rate->xAxis->setRange(time.first(), time.last());

    plot = ui->widget_plot_sat_rate;
    graph1 = plot->graph(0);
    graph2 = plot->graph(1);
    graph1->rescaleValueAxis(true);
    graph2->rescaleValueAxis(true);

    ui->widget_plot_sat_rate->replot();

    // Plot satellite yaw
    ui->widget_plot_yaw->graph(0)->setData(time, ysp);
    ui->widget_plot_yaw->graph(1)->setData(time, ypv);
    ui->widget_plot_yaw->xAxis->setRange(time.first(), time.last());

    plot = ui->widget_plot_yaw;
    graph1 = plot->graph(0);
    graph2 = plot->graph(1);
    graph1->rescaleValueAxis(true);
    graph2->rescaleValueAxis(true);

    ui->widget_plot_yaw->replot();

    // Set battery label
    QPixmap pixmap_batt(":/icons/assets/batt_full.png");
    ui->label_batt->setPixmap(pixmap_batt);
    ui->label_batt_3->setPixmap(pixmap_batt);
    ui->label_batt_4->setPixmap(pixmap_batt);
    ui->label_batt_5->setPixmap(pixmap_batt);
    ui->label_batt_6->setPixmap(pixmap_batt);
}

bool parse_telemetry(const QString &rx, telemetry_t &t);
void print_telemetry(const telemetry_t &t);

void MainWindow::handleReadyRead()
{
    rxbt += serial.readAll();

    // Still more to append
    if(!QString(rxbt).contains("\r\n"))
    {
      return;
    }

    QString rxstr = QString(rxbt);
    telemetry_t t = {.a = {0.0}, .m = {0.0}, .g = {0.0}, .e = {0.0}, .b = 0.0, .w = 0.0};

    if(parse_telemetry(rxstr, t))
    {
        populate_telemetry(t);
        // print_telemetry(t);
    }

    // Clear for next frame
    rxbt.clear();
}

void print_telemetry(const telemetry_t &t)
{
    qDebug() << "Telemetry:";
    qDebug() << "  Accelerometer      :" << t.a[0] << "," << t.a[1] << "," << t.a[2];
    qDebug() << "  Magnetometer       :" << t.m[0] << "," << t.m[1] << "," << t.m[2];
    qDebug() << "  Gyroscope          :" << t.g[0] << "," << t.g[1] << "," << t.g[2];
    qDebug() << "  Euler Angles       :" << t.e[0] << "," << t.e[1] << "," << t.e[2];
    qDebug() << "  Battery Voltage    :" << t.b;
    qDebug() << "  Motor Angular Rate :" << t.w;
}

bool parse_telemetry(const QString &rx, telemetry_t &t)
{
    // Split the input string by commas
    QStringList components = rx.split(',');

    // Iterate over each comma separated components
    for (const QString &component : components)
    {
        // Split by colon to separate type and values
        QStringList parts = component.split(':');
        if (parts.size() != 2)
        {
            qWarning() << "Invalid component:" << component;
            // continue;
            return false;
        }

        QString type = parts[0];
        QStringList values = parts[1].split('x');

        if (type == "a") // Accelerometer
        {
            for (int i = 0; i < 3; ++i)
            {
                t.a[i] = values[i].toFloat();
            }
        }
        else if (type == "m") // Magnetometer
        {
            for (int i = 0; i < 3; ++i) {
                t.m[i] = values[i].toFloat();
            }
        }
        else if (type == "g") // Gyroscope
        {
            for (int i = 0; i < 3; ++i)
            {
                t.g[i] = values[i].toFloat();
            }
        }
        else if (type == "e") // Euler angles
        {
            for (int i = 0; i < 3; ++i)
            {
                t.e[i] = values[i].toFloat();
            }
        }
        else if (type == "b") // Battery voltage
        {
            t.b = values[0].toFloat();
        }
        else if (type == "w") // Motor angular rate
        {
            t.w = values[0].toFloat();
        }
        else
        {
            qWarning() << "Unknown data type:" << type;
            return false;
        }
    }

    return true;
}

void MainWindow::on_push_set_spm_clicked()
{
    spm =  ui->lineEdit_motor->text().toDouble(); // Read value
    ui->hslider_motor->setValue((int)spm);        // Update slider
    transmit_telecommand(TCMD_SP_MW, spm);        // Transmit value
}

void MainWindow::on_push_set_spy_clicked()
{
    spy =  ui->lineEdit_satyaw->text().toDouble(); // Read value
    ui->hslider_satyaw->setValue((int)spy);        // Update slider
    transmit_telecommand(TCMD_SP_SY, spy);         // Transmit value
}

void MainWindow::on_push_set_spw_clicked()
{
    spw =  ui->lineEdit_satrate->text().toDouble(); // Read value
    ui->hslider_satrate->setValue((int)spw);        // Update slider
    transmit_telecommand(TCMD_SP_SW, spw);          // Transmit value
}

void MainWindow::on_lineEdit_kpm_editingFinished()
{
}

void MainWindow::on_lineEdit_kim_editingFinished()
{
}

void MainWindow::on_pushButton_mw_clicked()
{
    static bool flag = true;

    if(flag)
    {
      transmit_telecommand(TCMD_CTRL_MODE, (double)CTRL_MODE_MOTOR_OMEGA);
    }
    else
    {
        transmit_telecommand(TCMD_CTRL_MODE, (double)CTRL_MODE_IDLE);
    }

    flag = !flag;
}

