#include "qfrdrimagingfcs_data.h"
#include <QtXml>
#include "libtiff_tools.h"
#include "qfrdrimagereaderrh.h"


QFRDRImagingFCSData::QFRDRImagingFCSData(QFProject* parent):
    QFRawDataRecord(parent)
{
    correlations=NULL;
    correlationMean=NULL;
    correlationStdDev=NULL;
    sigmas=NULL;
    tau=NULL;
    N=0;
    width=0;
    height=0;
    overview=NULL;
    setResultsInitSize(1000);
    setEvaluationIDMetadataInitSize(1000);
}

QFRDRImagingFCSData::~QFRDRImagingFCSData() {
     allocateContents(0,0,0);
}

QString QFRDRImagingFCSData::getEditorName(int i) {
    if (i==0) return tr("Parameter Image");
    if (i==1) return tr("Correlation Curves");
    return QString("");
};

QFRawDataEditor* QFRDRImagingFCSData::createEditor(QFPluginServices* services, int i, QWidget* parent) {
    if (i==0) return new QFRDRImagingFCSImageEditor(services, parent);
    if (i==1) return new QFRDRImagingFCSDataEditor(services, parent);
    return NULL;
};


QStringList QFRDRImagingFCSData::getExportFiletypes() {
    QStringList sl;
    return sl;
};

QString QFRDRImagingFCSData::getExportDialogTitle() {
    return tr("");
}

QString QFRDRImagingFCSData::getExportDialogFiletypes() {
    return tr("");
}

void QFRDRImagingFCSData::exportData(const QString& format, const QString& filename)const  {
	// here you may export the data of the record into the specified format (see getExportFiletypes() )
	// THIS IS OPTIONAL
}


void QFRDRImagingFCSData::intWriteData(QXmlStreamWriter& w) {
	// write data to the project XML file using the QXmlStreamWriter
    if (leaveout.size()>0) {
        QString l="";
        for (int i=0; i<leaveout.size(); i++) {
            if (!l.isEmpty()) l=l+",";
            l=l+QString::number(leaveout[i]);
        }
        w.writeStartElement("leaveout");
        w.writeAttribute("list", l);
        w.writeEndElement();
    }
}

void QFRDRImagingFCSData::intReadData(QDomElement* e) {
    leaveout.clear();
	// read data from the project XML file
    leaveout.clear();
    if (e) {
        QDomElement te=e->firstChildElement("leaveout");
        QString l=te.attribute("list");
        QStringList li=l.split(",");
        for (int i=0; i<li.size(); i++) {
            bool ok=false;
            int lo=li[i].toUInt(&ok);
            if (ok) leaveout.append(lo);
        }
    }

    width=getProperty("WIDTH", 0).toInt();
    height=getProperty("HEIGHT", 0).toInt();
    QString filetype=getProperty("FILETYPE", "unknown").toString();

    // now also load the data file(s) this record is linked to
    // an error is reported when no file is given!
    if (files.size()<=0) {
            setError(tr("there are no files in the %1 record!").arg(getName()));
            return;
    }
    if (filetype.toUpper()=="VIDEO_CORRELATOR") {
        loadVideoCorrelatorFile(files[0]);
    } else if (filetype.toUpper()=="RADHARD2") {
        loadRadhard2File(files[0]);
        width=getProperty("WIDTH", 0).toInt();
        height=getProperty("HEIGHT", 0).toInt();
    } else {
        setError(tr("filetype '%1' is unknown for Imaging FCS data files (file is '%2')").arg(filetype).arg(files[0]));
    }
    if (files.size()>1) {
        for (int i=1; i<files.size(); i++) {
            if (i<files_types.size()) {
                if (files_types[i].toLower().trimmed()=="overview") {
                    loadOverview(files[i]);
                }
            }
        }
    }
}

bool QFRDRImagingFCSData::loadOverview(QString filename) {
    bool ok=false;

    if (!overview) return false;

    if (QFile::exists(filename)) {
        TIFF* tif=TIFFOpen(filename.toAscii().data(), "r");
        if (tif) {
            uint32 nx,ny;
            TIFFGetField(tif,TIFFTAG_IMAGEWIDTH,&nx);
            TIFFGetField(tif,TIFFTAG_IMAGELENGTH,&ny);
            uint16_t* ovw=(uint16_t*)malloc(nx*ny*sizeof(uint16_t));
            ok=TIFFReadFrame<uint16_t>(tif, ovw);

            //qDebug()<<"TIFF: "<<nx<<"x"<<ny<<"    data: "<<width<<"x"<<height;

            if (width*height<=(int64_t)nx*(int64_t)ny) {
                for (int32_t y=0; y<height; y++) {
                    for (int32_t x=0; x<width; x++) {
                        overview[y*width+x]=ovw[y*nx+x];
                    }

                }
            } else {
                ok=false;
            }

            free(ovw);
            TIFFClose(tif);
        }
    }

    if (!ok && overview) {
        for (int i=0; i<width*height; i++) {
            overview[i]=0;
        }
    }
    return ok;
}

bool QFRDRImagingFCSData::loadVideoCorrelatorFile(QString filename) {
	bool ok=true;
	QString errorDescription="";

	// LOAD YOUR DATAFILE
	//      and set ok=false, if an error occured
	// example:
	QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		ok=false;
	} else {
        int taucolumn=getProperty("TAU_COLUMN", 0).toInt();
        if (taucolumn<0) taucolumn=0;
        int corrcolumn=getProperty("CORRELATION_COLUMN", 1).toInt();
        if (corrcolumn<0) corrcolumn=1;
        int correrrcolumn=getProperty("CORRELATION_ERROR_COLUMN", -1).toInt();
        //int maxCol=qMax(corrcolumn, taucolumn);

        QTextStream stream(&file);
        bool last_empty, empty=true;
        QVector<QVector<QTriple<double, double, double> > > data_matrix;
        QVector<QTriple<double, double, double> > current_set;
        int NN=0;
        int runs=0;
        while ((!stream.atEnd()) && (runs<=width*height)) {
            QVector<double> data=csvReadline(stream, ',', '#', 0);
            last_empty=empty;
            empty=data.isEmpty();
            if ((!empty) && (corrcolumn<data.size()) && (taucolumn<data.size()) && (correrrcolumn<data.size()) && (correrrcolumn>=0)) {
                current_set.append(qMakeTriple(data[taucolumn], data[corrcolumn],  data[correrrcolumn]));
            } else if ((!empty) && (corrcolumn<data.size()) && (taucolumn<data.size())) {
                current_set.append(qMakeTriple(data[taucolumn], data[corrcolumn], 0.0));
                //qDebug()<<"  tau="<<data[0]<<"   c="<<data[1];
            }
            if (((last_empty&&empty)||(stream.atEnd()))&&(!current_set.isEmpty())) {
                // this cuts all lines from current_set which are equal to the lastval (if this is 0.0 or 1.0)
                // this cuts away channels where the correlations have not bee calculated!
                double lastval=0.0;
                if (current_set.size()>0) {
                    lastval=current_set[current_set.size()-1].second;
                    if (lastval==0.0 || lastval==1.0) {
                        while ((current_set.size()>10) && (current_set[current_set.size()-1].second==lastval)) current_set.pop_back();
                    }
                }

                data_matrix.append(current_set);
                if (NN<current_set.size()) NN=current_set.size();
                //qDebug()<<"runs="<<runs<<"     NN="<<NN<<"     current_set.size()="<<current_set.size()<<"     data_matrix.size()="<<data_matrix.size()<<"    all0="<<all0<<"    all1="<<all1;
                current_set.clear();
                runs++;
                if (runs%50==0) QApplication::processEvents();
            }
            //if (stream.atEnd()) //qDebug()<<"runs="<<runs<<"     NN="<<NN<<"     width*height="<<width*height<<"     stream.atEnd()="<<stream.atEnd()<<"    data="<<data;

        }
        width=getProperty("WIDTH").toInt();
        height=getProperty("HEIGHT").toInt();
        //std::cout<<"width="<<width<<"   height="<<height<<"   NN="<<NN<<std::endl;
        if (NN>0) {
            if (runs+1>=width*height) {
                allocateContents(width, height, NN);
                for (int i=0; i<width*height; i++) {
                    if (i>=data_matrix.size()) {
                        for (int j=0; j<NN; j++) {
                            correlations[i*NN+j]=0;
                            sigmas[i*NN+j]=0;
                        }
                    } else {
                        for (int j=0; j<NN; j++) {
                            //std::cout<<"i="<<i<<" data_matrix.size()="<<data_matrix.size()<<"    j="<<j<<" data_matrix[i].size()="<<data_matrix[i].size()<<std::endl;
                            if (j<data_matrix[i].size()) {
                                tau[j]=data_matrix[i].at(j).first;
                                correlations[i*NN+j]=data_matrix[i].at(j).second;
                                sigmas[i*NN+j]=data_matrix[i].at(j).third;
                            } else {
                                correlations[i*NN+j]=0;
                                sigmas[i*NN+j]=0;
                            }
                        }
                    }
                }
                QApplication::processEvents();
                recalcCorrelations();
                QApplication::processEvents();
            } else {
                ok=false;
                errorDescription=tr("too few records in file: %1 records found, but %2 * %3 = %4 required").arg(NN).arg(width).arg(height).arg(width*height);
            }
        } else {
            ok=false;
            errorDescription=tr("no records found in file");
        }
	}

	if (!ok) setError(tr("Error while reading file '%1': %2").arg(filename).arg(errorDescription));
    return ok;
}



bool QFRDRImagingFCSData::loadRadhard2File(QString filename) {
    bool ok=true;

    QString errorDescription="";

    QFRDRImageReaderRH ReaderRH;

    ok=ReaderRH.open(filename);

    // LOAD FILE

    if (ok) {
        width= ReaderRH.frameWidth();
        height=ReaderRH.frameHeight();
        if((width==(128*8))&&(height==32)) {

            uint32_t **hwc_dump = (uint32_t **)malloc(1024 * sizeof(uint32_t*));
                for(int i=0;i<1024;i++)
                    hwc_dump[i]=(uint32_t*)malloc((128) * sizeof(uint32_t));
            unsigned char *hwc_line = (unsigned char*) malloc(2*4+(64*128)*4+2);
            uint32_t header;
            //read data
            do {
                //ReaderRH.readFrameCharRaw(hwc_line,&header);
                uint32_t id= (header >> 8) & 0x00FFFFFFU;
                uint32_t *src=(uint32_t *) &hwc_line[4]; //first dword: tics
                uint32_t ticks=*((uint32_t *)hwc_line);
                for(int x=0;x<32;x++) {
                    uint32_t *dst=hwc_dump[(id&31)*32+x];
                    for(int i=0;i<128;i++)
                    {
                        *dst=*src;
                        dst++;
                        src++;
                    }
                    src+=128; //skip delay registers
                }

            }while(true);
            for(unsigned int correlator=0;correlator<1024;correlator++)
            {
                    uint32_t *hwc_value=hwc_dump[correlator];
                    unsigned int tau=0;

                    uint32_t channel0=*hwc_value & 0xFFFFFFFFU;
                    uint32_t count_global=hwc_dump[correlator][126];
                    uint32_t count_local=hwc_dump[correlator][127];
                    unsigned int blocks=14;
                    unsigned int lags=8;
                    unsigned int steps=blocks*lags;
                    for(unsigned int block=0;block<blocks;block++)
                    {


                            for(unsigned int lag=0;lag<lags;lag++)
                            {
                                    uint32_t dword=*hwc_value;
                                    hwc_value++;
                                    unsigned int value=(dword & 0xFFFFFFFFU);

                                    //G, taken from correlator_multitau.h, should use buitl-in normalization function
                                    float result=1.0;
                                    if(steps>tau)
                                    {
                                            float correction_factor=(float(steps-tau))/(float)steps;
                                            result=((float)value*(float)steps)/((float)(1<<block)*(float)(correction_factor*count_global*count_local));// see "A distributed algorithm for multi-tau autocorrelation"
                                    }
                                    //\G

                                    //correlator_hw_values[correlator][block*lags+lag].tau=tau*10;//*10E-6s
                                    //correlator_hw_values[correlator][block*lags+lag].raw=value;
                                    //6correlator_hw_values[correlator][block*lags+lag].val=result;
                                    tau+=(1<<block);
                            }
                    }
            }
                        allocateContents(32,height,14*8);
           // correlations[]


        } else {
            ok=false;
        }
        if(ok) {
            width=32;
            height=32;
            int NN=100; // number of lags per correlation function
            //setQFProperty("WIDTH", width, false, true);
            //setQFProperty("HEIGHT", height, false, true);




            for (int i=0; i<width*height; i++) {
                for (int j=0; j<NN; j++) {
                    tau[j]=j;//data_matrix[i].at(j).first;
                    correlations[i*NN+j]=0;//data_matrix[i].at(j).second;
                    sigmas[i*NN+j]=0;//data_matrix[i].at(j).third;
                }
            }
        }

        QApplication::processEvents();
        recalcCorrelations();
        QApplication::processEvents();
    } else {
        ok=false;
        errorDescription=tr("error loading file");
    }
    ReaderRH.close();


    if (!ok) setError(tr("Error while reading Radhard2 file '%1': %2").arg(filename).arg(errorDescription));
    return ok;
}






int QFRDRImagingFCSData::getCorrelationRuns() {
    return width*height;
}

long long QFRDRImagingFCSData::getCorrelationN() {
    return N;
}

double* QFRDRImagingFCSData::getCorrelationT() {
    return tau;
}

double* QFRDRImagingFCSData::getCorrelation() {
    return correlations;
}

double* QFRDRImagingFCSData::getCorrelationRunErrors() {
    return sigmas;
}

double* QFRDRImagingFCSData::getCorrelationRun(int run) {
    return &(correlations[run*N]);
}

double* QFRDRImagingFCSData::getCorrelationRunError(int run) {
    return &(sigmas[run*N]);
}

QString QFRDRImagingFCSData::getCorrelationRunName(int run) {
    int x=runToX(run);
    int y=runToY(run);
    return QString("[%1, %2]").arg(x).arg(y);
}

double* QFRDRImagingFCSData::getCorrelationMean() {
    return correlationMean;
}

double* QFRDRImagingFCSData::getCorrelationStdDev() {
    return correlationStdDev;
}

bool QFRDRImagingFCSData::isCorrelationRunVisible(int run) {
    return !leaveoutRun(run);
}

void QFRDRImagingFCSData::allocateContents(int x, int y, int N) {
    if (correlations) free(correlations);
    if (correlationMean) free(correlationMean);
    if (correlationStdDev) free(correlationStdDev);
    if (sigmas) free(sigmas);
    if (tau) free(tau);
    if (overview) free(overview);
    correlations=NULL;
    correlationMean=NULL;
    correlationStdDev=NULL;
    sigmas=NULL;
    tau=NULL;
    overview=NULL;
    if ((x>0) && (y>0) && (N>0)) {
        correlations=(double*)calloc(x*y*N,sizeof(double));
        sigmas=(double*)calloc(x*y*N,sizeof(double));
        correlationMean=(double*)calloc(N,sizeof(double));
        correlationStdDev=(double*)calloc(N,sizeof(double));
        overview=(uint16_t*)calloc(x*y,sizeof(uint16_t));
        tau=(double*)calloc(N,sizeof(double));
        width=x;
        height=y;
        this->N=N;
        setQFProperty("WIDTH", x, false, true);
        setQFProperty("HEIGHT", y, false, true);
    }
}

void QFRDRImagingFCSData::recalcCorrelations() {
    if (correlations && correlationMean && correlationStdDev) {
        for (int i=0; i<N; i++) {
            double norm=0;
            double sum=0;
            double sum2=0;
            for (int j=0; j<width*height; j++) {
                const double& v=correlations[j*N+i];
                if (QFFloatIsOK(v) && !leaveout.contains(j)) {
                    sum+=v;
                    sum2+=v*v;
                    norm++;
                }
            }
            if (norm>0) {
                correlationMean[i]=sum/norm;
                correlationStdDev[i]=sqrt(sum2/norm-sum*sum/norm/norm);
            } else {
                correlationMean[i]=0;
                correlationStdDev[i]=0;
            }
        }
    }
}



















int QFRDRImagingFCSData::getDataImageWidth() const {
    return width;
}

int QFRDRImagingFCSData::getDataImageHeight() const {
    return height;
}

int QFRDRImagingFCSData::xyToRun(int x, int y) const {
    return y*width+x;
}

int QFRDRImagingFCSData::runToX(int run) const {
    return run%width;
}

int QFRDRImagingFCSData::runToY(int run) const {
    return run/width;
}

int QFRDRImagingFCSData::xyToIndex(int x, int y) const {
    return (y*width+x)*N;
}

uint16_t* QFRDRImagingFCSData::getDataImagePreview() const {
    return overview;
}
