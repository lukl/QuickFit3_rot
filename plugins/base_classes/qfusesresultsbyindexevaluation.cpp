#include "qfusesresultsbyindexevaluation.h"

QFUsesResultsByIndexEvaluation::QFUsesResultsByIndexEvaluation(QFProject *parent, bool showRDRList, bool useSelection) :
    QFUsesResultsEvaluation(parent, showRDRList, useSelection)
{
}

QFUsesResultsByIndexEvaluation::~QFUsesResultsByIndexEvaluation()
{
}

bool QFUsesResultsByIndexEvaluation::hasResults(QFRawDataRecord *r1, int index) {
    QFRawDataRecord* r=r1;
    QString rsid=getEvaluationResultID(index);
    return r->resultsExistsFromEvaluation(rsid);
}

QString QFUsesResultsByIndexEvaluation::getEvaluationResultID(int index) {
    return QString("%1_%2_i%3").arg(getType()).arg(getID()).arg(index);
}

void QFUsesResultsByIndexEvaluation::setFitResultGroup(QFRawDataRecord *r, int index, const QString &parameterID, const QString &group) {
    setFitResultGroup(r, getEvaluationResultID(index), parameterID, group);
}

void QFUsesResultsByIndexEvaluation::setFitResultLabel(QFRawDataRecord *r, int index, const QString &parameterID, const QString &label, const QString &label_richtext)
{
    setFitResultLabel(r, getEvaluationResultID(index), parameterID, label, label_richtext);
}

void QFUsesResultsByIndexEvaluation::setFitResultEvaluationGroup(QFRawDataRecord *r, int index, const QString &group) {
    setFitResultEvaluationGroup(r, getEvaluationResultID(index), group);
}

void QFUsesResultsByIndexEvaluation::setFitResultEvaluationDescription(QFRawDataRecord *r, int index, const QString &description) {
    setFitResultEvaluationDescription(r, getEvaluationResultID(index), description);
}

void QFUsesResultsByIndexEvaluation::setFitValue(QFRawDataRecord *r, int index, const QString &parameterID, double value, const QString &unit) {
    setFitValue(r, getEvaluationResultID(index), parameterID, value, unit);
}

void QFUsesResultsByIndexEvaluation::setFitResultValue(QFRawDataRecord *r, int index, const QString &parameterID, double value, const QString &unit) {
    setFitResultValue(r, getEvaluationResultID(index), parameterID, value, unit);
}

void QFUsesResultsByIndexEvaluation::setFitResultValueString(QFRawDataRecord *r, int index, const QString &parameterID, const QString &value) {
    setFitResultValueString(r, getEvaluationResultID(index), parameterID, value);
}

void QFUsesResultsByIndexEvaluation::setFitResultValueInt(QFRawDataRecord *r, int index, const QString &parameterID, int64_t value, const QString &unit) {
    setFitResultValueInt(r, getEvaluationResultID(index), parameterID, value, unit);
}

void QFUsesResultsByIndexEvaluation::setFitResultValueBool(QFRawDataRecord *r, int index, const QString &parameterID, bool value) {
    setFitResultValueBool(r, getEvaluationResultID(index), parameterID, value);
}

void QFUsesResultsByIndexEvaluation::setFitResultValue(QFRawDataRecord *r, int index, const QString &parameterID, double value, double error, const QString &unit) {
    setFitResultValue(r, getEvaluationResultID(index), parameterID, value, error, unit);
}

void QFUsesResultsByIndexEvaluation::setFitResultError(QFRawDataRecord *r, int index, const QString &parameterID, double error) {
    setFitResultError(r, getEvaluationResultID(index), parameterID, error);
}

double QFUsesResultsByIndexEvaluation::getFitValue(QFRawDataRecord *r, int index, const QString &parameterID) {
    return getFitValue(r, getEvaluationResultID(index), parameterID);
}

double QFUsesResultsByIndexEvaluation::getFitError(QFRawDataRecord *r, int index, const QString &parameterID) {
    return getFitError(r, getEvaluationResultID(index), parameterID);
}

void QFUsesResultsByIndexEvaluation::setFitError(QFRawDataRecord *r, int index, const QString &parameterID, double error) {
    setFitError(r, getEvaluationResultID(index), parameterID, error);
}

void QFUsesResultsByIndexEvaluation::setFitFix(QFRawDataRecord *r, int index, const QString &parameterID, bool fix) {
    setFitFix(r, getEvaluationResultID(index), parameterID, fix);
}

void QFUsesResultsByIndexEvaluation::setFitResultFix(QFRawDataRecord *r, int index, const QString &parameterID, bool fix) {
    setFitResultFix(r, getEvaluationResultID(index), parameterID, fix);
}

bool QFUsesResultsByIndexEvaluation::getFitFix(QFRawDataRecord *r, int index, const QString &parameterID) {
    return getFitFix(r, getEvaluationResultID(index), parameterID);
}

void QFUsesResultsByIndexEvaluation::intWriteData(QXmlStreamWriter &w) {
    QFUsesResultsEvaluation::intWriteData(w);
}

void QFUsesResultsByIndexEvaluation::intReadData(QDomElement *e) {
    QFUsesResultsEvaluation::intReadData(e);
}
