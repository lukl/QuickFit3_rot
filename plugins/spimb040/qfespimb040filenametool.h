#ifndef QFESPIMB040FILENAMETOOL_H
#define QFESPIMB040FILENAMETOOL_H

#include <QMap>
#include <QString>
#include "qenhancedlineedit.h"
#include <QWidget>
#include <QDir>
#include <QFileInfo>

class QFESPIMB040FilenameTool {
    public:
        QFESPIMB040FilenameTool();

        QString transformFilename(const QString& filename) const;
        void setReplaceValues(const QMap<QString, QString>& replaceValues);
        void setReplaceValue(const QString& key, const QString& value);
        void bindLineEdit(QEnhancedLineEdit* edit);

    protected:
        QMap<QString, QString> replaceValues;
        QList<QEnhancedLineEdit*> edits;
        void fillLineEdits();
};

#endif // QFESPIMB040FILENAMETOOL_H