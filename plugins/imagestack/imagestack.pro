TEMPLATE = lib
CONFIG += plugin


TARGET = image_stack
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./

include(../plugins.pri)

DESTDIR = $$QFOUTPUT/plugins

include(../../libquickfitwidgets.pri)

# Input
HEADERS += qfrdrimagestack.h \
           qfrdrimagestack_data.h \
		   qfrdrimagestack_dataeditor.h \
    ../interfaces/qfrdrimagestackinterface.h \
    ../interfaces/qfrdrimagemask.h \
    ../base_classes/qfrdrimagemasktools.h \
    ../base_classes/qfrdrimagemaskedittools.h

SOURCES += qfrdrimagestack.cpp \
           qfrdrimagestack_data.cpp \
		   qfrdrimagestack_dataeditor.cpp \
    ../base_classes/qfrdrimagemasktools.cpp \
    ../base_classes/qfrdrimagemaskedittools.cpp

FORMS = 

RESOURCES += qfrdrimagestack.qrc \
    ../base_classes/qfrdrmaskeditor.qrc

TRANSLATIONS= ./translations/de.image_stack.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

ASSETS_TESTDATA.files = ./assets/testdata/*.*
ASSETS_TESTDATA.path = $${QFOUTPUT}/assets/plugins/$${TARGET}/testdata/
INSTALLS += ASSETS_TESTDATA

QT += gui xml core
CONFIG += exceptions rtti stl

OTHER_FILES += \
    ../base_classes/mask*



