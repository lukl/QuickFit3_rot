TEMPLATE = lib
CONFIG += plugin

TARGET = fcs_maxent
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./

include(../plugins.pri)

DESTDIR = $$QFOUTPUT/plugins

include(../../libquickfitwidgets.pri)
include(../../extlibs/eigen.pri)

# Input
HEADERS += qffcsmaxentevaluation.h \
           qffcsmaxentevaluation_item.h \
           qffcsmaxentevaluation_editor.h \
    ../base_classes/qfusesresultsevaluation.h \
    ../base_classes/qfusesresultsbyindexevaluation.h \
    ../interfaces/qfrdrfcsdatainterface.h \
    ../base_classes/qfusesresultsbyindexandmodelevaluation.h

SOURCES += qffcsmaxentevaluation.cpp \
           qffcsmaxentevaluation_item.cpp \
           qffcsmaxentevaluation_editor.cpp \
    ../base_classes/qfusesresultsevaluation.cpp \
    ../base_classes/qfusesresultsbyindexevaluation.cpp \
    ../base_classes/qfusesresultsbyindexandmodelevaluation.cpp

FORMS =   

RESOURCES += qffcsmaxentevaluation.qrc

TRANSLATIONS= ./translations/de.fcs_maxent.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml core
CONFIG += exceptions rtti stl


ASSETSTESTDATA_FILES.files = ./assets/testdata/*.*
ASSETSTESTDATA_FILES.path = $${QFOUTPUT}/assets/plugins/$${TARGET}/testdata/


INSTALLS += ASSETSTESTDATA_FILES


