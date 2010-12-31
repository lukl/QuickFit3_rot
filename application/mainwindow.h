
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "../lib/qfproject.h"
#include "../lib/qfprojecttreemodel.h"
#include "../lib/qfrawdatapropertyeditor.h"
#include "../lib/qfevaluationpropertyeditor.h"
#include "../lib/qfevaluationitem.h"
#include "../lib/qfevaluationeditor.h"
#include "../lib/qfevaluationitemfactory.h"
#include "../lib/programoptions.h"
#include "qt/qtlogfile.h"
#include "../lib/qfrawdatarecordfactory.h"
#include "../lib/qfpluginreportwrapper.h"


#define QF_THANKS_TO "Dr. Nicolas Dross, Dr. Gy�rgy V�mosi, Prof. J�rg Langowski, Dr. Katalin T�th, Vera B�hm"
#define QF_COPYRIGHT "&copy; 2010 by Jan Krieger"

/*! \brief main widget for QuickFit
    \ingroup qf3app
*/
class MainWindow : public QMainWindow, public QFPluginReportWrapper {
        Q_OBJECT

    public:
        /** \brief class constructor
         *  \param splash a splash screen to use for status output during startup/construction
         */
        MainWindow(QSplashScreen* splash);
        ~MainWindow();

        void setSettings(ProgramOptions* s) {
            settings=s;
            readSettings();
            rawDataFactory->distribute(project, settings, this, this);
            evaluationFactory->distribute(project, settings, this, this);
        }

        /** \copydoc QFPluginReportWrapper::log_text()  */
        virtual void log_text(QString message);
        /** \copydoc QFPluginReportWrapper::log_warning()  */
        virtual void log_warning(QString message);
        /** \copydoc QFPluginReportWrapper::log_error()  */
        virtual void log_error(QString message);
        /** \copydoc QFPluginReportWrapper::setStatusMessage()  */
        virtual void setStatusMessage(QString message);
        /** \copydoc QFPluginReportWrapper::setProgressRange()  */
        virtual void setProgressRange(int minP, int maxP);
        /** \copydoc QFPluginReportWrapper::setProgress()  */
        virtual void setProgress(int value);
        /** \copydoc QFPluginReportWrapper::incProgress()  */
        virtual void incProgress();
    protected:
        void closeEvent(QCloseEvent *event);

    private slots:
        /** \brief create a new project */
        void newProject();
        /** \brief display an open project dialog and open the selected project */
        void openProject();
        /** \brief save current project */
        bool saveProject();
        /** \brief save current project with a new filename */
        bool saveProjectAs();
        /** \brief display informaion about QuickFit */
        void about();
        /** \brief display information about plugins */
        void aboutPlugins();
        /** \brief emitted when a project element (in the treeview) is doubl-clicked
         *
         * This will open a new window containing the elements properties and data.
         */
        void projectElementDoubleClicked ( const QModelIndex & index );

        /** \brief emitted when the project name changes */
        void projectNameChanged(const QString& text);

        /** \brief emitted when the project creator changes */
        void projectCreatorChanged(const QString& text);

        /** \brief emitted when the project description changes */
        void projectDescriptionChanged();

        /** \brief reads the project properties from the project into the edit widgets */
        void readProjectProperties();


        /** \brief delete the currently selected item */
        void deleteItem();

        void modelReset();

        void insertRawData();

        void insertEvaluation();

        /** \brief output a message to the application logging widget */
        void showLogMessage(const QString& message);

        /** \brief open settings dialog */
        void openSettingsDialog();
    private:
        void createWidgets();
        void createActions();
        void createMenus();
        void createToolBars();
        void createStatusBar();
        void searchAndRegisterPlugins();
        void readSettings();
        void writeSettings();
        bool maybeSave();
        void loadProject(const QString &fileName);
        bool saveProject(const QString &fileName);
        void setCurrentProject(const QString &fileName);
        QString strippedName(const QString &fullFileName);

        QString curFile;
        QFProject* project;

        QMenu *fileMenu;
        QMenu *dataMenu;
        QMenu *evaluationMenu;
        QMenu *helpMenu;
        QMenu* insertMenu;
        QToolBar *fileToolBar;
        QToolBar *dataToolBar;
        QAction *newProjectAct;
        QAction *openProjectAct;
        QAction *saveProjectAct;
        QAction *saveProjectAsAct;
        QAction *exitAct;
        QAction *aboutAct;
        QAction *aboutQtAct;
        QAction *aboutPluginsAct;
        QAction* optionsAct;
        QProgressBar* prgMainProgress;

        QAction* delItemAct;
        /*QAction* insertRDTableAct;
        QAction* insertFCSFileAct;
        QAction* insertRDTableFileAct;*/

        QTreeView* tvMain;
        QSplitter* spMain;
        QSplitter* spCenter;

        QLabel* labFile;
        QPlainTextEdit* pteDescription;
        QLineEdit* edtName;
        QLineEdit* edtCreator;

        QtLogFile* logFileProjectWidget;
        QtLogFile* logFileMainWidget;
        QTabWidget* tabLogs;


        QList<QPointer<QFRawDataPropertyEditor> > rawDataPropEditors;
        QList<QPointer<QFEvaluationPropertyEditor> > evaluationPropEditors;
        /** \brief the last project open/save directory */
        QString currentProjectDir;
        /** \brief the last rawdata insert directory */
        QString currentRawDataDir;
        /** \brief ProgramOptions object used for the application settings */
        ProgramOptions* settings;

        /** \brief evaluation item factory */
        QFEvaluationItemFactory* evaluationFactory;
        /** \brief raw data record item factory */
        QFRawDataRecordFactory* rawDataFactory;

        /** \brief return a pointer to the raw data record factry object */
        inline QFRawDataRecordFactory* getRawDataRecordFactory() { return rawDataFactory; };
        /** \brief return a pointer to the evaluation item factry object */
        inline QFEvaluationItemFactory* getEvaluationItemFactory() { return evaluationFactory; };
        /** \brief used to store the splash screen pixmap, as given to the constructor. This pixmap may be used for info dialogs. */
        QPixmap splashPix;
};

#endif
