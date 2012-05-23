#include "qfshutterconfigwidget.h"
#include <QtGui>
#include <iostream>


QFShutterConfigWidget::QFShutterConfigWidget(QWidget* parent):
    QWidget(parent)
{

    shutterStateUpdateInterval=351;
    iconOpened=QPixmap(":/libqf3widgets/shutter_open.png");
    iconClosed=QPixmap(":/libqf3widgets/shutter_closed.png");

    timUpdate=new QTimer(this);
    timUpdate->setSingleShot(true);
    timUpdate->setInterval(shutterStateUpdateInterval);

    m_log=NULL;
    m_pluginServices=NULL;
    locked=false;
    moving=false;


    createWidgets();
    createActions();
    updateStates();


    connect(timUpdate, SIGNAL(timeout()), this, SLOT(displayShutterStates()));
    timUpdate->start(shutterStateUpdateInterval);
    //QTimer::singleShot(stageStateUpdateInterval, this, SLOT(displayAxisStates()));
}


QFShutterConfigWidget::~QFShutterConfigWidget()
{
    locked=true;
    disconnect(timUpdate, SIGNAL(timeout()), this, SLOT(displayShutterStates()));
    timUpdate->stop();
}

void QFShutterConfigWidget::lockShutters() {
    locked=true;
    disconnect(timUpdate, SIGNAL(timeout()), this, SLOT(displayShutterStates()));
    timUpdate->stop();
}


void QFShutterConfigWidget::unlockShutters() {
    locked=false;
    connect(timUpdate, SIGNAL(timeout()), this, SLOT(displayShutterStates()));
    timUpdate->setSingleShot(true);
    timUpdate->setInterval(shutterStateUpdateInterval);
    timUpdate->start(shutterStateUpdateInterval);
}


void QFShutterConfigWidget::setLog(QFPluginLogService* log) {
    m_log=log;
}

void QFShutterConfigWidget::init(QFPluginLogService* log, QFPluginServices* pluginServices) {
    m_log=log;
    m_pluginServices=pluginServices;

    if (m_pluginServices) {
        cmbShutter->init(m_pluginServices->getExtensionManager());
    } else {
        //stages.clear();
        cmbShutter->clear();
    }
    updateStates();
}

void QFShutterConfigWidget::loadSettings(QSettings& settings, QString prefix) {
    cmbShutter->loadSettings(settings, prefix+"shutter/");

    shutterStateUpdateInterval=settings.value(prefix+"update_interval", shutterStateUpdateInterval).toDouble();
}

void QFShutterConfigWidget::saveSettings(QSettings& settings, QString prefix) {
    cmbShutter->storeSettings(settings, prefix+"shutter/");
    settings.setValue(prefix+"update_interval", shutterStateUpdateInterval);
}

void QFShutterConfigWidget::createWidgets() {
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create main layout
    ////////////////////////////////////////////////////////////////////////////////////////////////
    QGridLayout* widgetLayout=new QGridLayout(this);
    setLayout(widgetLayout);
    widgetLayout->setContentsMargins(0,0,0,0);
    widgetLayout->setHorizontalSpacing(2);



    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create input widgets for camera devices
    ////////////////////////////////////////////////////////////////////////////////////////////////
    btnState=new QToolButton(this);
    btnState->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    widgetLayout->addWidget(btnState,0,0);
    cmbShutter=new QFShutterComboBox(this);
    widgetLayout->addWidget(cmbShutter,0,1);
    btnConnect=new QToolButton(this);
    widgetLayout->addWidget(btnConnect,0,2);
    btnConfigure=new QToolButton(this);
    widgetLayout->addWidget(btnConfigure,0,3);
    QWidget* w=new QWidget(this);
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    widgetLayout->addWidget(w,0,4);
    widgetLayout->setColumnStretch(4,1);
    cmbShutter->setEnabled(false);


}

void QFShutterConfigWidget::createActions() {
    actConnect=new QAction(QIcon(":/libqf3widgets/connect_shutter.png"), tr("Connect to shutter driver/hardware ..."), this);
    actConnect->setCheckable(true);
    connect(actConnect, SIGNAL(toggled(bool)), this, SLOT(disConnect()));
    btnConnect->setDefaultAction(actConnect);


    actConfigure=new QAction(QIcon(":/libqf3widgets/configure_shutter.png"), tr("Configure shutter ..."), this);
    connect(actConfigure, SIGNAL(triggered()), this, SLOT(configure()));
    btnConfigure->setDefaultAction(actConfigure);


    actState=new QAction(iconClosed, tr("closed"), this);
    actState->setCheckable(true);
    actState->setToolTip(tr("click to open and close the shutter"));
    connect(actState, SIGNAL(toggled(bool)), this, SLOT(shutterActionClicked(bool)));
    btnState->setDefaultAction(actState);

}

void QFShutterConfigWidget::updateStates() {
    QFExtensionShutter* shutter;
    int shutterID;
    bool conn;


    shutter=getShutter();
    shutterID=getShutterID();
    conn=false;
    if (shutter) {
        conn=shutter->isShutterConnected(shutterID);
        if (conn) {
            actConnect->setChecked(true);
            actConnect->setIcon(QIcon(":/libqf3widgets/disconnect_shutter.png"));
            actConnect->setText(tr("Disconnect from shutter driver/hardware ..."));
        } else {
            actConnect->setChecked(false);
            actConnect->setIcon(QIcon(":/libqf3widgets/connect_shutter.png"));
            actConnect->setText(tr("Connect from shutter driver/hardware ..."));
        }
    }
    actConfigure->setEnabled(shutter!=NULL && shutterID>=0);
    actConnect->setEnabled(shutter!=NULL && shutterID>=0);
    cmbShutter->setEnabled(!conn);
    actState->setEnabled(conn && shutter!=NULL && (!moving));

}

void QFShutterConfigWidget::disConnect() {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    bool conn=actConnect->isChecked();
    QFExtensionShutter* shutter=getShutter();
    int shutterID=getShutterID();

    if (shutter) {
        //qDebug()<<"connecting "<<conn;
        if (conn) {
            shutter->setShutterLogging(m_log);
            shutter->shutterConnect(shutterID);
            if (shutter->isShutterConnected(shutterID)) {
                m_log->log_text("connected to shutter driver ...\n");
            } else {
                actConnect->setChecked(false);
                shutter->shutterDisonnect(shutterID);
                m_log->log_error("error connecting to shutter driver ...\n");
            }
        } else {
            shutter->shutterDisonnect(shutterID);
            m_log->log_text("disconnected from shutter driver ...\n");
        }
    } else {
        actConnect->setChecked(false);
    }
    updateStates();
    QApplication::restoreOverrideCursor();
}





void QFShutterConfigWidget::configure() {
    QFExtensionShutter* shutter=getShutter();
    int shutterID=getShutterID();
    if (shutter) shutter->showShutterSettingsDialog(shutterID, this);
    updateStates();
}




QFExtensionShutter* QFShutterConfigWidget::getShutter() const {
    return cmbShutter->currentExtensionShutter();
}

QFExtension* QFShutterConfigWidget::getShutterExtension() const {
    return cmbShutter->currentExtension();
}

int QFShutterConfigWidget::getShutterID()  const{

    return cmbShutter->currentShutterID();
}

bool QFShutterConfigWidget::getShutterState() {
    QFExtensionShutter* shutter;
    int shutterID;
    shutter=getShutter();
    shutterID=getShutterID();
    if (shutter) {
        return shutter->isShutterOpen(shutterID);
    }
    return false;
}


void QFShutterConfigWidget::displayShutterStates(/*bool automatic*/) {
    if (locked) return;

    QFExtensionShutter* shutter;
    int shutterID;
    shutter=getShutter();
    shutterID=getShutterID();
    if (shutter) {
        if (!moving)  {
            bool opened=shutter->isShutterOpen(shutterID);
            disconnect(actState, SIGNAL(toggled(bool)), this, SLOT(shutterActionClicked(bool)));
            if (opened) {
                actState->setIcon(iconOpened);
                actState->setText(tr("opened"));
                actState->setChecked(true);
            } else {
                actState->setIcon(iconClosed);
                actState->setText(tr("closed"));
                actState->setChecked(false);
            }
            connect(actState, SIGNAL(toggled(bool)), this, SLOT(shutterActionClicked(bool)));
            actState->setEnabled(true);
        } else {
            actState->setEnabled(false);
        }
    }
    updateStates();

    if (!locked) {
        timUpdate->setSingleShot(true);
        timUpdate->setInterval(shutterStateUpdateInterval);
        timUpdate->start(shutterStateUpdateInterval);

    }

}

void QFShutterConfigWidget::setShutter(bool opened) {
    actState->setChecked(opened);
}

void QFShutterConfigWidget::toggleShutter() {
    actState->setChecked(!actState->isChecked());
}

void QFShutterConfigWidget::shutterOff() {
    actState->setChecked(false);
}

void QFShutterConfigWidget::shutterOn() {
    actState->setChecked(true);
}

void QFShutterConfigWidget::shutterActionClicked(bool opened) {
    QFExtensionShutter* shutter;
    int shutterID;
    shutter=getShutter();
    shutterID=getShutterID();
    if (shutter) {
        //qDebug()<<"set shutter state opened="<<opened;
        shutter->setShutterState(shutterID, opened);
        if (!locked) {
            moving=true;
            actState->setEnabled(false);
            QTime started=QTime::currentTime();
            while (!shutter->isLastShutterActionFinished(shutterID) && (started.elapsed()<10000)) {
                //qDebug()<<started.elapsed();
                QApplication::processEvents();
            }
            moving=false;
        }
    }
}

void QFShutterConfigWidget::connectShutter() {
    bool b=actConnect->signalsBlocked();
    actConnect->blockSignals(true);
    actConnect->setChecked(true);
    disConnect();
    actConnect->blockSignals(b);
}

void QFShutterConfigWidget::disconnectShutter() {
    actConnect->setChecked(false);
}

void QFShutterConfigWidget::setReadOnly(bool readonly) {
    cmbShutter->setReadOnly(readonly);
}



bool QFShutterConfigWidget::isShutterConnected() const {
    return actConnect->isChecked();
}

bool QFShutterConfigWidget::isShutterDone() const {
    QFExtensionShutter* shutter;
    int shutterID;
    shutter=getShutter();
    shutterID=getShutterID();
    if (shutter) {
        return shutter->isLastShutterActionFinished(shutterID);
    }
    return true;
}
