#ifndef QFLIGHTSOURCECOMBOBOX_H
#define QFLIGHTSOURCECOMBOBOX_H

#include <QComboBox>
#include "qfextensionmanager.h"
#include "../interfaces/qfextensionlightsource.h"
#include "qfextension.h"
#include "libwid_imexport.h"
#include "qenhancedcombobox.h"

/*! \brief QComboBox which is filled with all available cameras
    \ingroup qf3lib_widgets

    If this QComboBox is initialized with a valid QFExtensionManager object, it will be automatically
    filled with all available cameras.
 */
class QFWIDLIB_EXPORT QFLightSourceComboBox : public QEnhancedComboBox {
        Q_OBJECT
    public:
        QFLightSourceComboBox(QFExtensionManager* extManager, QWidget* parent=NULL);
        QFLightSourceComboBox(QWidget* parent=NULL);
        virtual ~QFLightSourceComboBox();
        /** \brief init the combobox with all cameras registered in the QFExtensionManager */
        void init(QFExtensionManager* extManager);
        /** \brief save the currently selected camera to a QSettings object */
        void storeSettings(QSettings& settings, QString prefix=QString("")) const;
        /** \brief load the currently selected camera from a QSettings object */
        void loadSettings(QSettings& settings, QString prefix=QString(""));
        /** \brief return the currently selected camera extension as QFExtension */
        QFExtension* currentExtension() const;
        /** \brief return the currently selected camera extension as QFExtensionLightSource */
        QFExtensionLightSource* currentExtensionLightSource() const;
        /** \brief return the ID of the current camera device inside a QFExtensionLightSource */
        int currentLightSourceID() const;
    public slots:
        /** \brief update the filled combobox */
        void findExtensions();

    protected:
        /** \brief list of all available QFExtensionLightSource plugins, initialized by findExtensions() */
        QList<QObject*> LightSources;

        /** \brief return the ID of the current camera device inside cameras */
        int currentExtensionID() const;

        QFExtensionManager* m_extManager;
    protected slots:
        void comboIndexChanged(int index);
    private:
};

#endif // QFLIGHTSOURCECOMBOBOX_H