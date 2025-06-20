greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT += core gui opengl printsupport designer serialport

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    qcgaugewidget.cpp \
    mainwindow.cpp \
    qcustomplot.cpp

HEADERS += \
    qcgaugewidget.h \
    mainwindow.h \
    qcustomplot.h \
    satconfig.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    fsat-gs_en_GB.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
