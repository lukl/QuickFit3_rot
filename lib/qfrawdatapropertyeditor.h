#ifndef QFRAWDATAPROPERTYEDITOR_H
#define QFRAWDATAPROPERTYEDITOR_H

#include <QtGui>
#include "qfproject.h"
#include "qfrawdatarecord.h"
#include "programoptions.h"

/*! \brief editor widget (window) for raw data items


 As there were problems when changing the displayed editor for different types of raw data items. One editor window
 is only useable for one type of  raw data!
*/
class QFRawDataPropertyEditor : public QWidget {
        Q_OBJECT
    public:
        /** Default constructor */
        QFRawDataPropertyEditor(ProgramOptions* set, QFRawDataRecord* current=NULL, int id=0, QWidget* parent=NULL, Qt::WindowFlags f = 0);
        /** Default destructor */
        virtual ~QFRawDataPropertyEditor();
        /** \brief set the current record */
        void setCurrent(QFRawDataRecord* c);
        /** \brief set a Programoptions object to use for storing application settings */
        void setSettings(ProgramOptions* settings);
        /** \brief write the settings */
        void writeSettings();
    protected:
        /** \brief points to the record currently displayed */
        QFRawDataRecord* current;
        /** \brief read the settings */
        void readSettings();
        void closeEvent( QCloseEvent * event );
        void resizeEvent ( QResizeEvent * event );
    private slots:
        /** \brief called when the name editor changes its contents */
        void nameChanged(const QString& text);
        /** \brief called when the description editor changes its contents */
        void descriptionChanged();
        /** \brief this will be connected to the project to indicate when  the currently
         *         displayed record should be deleted */
        void recordAboutToBeDeleted(QFRawDataRecord* r);
        /** \brief emitted when the "next" button is pressed */
        void nextPressed();
        /** \brief emitted when the "previous" button is pressed */
        void previousPressed();
        /** \brief emitted when the raw data record data changes (i.e. ID, name, description, ...) */
        void propsChanged();
        /** \brief insert a new property */
        void newPropClicked();
        /** \brief delete a property */
        void deletePropClicked();
        /** \brief resize the cells in the properties table */
        void resizePropertiesTable();
    private:
        /** \brief create all widgets needed to display data */
        void createWidgets();
        /** \brief QLineEdit for the Name */
        QLineEdit* edtName;
        /** \brief QPlainTextEdit for the description */
        QPlainTextEdit* pteDescription;
        /** \brief list of files associated with this rawdatarecord */
        QListWidget* lstFiles;
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
        /** \brief tabel view for the properties */
        QTableView* tvProperties;
        /** \brief button to switch to next record */
        QPushButton* btnNext;
        /** \brief button to insert a new property */
        QPushButton* btnNewProperty;
        /** \brief button to delete a property */
        QPushButton* btnDeleteProperty;
        /** \brief button to switch to previous record */
        QPushButton* btnPrevious;
        /** \brief points to a settings object that is used to store application settings */
        ProgramOptions* settings;
        /** \brief tabel display the evaluation results associated with this file  */
        QTableView* tvResults;
        /** \brief widget that is used to display the tvResults table + opt. some more compoinents */
        QWidget* widResults;

        QList<QFRawDataEditor*> editorList;
        int id;
};

#endif // QFRAWDATAPROPERTYEDITOR_H
