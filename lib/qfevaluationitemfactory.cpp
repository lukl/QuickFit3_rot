#include "qfevaluationitemfactory.h"
#include "qfevaluationitem.h"

QFEvaluationItemFactory::QFEvaluationItemFactory(ProgramOptions* options, QObject* parent):
    QObject(parent)
{
    m_options=options;
}

QFEvaluationItemFactory::~QFEvaluationItemFactory()
{
    //dtor
}


void QFEvaluationItemFactory::searchPlugins(QString directory) {
    QDir pluginsDir = QDir(directory);
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (plugin) {
            QFPluginEvaluationItem* iRecord = qobject_cast<QFPluginEvaluationItem*>(plugin);
            if (iRecord) {
                items[iRecord->getID()]=iRecord;
                filenames[iRecord->getID()]=pluginsDir.absoluteFilePath(fileName);
                emit showMessage(tr("loaded evaluation plugin '%2' (%1) ...").arg(fileName).arg(iRecord->getName()));
                emit showLongMessage(tr("loaded evaluation plugin '%2':\n   author: %3\n   copyright: %4\n   file: %1").arg(filenames[iRecord->getID()]).arg(iRecord->getName()).arg(iRecord->getAuthor()).arg(iRecord->getCopyright()));
            }
        }
    }
}

void QFEvaluationItemFactory::distribute(QFProject* project, ProgramOptions* settings, QFPluginServices* services, QWidget* parent) {
    for (int i=0; i<getIDList().size(); i++) {
        items[getIDList().at(i)]->setProject(project);
        items[getIDList().at(i)]->setSettings(settings);
        items[getIDList().at(i)]->setServices(services);
        items[getIDList().at(i)]->setParentWidget(parent);
    }
}


int QFEvaluationItemFactory::getMajorVersion(QString id) {
    int ma, mi;
    if (items.contains(id)) {
        items[id]->getVersion(ma, mi);
        return ma;
    }
    return 0;
}

int QFEvaluationItemFactory::getMinorVersion(QString id) {
    int ma, mi;
    if (items.contains(id)) {
        items[id]->getVersion(ma, mi);
        return mi;
    }
    return 0;
}




QStringList QFEvaluationItemFactory::getIDList() {
    return items.keys();
}

QString QFEvaluationItemFactory::getDescription(QString ID) {
    if (items.contains(ID)) return items[ID]->getDescription();
    return QString("");
};

QString QFEvaluationItemFactory::getName(QString ID) {
    if (items.contains(ID)) return items[ID]->getName();
    return QString("");
};

QFEvaluationItem* QFEvaluationItemFactory::createRecord(QString ID, QFPluginServices* services, QFProject* parent)  {
    if (items.contains(ID)) return items[ID]->createRecord(parent);
    return NULL;
};


QString QFEvaluationItemFactory::getAuthor(QString ID) {
    if (items.contains(ID)) return items[ID]->getAuthor();
    return QString("");
};

QString QFEvaluationItemFactory::getCopyright(QString ID) {
    if (items.contains(ID)) return items[ID]->getCopyright();
    return QString("");
};

QString QFEvaluationItemFactory::getWeblink(QString ID) {
    if (items.contains(ID)) return items[ID]->getWeblink();
    return QString("");
};

QString QFEvaluationItemFactory::getIconFilename(QString ID) {
    if (items.contains(ID)) return items[ID]->getIconFilename();
    return QString("");
};

QString QFEvaluationItemFactory::getPluginFilename(QString ID) {
    if (items.contains(ID)) return filenames[ID];
    return QString("");
};

void QFEvaluationItemFactory::registerMenu(QString ID, QMenu* menu)  {
    if (items.contains(ID)) {
        return items[ID]->registerToMenu(menu);
    }
};

QString QFEvaluationItemFactory::getPluginHelp(QString ID) {
    if (items.contains(ID)) {
        QString basename=QFileInfo(getPluginFilename(ID)).baseName();
    #ifndef Q_OS_WIN32
        if (basename.startsWith("lib")) basename=basename.right(basename.size()-3);
    #endif
        return m_options->getAssetsDirectory()+QString("/plugins/help/%1/%2.html").arg(basename).arg(ID);
    }
    return "";
}

QString QFEvaluationItemFactory::getPluginTutorial(QString ID) {
    if (items.contains(ID)) {
        QString basename=QFileInfo(getPluginFilename(ID)).baseName();
    #ifndef Q_OS_WIN32
        if (basename.startsWith("lib")) basename=basename.right(basename.size()-3);
    #endif
        return m_options->getAssetsDirectory()+QString("/plugins/help/%1/tutorial.html").arg(basename);
    }
    return "";
}

