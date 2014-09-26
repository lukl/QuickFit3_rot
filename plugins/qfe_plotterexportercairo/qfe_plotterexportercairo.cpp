/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "qfe_plotterexportercairo.h"
#include <QtGui>
#include <QtPlugin>
#include <iostream>
#include "jkqtcairoengineadapter.h"

#include "jkqtpbaseplotter.h"

#define LOG_PREFIX QString("qfe_plotterexportercairo >>> ").toUpper()

QFEPlotterExporterCairo::QFEPlotterExporterCairo(QObject* parent):
    QObject(parent)
{
	logService=NULL;
}

QFEPlotterExporterCairo::~QFEPlotterExporterCairo() {

}


void QFEPlotterExporterCairo::deinit() {
	/* add code for cleanup here */
}

void QFEPlotterExporterCairo::projectChanged(QFProject* oldProject, QFProject* project) {
	/* usually cameras do not have to react to a change of the project in QuickFit .. so you don't need to do anything here
	   But: possibly you could read config information from the project here
	 */
}

void QFEPlotterExporterCairo::initExtension() {
    /* do initializations here but do not yet connect to the camera! */
    
    JKQtBasePlotter::registerPaintDeviceAdapter(new JKQTPCairoEngineAdapter(JKQTPCairoEngineAdapter::formatPDF, true));
    JKQtBasePlotter::registerPaintDeviceAdapter(new JKQTPCairoEngineAdapter(JKQTPCairoEngineAdapter::formatPDF, false));
    JKQtBasePlotter::registerPaintDeviceAdapter(new JKQTPCairoEngineAdapter(JKQTPCairoEngineAdapter::formatPS, true));
    JKQtBasePlotter::registerPaintDeviceAdapter(new JKQTPCairoEngineAdapter(JKQTPCairoEngineAdapter::formatPS, false));
    JKQtBasePlotter::registerPaintDeviceAdapter(new JKQTPCairoEngineAdapter(JKQTPCairoEngineAdapter::formatEPS, true));
    JKQtBasePlotter::registerPaintDeviceAdapter(new JKQTPCairoEngineAdapter(JKQTPCairoEngineAdapter::formatEPS, false));
    JKQtBasePlotter::registerPaintDeviceAdapter(new JKQTPCairoEngineAdapter(JKQTPCairoEngineAdapter::formatSVG, true));
    JKQtBasePlotter::registerPaintDeviceAdapter(new JKQTPCairoEngineAdapter(JKQTPCairoEngineAdapter::formatSVG, false));

}


void QFEPlotterExporterCairo::loadSettings(ProgramOptions* settingspo) {
	/* here you could read config information from the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini
	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFEPlotterExporterCairo::storeSettings(ProgramOptions* settingspo) {
	/* here you could write config information to the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFEPlotterExporterCairo::log_text(QString message) {
	if (logService) logService->log_text(LOG_PREFIX+message);
	else if (services) services->log_text(LOG_PREFIX+message);
}

void QFEPlotterExporterCairo::log_warning(QString message) {
	if (logService) logService->log_warning(LOG_PREFIX+message);
	else if (services) services->log_warning(LOG_PREFIX+message);
}

void QFEPlotterExporterCairo::log_error(QString message) {
	if (logService) logService->log_error(LOG_PREFIX+message);
	else if (services) services->log_error(LOG_PREFIX+message);
}


Q_EXPORT_PLUGIN2(qfe_plotterexporterCairo, QFEPlotterExporterCairo)
