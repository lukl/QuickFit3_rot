#include "qfpevalimfcsfit.h"
#include <QtGui>
#include "qfimfcsfitevaluation.h"
#include "imfcscalibrationdialog.h"
#include "optionswidget.h"
#include "qfrdrtableinterface.h"
#include "qftools.h"
#include "qfrdrcolumngraphsinterface.h"
#include "qfrdrrunselection.h"
#include "qfevaluationeditor.h"
#include "qfimfcsfitevaluationeditor.h"
#include "qfrdrimfcscorrelatorremote.h"
#include "qfrawdatarecordfactory.h"


QFPEvalIMFCSFit::QFPEvalIMFCSFit(QObject* parent):
    QObject(parent)
{
    //ctor
    QFPluginServices::getInstance()->registerSettingsPane(this);
    menuCalibration=NULL;
    calibrationWizard=NULL;
}

QFPEvalIMFCSFit::~QFPEvalIMFCSFit()
{
    //dtor
    if (calibrationWizard) {
        calibrationWizard->close();
        delete calibrationWizard;
    }
}

QFEvaluationItem* QFPEvalIMFCSFit::createRecord(QFProject* parent) {
    return new QFImFCSFitEvaluation(parent);
}


void QFPEvalIMFCSFit::registerToMenu(QMenu* menu) {
    QAction* actFCS=new QAction(QIcon(":/imfcs_fit.png"), tr("imFCS Curve Fitting"), parentWidget);
    actFCS->setStatusTip(tr("Insert a new imFCS least-squares fit evaluation"));
    connect(actFCS, SIGNAL(triggered()), this, SLOT(insertFCSFit()));
    menu->addAction(actFCS);

    menu->addMenu(menuCalibration);


}

void QFPEvalIMFCSFit::init()
{
    if (services) {
        QMenu* menu=new QMenu(tr("imFCS &Calibration Tool"));
        menu->setIcon(QIcon(":/imfcsfit/imfcs_fitcalib.png"));

        QAction* actHelp=new QAction(QIcon(":/lib/help.png"), tr("Calbration Tutorial"), this);
        connect(actHelp, SIGNAL(triggered()), this, SLOT(showCalibrationTutorial()));
        menu->addAction(actHelp);
        QAction* actWizard=new QAction(QIcon(":/imfcsfit/imfcs_fitcalib.png"), tr("Calbration Wizard"), this);
        connect(actWizard, SIGNAL(triggered()), this, SLOT(insertFCSCalibrationWizard()));
        menu->addAction(actWizard);
        QMenu* menu1=menu->addMenu("steps ...");

        QAction* actFCSCalib=new QAction(QIcon(":/imfcsfit/imfcs_calib_addfiles.png"), tr("&0: add imFCS Calibration Fits"), this);
        actFCSCalib->setStatusTip(tr("Insert a set of imFCS least-squares fit evaluations for a SPIM calibration"));
        connect(actFCSCalib, SIGNAL(triggered()), this, SLOT(insertFCSFitForCalibration()));
        menu1->addAction(actFCSCalib);

        QAction* actImFCSCalibFit=new QAction(tr("&1: Fit D's"), this);
        menu1->addAction(actImFCSCalibFit);
        connect(actImFCSCalibFit, SIGNAL(triggered()), this, SLOT(imFCSCalibrationTool1()));
        QAction* actImFCSCalib=new QAction(tr("&2: Collect D Data"), this);
        menu1->addAction(actImFCSCalib);
        connect(actImFCSCalib, SIGNAL(triggered()), this, SLOT(imFCSCalibrationTool2()));
        QAction* actImFCSCalib1=new QAction(tr("&3: Fit wxy"), this);
        menu1->addAction(actImFCSCalib1);
        connect(actImFCSCalib1, SIGNAL(triggered()), this, SLOT(imFCSCalibrationTool3()));
        QAction* actImFCSCalib2=new QAction(tr("&4: Collect wxy Data"), this);
        menu1->addAction(actImFCSCalib2);
        connect(actImFCSCalib2, SIGNAL(triggered()), this, SLOT(imFCSCalibrationTool4()));

        QMenu* extm=services->getMenu("tools");
        if (extm) {
            //extm->addAction(actImFCSCalib);
            extm->addMenu(menu);
        }
        menuCalibration=menu;
    }
}

QString QFPEvalIMFCSFit::pluginOptionsName() const
{
    return getName();
}

QIcon QFPEvalIMFCSFit::pluginOptionsIcon() const
{
    return QIcon(getIconFilename());
}

QFPluginOptionsWidget *QFPEvalIMFCSFit::createOptionsWidget(QWidget *parent)
{
    return new OptionsWidget(this, parent);
}



void QFPEvalIMFCSFit::insertFCSFit() {
    if (project) {
        project->addEvaluation(getID(), "imFCS Fit");
    }
}

void QFPEvalIMFCSFit::insertFCSCalibrationWizard()
{
    if (!calibrationWizard) {
        calibrationWizard=new ImFCSCalibrationWizard(NULL);
        connect(calibrationWizard, SIGNAL(showTutorial()), this, SLOT(showCalibrationTutorial()));
        connect(calibrationWizard, SIGNAL(run1()), this, SLOT(insertFCSFitForCalibration()));
        connect(calibrationWizard, SIGNAL(run2()), this, SLOT(imFCSCalibrationTool1()));
        connect(calibrationWizard, SIGNAL(run3()), this, SLOT(imFCSCalibrationTool2()));
        connect(calibrationWizard, SIGNAL(run4()), this, SLOT(imFCSCalibrationTool3()));
        connect(calibrationWizard, SIGNAL(run5()), this, SLOT(imFCSCalibrationTool4()));
        connect(calibrationWizard, SIGNAL(loadFile()), this, SLOT(imFCSCalibrationSelectFile()));
        connect(calibrationWizard, SIGNAL(correlate()), this, SLOT(imFCSCalibrationCorrelate()));
    }
    calibrationWizard->show();
    calibrationWizard->activateWindow();
    calibrationWizard->raise();
}

void QFPEvalIMFCSFit::insertFCSFitForCalibration() {
    if (project) {

        ImFCSCalibrationDialog* dlg=new ImFCSCalibrationDialog(NULL);
        QFEvaluationItem* edummy=project->addEvaluation(getID(), "imFCS Fit");
        QFImFCSFitEvaluation* imFCS=qobject_cast<QFImFCSFitEvaluation*>(edummy);
        if (imFCS) dlg->setFitModels(imFCS->getAvailableFitFunctions(), imFCS->getFitFunctionID());
        if (dlg->exec()) {
            QList<double> vals=dlg->getValues();
            for (int i=0; i<vals.size(); i++) {
                QFEvaluationItem* e=edummy;
                if (i>0) e=project->addEvaluation(getID(), "imFCS Fit");
                QFImFCSFitEvaluation* eimFCS=qobject_cast<QFImFCSFitEvaluation*>(e);
                e->setQFProperty("PRESET_FOCUS_WIDTH", vals[i], false, false);
                e->setQFProperty("PRESET_FOCUS_HEIGHT", dlg->getFocusHeight(), false, false);
                e->setQFProperty("PRESET_FOCUS_HEIGHT_ERROR", dlg->getFocusHeightError(), false, false);
                e->setQFProperty("PRESET_FOCUS_WIDTH_FIX", true, false, false);
                e->setQFProperty("PRESET_FOCUS_HEIGHT_FIX", true, false, false);
                e->setQFProperty("PRESET_D1_FIX", false, false, false);
                e->setQFProperty("IMFCS_CALIBRATION_FOCUSWIDTH", vals[i], false, false);
                e->setQFProperty("IMFCS_CALIBRATION_FOCUSHEIGHT", dlg->getFocusHeight(), false, false);
                e->setQFProperty("IMFCS_CALIBRATION_FOCUSHEIGHT_ERROR", dlg->getFocusHeightError(), false, false);
                e->setQFProperty("IMFCS_CALIBRATION_MODEL", dlg->getFitModel(), false, false);
                e->setName(tr("wxy=%1 nm").arg(vals[i]));
                if (eimFCS) eimFCS->setFitFunction(dlg->getFitModel());
            }
            if (vals.size()<=0) delete edummy;
            QFRawDataRecord* et=project->addRawData("table", "imFCS Calibration results");
            et->setQFProperty("IMFCS_CALIBRATION_RESULTTABLE", true, false, false);

         } else {
            delete edummy;
        }
        delete dlg;
    }
}

void QFPEvalIMFCSFit::imFCSCalibrationSelectFile()
{
    QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
    if(p&&p->getRawDataRecordFactory()->contains("imaging_fcs")) {
        QFRDRIMFCSCorrelatorRemote* r=dynamic_cast<QFRDRIMFCSCorrelatorRemote*>(p->getRawDataRecordFactory()->getPlugin("imaging_fcs"));
        r->imfcsCorrRemoteUserSelectFile();
    }
}

void QFPEvalIMFCSFit::imFCSCalibrationCorrelate()
{
    QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
    if(p&&p->getRawDataRecordFactory()->contains("imaging_fcs")) {
        QFRDRIMFCSCorrelatorRemote* r=dynamic_cast<QFRDRIMFCSCorrelatorRemote*>(p->getRawDataRecordFactory()->getPlugin("imaging_fcs"));
        r->imfcsCorrRemoteAddJobSeries("binning", 1, 5, 1);
    }
}

void QFPEvalIMFCSFit::imFCSCalibrationTool1()
{
    if (!project || !services) {
        QMessageBox::critical(parentWidget, tr("imFCS Calibration"), tr("No project loaded!"));
        return;
    }
    log_text(tr("imFCS calibration tool 1: fitting D's ... \n"));
    for (int i=0; i<project->getEvaluationCount(); i++) {
        QFEvaluationItem* e=project->getEvaluationByNum(i);
        QFImFCSFitEvaluation* imFCS=qobject_cast<QFImFCSFitEvaluation*>(e);
        if (imFCS) {
            log_text(tr("   - fitting %1 \n").arg(e->getName()));
            QFEvaluationPropertyEditor* pedt=services->openEvaluationEditor(e);
            QFEvaluationEditor* edt=pedt->getEditor();
            QFImFCSFitEvaluationEditor* eedt=qobject_cast<QFImFCSFitEvaluationEditor*>(edt);
            if (eedt && e->getName().toLower().contains("wxy")) {
                eedt->fitEverythingThreaded();
            }
            if (pedt) pedt->close();
            log_text(tr("        DONE!\n"));
        }
    }
    log_text(tr("imFCS calibration tool 1: fitting D's ... DONE!\n"));
}

void QFPEvalIMFCSFit::imFCSCalibrationTool2()
{
    if (!project)  {
        QMessageBox::critical(parentWidget, tr("imFCS Calibration"), tr("No project loaded!"));
        return;
    }
     log_text(tr("imFCS calibration tool 2: collecting D data ... \n"));

    // find table for resultsQFRDRRunSelectionsInterface
    QFRawDataRecord* etab=NULL;
    QFRDRTableInterface* tab=NULL;
    for (int i=0; i<project->getRawDataCount(); i++) {
        QFRawDataRecord* e=project->getRawDataByNum(i);
        QFRDRTableInterface* eetab=qobject_cast<QFRDRTableInterface*>(e);
        //qDebug()<<i<<e<<eetab;
        if (e && eetab) {
            if (e->getProperty("IMFCS_CALIBRATION_RESULTTABLE", false).toBool()) {
                etab=e;
                tab=eetab;
            }
        }
    }
    if (!etab || !tab) {
        etab=project->addRawData("table", "imFCS Calibration results");
        if (etab) {
            etab->setQFProperty("IMFCS_CALIBRATION_RESULTTABLE", true, false, false);
        }
        tab=qobject_cast<QFRDRTableInterface*>(etab);
        //qDebug()<<"create: "<<etab<<tab;
    }
    if (!etab || !tab) {
        QMessageBox::critical(parentWidget, tr("imFCS Calibration"), tr("Could not create or find proper results table in project!"));
        return;
    }
    QFRDRColumnGraphsInterface* graph=qobject_cast<QFRDRColumnGraphsInterface*>(etab);

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    tab->tableSetColumnTitle(0, tr("pixel size [nm]"));
    int counter=0;
    bool first=true;
    QList<QFRawDataRecord*> rdrs;
    QVector<double> focus_widths, Ds, Ds2, pixel_sizes;
    QString model="";
    double wz=0;
    double ewz=0;
    double focus_height=0;
    double xmin=0;
    double xmax=0;
    double ymin=0;
    double ymax=0;
    QVector<double> pixwidths;
    QList<QVector<double> > Dvals, Derrs;
    for (int i=0; i<project->getEvaluationCount(); i++) {
        QFEvaluationItem* e=project->getEvaluationByNum(i);
        QFImFCSFitEvaluation* imFCS=qobject_cast<QFImFCSFitEvaluation*>(e);
        if (e) {
            double fw=e->getProperty("IMFCS_CALIBRATION_FOCUSWIDTH", -1.0).toDouble();
            QString colName=tr("D%1 [�m�/s]").arg(round(fw));
            QString colNameE=tr("D%1_error [�m�/s]").arg(round(fw));
            if (fw>0) {
                wz=focus_height=e->getProperty("PRESET_FOCUS_HEIGHT", 0).toDouble();
                ewz=e->getProperty("PRESET_FOCUS_HEIGHT_ERROR", 0).toDouble();
                model=e->getProperty("IMFCS_CALIBRATION_MODEL", "").toString();
                focus_widths<<fw;
                if (first) {
                    // build list of fit RDRs
                    int rcounter=0;
                    for (int ri=0; ri<project->getRawDataCount(); ri++) {
                        QFRawDataRecord* r=project->getRawDataByNum(ri);
                        if (imFCS->isFilteredAndApplicable(r)) {
                            rdrs<<r;
                            double bin=r->getProperty("BINNING", 1.0).toDouble();
                            double width=r->getProperty("PIXEL_WIDTH", 0).toDouble();


                            //tab->tableSetData(rcounter, 0, imFCS->getFitValue(r, imFCS->getEvaluationResultID(-1), "pixel_size"));
                            //tab->tableSetData(rcounter, 0, bin*width);
                            pixwidths<<bin*width;
                            if (bin*width>xmax) xmax=1.1*bin*width;

                            rcounter++;
                            Ds<<0;
                            Ds2<<0;
                            pixel_sizes<<bin*width;
                        }
                    }
                    tab->tableSetColumnDataAsDouble(0, pixwidths);

                }
                first=false;

                Dvals.append(QVector<double>());
                Derrs.append(QVector<double>());

                for (int ri=0; ri<rdrs.size(); ri++) {
                    QFRawDataRecord* r=rdrs[ri];
                    QString erid=imFCS->getEvaluationResultID(0, r);
                    QString en=imFCS->transformResultID(erid);
                    //qDebug()<<r->getName()<<":  "<<en<<"    ["<<erid<<"]"<<r->resultsExistsFromEvaluation(en)<<r->resultsExists(en, "diff_coeff1")<<r->resultsCalcNames(en)<<r->resultsCalcNames(erid);
                    QVector<double> D=r->resultsGetAsDoubleList(en,  "fitparam_diff_coeff1");

                    QFRDRRunSelectionsInterface* runsel=qobject_cast<QFRDRRunSelectionsInterface*>(r);
                    if (runsel) {
                        QVector<double> DD;
                        for (int di=D.size()-1; di>=0; di--) {
                            if (!runsel->leaveoutRun(di)) DD<<D[di];
                        }
                        D=DD;
                    }
                    qSort(D);
                    double dmin=qfstatisticsSortedMin(D);
                    double dmax=qfstatisticsSortedMax(D);

                    if (dmax>ymax) ymax=dmax*1.1;
                    if (dmin<ymin) ymin=dmin*1.1;

                    double Dvar=0;
                    double Dmean=qfstatisticsAverageVariance(Dvar, D);
                    //tab->tableSetData(ri, 1+counter*2, Dmean);
                    qDebug()<<"Dvals["<<counter<<"/"<<Dvals.size()<<"]="<<Dmean;
                    Dvals[counter]<<Dmean;
                    Derrs[counter]<<sqrt(Dvar);
                    //tab->tableSetData(ri, 1+counter*2+1, sqrt(Dvar));
                    Ds[ri]=Ds[ri]+Dmean;
                    Ds2[ri]=Ds2[ri]+Dmean*Dmean;
                }

                tab->tableSetColumnDataAsDouble(1+counter*2, Dvals[counter]);
                tab->tableSetColumnDataAsDouble(1+counter*2+1, Derrs[counter]);
                tab->tableSetColumnTitle(1+counter*2, colName);
                tab->tableSetColumnTitle(1+counter*2+1, colNameE);

                counter++;
            }
        }
    }

    QStringList DsNames;
    for (int ri=0; ri<Ds.size(); ri++) {
        if (counter>1) Ds2[ri]=sqrt((Ds2[ri]-Ds[ri]*Ds[ri]/double(counter))/double(counter-1));
        else Ds2[ri]=0;
        Ds[ri]=Ds[ri]/double(counter);
        DsNames<<tr("a=%1nm:  D= (%2 +/- %3) �m�/s").arg(pixel_sizes[ri]).arg(Ds[ri]).arg(Ds2[ri]);
    }
    if (calibrationWizard)  {
        calibrationWizard->getPltD()->set_doDrawing(false);
        JKQTPdatastore* ds=calibrationWizard->getPltD()->getDatastore();
        ds->clear();
        calibrationWizard->getPltD()->clearGraphs();
        calibrationWizard->getPltD()->get_plotter()->set_plotLabel(tr("pixel size vs. diffusion coefficient"));
        calibrationWizard->getPltD()->get_plotter()->getXAxis()->set_axisLabel(tr("pixel size [nm]"));
        calibrationWizard->getPltD()->get_plotter()->getYAxis()->set_axisLabel(tr("diffusion coefficient [�m�/s]"));
        if (pixwidths.size()>0 && counter>0) {
            size_t c_ps=ds->addCopiedColumn(pixwidths.data(), pixwidths.size(), tr("pixel widths"));
            size_t c_Davg=ds->addCopiedColumn(Ds.data(), Ds.size(), tr("avg(D) [�m�/s]"));
            size_t c_Dsd=ds->addCopiedColumn(Ds2.data(), Ds2.size(), tr("std(D) [�m�/s]"));
            for (int i=0; i<counter; i++) {
                size_t c_D=ds->addCopiedColumn(Dvals[i].data(), Dvals[i].size(), tr("D(w_{xy}=%1nm)").arg(focus_widths[i]));
                size_t c_ED=ds->addCopiedColumn(Derrs[i].data(), Derrs[i].size(), tr("error: D(w_{xy}=%1nm)").arg(focus_widths[i]));
                JKQTPxyLineErrorGraph* plt=new JKQTPxyLineErrorGraph(calibrationWizard->getPltD()->get_plotter());
                plt->set_title( tr("w_{xy}=%1nm").arg(focus_widths[i]));
                plt->set_xColumn(c_ps);
                plt->set_yColumn(c_D);
                plt->set_yErrorColumn(c_ED);
                plt->set_xErrorStyle(JKQTPnoError);
                plt->set_yErrorStyle(JKQTPnoError);
                calibrationWizard->getPltD()->get_plotter()->addGraph(plt);
            }
        }

        calibrationWizard->getPltD()->set_doDrawing(true);
        calibrationWizard->getPltD()->zoomToFit();
        calibrationWizard->raise();
    }

    int gr=-1;
    if (graph) {
        /*int i=0;
        while (graph->colgraphGetGraphCount()<1 && i<1) {
            graph->colgraphAddGraph("title");
            i++;
        }*/
        graph->colgraphAddGraph("title");
        gr=graph->colgraphGetGraphCount()-1;

        graph->colgraphSetGraphTitle(gr, tr("pixel size vs. diffusion coefficient"));
        graph->colgraphSetGraphXAxisProps(gr, tr("pixel size [nm]"));
        graph->colgraphSetGraphYAxisProps(gr, tr("diffusion coefficient [�m�/s]"));
        while (graph->colgraphGetPlotCount(gr)>0){
            graph->colgraphRemovePlot(gr, 0);
        }
        for (int i=0; i<focus_widths.size(); i++) {
            graph->colgraphAddPlot(gr, 0, 1+i*2, QFRDRColumnGraphsInterface::cgtLinesPoints, tr("w_{xy}=%1nm").arg(focus_widths[i]));
        }
        graph->colgraphsetXRange(gr, xmin, xmax);
        graph->colgraphsetYRange(gr, ymin, ymax);


    }

    QFRawDataPropertyEditor* editor=QFPluginServices::getInstance()->openRawDataEditor(etab, false);
    if (editor) {
        if (gr>=0) editor->sendEditorCommand("showPlot", gr);
        editor->showTab(2);
        editor->raise();
    }
    if (calibrationWizard) calibrationWizard->raise();

    QApplication::restoreOverrideCursor();



    bool dlgOK=false;
    int DItem=DsNames.indexOf(QInputDialog::getItem(calibrationWizard, tr("imFCS Calibration"), tr("Please choose the binning at which to measure the \"true\" diffusion coefficient for the calibration:"), DsNames, 4, false, &dlgOK));

    if (dlgOK) {

        double Dcalib=Ds.value(DItem, 0);
        double DcalibE=Ds2.value(DItem, 0);
        QFEvaluationItem* e=project->addEvaluation(getID(), tr("calibration D=%1�m�/s").arg(Dcalib));
        e->setQFProperty("PRESET_D1", Dcalib, false, false);
        e->setQFProperty("PRESET_D1_ERROR", DcalibE, false, false);
        e->setQFProperty("PRESET_D1_FIX", true, false, false);
        e->setQFProperty("PRESET_FOCUS_WIDTH_FIX", false, false, false);
        e->setQFProperty("PRESET_FOCUS_HEIGHT", focus_height, false, false);
        e->setQFProperty("PRESET_FOCUS_HEIGHT_ERROR", ewz, false, false);
        e->setQFProperty("IMFCS_CALIBRATION_D", Dcalib, false, false);
        e->setQFProperty("IMFCS_CALIBRATION_D_ERROR", DcalibE, false, false);
        e->setQFProperty("IMFCS_CALIBRATION_FITWXY", true, false, false);
        e->setName(tr("calibration D=%1�m�/s").arg(Dcalib));
        QFImFCSFitEvaluation* eimFCS=qobject_cast<QFImFCSFitEvaluation*>(e);
        if (eimFCS && (!model.isEmpty())) eimFCS->setFitFunction(model);
    }

    log_text(tr("imFCS calibration tool 2: collecting D data ... DONE!\n"));
}

void QFPEvalIMFCSFit::imFCSCalibrationTool3()
{
    if (!project || !services) {
        QMessageBox::critical(parentWidget, tr("imFCS Calibration"), tr("No project loaded!"));
        return;
    }
    log_text(tr("imFCS calibration tool 3: fitting wxy ... \n"));
    for (int i=0; i<project->getEvaluationCount(); i++) {
        QFEvaluationItem* e=project->getEvaluationByNum(i);
        QFImFCSFitEvaluation* imFCS=qobject_cast<QFImFCSFitEvaluation*>(e);
        if (imFCS && e->getProperty("IMFCS_CALIBRATION_FITWXY", false).toBool()) {
            log_text(tr("   - fitting %1 \n").arg(e->getName()));
            QFEvaluationPropertyEditor* pedt=services->openEvaluationEditor(e);
            QFEvaluationEditor* edt=pedt->getEditor();
            QFImFCSFitEvaluationEditor* eedt=qobject_cast<QFImFCSFitEvaluationEditor*>(edt);
            if (eedt) {
                eedt->fitEverythingThreaded();
            }
            if (pedt) pedt->close();
            log_text(tr("        DONE!\n"));
        }
    }
    log_text(tr("imFCS calibration tool 3: fitting wxy's ... DONE!\n"));
}

void QFPEvalIMFCSFit::imFCSCalibrationTool4()
{
    if (!project)  {
        QMessageBox::critical(parentWidget, tr("imFCS Calibration"), tr("No project loaded!"));
        return;
    }
    log_text(tr("imFCS calibration tool 4: collecting wxy data ... \n"));

    // find table for resultsQFRDRRunSelectionsInterface
    QFRawDataRecord* etab=NULL;
    QFRDRTableInterface* tab=NULL;
    for (int i=0; i<project->getRawDataCount(); i++) {
        QFRawDataRecord* e=project->getRawDataByNum(i);
        QFRDRTableInterface* eetab=qobject_cast<QFRDRTableInterface*>(e);
        //qDebug()<<i<<e<<eetab;
        if (e && eetab) {
            if (e->getProperty("IMFCS_CALIBRATION_RESULTTABLE", false).toBool()) {
                etab=e;
                tab=eetab;
            }
        }
    }
    if (!etab || !tab) {
        etab=project->addRawData("table", "imFCS Calibration results");
        if (etab) {
            etab->setQFProperty("IMFCS_CALIBRATION_RESULTTABLE", true, false, false);
        }
        tab=qobject_cast<QFRDRTableInterface*>(etab);
        //qDebug()<<"create: "<<etab<<tab;
    }
    if (!etab || !tab) {
        QMessageBox::critical(parentWidget, tr("imFCS Calibration"), tr("Could not create or find proper results table in project!"));
        return;
    }
    QFRDRColumnGraphsInterface* graph=qobject_cast<QFRDRColumnGraphsInterface*>(etab);
    int gr=-1;

    QList<QFRawDataRecord*> rdrs;
    double xmin=0;
    double xmax=0;
    double ymin=0;
    double ymax=0;
    double Dcalib=0;
    double DcalibE=0;
    double wxymean=0, wxyvar=0;
    for (int i=0; i<project->getEvaluationCount(); i++) {
        QFEvaluationItem* e=project->getEvaluationByNum(i);
        QFImFCSFitEvaluation* imFCS=qobject_cast<QFImFCSFitEvaluation*>(e);
        if (e) {
            bool fitwxy=e->getProperty("IMFCS_CALIBRATION_FITWXY", false).toBool();
            Dcalib=e->getProperty("IMFCS_CALIBRATION_D", Dcalib).toDouble();
            DcalibE=e->getProperty("IMFCS_CALIBRATION_D_ERROR", DcalibE).toDouble();
            QString colName=tr("wxy [nm]");
            QString colNameE=tr("wxy_error [nm]");
            if (fitwxy) {
                int cols=e->getProperty("IMFCS_CALIBRATION_COLS", tab->tableGetColumnCount()).toInt();
                e->setQFProperty("IMFCS_CALIBRATION_COLS", cols);
                //qDebug()<<"cols="<<cols;
                int rcounter=0;
                tab->tableSetColumnTitle(cols, tr("pixel size [nm]"));
                tab->tableSetColumnTitle(cols+1, colName);
                tab->tableSetColumnTitle(cols+2, colNameE);
                QVector<double> wxyvec;
                for (int ri=0; ri<project->getRawDataCount(); ri++) {
                    QFRawDataRecord* r=project->getRawDataByNum(ri);
                    if (imFCS->isFilteredAndApplicable(r)) {
                        rdrs<<r;
                        double bin=r->getProperty("BINNING", 1.0).toDouble();
                        double width=r->getProperty("PIXEL_WIDTH", 0).toDouble();

                        tab->tableSetData(rcounter, cols, bin*width);
                        if (bin*width>xmax) xmax=1.1*bin*width;


                        QString erid=imFCS->getEvaluationResultID(0, r);
                        QString en=imFCS->transformResultID(erid);
                        QVector<double> wxy=r->resultsGetAsDoubleList(en,  "fitparam_focus_width");
                        //if (r->resultsExists(en,  "fitparam_diff_coeff1"))  Dcalib=r->resultsGetAsDouble(en,  "fitparam_diff_coeff1");

                        QFRDRRunSelectionsInterface* runsel=qobject_cast<QFRDRRunSelectionsInterface*>(r);
                        if (runsel) {
                            QVector<double> DD;
                            for (int di=wxy.size()-1; di>=0; di--) {
                                if (!runsel->leaveoutRun(di)) DD<<wxy[di];
                            }
                            wxy=DD;
                        }
                        /*qSort(wxy);
                        double dmin=qfstatisticsSortedMin(wxy);
                        double dmax=qfstatisticsSortedMax(wxy);*/


                        wxyvar=0;
                        wxymean=qfstatisticsAverageVariance(wxyvar, wxy);
                        tab->tableSetData(rcounter, cols+1, wxymean);
                        tab->tableSetData(rcounter, cols+2, sqrt(wxyvar));
                        wxyvec<<wxymean;
                        //qDebug()<<rcounter<<cols+1<<wxymean<<sqrt(wxyvar);

                        if (wxymean+sqrt(wxyvar)>ymax) ymax=(wxymean+sqrt(wxyvar))*1.1;
                        if (wxymean-sqrt(wxyvar)<ymin) ymin=(wxymean-sqrt(wxyvar));

                        rcounter++;
                    }
                }
                if (rcounter>0) {
                    tab->tableSetData(0, cols+3, qfstatisticsAverage(wxyvec));
                    tab->tableSetData(0, cols+4, sqrt(qfstatisticsVariance(wxyvec)));
                    tab->tableSetData(rcounter-1, cols+3, qfstatisticsAverage(wxyvec));
                    tab->tableSetData(rcounter-1, cols+4, sqrt(qfstatisticsVariance(wxyvec)));
                    if (graph) {
                        int ggraph=e->getProperty("IMFCS_CALIBRATION_GRAPHS", -1).toInt();
                        if (ggraph<0) {
                            graph->colgraphAddGraph(tr("pixel size vs. lat. focus size, D=%2�m�/s").arg(Dcalib));
                            ggraph=graph->colgraphGetGraphCount()-1;
                        } else {
                            graph->colgraphSetGraphTitle(ggraph, tr("pixel size vs. lat. focus size, D=%2�m�/s").arg(Dcalib));
                        }
                        e->setQFProperty("IMFCS_CALIBRATION_GRAPHS", ggraph);
                        graph->colgraphSetGraphTitle(ggraph, tr("pixel size vs. lat. focus size"));
                        graph->colgraphSetGraphXAxisProps(ggraph, tr("pixel size [nm]"));
                        graph->colgraphSetGraphYAxisProps(ggraph, tr("lateral focus size w_{xy} [nm]"));
                        graph->colgraphAddErrorPlot(ggraph, cols, -1, cols+1, cols+2, QFRDRColumnGraphsInterface::cgtLinesPoints, tr("calibration D=(%1\\pm %2)�m�/s").arg(Dcalib).arg(DcalibE));
                        graph->colgraphSetPlotColor(ggraph, graph->colgraphGetPlotCount(ggraph)-1, QColor("red"));
                        graph->colgraphAddPlot(ggraph, cols, cols+3, QFRDRColumnGraphsInterface::cgtLines, tr("w_{xy}=(%1\\pm %2)nm").arg(qfstatisticsAverage(wxyvec)).arg(sqrt(qfstatisticsVariance(wxyvec))));
                        graph->colgraphSetPlotColor(ggraph, graph->colgraphGetPlotCount(ggraph)-1, QColor("blue"));
                        graph->colgraphsetXRange(ggraph, xmin, xmax);
                        graph->colgraphsetYRange(ggraph, ymin, ymax);
                        gr=ggraph;

                    }
                }
            }
        }


    }

    QFRawDataPropertyEditor* editor=QFPluginServices::getInstance()->openRawDataEditor(etab, false);
    if (editor) {
        if (gr>=0) editor->sendEditorCommand("showPlot", gr);
        editor->showTab(2);
        editor->raise();
    }


    log_text(tr("imFCS calibration tool 4: collecting wxy data ... DONE!\n"));

}

void QFPEvalIMFCSFit::showCalibrationTutorial()
{
    if (services) {
        services->displayHelpWindow(services->getPluginHelpDirectory(getID())+"/calibration.html");
    }
}


Q_EXPORT_PLUGIN2(imfcsfit, QFPEvalIMFCSFit)
