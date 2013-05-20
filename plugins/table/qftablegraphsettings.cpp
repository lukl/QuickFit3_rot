#include "qftablegraphsettings.h"
#include "ui_qftablegraphsettings.h"
#include "programoptions.h"
#include "qfrdrtable.h"
#include "qftools.h"
#include "qfdoubleedit.h"
#include "qfmathparserxfunctionlinegraph.h"

QFTableGraphSettings::QFTableGraphSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QFTableGraphSettings)
{
    functionRef=new QFFunctionReferenceTool(NULL);
    functionRef->setCompleterFile(ProgramOptions::getInstance()->getConfigFileDirectory()+"/completers/table/table_expression.txt");
    functionRef->setDefaultWordsMathExpression();


    updating=true;

    ui->setupUi(this);
    functionRef->registerEditor(ui->edtFunction);
    ui->edtImageHeight->setCheckBounds(true, false);
    ui->edtImageHeight->setRange(0, 1e10);
    ui->edtImageMax->setCheckBounds(false, false);
    ui->edtImageMin->setCheckBounds(false, false);
    ui->edtImageWidth->setCheckBounds(true, false);
    ui->edtImageWidth->setRange(0, 1e10);
    ui->edtImageX->setCheckBounds(true, false);
    ui->edtImageY->setCheckBounds(true, false);
    current=NULL;
    this->plot=-1;
    updating=false;
    if (ProgramOptions::getInstance() && ProgramOptions::getInstance()->getQSettings()) readSettings(*(ProgramOptions::getInstance()->getQSettings()), "table/QFRDRTablePlotWidget/");
}

QFTableGraphSettings::~QFTableGraphSettings()
{
    updating=true;
    if (ProgramOptions::getInstance() && ProgramOptions::getInstance()->getQSettings()) writeSettings(*(ProgramOptions::getInstance()->getQSettings()), "table/QFRDRTablePlotWidget/");
    delete ui;
}

void QFTableGraphSettings::setRecord(QFRDRTable *record, int plot)
{
    current=record;
    this->plot=plot;
    updating=true;
    disconnectWidgets();

    reloadColumns(ui->cmbLinesXData);
    reloadColumns(ui->cmbLinesXError);
    reloadColumns(ui->cmbLinesYData);
    reloadColumns(ui->cmbLinesYError);
    updating=false;
    connectWidgets();

    //updateGraph();
}

void QFTableGraphSettings::rawDataChanged()
{
    if (updating) return;
    disconnectWidgets();
    reloadColumns(ui->cmbLinesXData);
    reloadColumns(ui->cmbLinesXError);
    reloadColumns(ui->cmbLinesYData);
    reloadColumns(ui->cmbLinesYError);
    connectWidgets();
    //updateGraph();
}

void QFTableGraphSettings::readSettings(QSettings &settings, const QString &prefix)
{
}

void QFTableGraphSettings::writeSettings(QSettings &settings, const QString &prefix)
{
}

QIcon QFTableGraphSettings::getGraphIcon(int i) const
{
    return ui->cmbGraphType->itemIcon(i);
}



void QFTableGraphSettings::writeGraphData()
{
    emit graphDataChanged();
}

void QFTableGraphSettings::reloadColumns(QComboBox *combo)
{
    bool updt=updating;
    updating=true;
    int idx=combo->currentIndex();
    //disconnect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));

    if (current) {
        QFTablePluginModel* tab=current->model();
        while (combo->count()-1>tab->columnCount() && combo->count()>1) {
            combo->removeItem(combo->count()-1);
        }
        while (combo->count()-1<tab->columnCount() && tab->columnCount()>0) {
            combo->addItem("");
        }
        combo->setItemText(0, tr("--- none ---"));
        combo->setItemData(0, QVariant());
        for (int i=0; i<tab->columnCount(); i++) {
            combo->setItemText(i+1, tr("#%1: %2").arg(i+1).arg(tab->columnTitle(i)));
            combo->setItemData(i+1, tab->columnTitle(i));
        }
    } else {
        combo->clear();
        combo->addItem(tr("--- none ---"));
    }
    updating=updt;
    //connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    combo->setCurrentIndex(idx);
}

void QFTableGraphSettings::writeGraphData(QFRDRTable::GraphInfo& graph)
{
    //qDebug()<<"QFTableGraphSettings::graphDataChanged    updating="<<updating;
    if (updating) return;
    if (current) {
        updating=true;
        if (this->plot<0 || this->plot>=current->getPlotCount()) return;

        graph.title=ui->edtGraphTitle->text();

        switch(ui->cmbGraphType->currentIndex()) {
            case 1: graph.type=QFRDRTable::gtImpulsesVertical; break;
            case 2: graph.type=QFRDRTable::gtImpulsesHorizontal; break;
            case 3: graph.type=QFRDRTable::gtFilledCurveX; break;
            case 4: graph.type=QFRDRTable::gtFilledCurveY; break;
            case 5: graph.type=QFRDRTable::gtStepsVertical; break;
            case 6: graph.type=QFRDRTable::gtStepsHorizontal; break;
            case 7: graph.type=QFRDRTable::gtBarsVertical; break;
            case 8: graph.type=QFRDRTable::gtBarsHorizontal; break;
            case 9: graph.type=QFRDRTable::gtImage; break;
            case 10: graph.type=QFRDRTable::gtRGBImage; break;
            case 11: graph.type=QFRDRTable::gtMaskImage; break;
            case 12: graph.type=QFRDRTable::gtFunction; break;
            case 0:
            default: graph.type=QFRDRTable::gtLines; break;
        }
        updatePlotWidgetVisibility(graph);



        graph.xcolumn=qMax(-1, ui->cmbLinesXData->currentIndex()-1);
        graph.xerrorcolumn=qMax(-1, ui->cmbLinesXError->currentIndex()-1);
        graph.ycolumn=qMax(-1, ui->cmbLinesYData->currentIndex()-1);
        graph.yerrorcolumn=qMax(-1, ui->cmbLinesYError->currentIndex()-1);

        QColor oldColor=graph.color;
        QColor oldDefaultErrorColor=oldColor.darker();
        QColor oldDefaultFillColor=oldColor.lighter();
        graph.color=ui->cmbLineColor->currentColor();
        if (graph.errorColor!=oldDefaultErrorColor) graph.errorColor=ui->cmbErrorColor->currentColor();
        else {
            graph.errorColor=graph.color.darker();
            ui->cmbErrorColor->setCurrentColor(graph.errorColor);
        }
        //qDebug()<<graph.fillColor.name()<<oldDefaultFillColor.name();

        if (graph.fillColor!=oldDefaultFillColor) graph.fillColor=ui->cmbFillColor->currentColor();
        else {
            graph.fillColor=graph.color.lighter();
            ui->cmbFillColor->setCurrentColor(graph.fillColor);
        }
        graph.errorStyle=ui->cmbErrorStyle->getErrorStyle();
        graph.symbol=ui->cmbSymbol->getSymbol();
        graph.style=ui->cmbLineStyle->currentLineStyle();
        graph.linewidth=ui->spinLineWidth->value();
        graph.symbolSize=ui->spinSymbolSize->value();
        graph.drawLine=ui->chkDrawLine->isChecked();
        graph.colorTransparent=double(ui->sliderPlotTransparent->value())/255.0;
        graph.errorColorTransparent=double(ui->sliderErrorTransparent->value())/255.0;
        graph.fillColorTransparent=double(ui->sliderFillTransparent->value())/255.0;
        graph.imageFalseTransparent=double(ui->sliderImageFalseColor->value())/255.0;
        graph.imageTrueTransparent=double(ui->sliderImageTrueColor->value())/255.0;
        graph.imageFalseColor=ui->cmbImageFalseColor->currentColor();
        graph.imageTrueColor=ui->cmbImageTrueColor->currentColor();
        graph.imageHeight=ui->edtImageHeight->value();
        graph.imageWidth=ui->edtImageWidth->value();
        graph.imageMax=ui->edtImageMax->value();
        graph.imageMin=ui->edtImageMin->value();
        graph.imageX=ui->edtImageX->value();
        graph.imageY=ui->edtImageY->value();
        graph.imagePixelWidth=ui->spinImageWidth->value();
        graph.imagePalette=ui->cmbColormap->colorPalette();
        graph.imageAutoRange=ui->chkImageAutorange->isChecked();
        graph.imageColorbarRight=ui->chkImageColorbarRight->isChecked();
        graph.imageColorbarTop=ui->chkImageColorbarTop->isChecked();
        graph.imageLegend=ui->edtColorbarLabel->text();
        graph.imageLegendMod=ui->edtColorbarLabelMod->text();
        graph.colorbarWidth=ui->spinColorbarWidth->value();
        graph.colorbarRelativeHeight=ui->spinColorbarHeight->value()/100.0;
        graph.function=ui->edtFunction->text();
        graph.isStrided=ui->chkSTrided->isChecked();
        graph.stride=ui->spinStride->value();
        graph.strideStart=ui->spinStrideStart->value();
        graph.modifierMode=(JKQTPMathImage::ModifierMode)ui->cmbModifierMode->currentIndex();

        updating=false;
    }
}

void QFTableGraphSettings::loadGraphData(const QFRDRTable::GraphInfo &graph)
{
    updating=true;
    ui->edtGraphTitle->setText(graph.title);
    ui->cmbLinesXData->setCurrentIndex(graph.xcolumn+1);
    ui->cmbLinesXError->setCurrentIndex(graph.xerrorcolumn+1);
    ui->cmbLinesYData->setCurrentIndex(graph.ycolumn+1);
    ui->cmbLinesYError->setCurrentIndex(graph.yerrorcolumn+1);
    switch(graph.type) {
        case QFRDRTable::gtImpulsesVertical:
            ui->cmbGraphType->setCurrentIndex(1);
            break;
        case QFRDRTable::gtImpulsesHorizontal:
            ui->cmbGraphType->setCurrentIndex(2);
            break;
        case QFRDRTable::gtFilledCurveX:
            ui->cmbGraphType->setCurrentIndex(3);
            break;
        case QFRDRTable::gtFilledCurveY:
            ui->cmbGraphType->setCurrentIndex(4);
            break;
        case QFRDRTable::gtStepsVertical:
            ui->cmbGraphType->setCurrentIndex(5);
            break;
        case QFRDRTable::gtStepsHorizontal:
            ui->cmbGraphType->setCurrentIndex(6);
            break;
        case QFRDRTable::gtBarsVertical:
            ui->cmbGraphType->setCurrentIndex(7);
            break;
        case QFRDRTable::gtBarsHorizontal:
            ui->cmbGraphType->setCurrentIndex(8);
            break;
        case QFRDRTable::gtImage:
            ui->cmbGraphType->setCurrentIndex(9);
            break;
        case QFRDRTable::gtRGBImage:
            ui->cmbGraphType->setCurrentIndex(10);
            break;
        case QFRDRTable::gtMaskImage:
            ui->cmbGraphType->setCurrentIndex(11);
            break;
        case QFRDRTable::gtFunction:
            ui->cmbGraphType->setCurrentIndex(12);
            break;
        case QFRDRTable::gtLines:
        default:
            ui->cmbGraphType->setCurrentIndex(0);
            break;
    }
    ui->cmbErrorColor->setCurrentColor(graph.errorColor);
    ui->cmbLineColor->setCurrentColor(graph.color);
    ui->cmbFillColor->setCurrentColor(graph.fillColor);
    ui->cmbErrorStyle->setSymbol(graph.errorStyle);
    ui->cmbSymbol->setSymbol(graph.symbol);
    ui->cmbLineStyle->setCurrentLineStyle(graph.style);
    ui->spinLineWidth->setValue(graph.linewidth);
    ui->spinSymbolSize->setValue(graph.symbolSize);
    ui->chkDrawLine->setChecked(graph.drawLine);
    ui->sliderPlotTransparent->setValue(graph.colorTransparent*255.0);
    ui->sliderErrorTransparent->setValue(graph.errorColorTransparent*255.0);
    ui->sliderFillTransparent->setValue(graph.fillColorTransparent*255.0);
    ui->cmbImageFalseColor->setCurrentColor(graph.imageFalseColor);
    ui->sliderImageFalseColor->setValue(graph.imageFalseTransparent*255.0);
    ui->cmbImageTrueColor->setCurrentColor(graph.imageTrueColor);
    ui->sliderImageTrueColor->setValue(graph.imageTrueTransparent*255.0);
    ui->edtImageHeight->setValue(graph.imageHeight);
    ui->edtImageWidth->setValue(graph.imageWidth);
    ui->edtImageMax->setValue(graph.imageMax);
    ui->edtImageMin->setValue(graph.imageMin);
    ui->edtImageX->setValue(graph.imageX);
    ui->edtImageY->setValue(graph.imageY);
    ui->spinImageWidth->setValue(graph.imagePixelWidth);
    ui->cmbColormap->setColorPalette(graph.imagePalette);
    ui->chkImageAutorange->setChecked(graph.imageAutoRange);


    ui->chkImageColorbarRight->setChecked(graph.imageColorbarRight);
    ui->chkImageColorbarTop->setChecked(graph.imageColorbarTop);
    ui->edtColorbarLabel->setText(graph.imageLegend);
    ui->edtColorbarLabelMod->setText(graph.imageLegendMod);
    ui->spinColorbarWidth->setValue(graph.colorbarWidth);
    ui->spinColorbarHeight->setValue(graph.colorbarRelativeHeight*100.0);
    ui->edtFunction->setText(graph.function);
    ui->chkSTrided->setChecked(graph.isStrided);
    ui->spinStride->setValue(graph.stride);
    ui->spinStrideStart->setValue(graph.strideStart);
    ui->cmbModifierMode->setCurrentIndex(graph.modifierMode);
    updating=false;
}

void QFTableGraphSettings::updatePlotWidgetVisibility(const QFRDRTable::GraphInfo& graph)
{
    if (current) {
        if (this->plot<0 || this->plot>=current->getPlotCount()) return;



        /*ui->cmbErrorColor->setVisible(true);
        ui->cmbErrorStyle->setVisible(true);
        ui->cmbLinesXError->setVisible(true);
        ui->cmbLinesYError->setVisible(true);
        ui->chkDrawLine->setVisible(true);
        ui->cmbSymbol->setVisible(true);
        ui->spinSymbolSize->setVisible(true);
        ui->cmbFillColor->setVisible(true);
        ui->cmbLineStyle->setVisible(true);
        ui->sliderErrorTransparent->setVisible(true);
        ui->sliderFillTransparent->setVisible(true);
        ui->sliderPlotTransparent->setVisible(true);*/
        ui->widErrorStyle->setVisible(true);
        ui->widFillColor->setVisible(true);
        this->setVisible(true);
        ui->widImage->setVisible(true);
        ui->widLineStyle->setVisible(true);
        ui->widSymbol->setVisible(true);
        ui->labErrorStyle->setVisible(true);
        ui->labDataX->setVisible(true);
        ui->labDataY->setVisible(true);
        ui->labErrorX->setVisible(true);
        ui->labErrorY->setVisible(true);
        ui->labFillColor->setVisible(true);
        ui->labImage->setVisible(true);
        ui->labLinestyle->setVisible(true);
        ui->labSymbol->setVisible(true);
        ui->labTitle->setVisible(true);
        ui->labType->setVisible(true);
        ui->chkDrawLine->setVisible(true);
        ui->cmbLineStyle->setVisible(true);
        ui->cmbLinesXData->setVisible(true);
        ui->cmbLinesXError->setVisible(true);
        ui->cmbLinesYData->setVisible(true);
        ui->cmbLinesYError->setVisible(true);
        ui->widFunction->setVisible(false);
        ui->labFuction->setVisible(false);
        ui->chkSTrided->setVisible(true);
        ui->widStride->setVisible(true);

        ui->labDataX->setText(tr("X Data Column:"));
        ui->labDataY->setText(tr("Y Data Column:"));
        ui->labErrorX->setText(tr("X Error Column:"));
        ui->labErrorY->setText(tr("Y Error Column:"));

        switch(ui->cmbGraphType->currentIndex()) {


            case 1:
                //graph.type=QFRDRTable::gtImpulsesVertical;
                ui->cmbLinesXError->setVisible(false);
                ui->labErrorX->setVisible(false);
                ui->chkDrawLine->setVisible(false);
                ui->cmbLineStyle->setVisible(false);
                ui->labImage->setVisible(false);
                ui->widImage->setVisible(false);

                /*ui->widLineStyle->setVisible(false);
                ui->cmbLinesXError->setVisible(false);
                ui->chkDrawLine->setVisible(false);
                ui->cmbLineStyle->setVisible(false);*/
                break;
            case 2:
                //graph.type=QFRDRTable::gtImpulsesHorizontal;
                ui->cmbLinesXError->setVisible(false);
                ui->labErrorX->setVisible(false);
                ui->chkDrawLine->setVisible(false);
                ui->cmbLineStyle->setVisible(false);
                ui->labImage->setVisible(false);
                ui->widImage->setVisible(false);

                /*ui->cmbErrorColor->setVisible(true);
                ui->cmbErrorStyle->setVisible(true);
                ui->cmbLinesXError->setVisible(false);
                ui->cmbLinesYError->setVisible(true);
                ui->chkDrawLine->setVisible(false);
                ui->cmbSymbol->setVisible(true);
                ui->spinSymbolSize->setVisible(true);
                ui->cmbFillColor->setVisible(true);
                ui->cmbLineStyle->setVisible(false);                */

                break;
            case 3:
                //graph.type=QFRDRTable::gtFilledCurveX;
                ui->cmbLinesXError->setVisible(false);
                ui->labErrorX->setVisible(false);
                ui->labSymbol->setVisible(false);
                ui->widSymbol->setVisible(false);
                ui->labImage->setVisible(false);
                ui->widImage->setVisible(false);

                /*ui->cmbErrorColor->setVisible(true);
                ui->cmbErrorStyle->setVisible(true);
                ui->cmbLinesXError->setVisible(false);
                ui->cmbLinesYError->setVisible(true);
                ui->chkDrawLine->setVisible(false);
                ui->cmbSymbol->setVisible(false);
                ui->spinSymbolSize->setVisible(false);
                ui->cmbFillColor->setVisible(true);
                ui->cmbLineStyle->setVisible(true);*/
                break;
            case 4:
                //graph.type=QFRDRTable::gtFilledCurveY;
                ui->labImage->setVisible(false);
                ui->widImage->setVisible(false);
                ui->cmbLinesXError->setVisible(false);
                ui->labErrorX->setVisible(false);
                ui->labSymbol->setVisible(false);
                ui->widSymbol->setVisible(false);

                /*ui->cmbErrorColor->setVisible(true);
                ui->cmbErrorStyle->setVisible(true);
                ui->cmbLinesXError->setVisible(false);
                ui->cmbLinesYError->setVisible(true);
                ui->chkDrawLine->setVisible(false);
                ui->cmbSymbol->setVisible(false);
                ui->spinSymbolSize->setVisible(false);
                ui->cmbFillColor->setVisible(true);
                ui->cmbLineStyle->setVisible(true);*/
                break;
            case 5:
                //graph.type=QFRDRTable::gtStepsVertical;
                ui->labImage->setVisible(false);
                ui->widImage->setVisible(false);
                ui->cmbLinesXError->setVisible(false);
                ui->labErrorX->setVisible(false);
                ui->cmbLinesYError->setVisible(false);
                ui->labErrorY->setVisible(false);
                ui->labSymbol->setVisible(false);
                ui->widSymbol->setVisible(false);
                ui->widErrorStyle->setVisible(false);
                ui->labErrorStyle->setVisible(false);

                /*ui->cmbErrorColor->setVisible(false);
                ui->cmbErrorStyle->setVisible(false);
                ui->cmbLinesXError->setVisible(false);
                ui->cmbLinesYError->setVisible(false);
                ui->cmbSymbol->setVisible(false);
                ui->spinSymbolSize->setVisible(false);
                ui->sliderErrorTransparent->setVisible(false);*/
                break;
            case 6:
                //graph.type=QFRDRTable::gtStepsHorizontal;
                ui->labImage->setVisible(false);
                ui->widImage->setVisible(false);
                ui->labImage->setVisible(false);
                ui->widImage->setVisible(false);
                ui->cmbLinesXError->setVisible(false);
                ui->labErrorX->setVisible(false);
                ui->cmbLinesYError->setVisible(false);
                ui->labErrorY->setVisible(false);
                ui->labSymbol->setVisible(false);
                ui->widSymbol->setVisible(false);
                ui->widErrorStyle->setVisible(false);
                ui->labErrorStyle->setVisible(false);
/*                    ui->cmbErrorColor->setVisible(false);
                ui->cmbErrorStyle->setVisible(false);
                ui->cmbLinesXError->setVisible(false);
                ui->cmbLinesYError->setVisible(false);
                ui->cmbSymbol->setVisible(false);
                ui->spinSymbolSize->setVisible(false);
                ui->sliderErrorTransparent->setVisible(false);*/
                break;
            case 7:
                //graph.type=QFRDRTable::gtbarsVertical;
                ui->labImage->setVisible(false);
                ui->widImage->setVisible(false);
                ui->cmbLinesXError->setVisible(false);
                ui->labErrorX->setVisible(false);
                ui->cmbLinesYError->setVisible(false);
                ui->labErrorY->setVisible(false);
                ui->labSymbol->setVisible(false);
                ui->widSymbol->setVisible(false);
                ui->widErrorStyle->setVisible(false);
                ui->labErrorStyle->setVisible(false);
                ui->chkDrawLine->setVisible(false);

/*                    ui->cmbErrorColor->setVisible(false);
                ui->cmbErrorStyle->setVisible(false);
                ui->cmbLinesXError->setVisible(false);
                ui->cmbLinesYError->setVisible(false);
                ui->chkDrawLine->setVisible(false);
                ui->cmbSymbol->setVisible(false);
                ui->spinSymbolSize->setVisible(false);
                ui->sliderErrorTransparent->setVisible(false);*/
                break;
            case 8:
                //graph.type=QFRDRTable::gtbarsHorizontal;
                ui->labImage->setVisible(false);
                ui->widImage->setVisible(false);
                ui->cmbLinesXError->setVisible(false);
                ui->labErrorX->setVisible(false);
                ui->cmbLinesYError->setVisible(false);
                ui->labErrorY->setVisible(false);
                ui->labSymbol->setVisible(false);
                ui->widSymbol->setVisible(false);
                ui->widErrorStyle->setVisible(false);
                ui->labErrorStyle->setVisible(false);
                ui->chkDrawLine->setVisible(false);
                break;
            case 9:
                //graph.type=QFRDRTable::gtImage;
                ui->cmbLinesXError->setVisible(false);
                ui->labErrorX->setVisible(false);
                ui->cmbLinesYData->setVisible(true);
                ui->labDataY->setVisible(true);
                ui->cmbLinesYError->setVisible(false);
                ui->labErrorY->setVisible(false);
                ui->labSymbol->setVisible(false);
                ui->widSymbol->setVisible(false);
                ui->widErrorStyle->setVisible(false);
                ui->labErrorStyle->setVisible(false);
                ui->widColor->setVisible(false);
                ui->labColor->setVisible(false);
                ui->widFillColor->setVisible(false);
                ui->labFillColor->setVisible(false);
                ui->widLineStyle->setVisible(false);
                ui->labLinestyle->setVisible(false);
                ui->labDataX->setText(tr("Image Column:"));
                ui->labDataY->setText(tr("Modifier Column:"));
                break;
            case 10:
                //graph.type=QFRDRTable::gtRGBImage;
                ui->cmbLinesXError->setVisible(true);
                ui->labErrorX->setVisible(true);
                ui->cmbLinesYData->setVisible(true);
                ui->labDataY->setVisible(true);
                ui->cmbLinesYError->setVisible(true);
                ui->labErrorY->setVisible(true);
                ui->labSymbol->setVisible(false);
                ui->widSymbol->setVisible(false);
                ui->widErrorStyle->setVisible(false);
                ui->labErrorStyle->setVisible(false);
                ui->widColor->setVisible(false);
                ui->labColor->setVisible(false);
                ui->widFillColor->setVisible(false);
                ui->labFillColor->setVisible(false);
                ui->widLineStyle->setVisible(false);
                ui->labLinestyle->setVisible(false);
                ui->labDataX->setText(tr("R Column:"));
                ui->labErrorX->setText(tr("G Column:"));
                ui->labDataY->setText(tr("B Column:"));
                ui->labErrorY->setText(tr("Modifier Column:"));
                break;
            case 11:
                //graph.type=QFRDRTable::gtMaskImage;
                ui->labDataX->setText(tr("Mask Column:"));
                ui->cmbLinesXError->setVisible(false);
                ui->labErrorX->setVisible(false);
                ui->cmbLinesYData->setVisible(false);
                ui->labDataY->setVisible(false);
                ui->cmbLinesYError->setVisible(false);
                ui->labErrorY->setVisible(false);
                ui->labSymbol->setVisible(false);
                ui->widSymbol->setVisible(false);
                ui->widErrorStyle->setVisible(false);
                ui->labErrorStyle->setVisible(false);
                ui->widColor->setVisible(false);
                ui->labColor->setVisible(false);
                ui->widFillColor->setVisible(false);
                ui->labFillColor->setVisible(false);
                ui->widLineStyle->setVisible(false);
                ui->labLinestyle->setVisible(false);
                ui->cmbModifierMode->setVisible(false);
                ui->labModifierMode->setVisible(false);
                break;
            case 12:
                //graph.type=QFRDRTable::gtFunction;
                ui->labImage->setVisible(false);
                ui->widImage->setVisible(false);
                ui->cmbLinesXError->setVisible(false);
                ui->labErrorX->setVisible(false);
                ui->cmbLinesXData->setVisible(false);
                ui->labDataX->setVisible(false);
                ui->cmbLinesYError->setVisible(false);
                ui->labErrorY->setVisible(false);
                ui->labImage->setVisible(false);
                ui->widImage->setVisible(false);
                ui->widFunction->setVisible(true);
                ui->labFuction->setVisible(true);
                ui->labSymbol->setVisible(false);
                ui->widSymbol->setVisible(false);
                ui->chkSTrided->setVisible(false);
                ui->widStride->setVisible(false);

                /*ui->widLineStyle->setVisible(false);
                ui->cmbLinesXError->setVisible(false);
                ui->chkDrawLine->setVisible(false);
                ui->cmbLineStyle->setVisible(false);*/
                break;

            case 0:
            default:
                //graph.type=QFRDRTable::gtLines;
                ui->labImage->setVisible(false);
                ui->widImage->setVisible(false);
                break;
        }

    }
}

void QFTableGraphSettings::connectWidgets()
{
    //qDebug()<<"connectWidgets";


    connect(ui->edtFunction, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    connect(ui->edtGraphTitle, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    connect(ui->cmbGraphType, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->cmbLinesXData, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->cmbLinesXError, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->cmbLinesYData, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->cmbLinesYError, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->cmbFillColor, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->cmbErrorColor, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->cmbErrorStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->cmbLineColor, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->cmbLineStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->cmbSymbol, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->chkDrawLine, SIGNAL(toggled(bool)), this, SLOT(writeGraphData()));
    connect(ui->spinLineWidth, SIGNAL(valueChanged(double)), this, SLOT(writeGraphData()));
    connect(ui->spinSymbolSize, SIGNAL(valueChanged(double)), this, SLOT(writeGraphData()));
    connect(ui->sliderErrorTransparent, SIGNAL(valueChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->sliderFillTransparent, SIGNAL(valueChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->sliderPlotTransparent, SIGNAL(valueChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->cmbImageFalseColor, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->cmbImageTrueColor, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->cmbColormap, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->cmbModifierMode, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->spinImageWidth, SIGNAL(valueChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->edtImageHeight, SIGNAL(valueChanged(double)), this, SLOT(writeGraphData()));
    connect(ui->edtImageMax, SIGNAL(valueChanged(double)), this, SLOT(writeGraphData()));
    connect(ui->edtImageMin, SIGNAL(valueChanged(double)), this, SLOT(writeGraphData()));
    connect(ui->edtImageHeight, SIGNAL(valueChanged(double)), this, SLOT(writeGraphData()));
    connect(ui->edtImageX, SIGNAL(valueChanged(double)), this, SLOT(writeGraphData()));
    connect(ui->edtImageY, SIGNAL(valueChanged(double)), this, SLOT(writeGraphData()));
    connect(ui->sliderImageFalseColor, SIGNAL(valueChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->sliderImageTrueColor, SIGNAL(valueChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->edtColorbarLabel, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    connect(ui->spinColorbarWidth, SIGNAL(valueChanged(double)), this, SLOT(writeGraphData()));
    connect(ui->spinColorbarHeight, SIGNAL(valueChanged(double)), this, SLOT(writeGraphData()));
    connect(ui->chkImageColorbarRight, SIGNAL(toggled(bool)), this, SLOT(writeGraphData()));
    connect(ui->chkImageColorbarTop, SIGNAL(toggled(bool)), this, SLOT(writeGraphData()));
    connect(ui->chkImageAutorange, SIGNAL(toggled(bool)), this, SLOT(writeGraphData()));
    connect(ui->chkSTrided, SIGNAL(toggled(bool)), this, SLOT(writeGraphData()));
    connect(ui->spinStride, SIGNAL(valueChanged(int)), this, SLOT(writeGraphData()));
    connect(ui->spinStrideStart, SIGNAL(valueChanged(int)), this, SLOT(writeGraphData()));
}

void QFTableGraphSettings::disconnectWidgets()
{
    //qDebug()<<"disconnectWidgets";


    disconnect(ui->edtFunction, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    disconnect(ui->edtGraphTitle, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    disconnect(ui->cmbGraphType, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->cmbLinesXData, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->cmbLinesXError, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->cmbLinesYData, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->cmbLinesYError, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->cmbErrorColor, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->cmbFillColor, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->cmbErrorStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->cmbLineColor, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->cmbLineStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->cmbSymbol, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->cmbModifierMode, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->chkDrawLine, SIGNAL(toggled(bool)), this, SLOT(writeGraphData()));
    disconnect(ui->spinLineWidth, SIGNAL(valueChanged(double)), this, SLOT(writeGraphData()));
    disconnect(ui->spinSymbolSize, SIGNAL(valueChanged(double)), this, SLOT(writeGraphData()));
    disconnect(ui->sliderErrorTransparent, SIGNAL(valueChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->sliderFillTransparent, SIGNAL(valueChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->sliderPlotTransparent, SIGNAL(valueChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->cmbImageFalseColor, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->cmbImageTrueColor, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->cmbColormap, SIGNAL(currentIndexChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->spinImageWidth, SIGNAL(valueChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->edtImageHeight, SIGNAL(valueChanged(double)), this, SLOT(writeGraphData()));
    disconnect(ui->edtImageMax, SIGNAL(valueChanged(double)), this, SLOT(writeGraphData()));
    disconnect(ui->edtImageMin, SIGNAL(valueChanged(double)), this, SLOT(writeGraphData()));
    disconnect(ui->edtImageHeight, SIGNAL(valueChanged(double)), this, SLOT(writeGraphData()));
    disconnect(ui->edtImageX, SIGNAL(valueChanged(double)), this, SLOT(writeGraphData()));
    disconnect(ui->edtImageY, SIGNAL(valueChanged(double)), this, SLOT(writeGraphData()));
    disconnect(ui->sliderImageFalseColor, SIGNAL(valueChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->sliderImageTrueColor, SIGNAL(valueChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->edtColorbarLabel, SIGNAL(editingFinished()), this, SLOT(writeGraphData()));
    disconnect(ui->spinColorbarWidth, SIGNAL(valueChanged(double)), this, SLOT(writeGraphData()));
    disconnect(ui->spinColorbarHeight, SIGNAL(valueChanged(double)), this, SLOT(writeGraphData()));
    disconnect(ui->chkImageColorbarRight, SIGNAL(toggled(bool)), this, SLOT(writeGraphData()));
    disconnect(ui->chkImageColorbarTop, SIGNAL(toggled(bool)), this, SLOT(writeGraphData()));
    disconnect(ui->chkImageAutorange, SIGNAL(toggled(bool)), this, SLOT(writeGraphData()));
    disconnect(ui->chkSTrided, SIGNAL(toggled(bool)), this, SLOT(writeGraphData()));
    disconnect(ui->spinStride, SIGNAL(valueChanged(int)), this, SLOT(writeGraphData()));
    disconnect(ui->spinStrideStart, SIGNAL(valueChanged(int)), this, SLOT(writeGraphData()));

}

void QFTableGraphSettings::initFocus()
{
    ui->edtGraphTitle->setFocus();
}

void QFTableGraphSettings::on_edtFunction_textChanged(const QString &text)
{
    ui->labFunctionOK->setText(tr("<font color=\"darkgreen\">function OK</font>"));
    QFMathParser mp; // instanciate
    QFMathParser::qfmpNode* n;
    // parse some numeric expression
    n=mp.parse(text);
    delete n;
    if (mp.hasErrorOccured()) {
        ui->labFunctionOK->setText(tr("<font color=\"red\">ERROR in function:<br>&nbsp;&nbsp;&nbsp;&nbsp;%1</font>").arg(mp.getLastErrors().join("<br>&nbsp;&nbsp;&nbsp;&nbsp;")));
    }
}

void QFTableGraphSettings::on_btnFunctionHelp_clicked()
{
     if (current)
         QFPluginServices::getInstance()->displayHelpWindow(QFPluginServices::getInstance()->getPluginHelpDirectory("table")+"/mathparser.html");
}
