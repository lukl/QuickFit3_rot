#
#    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg
#	
#	 last modification: $LastChangedDate$  (revision $Rev$)
#
#    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).
#
#    This software is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

TEMPLATE = lib
CONFIG += plugin

TARGET = imaging_fcs
DEPENDPATH += ./

include(../plugins.pri)
include(../../extlibs/zlib.pri)
include(../../extlibs/tiff.pri)
include(../../extlibs/lmfit.pri)

DESTDIR = $$QFOUTPUT/plugins

include(../../libquickfitwidgets.pri)

# Input
HEADERS += qfrdrimagingfcs.h \
           qfrdrimagingfcs_data.h \
		   qfrdrimagingfcs_dataeditor.h \
           ../interfaces/qfrdrfcsdatainterface.h \
    ../interfaces/qfrdroverviewimageinterface.h \
    ../base_classes/qfrdrimagingfcstools.h \
    ../base_classes/qfrdrimagemasktools.h \
    ../base_classes/qfcorrelationmasktools.h \
    ../base_classes/qffinderroneouscorrelationdlg.h \
    ../base_classes/qffcstools.h \
    ../interfaces/qfrdrimagestackinterface.h \
    ../interfaces/qfrdrsimplecountrateinterface.h \
    ../interfaces/qfrdrimageselectioninterface.h \
    ../base_classes/qfrdroverviewimagedisplay.h \
    ../interfaces/qfrdrimagemask.h \
    ../interfaces/qfrdrimfcscorrelatorremote.h \
    qfrdrimagingfcs_dataeditor_image.h \
    qfrdrimagingfcsrunsmodel.h \
    qfrdrimagingfcscorrelationdialog.h \
    qfrdrimagingfcsthreadprogress.h \
    qfrdrimagingfcscorrelationjobthread.h \
    ../../../../../LIB/trunk/tinytiffwriter.h \
    ../../../../../LIB/trunk/libtiff_tools.h \
    ../../../../../LIB/trunk/shiftregister.h \
    ../../../../../LIB/trunk/multitau-correlator.h \
    ../../../../../LIB/trunk/correlator_multitau.h \
    qfrdrimagingfcsdataexplorer.h \
    qfrdrimagingfcsmaskbyintensity.h \
    qfrdrimagingfcscountratedisplay.h \
    qfrdrimagingfcsoverviewrateeditor.h \
    ../../../../../LIB/trunk/yaid_rh.h \
    qfrdrimagingfcsseriesdialog.h \
    qfrdrimagingfcs_dataeditor_countrate.h \
    qfrdrimagingfcsedittools.h \
    qfrdrimagingfcssimulator.h \
    qfrdrimagingfcssimulationthread.h \
    ../../extlibs/MersenneTwister.h \
    ../base_classes/qfrdrrunselectiontools.h \
    ../base_classes/frawdataimageeditor.h \
    ../base_classes/qfrdrimagecopydataselectdialog.h \
    ../base_classes/qfoverlaystylecombobox.h \
    ../base_classes/qfimageplotter.h \
    ../base_classes/qfimageparametergroupbox.h \
    qfrdrimagingfcsmaskbuilder.h \
    qfrdrimagingfcscopycorrasrdrdialog.h \
    qfrdrimagingfcspostprocessbinningdialog.h \
    ../../../../../LIB/trunk/textcolor.h \
    ../../../../../LIB/trunk/crc_ccitt.h \
    ../../../../../LIB/trunk/crc.h \
    ../../../../../LIB/trunk/correlator_multitau.h



SOURCES += qfrdrimagingfcs.cpp \
           qfrdrimagingfcs_data.cpp \
		   qfrdrimagingfcs_dataeditor.cpp \
    qfrdrimagingfcs_dataeditor_image.cpp \
    qfrdrimagingfcsrunsmodel.cpp \
    qfrdrimagingfcscorrelationdialog.cpp \
    qfrdrimagingfcsthreadprogress.cpp \
    qfrdrimagingfcscorrelationjobthread.cpp \
    ../../../../../LIB/trunk/tinytiffwriter.cpp \
    ../../../../../LIB/trunk/libtiff_tools.cpp \
    qfrdrimagingfcsdataexplorer.cpp \
    qfrdrimagingfcsmaskbyintensity.cpp \
    qfrdrimagingfcscountratedisplay.cpp \
    qfrdrimagingfcsoverviewrateeditor.cpp \
    ../base_classes/qfrdrimagingfcstools.cpp \
    qfrdrimagingfcsseriesdialog.cpp \
    ../base_classes/qfrdroverviewimagedisplay.cpp \
    qfrdrimagingfcs_dataeditor_countrate.cpp \
    ../base_classes/qfrdrimagemasktools.cpp\
    ../base_classes/qfcorrelationmasktools.cpp \
    ../base_classes/qffinderroneouscorrelationdlg.cpp \
    qfrdrimagingfcsedittools.cpp \
    qfrdrimagingfcssimulator.cpp \
    qfrdrimagingfcssimulationthread.cpp \
    ../base_classes/qffcstools.cpp \
    ../base_classes/qfrdrrunselectiontools.cpp \
    ../base_classes/frawdataimageeditor.cpp \
    ../base_classes/qfrdrimagecopydataselectdialog.cpp \
    ../base_classes/qfoverlaystylecombobox.cpp \
    ../base_classes/qfimageplotter.cpp \
    ../base_classes/qfimageparametergroupbox.cpp \
    qfrdrimagingfcsmaskbuilder.cpp \
    qfrdrimagingfcscopycorrasrdrdialog.cpp \
    qfrdrimagingfcspostprocessbinningdialog.cpp

FORMS = \
    qfrdrimagingfcscorrelationdialog.ui \
    qfrdrimagingfcsthreadprogress.ui \
    qfrdrimagingfcsdataexplorer.ui \
    qfrdrimagingfcsmaskbyintensity.ui \
    qfrdrimagingfcscountratedisplay.ui \
    qfrdrimagingfcsseriesdialog.ui \
    ../base_classes/qffinderroneouscorrelationdlg.ui \
    qfrdrimagingfcssimulator.ui \
    ../base_classes/qfrdrimagecopydataselectdialog.ui \
    qfrdrimagingfcsmaskbuilder.ui \
    qfrdrimagingfcscopycorrasrdrdialog.ui \
    qfrdrimagingfcspostprocessbinningdialog.ui

RESOURCES += qfrdrimagingfcs.qrc

TRANSLATIONS= ./translations/de.imaging_fcs.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg opengl
CONFIG += exceptions rtti stl

#DEFINES += DEBUG_TIMIMNG JKQTBP_DEBUGTIMING


ASSETS_TESTDATA.files = ./examples/imfcs_testdata/*.*
ASSETS_TESTDATA.path = $${QFOUTPUT}/examples/$${TARGET}/imfcs_testdata/
ASSETS_TESTDATARESULTS.files = ./examples/imfcs_testdata/results/*.*
ASSETS_TESTDATARESULTS.path = $${QFOUTPUT}/examples/$${TARGET}/imfcs_testdata/results/

INSTALLS += ASSETS_TESTDATA ASSETS_TESTDATARESULTS
