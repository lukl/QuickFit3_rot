/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef QFFITRESULTSBYINDEXASVECTOREVALUATION_H
#define QFFITRESULTSBYINDEXASVECTOREVALUATION_H

#include "qffitresultsbyindexevaluation.h"
#include "qffitresultsevaluation.h"

#include <QHash>
#include <QMap>
#include <QSettings>

#include "qfevaluationitem.h"
#include "qfevaluationitemfactory.h"
#include "qffitfunction.h"
#include "qffitalgorithm.h"
#include "qffitparameterbasicinterface.h"

/*! \brief evaluation item base class that changes the methods in QFFitResultsByIndexEvaluation so they store fit parameters as vectors, not as single values
    \ingroup qf3evaluationplugins

    The changed storage only applies for index values >=0. So the average run is still stored in the standard form.
    The parameters are stored as lists of values indexed by the given run/index. To distinguish between local and global parameters, a second
    list of booleans result is used for every parameter, indicating whether it is local or global. the name of this second list is created
    from the original parameterID using getParamNameLocalStore().

    To implement this behaviour this class reimplements all functions from its ancestors that write directly to \c QFRawDaataRecord::results... functions.
*/
class QFFitResultsByIndexAsVectorEvaluation : public QFFitResultsByIndexEvaluation {
        Q_OBJECT
    public:
        QFFitResultsByIndexAsVectorEvaluation(const QString& fitFunctionPrefix, QFProject* parent, bool showRDRList=true, bool useSelection=false);
        virtual ~QFFitResultsByIndexAsVectorEvaluation();


        /** \copydoc QFEvaluationItem::setPresetProperty() */
        virtual void setPresetProperty(const QString& id, const QVariant& data, bool usereditable=true, bool visible=true);



        virtual QString transformResultID(const QString& resultID) const;

        virtual void resetAllFitResultsCurrentFileAllIndices();
        virtual void resetAllFitResultsAllFilesAllIndices();
        virtual void resetDefaultFitValue(const QString &id, QFRawDataRecord *r);
        virtual void resetDefaultFitFix(const QString &id, QFRawDataRecord *r);
        virtual void resetAllFitResultsCurrent(QFRawDataRecord *r=NULL);
        virtual void resetAllFitResults(QFRawDataRecord* r, const QString &resultID);
        virtual void resetDefaultFitValue(QFRawDataRecord *r, const QString &resultID, const QString &id);
        virtual void resetAllDefaultFitValue(QFRawDataRecord *r, const QString &id);
        virtual void resetAllDefaultFitFix(QFRawDataRecord *r, const QString &id);
        virtual void resetAllFitValue();
        virtual void resetAllFitFix();
        virtual void resetDefaultFitFix(QFRawDataRecord *r, const QString &resultID, const QString &id);
        virtual bool hasFit(QFRawDataRecord *r1, const QString &evalID) const;
        virtual void setFitResultValue(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, double value) ;
        virtual void setAllFitResultValue(QFRawDataRecord *r, const QString &parameterID, double value) ;
        virtual void setAllFitResultError(QFRawDataRecord *r, const QString &parameterID, double error) ;
        virtual void setAllFitResultFix(QFRawDataRecord *r, const QString &parameterID, bool fix) ;
        virtual void setFitResultValue(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, double value, QString unit) ;
        virtual void setFitResultValue(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, double value, double error) ;
        virtual void setFitResultValue(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, double value, double error, QString unit)  ;

        virtual void setFitResultValue(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, QVector<double> value, QVector<double> error, QString unit=QString(""))  ;
        virtual void setFitResultValue(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, QVector<double> value, QString unit=QString(""))  ;

        virtual void setFitResultError(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, double error)  ;
        virtual void setFitResultValueString(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, QString value) ;
        virtual void setFitResultValueInt(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, int64_t value, QString unit) ;
        virtual void setFitResultValueInt(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, int64_t value) ;
        virtual void setFitResultValueBool(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, bool value) ;
        virtual void setFitResultValues(QFRawDataRecord *r, const QString &resultID, double *values, double *errors) ;
        virtual void setFitResultValuesVisibleWithGroupAndLabel(QFRawDataRecord *r, const QString &resultID, const double *values, const double *errors, const QString &group,const  bool *fix, const QString &fixGroup, bool sortPriority) ;
        virtual void setFitResultValuesVisible(QFRawDataRecord *r, const QString &resultID, double *values, double *errors) ;
        virtual void setFitResultFix(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, bool fix) ;
        virtual void setFitResultGroup(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, const QString &group) ;
        virtual void setFitResultLabel(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, const QString &label, const QString &label_richtext) ;
        virtual double getFitError(QFRawDataRecord *r, const QString &resultID, const QString &parameterID) const;
        virtual double getFitValue(QFRawDataRecord *r, const QString &resultID, const QString &parameterID) const;
        virtual bool getFitFix(QFRawDataRecord *r, const QString &resultID, const QString &parameterID) const;
        virtual void setAllFitValues(const QString &id, double value, double error, bool currentFileOnly);
        virtual void setAllFitFixes(const QString &id, bool fix, bool currentFileOnly);
        virtual void resetAllFitResultsAllFiles();
    protected:
        virtual QString getParamNameLocalStore(const QString& paramID) const;
        virtual bool isParamNameLocalStore(const QString& paramID) const;

    public:

        /* explicitly make some functions visible again, as the C++ compiler hides function definitions
           from a base class that are also declared in the derived class, but with different parameter sets!

           see http://www.parashift.com/c++-faq-lite/strange-inheritance.html#faq-23.9
         */


        using QFFitResultsByIndexEvaluation::setFitResultValuesVisibleWithGroupAndLabel;
        using QFFitResultsByIndexEvaluation::hasFit;
        using QFFitResultsByIndexEvaluation::setFitResultValue;
        using QFFitResultsByIndexEvaluation::setFitResultGroup;
        using QFFitResultsByIndexEvaluation::setFitResultLabel;
        using QFFitResultsByIndexEvaluation::resetDefaultFitValue;
        using QFFitResultsByIndexEvaluation::resetDefaultFitFix;
        using QFFitResultsByIndexEvaluation::getFitError;
        using QFFitResultsByIndexEvaluation::getFitValue;
        using QFFitResultsByIndexEvaluation::getFitFix;

};

#endif // QFFITRESULTSBYINDEXASVECTOREVALUATION_H



/*
  TODO: overwrite function implementations in QFFitResultsByIndexEvaluation that use ->results... from QFRawDataRecord
        ... reenable the imfcsfit plugin!

 */



/*

    bool doEmit=r->isEmitResultsChangedEnabled();
    r->disableEmitResultsChanged();
    if (doEmit) r->enableEmitResultsChanged(true);

  */
