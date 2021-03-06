/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "programoptions.h"
#include <iostream>
#include <QtCore>
#include <QDir>
#include <QNetworkProxy>
#include "qftools.h"
#include <QtGlobal>
#ifndef __WINDOWS__
# if defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32)
#  define __WINDOWS__
# endif
#endif

#ifndef __LINUX__
# if defined(linux)
#  define __LINUX__
# endif
#endif

#ifndef __WINDOWS__
# ifndef __LINUX__
#  warning("these methods are ment to be used under windows or linux ... no other system were tested")
# endif
#endif

ProgramOptions* ProgramOptions::inst=NULL;

ProgramOptions::ProgramOptions( QString ini, QObject * parent, QApplication* app  ):
    QObject(parent)
{

    userSaveAfterFirstEdit=true;
    childWindowsStayOnTop=true;
    helpWindowsStayOnTop=true;
    projectWindowStayOnTop=false;
    m_debugLogVisible=false;

    proxyHost=QNetworkProxy::applicationProxy().hostName();
    proxyType=QNetworkProxy::applicationProxy().type();
    proxyPort=QNetworkProxy::applicationProxy().port();

    QFileInfo fi(app->applicationFilePath());
    appDir=app->applicationDirPath();
    #if defined(Q_OS_MAC)
        // get out of /MyApp.app/Contents/MacOS in MacOSX App-Bundle
        QDir aappDir(appDir);
        QDir aapppDir(appDir);
        if (aappDir.dirName() == "MacOS") {
            aappDir.cdUp();
            //aappDir.cdUp();
            //aappDir.cdUp();
            aappDir.cd("SharedSupport");
            aapppDir.cdUp();
            aapppDir.cdUp();
            aapppDir.cdUp();
        }
        appDir=	aappDir.absolutePath();
    #endif
    globalConfigDir=appDir+"/globalconfig/";
    configDir=QDir::homePath()+"/."+fi.completeBaseName()+"/";
    assetsDir=appDir+"/assets/";
    examplesDir=appDir+"/examples/";
    pluginsDir=appDir+"/plugins/";

    #ifdef Q_OS_MAC
    //assetsDir=appDir+"/quickfit3.app/Contents/SharedSupport/assets/";
    //examplesDir=appDir+"/quickfit3.app/Contents/SharedSupport/examples/";
    pluginsDir=appDir+"/../PlugIns/";
    QString altPluginDir=aapppDir.absolutePath()+"/plugins/";
    if (!QDir(pluginsDir).exists() && QDir(altPluginDir).exists()) pluginsDir=altPluginDir;
    #endif

    #if defined(__LINUX__) || defined(Q_OS_MAC)
    globalConfigDir=QString("%1/quickfit3/").arg(GLOBALCONFIGDIR);
    #endif




    QDir d(appDir);
    d.mkpath(configDir);

    this->app=app;
    iniFilename=ini;
    if (iniFilename.isEmpty()) {
        iniFilename= configDir+"/"+fi.completeBaseName()+".ini";
    }
    currentRawDataDir=fi.absolutePath();
    //std::cout<<"config file is: "<<iniFilename.toStdString()<<std::endl;
    settings=NULL;

    // default values
    style= app->style()->metaObject()->className();
#if defined(Q_OS_WIN)
    style="Fusion";
#elif defined(Q_OS_MAC)
    style="Macintosh";
#elif defined(Q_OS_LINUX)
    style="Cleanlooks";
#else
    style="Fusion";
#endif
    stylesheet="default";
    languageID="en";
    maxThreads=qMax(4,QThread::idealThreadCount());
    autosave=5;

    readSettings();
    d.mkpath(globalConfigDir);
    if (inst==NULL) inst=this;
}

ProgramOptions::~ProgramOptions()
{
    writeSettings();
    if (inst==this) inst=NULL;
}

void ProgramOptions::setCurrentRawDataDir(QString d, bool write)
{
    currentRawDataDir=d;
    if (write) writeSettings();
}


void ProgramOptions::writeSettings() {
    settings->setValue("quickfit/language", languageID);
    settings->setValue("quickfit/stylesheet", stylesheet);
    settings->setValue("quickfit/style", style);
    settings->setValue("quickfit/max_threads", maxThreads);
    settings->setValue("quickfit/autosave", autosave);
    settings->setValue("quickfit/current_raw_data_dir", currentRawDataDir);
    settings->setValue("quickfit/userSaveAfterFirstEdit", userSaveAfterFirstEdit);
    settings->setValue("quickfit/childWindowsStayOnTop", childWindowsStayOnTop);
    settings->setValue("quickfit/helpWindowsStayOnTop", helpWindowsStayOnTop);
    settings->setValue("quickfit/projectWindowStayOnTop", projectWindowStayOnTop);
    settings->setValue("quickfit/debugLogVisible", m_debugLogVisible);
    settings->setValue("quickfit/proxy/host", proxyHost);
    settings->setValue("quickfit/proxy/port", proxyPort);
    settings->setValue("quickfit/proxy/type", proxyType);
    settings->setValue("quickfit/global_config_dir", globalConfigDir);
}


void ProgramOptions::readSettings() {
    if (!settings) {
        //qDebug()<<"iniFilename="<<iniFilename;
        if (iniFilename=="native") { // on windows: registry, on Linux/MacOS: default
            settings= new QSettings(this);
        } else if (iniFilename=="native_inifile") { // ensures a INI file at the default location, even on windows
            settings= new QSettings(QSettings::IniFormat, QSettings::UserScope, app->organizationName(), app->applicationName(), this);
        } else {
            settings= new QSettings(iniFilename, QSettings::IniFormat, this);
        }
    }

    if (!settings->contains("quickfit/native_file_dialog")) {
    #ifdef Q_OS_UNIX
        settings->setValue("quickfit/native_file_dialog", false);
    #else
        settings->setValue("quickfit/native_file_dialog", true);
    #endif

    #ifdef Q_OS_MAC
        settings->setValue("quickfit/native_file_dialog", true);
    #endif
    }




    maxThreads=settings->value("quickfit/max_threads", maxThreads).toInt();
    autosave=settings->value("quickfit/autosave", autosave).toInt();
    currentRawDataDir=settings->value("quickfit/current_raw_data_dir", currentRawDataDir).toString();
    userSaveAfterFirstEdit=settings->value("quickfit/userSaveAfterFirstEdit", userSaveAfterFirstEdit).toBool();
    childWindowsStayOnTop=settings->value("quickfit/childWindowsStayOnTop", childWindowsStayOnTop).toBool();
    helpWindowsStayOnTop=settings->value("quickfit/helpWindowsStayOnTop", helpWindowsStayOnTop).toBool();
    projectWindowStayOnTop=settings->value("quickfit/projectWindowStayOnTop", projectWindowStayOnTop).toBool();
    m_debugLogVisible=settings->value("quickfit/debugLogVisible", m_debugLogVisible).toBool();
    globalConfigDir=settings->value("quickfit/global_config_dir", globalConfigDir).toString();

    proxyHost=(settings->value("quickfit/proxy/host", proxyHost).toString());
    proxyPort=(settings->value("quickfit/proxy/port", proxyPort).toUInt());
    proxyType=((QNetworkProxy::ProxyType)settings->value("quickfit/proxy/type", proxyType).toInt());


    languageID=settings->value("quickfit/language", languageID).toString();
    if (languageID != "en") { // english is default
        QDir d(assetsDir+"/translations");
        QStringList filters;
        filters << "*.qm";
        QStringList sl=qfDirListFilesRecursive(d, filters);//d.entryList(filters, QDir::Files);
        for (int i=0; i<sl.size(); i++) {
            QString s=sl[i];
            if (s.startsWith(languageID+".")) {
                QString fn=d.absoluteFilePath(s);
                //std::cout<<"loading translation '"<<fn.toStdString()<<"' ... \n";
                QTranslator* translator=new QTranslator(this);
                if (app && translator->load(fn)) {
                    app->installTranslator(translator);
                    //std::cout<<"OK\n";
                } else {
                    //std::cout<<"ERROR\n";
                }
            }
        }
        emit languageChanged(languageID);

    }
    style=settings->value("quickfit/style", style).toString();
    if (app) {
        app->setStyle(style);
    }
    emit styleChanged(style);
    stylesheet=settings->value("quickfit/stylesheet", stylesheet).toString();
    if (app) {
        QString fn=QString(assetsDir+"/stylesheets/%1.qss").arg(stylesheet);
        //std::cout<<"loading stylesheet '"<<fn.toStdString()<<"' ... ";
        QFile f(fn);
        if (f.open(QFile::ReadOnly)) {
            QTextStream s(&f);
            QString qss=s.readAll();
            //std::cout<<qss.toStdString()<<std::endl;
            app->setStyleSheet(qss);
        } else {
            app->setStyleSheet("");
        }
        //std::cout<<"OK\n";
    }
    emit stylesheetChanged(stylesheet);

}

QString ProgramOptions::getConfigFileDirectory() const {
    return configDir;
}

QString ProgramOptions::getGlobalConfigFileDirectory() const {
    return globalConfigDir;
}

void ProgramOptions::setGlobalConfigFileDirectory(const QString &dir)
{
    globalConfigDir=dir;
}

QString ProgramOptions::getPluginDirectory() const {
    return pluginsDir;
}

QString ProgramOptions::getAssetsDirectory() const {
    return assetsDir;
}

QString ProgramOptions::getExamplesDirectory() const {
    return examplesDir;
}
QString ProgramOptions::getMainHelpDirectory() const
{
    return assetsDir+"/help/";
}

QString ProgramOptions::getApplicationDirectory() const {
    return appDir;
}

QString ProgramOptions::getSourceDirectory() const
{
#ifdef Q_OS_MAC
    return appDir+"/quickfit3.app/Contents/SharedSupport/sources/";
#else
    return appDir+"/source/";
#endif
}

QString ProgramOptions::getHomeQFDirectory() const
{
    QString cfg=getConfigValue("quickfit/homedir", "").toString();
    if (!cfg.isEmpty() && QDir(cfg).exists()) return cfg;

    QDir d=QDir::home();
    if (!QDir(d.absolutePath()+"/quickfit3").exists()) d.mkdir("quickfit3");
    return d.absolutePath()+"/quickfit3/";
}

void ProgramOptions::setHomeQFDirectory(const QString &dir)
{
    setConfigValue("quickfit/homedir", dir);
}


void ProgramOptions::setProxyHost(const QString &host, bool write)
{
    //proxy.setHostName(host);
    proxyHost=host;
    if (write) writeSettings();
}

void ProgramOptions::setProxyPort(quint16 port, bool write)
{
    //proxy.setPort(port);
    proxyPort=port;
    if (write) writeSettings();
}

quint16 ProgramOptions::getProxyPort() const
{
    //return proxy.port();
    return proxyPort;
}

QString ProgramOptions::getProxyHost() const
{
    //return proxy.hostName();
    return proxyHost;
}

int ProgramOptions::getProxyType() const
{
    //return proxy.type();
    return proxyType;
}

void ProgramOptions::setProxyType(int type, bool write)
{
    if (type<0) proxyType=0; //proxy.setType(QNetworkProxy::DefaultProxy);
    else proxyType=type; //proxy.setType(type);
    if (write) writeSettings();
}


bool ProgramOptions::getUserSaveAfterFirstEdit() const
{
    return userSaveAfterFirstEdit;
}

void ProgramOptions::setUserSaveAfterFirstEdit(bool set, bool write)
{
    userSaveAfterFirstEdit=set;
    if (write) writeSettings();
}

bool ProgramOptions::getChildWindowsStayOnTop() const
{
    return childWindowsStayOnTop;
}

void ProgramOptions::setChildWindowsStayOnTop(bool set, bool write)
{
    childWindowsStayOnTop=set;
    if (write) writeSettings();
}

bool ProgramOptions::getHelpWindowsStayOnTop() const
{
    return helpWindowsStayOnTop;
}

void ProgramOptions::setHelpWindowsStayOnTop(bool set, bool write)
{
    helpWindowsStayOnTop=set;
    if (write) writeSettings();
}

bool ProgramOptions::getProjectWindowsStayOnTop() const
{
    return projectWindowStayOnTop;
}

void ProgramOptions::setProjectWindowsStayOnTop(bool set, bool write)
{
    projectWindowStayOnTop=set;
    if (write) writeSettings();
}

bool ProgramOptions::debugLogVisible() const
{
    return m_debugLogVisible;
}

void ProgramOptions::setDebugLogVisible(bool visible, bool write)
{
    m_debugLogVisible=visible;
    if (write) writeSettings();
}

void ProgramOptions::setLanguageID(QString id, bool write) {
    languageID=id;
    if (write) writeSettings();
}

void ProgramOptions::setStylesheet(QString st, bool write) {
    stylesheet=st;
    if (write) writeSettings();
}

void ProgramOptions::setStyle(QString st, bool write) {
    style=st;
    if (write) writeSettings();
}

void ProgramOptions::setMaxThreads(int threads, bool write)
{
    maxThreads=threads;
    if (write) writeSettings();
}

void ProgramOptions::setAutosave(int interval, bool write)
{
    autosave=interval;
    if (write) writeSettings();
}


