TEMPLATE = lib
CONFIG += plugin

TARGET = calc_diffcoeff
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./


include(../plugins.pri)

include(../../libquickfitwidgets.pri)

DESTDIR = $${QFOUTPUT}/plugins/

# Input
HEADERS += qfediffusioncoefficientcalculator.h \
    dlgcalcdiffcoeff.h \
    ../base_classes/qftablemodel.h \
    ../interfaces/qfextensiontool.h

SOURCES += qfediffusioncoefficientcalculator.cpp \
    dlgcalcdiffcoeff.cpp \
    ../base_classes/qftablemodel.cpp

FORMS = \
    dlgcalcdiffcoeff.ui

RESOURCES += qfediffusioncoefficientcalculator.qrc

TRANSLATIONS= ./translations/de.qfe_calc_diffcoeff.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg
CONFIG += exceptions rtti stl


