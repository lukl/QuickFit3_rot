#include "qfrawdatarecordfactory.h"
#include "qfrawdatarecord.h"

QFRawDataRecordFactory::QFRawDataRecordFactory(ProgramOptions* options, QObject* parent):
    QObject(parent)
{
    m_options=options;
}

QFRawDataRecordFactory::~QFRawDataRecordFactory()
{
    //dtor
}

void QFRawDataRecordFactory::searchPlugins(QString directory, QList<QFPluginServices::HelpDirectoryInfo>* pluginHelpList) {
    QDir pluginsDir = QDir(directory);
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (plugin) {
            QFPluginRawDataRecord* iRecord = qobject_cast<QFPluginRawDataRecord*>(plugin);
            if (iRecord) {
                items[iRecord->getID()]=iRecord;
                filenames[iRecord->getID()]=pluginsDir.absoluteFilePath(fileName);
                emit showMessage(tr("loaded raw data plugin '%2' (%1) ...").arg(fileName).arg(iRecord->getName()));
                emit showLongMessage(tr("loaded raw data plugin '%2':\n   author: %3\n   copyright: %4\n   file: %1").arg(filenames[iRecord->getID()]).arg(iRecord->getName()).arg(iRecord->getAuthor()).arg(iRecord->getCopyright()));
                // , QList<QFPluginServices::HelpDirectoryInfo>* pluginHelpList
                if (pluginHelpList) {
                    QFPluginServices::HelpDirectoryInfo info;
                    info.plugin=iRecord;
                    info.directory=m_options->getAssetsDirectory()+QString("/plugins/help/")+QFileInfo(fileName).baseName()+QString("/");
                    info.mainhelp=info.directory+iRecord->getID()+QString(".html");
                    info.tutorial=info.directory+QString("tutorial.html");
                    if (!QFile::exists(info.mainhelp)) info.mainhelp="";
                    if (!QFile::exists(info.tutorial)) info.tutorial="";
                    info.plugintypehelp=m_options->getAssetsDirectory()+QString("/help/qf3_rdrscreen.html");
                    info.plugintypename=tr("Raw Data Plugins");
                    pluginHelpList->append(info);
                }
            }
        }
    }
}

void QFRawDataRecordFactory::distribute(QFProject* project, ProgramOptions* settings, QFPluginServices* services, QWidget* parent) {
    for (int i=0; i<getIDList().size(); i++) {
        items[getIDList().at(i)]->setProject(project);
        items[getIDList().at(i)]->setSettings(settings);
        items[getIDList().at(i)]->setServices(services);
        items[getIDList().at(i)]->setParentWidget(parent);
    }
}

void QFRawDataRecordFactory::deinit() {
    for (int i=0; i<getIDList().size(); i++) {
        items[getIDList().at(i)]->deinit();
    }
}

int QFRawDataRecordFactory::getMajorVersion(QString id) {
    int ma, mi;
    if (items.contains(id)) {
        items[id]->getVersion(ma, mi);
        return ma;
    }
    return 0;
}

int QFRawDataRecordFactory::getMinorVersion(QString id) {
    int ma, mi;
    if (items.contains(id)) {
        items[id]->getVersion(ma, mi);
        return mi;
    }
    return 0;
}


QStringList QFRawDataRecordFactory::getIDList() {
    return items.keys();
}

QString QFRawDataRecordFactory::getDescription(QString ID) {
    if (items.contains(ID)) return items[ID]->getDescription();
    return QString("");
};

QString QFRawDataRecordFactory::getName(QString ID) {
    if (items.contains(ID)) return items[ID]->getName();
    return QString("");
};

QString QFRawDataRecordFactory::getAuthor(QString ID) {
    if (items.contains(ID)) return items[ID]->getAuthor();
    return QString("");
};

QString QFRawDataRecordFactory::getCopyright(QString ID) {
    if (items.contains(ID)) return items[ID]->getCopyright();
    return QString("");
};

QString QFRawDataRecordFactory::getWeblink(QString ID) {
    if (items.contains(ID)) return items[ID]->getWeblink();
    return QString("");
};

QString QFRawDataRecordFactory::getIconFilename(QString ID) {
    if (items.contains(ID)) return items[ID]->getIconFilename();
    return QString("");
};

QString QFRawDataRecordFactory::getPluginFilename(QString ID) {
    if (items.contains(ID)) return filenames[ID];
    return QString("");
};


QFRawDataRecord* QFRawDataRecordFactory::createRecord(QString ID, QFProject* parent)  {
    if (items.contains(ID)) return items[ID]->createRecord(parent);
    return NULL;
};

void QFRawDataRecordFactory::registerMenu(QString ID, QMenu* menu)  {
    if (items.contains(ID)) {
        return items[ID]->registerToMenu(menu);
    }
};


QString QFRawDataRecordFactory::getPluginHelp(QString ID) {
    if (items.contains(ID)) {
        QString basename=QFileInfo(getPluginFilename(ID)).baseName();
    #ifndef Q_OS_WIN32
        if (basename.startsWith("lib")) basename=basename.right(basename.size()-3);
    #endif
        return m_options->getAssetsDirectory()+QString("/plugins/help/%1/%2.html").arg(basename).arg(ID);
    }
    return "";
}

QString QFRawDataRecordFactory::getPluginTutorial(QString ID) {
    if (items.contains(ID)) {
        QString basename=QFileInfo(getPluginFilename(ID)).baseName();
    #ifndef Q_OS_WIN32
        if (basename.startsWith("lib")) basename=basename.right(basename.size()-3);
    #endif
        return m_options->getAssetsDirectory()+QString("/plugins/help/%1/tutorial.html").arg(basename);
    }
    return "";
}

QString QFRawDataRecordFactory::getPluginCopyrightFile(QString ID) {
    if (items.contains(ID)) {
        QString basename=QFileInfo(getPluginFilename(ID)).baseName();
    #ifndef Q_OS_WIN32
        if (basename.startsWith("lib")) basename=basename.right(basename.size()-3);
    #endif
        return m_options->getAssetsDirectory()+QString("/plugins/help/%1/copyright.html").arg(basename);
    }
    return "";
}


bool  QFRawDataRecordFactory::contains(QString ID) {
    return items.contains(ID);
}

