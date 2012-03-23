#include "syscamconfigdialog.h"
#include "ui_syscamconfigdialog.h"
#include "videocapture.h"

SyscamConfigDialog::SyscamConfigDialog(VideoCapture *vid, int cam, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SyscamConfigDialog)
{
    ui->setupUi(this);
    int min=0;
    int max=0;

    min=max=0; vid->getUserPropertyRange(VideoCapture::upBrightness, min, max);
    ui->spinBrightsness->setRange(min, max);
    ui->spinBrightsness->setEnabled(vid->supportsUserProperty(VideoCapture::upBrightness));
    ui->spinBrightsness->setValue(vid->getUserProperty(VideoCapture::upBrightness));

    min=max=0; vid->getUserPropertyRange(VideoCapture::upContrast, min, max);
    ui->spinContrast->setRange(min, max);
    ui->spinContrast->setEnabled(vid->supportsUserProperty(VideoCapture::upContrast));
    ui->spinContrast->setValue(vid->getUserProperty(VideoCapture::upContrast));

    min=max=0; vid->getUserPropertyRange(VideoCapture::upSaturation, min, max);
    ui->spinSaturation->setRange(min, max);
    ui->spinSaturation->setEnabled(vid->supportsUserProperty(VideoCapture::upSaturation));
    ui->spinSaturation->setValue(vid->getUserProperty(VideoCapture::upSaturation));

    min=max=0; vid->getUserPropertyRange(VideoCapture::upHue, min, max);
    ui->spinHue->setRange(min, max);
    ui->spinHue->setEnabled(vid->supportsUserProperty(VideoCapture::upHue));
    ui->spinHue->setValue(vid->getUserProperty(VideoCapture::upHue));

    min=max=0; vid->getUserPropertyRange(VideoCapture::upRedBalance, min, max);
    ui->spinRedBalance->setRange(min, max);
    ui->spinRedBalance->setEnabled(vid->supportsUserProperty(VideoCapture::upRedBalance));
    ui->spinRedBalance->setValue(vid->getUserProperty(VideoCapture::upRedBalance));

    min=max=0; vid->getUserPropertyRange(VideoCapture::upBlueBalance, min, max);
    ui->spinBlueBalance->setRange(min, max);
    ui->spinBlueBalance->setEnabled(vid->supportsUserProperty(VideoCapture::upBlueBalance));
    ui->spinBlueBalance->setValue(vid->getUserProperty(VideoCapture::upBlueBalance));

    min=max=0; vid->getUserPropertyRange(VideoCapture::upGamma, min, max);
    ui->spinGamma->setRange(min, max);
    ui->spinGamma->setEnabled(vid->supportsUserProperty(VideoCapture::upGamma));
    ui->spinGamma->setValue(vid->getUserProperty(VideoCapture::upGamma));

    min=max=0; vid->getUserPropertyRange(VideoCapture::upExposure, min, max);
    ui->spinExposure->setRange(min, max);
    ui->spinExposure->setEnabled(vid->supportsUserProperty(VideoCapture::upExposure));
    ui->spinExposure->setValue(vid->getUserProperty(VideoCapture::upExposure));

    min=max=0; vid->getUserPropertyRange(VideoCapture::upGain, min, max);
    ui->spinGain->setRange(min, max);
    ui->spinGain->setEnabled(vid->supportsUserProperty(VideoCapture::upGain));
    ui->spinGain->setValue(vid->getUserProperty(VideoCapture::upGain));

    min=max=0; vid->getUserPropertyRange(VideoCapture::upSharpness, min, max);
    ui->spinSharpness->setRange(min, max);
    ui->spinSharpness->setEnabled(vid->supportsUserProperty(VideoCapture::upSharpness));
    ui->spinSharpness->setValue(vid->getUserProperty(VideoCapture::upSharpness));

    min=max=0; vid->getUserPropertyRange(VideoCapture::upBacklightCompensation, min, max);
    ui->spinBacklightCompensation->setRange(min, max);
    ui->spinBacklightCompensation->setEnabled(vid->supportsUserProperty(VideoCapture::upBacklightCompensation));
    ui->spinBacklightCompensation->setValue(vid->getUserProperty(VideoCapture::upBacklightCompensation));


    ui->chkAutoGain->setEnabled(vid->supportsUserProperty(VideoCapture::upAutogain));
    ui->chkAutoGain->setChecked(vid->getUserProperty(VideoCapture::upAutogain)!=0);

    ui->chkAutoWhiteBalance->setEnabled(vid->supportsUserProperty(VideoCapture::upAutoWhitebalance));
    ui->chkAutoWhiteBalance->setChecked(vid->getUserProperty(VideoCapture::upAutoWhitebalance)!=0);

    ui->cmbPowerlineFilter->setEnabled(vid->supportsUserProperty(VideoCapture::upPowerlineFilter));
    ui->cmbPowerlineFilter->setCurrentIndex(vid->getUserProperty(VideoCapture::upPowerlineFilter));


}

SyscamConfigDialog::~SyscamConfigDialog()
{
    delete ui;
}

void SyscamConfigDialog::loadFromQSettings(QSettings &data) {
    ui->chkAutoGain->setChecked(data.value("syscam/autogain", ui->chkAutoGain->isChecked()).toBool());
    ui->chkAutoWhiteBalance->setChecked(data.value("syscam/autowhitebalance", ui->chkAutoWhiteBalance->isChecked()).toBool());
    ui->cmbPowerlineFilter->setCurrentIndex(data.value("syscam/powerlinefilter", ui->cmbPowerlineFilter->currentIndex()).toInt());
    ui->cmbToGray->setCurrentIndex(data.value("syscam/togray", ui->cmbToGray->currentIndex()).toInt());
    ui->spinBacklightCompensation->setValue(data.value("syscam/backlightcompensation", ui->spinBacklightCompensation->value()).toInt());
    ui->spinBlueBalance->setValue(data.value("syscam/bluebalance", ui->spinBlueBalance->value()).toInt());
    ui->spinBrightsness->setValue(data.value("syscam/brightness", ui->spinBrightsness->value()).toInt());
    ui->spinContrast->setValue(data.value("syscam/contrast", ui->spinContrast->value()).toInt());
    ui->spinExposure->setValue(data.value("syscam/exposure", ui->spinExposure->value()).toInt());
    ui->spinGain->setValue(data.value("syscam/gain", ui->spinGain->value()).toInt());
    ui->spinGamma->setValue(data.value("syscam/gamma", ui->spinGamma->value()).toInt());
    ui->spinHue->setValue(data.value("syscam/hue", ui->spinHue->value()).toInt());
    ui->spinRedBalance->setValue(data.value("syscam/redbalance", ui->spinRedBalance->value()).toInt());
    ui->spinSaturation->setValue(data.value("syscam/saturation", ui->spinSaturation->value()).toInt());
    ui->spinSharpness->setValue(data.value("syscam/sharpness", ui->spinSharpness->value()).toInt());

}

void SyscamConfigDialog::saveToQSettings(QSettings &data) {
    data.setValue("syscam/autogain", ui->chkAutoGain->isChecked());
    data.setValue("syscam/autowhitebalance", ui->chkAutoWhiteBalance->isChecked());
    data.setValue("syscam/powerlinefilter", ui->cmbPowerlineFilter->currentIndex());
    data.setValue("syscam/togray", ui->cmbToGray->currentIndex());
    data.setValue("syscam/backlightcompensation", ui->spinBacklightCompensation->value());
    data.setValue("syscam/bluebalance", ui->spinBlueBalance->value());
    data.setValue("syscam/brightness", ui->spinBrightsness->value());
    data.setValue("syscam/contrast", ui->spinContrast->value());
    data.setValue("syscam/exposure", ui->spinExposure->value());
    data.setValue("syscam/gain", ui->spinGain->value());
    data.setValue("syscam/gamma", ui->spinGamma->value());
    data.setValue("syscam/hue", ui->spinHue->value());
    data.setValue("syscam/redbalance", ui->spinRedBalance->value());
    data.setValue("syscam/saturation", ui->spinSaturation->value());
    data.setValue("syscam/sharpness", ui->spinSharpness->value());
}