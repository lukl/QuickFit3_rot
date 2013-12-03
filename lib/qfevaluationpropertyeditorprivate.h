#ifndef QFEVALUATIONPROPERTYEDITORPRIVATE_H
#define QFEVALUATIONPROPERTYEDITORPRIVATE_H

#include <QObject>
#include "lib_imexport.h"
#include <QtGui>
#include <QSortFilterProxyModel>
#include "qfproject.h"
#include "qfevaluationitem.h"
#include "programoptions.h"
#include "qfpluginservices.h"
#include "qftools.h"
#include "qfhtmldelegate.h"
#include "qfevaluationresultsmodel.h"
#include "qenhancedtableview.h"
#include "qvisiblehandlesplitter.h"
#include "qfenhancedlineedit.h"
#include "qfstyledbutton.h"
#include "qfcompleterfromfile.h"
#include "qfproject.h"
#include "qfhistogramservice.h"


class QFEvaluationPropertyEditor;

class QFEvaluationPropertyEditorPrivate : public QObject {
        Q_OBJECT
    public:
        friend class QFEvaluationPropertyEditor;

        explicit QFEvaluationPropertyEditorPrivate(QFEvaluationPropertyEditor *parent );

        QFEvaluationPropertyEditor* d;

        /** \brief create all widgets needed to display data */
        void createWidgets();
        /** \brief QLineEdit for the Name */
        QLineEdit* edtName;
        /** \brief QPlainTextEdit for the description */
        QPlainTextEdit* pteDescription;
        /** \brief label next to buttons on top */
        QLabel* labTop;
        /** \brief label next to buttons on top */
        QLabel* labTopIcon;
        /** \brief label for the ID */
        QLabel* labID;
        /** \brief label for the type */
        QLabel* labType;
        /** \brief label for the type icon */
        QLabel* labTypeIcon;
        /** \brief Tab to change between property editor and data view */
        QTabWidget* tabMain;
        /** \brief QListView to display raw data records to which an evaluation is applicable */
        QListView* lstRawData;
        /** \brief splitter between files list and evaluation tab */
        QVisibleHandleSplitter* splitMain;
        /** \brief Layout widget for the evaluation editor widget */
        QHBoxLayout* layWidgets;
        /** \brief remove the currently selected QFRawDataRecord */
        QPushButton* btnRemoveRawData;
        /** \brief widget containing the RDR list on the rhs */
        QWidget* widRDRList;

        /** \brief tabel display the evaluation results associated with this file  */
        QEnhancedTableView* tvResults;
        /** \brief label for averaging over selected values in tvResults */
        QLabel* labAveragedresults;
        /** \brief toolbar to access functions of tvResults */
        QToolBar* tbResults;
        /** \brief toolbar to access functions of tvResults */
        QToolBar* tbResultsFilter;
        /** \brief action used to copy selection in tvResults to clipbord */
        QAction* actCopyResults;
        QAction* actCopyResultsNoHead;
        /** \brief action used to copy selection in tvResults to clipbord */
        QAction* actCopyValErrResults;
        QAction* actCopyValErrResultsNoHead;
        /** \brief action used to copy selection in tvResults to clipbord */
        QAction* actCopyMedianQuantilesResults;
        QAction* actCopyMedianQuantilesNoHead;
        /** \brief action used to copy selection in tvResults to clipbord, in expanded form */
        QAction* actCopyExpanded;
        QAction* actCopyExpandedNoHead;
        QAction* actCopyExpandedNoHeadMatlab;

        /** \brief action used to save selection in tvResults to file */
        QAction* actSaveResults;
        QAction* actSaveResultsAveraged;
        QAction* actRefreshResults;
        QAction* actDeleteResults;
        /** \brief widget that is used to display the tvResults table + opt. some more compoinents */
        QWidget* widResults;
        QAction* actStatistics;
        QAction* actStatisticsComparing;

        QFEnhancedLineEdit* edtDisplayProperties;
        QFEnhancedLineEdit* edtFilterFiles;
        QFEnhancedLineEdit* edtFilterResults;
        QFEnhancedLineEdit* edtFilterFilesNot;
        QFEnhancedLineEdit* edtFilterResultsNot;
        QFCompleterFromFile* compFilterFiles;
        QFCompleterFromFile* compFilterResults;
        QFCompleterFromFile* compFilterFilesNot;
        QFCompleterFromFile* compFilterResultsNot;
        QCheckBox* chkFilterFilesRegExp;
        QCheckBox* chkFilterResultsRegExp;

        QFEnhancedLineEdit* edtFilterRecords;
        QFEnhancedLineEdit* edtFilterRecordsNot;
        QFCompleterFromFile* compFilterRecords;
        QFCompleterFromFile* compFilterRecordsNot;
        QCheckBox* chkFilterRecordsRegExp;
        QWidget* widFilterRecords;
        QCheckBox* chkShowAvg;
        bool filesListFiltered;


        /** \brief points to a settings object that is used to store application settings */
        ProgramOptions* settings;
        /** \brief of all currently instaciated editors */
        QPointer<QFEvaluationEditor> editor;
        /** \brief ID used to distinguish between different dialog (e.g. for settings) */
        int id;
        /** \brief pointer that allows for access to central QuickFit services */
        QFPluginServices* services;
        /** \brief button to diaply online-help */
        QToolButton* btnHelp;

        QString currentSaveDir;
        QMenuBar* menuBar;

        QAction* actHelp;
        QAction* actHelpPlugin;
        QAction* actHelpPluginTutorial;
        QAction* actHelpPluginCopyright;
        QAction* actHelpEval;
        QMenu* menuHelp;
        QMenu* menuResults;
        QList<QPair<int, QMenu*> > menus;

    public slots:
        /** \brief called when the name editor changes its contents */
        void nameChanged(const QString& text);
        /** \brief called when the description editor changes its contents */
        void descriptionChanged();
        /** \brief emitted when the raw data record data changes (i.e. ID, name, description, ...) */
        void propsChanged(const QString &property, bool visible);
        /** \brief this will be connected to the project to indicate when  the currently
         *         displayed evaluation item should be deleted */
        void evaluationAboutToBeDeleted(QFEvaluationItem* r);
        /** \brief activated when the selection in lstRawData changes
         *  \see rdrModelReset()
         */
        void selectionChanged(const QModelIndex& index, const QModelIndex& oldindex);
        /*! \brief emitted when the model representing the list of raw data records is reset (rdrProxy).

            This is used to work a round the behaviour of Qt that a selection changes whenever
            a model is reset. In this case we simply select the currently selected record, if it
            is still available or another record, if not (it has been deleted).
        */
        void rdrModelReset();

        void tvResultsSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

        void resultsChanged(QFRawDataRecord *record=NULL, const QString &evalName=QString(""), const QString &resultName=QString(""));

        void refreshResults();

        /** \brief save results to a file */
        void saveResults();
        /** \brief save results to a file with averaged vector/matrix items */
        void saveResultsAveraged();

        /** \brief remove the currently selected QFRawDataRecord */
        void removeRawData();

        /** \brief if this was the last record this applies to, we close it */
        void recordAboutToBeDeleted(QFRawDataRecord* record);
        /** \brief display the online help dialog for this plugin */
        void displayHelp();

        /** \brief display help dialog with the plugin help */
        void displayHelpPlugin();

        /** \brief display help dialog with the plugin tutorial */
        void displayHelpPluginTutorial();

        /** \brief display help dialog with the plugin copyright note */
        void displayHelpPluginCopyright();

        /** \brief display help dialog with the common RDR help */
        void displayHelpEval();
        void copyValErrResults();
        void copyValErrResultsNoHead();
        void copyMedianQuantilesResults();
        void copyMedianQuantilesResultsNoHead();
        void copyExpandedResults();
        void copyExpandedResultsNoHead();
        void copyExpandedResultsNoHeadMatlab();

        void filterFilesTextChanged(const QString &text);
        void filterResultsTextChanged(const QString &text);
        void filterRecordsTextChanged(const QString &text);
        void currentTabChanged(int tab);
        void checkHelpAvailable();
        void propertiesTextChanged(const QString& text);

        void filterRecordsChanged();

        void deleteSelectedRecords();

        void showAvgClicked(bool checked);

        void showStatistics();
        void showStatisticsComparing();

};


#endif // QFEVALUATIONPROPERTYEDITORPRIVATE_H