#ifndef QFLISTPROGRESSDIALOG_H
#define QFLISTPROGRESSDIALOG_H

#include <QDialog>
#include "lib_imexport.h"
#include "qfproject.h"
#include "qftools.h"

namespace Ui {
    class QFListProgressDialog;
}

/*! \brief a progress dialog with a (growing) list of tasks
    \ingroup qf3lib_tools

  */

class QFLIB_EXPORT QFListProgressDialog : public QDialog, public QFProgressMinorProgress
{
        Q_OBJECT

    public:
        explicit QFListProgressDialog(QWidget *parent = 0);
        ~QFListProgressDialog();

        bool wasCanceled() const;
    public slots:
        void setProgress(int value);
        void setRange(int rmin, int rmax);
        void setMessage(const QString& message);
        void setMainMessage(const QString& message);
        void addMessage(const QString& message);
        void incProgress(int inc=1);
        void setMinorProgressEnabled(bool en);
        void setMinorPrgressLabel(const QString& label);

        void setUserClose(bool enabled=true);
        void setProgresComplete();
        virtual void setMinorProgress(int value);
        virtual void setMinorProgressRange(int min=0, int max=100);
        virtual bool wasMinorProgressCanceled() const;
    protected slots:
        void on_btnCancel_clicked();
        void on_btnClose_clicked();
    private:
        Ui::QFListProgressDialog *ui;
        bool m_canceled;
};

#endif // QFLISTPROGRESSDIALOG_H
