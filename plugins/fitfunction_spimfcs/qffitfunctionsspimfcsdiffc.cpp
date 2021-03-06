/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

    

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

#include "qffitfunctionsspimfcsdiffc.h"
#include "qfmathtools.h"
#include <cmath>
#include "qftools.h"
#define sqr(x) qfSqr(x)
#define cube(x) qfCube(x)
#define pow4(x) qfPow4(x)
#define NAVOGADRO QF_NAVOGADRO

QFFitFunctionsSPIMFCSDiffC::QFFitFunctionsSPIMFCSDiffC() {
    //           type,         id,                        name,                                                    label,                      unit,          unitlabel,               fit,       userEditable, userRangeEditable, displayError,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(IntCombo,     "n_components",            "number of diffusing components",                        "components",               "",            "",                      false,     true,         false,             QFFitFunction::NoError,      false, 1,            1,        3,        1,   1,      2);
    #define FCSSDiff_n_components 0
    addParameter(FloatNumber,  "concentration",           "particle concentration in focus",                       "C<sub>all</sub>",          "nM",         "nM",                     true,      true,          true,              QFFitFunction::DisplayError, false, 10,          0,        1e50,     1    );
    #define FCSSDiff_concentration 1

    addParameter(FloatNumber,  "diff_rho1",               "fraction of first component",                           "&rho;<sub>1</sub>",        "",            "",                      false,     false,        false,             QFFitFunction::DisplayError, false, 0.5,          0,        0.99999,  0.1, 0,      1);
    #define FCSSDiff_diff_rho1 2
    addParameter(FloatNumber,  "diff_coeff1",             "diffusion coefficient of species 1",                    "D<sub>1</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    true,      true,         true,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e50,     1    );
    #define FCSSDiff_diff_coeff1 3
    addParameter(FloatNumber,  "diff_rho2",               "fraction of second component",                          "&rho;<sub>2</sub>",        "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 0.5,          0,        0.99999,  0.1, 0,      1  );
    #define FCSSDiff_diff_rho2 4
    addParameter(FloatNumber,  "diff_coeff2",             "diffusion coefficient of species 2",                    "D<sub>2</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    true,      true,         true,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e50,     1    );
    #define FCSSDiff_diff_coeff2 5
    addParameter(FloatNumber,  "diff_rho3",               "fraction of third component",                           "&rho;<sub>3</sub>",        "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 0.5,          0,        0.99999,  0.1, 0,      1  );
    #define FCSSDiff_diff_rho3 6
    addParameter(FloatNumber,  "diff_coeff3",             "diffusion coefficient of species 3",                    "D<sub>3</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    true,      true,         true,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e50,     1    );
    #define FCSSDiff_diff_coeff3 7

    addParameter(FloatNumber,  "offset",                  "correlation offset",                                    "G<sub>&infin;</sub>",      "",           "",                       true,      true,         true,              QFFitFunction::DisplayError, true, 0,            -10,      10,       0.1  );
    #define FCSSDiff_offset 8
    addParameter(FloatNumber,  "focus_hieght",            "PSF: axial radius (1/sqrt(e))",                             "&sigma;<sub>z</sub>",      "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 620,         0.01,     1e5,      10  );
    #define FCSSDiff_focus_height 9
    addParameter(FloatNumber,  "focus_width",             "PSF: lateral radius (1/sqrt(e))",                       "&sigma;<sub>x,y</sub>",    "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 300,          0,        1e4,      10    );
    #define FCSSDiff_focus_width 10
    addParameter(FloatNumber,  "pixel_width",             "pixel width",                                           "a",                        "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 400,          0,        1e4,      10    );
    #define FCSSDiff_pixel_width 11
    addParameter(FloatNumber,  "focus_volume",            "focus: effective volume",                               "V<sub>eff</sub>",          "fl",         "fl",                     false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define FCSSDiff_focus_volume 12
    addParameter(FloatNumber,  "effective_area",            "focus: effective area",                               "A<sub>eff</sub>",          "micron^2",         "&mu;m<sup>2</sup>",                     false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
     #define FCSSDiff_focus_area 13

    addParameter(FloatNumber,  "count_rate",              "count rate during measurement",                         "count rate",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError,    false, 0,            0,        1e50,     1    );
    #define FCSSDiff_count_rate 14
    addParameter(FloatNumber,  "background",              "background count rate during measurement",              "background",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError  ,  false, 0,            0,        1e50,     1    );
    #define FCSSDiff_background 15

}


double QFFitFunctionsSPIMFCSDiffC::evaluate(double t, const double* data) const {
    const int comp=data[FCSSDiff_n_components];
    const double c=data[FCSSDiff_concentration]*6.022e-1; // FCSSDiff_concentration given in 1e-9*6.022e23 particles/litre but c should be in particles per �m�= particles/10^{-15}litres=1e15 particles/litre
    const double D1=data[FCSSDiff_diff_coeff1];
    const double D2=data[FCSSDiff_diff_coeff2];
    const double rho2=data[FCSSDiff_diff_rho2];
    const double D3=data[FCSSDiff_diff_coeff3];
    const double rho3=data[FCSSDiff_diff_rho3];
    const double sigmaz=data[FCSSDiff_focus_height]/1.0e3;
    const double wxy=data[FCSSDiff_focus_width]/1.0e3;
    const double a=data[FCSSDiff_pixel_width]/1.0e3;
    const double offset=data[FCSSDiff_offset];
    const double background=data[FCSSDiff_background];
    const double cr=data[FCSSDiff_count_rate];
    double backfactor=sqr(cr-background)/sqr(cr);
    if (fabs(cr)<1e-15 || fabs(background)<1e-10) backfactor=1;

    double rho1=1;
    if (comp==2) {
        rho1=1.0-rho2;
    }
    if (comp==3) {
        rho1=1.0-rho2-rho3;
    }

    double cfac=0;

    if (comp>0) {
        const double fac_z1=sqrt(1.0+D1*t/sqr(sigmaz));
        const double dt_sigma2=D1*t+sqr(wxy);
        const double fac_xy1=2.0*a*erf(a/2.0/sqrt(dt_sigma2))+4.0*sqrt(dt_sigma2/M_PI)*(exp(-0.25*sqr(a)/dt_sigma2)-1.0);
        const double d1=1.0/fac_z1*sqr(fac_xy1);
        cfac=rho1*d1;
    }

    if (comp>1) {
        const double fac_z2=sqrt(1.0+D2*t/sqr(sigmaz));
        const double dt_sigma22=D2*t+sqr(wxy);
        const double fac_xy2=2.0*a*erf(a/2.0/sqrt(dt_sigma22))+4.0*sqrt(dt_sigma22/M_PI)*(exp(-0.25*sqr(a)/dt_sigma22)-1.0);
        const double d2=1.0/fac_z2*sqr(fac_xy2);
        cfac=cfac+rho2*d2;
    }

    if (comp>2) {
        const double fac_z3=sqrt(1.0+D3*t/sqr(sigmaz));
        const double dt_sigma23=D3*t+sqr(wxy);
        const double fac_xy3=2.0*a*erf(a/2.0/sqrt(dt_sigma23))+4.0*sqrt(dt_sigma23/M_PI)*(exp(-0.25*sqr(a)/dt_sigma23)-1.0);
        const double d3=1.0/fac_z3*sqr(fac_xy3);
        cfac=cfac+rho3*d3;
    }

    const double pre=1.0/(8.0*pow4(a)*sigmaz*sqrt(M_PI));
    return offset+pre/c*cfac*backfactor;
}


void QFFitFunctionsSPIMFCSDiffC::sortParameter(double *parameterValues, double *error, bool *fix) const {

    const int comp=parameterValues[FCSSDiff_n_components];
    QList<QPair<int, double> > dataForSort;

    if (comp>1) {
        if (!fix || (!fix[FCSSDiff_diff_coeff1]))dataForSort.append(qMakePair(FCSSDiff_diff_coeff1, parameterValues[FCSSDiff_diff_coeff1]));
        if (!fix || (!fix[FCSSDiff_diff_coeff2]))dataForSort.append(qMakePair(FCSSDiff_diff_coeff2, parameterValues[FCSSDiff_diff_coeff2]));
        if (comp>2) {
            if (!fix || (!fix[FCSSDiff_diff_coeff3]))dataForSort.append(qMakePair(FCSSDiff_diff_coeff3, parameterValues[FCSSDiff_diff_coeff3]));
        }

        if (dataForSort.size()>1) {
            QList<int> init, final;
            final=qfQPairListToFirstList(dataForSort);
            //QList<double> initvals=qfQPairListToSecondList(dataForSort);
            qSort(dataForSort.begin(), dataForSort.end(), qfQPairCompareSecondDec<int, double>);
            init=qfQPairListToFirstList(dataForSort);
            /*qDebug()<<"sorting: "<<id();
            qDebug()<<"   Ds:";
            qDebug()<<"      init  = "<<init;
            qDebug()<<"       vals = "<<qfSelectFromVector(init, parameterValues);
            qDebug()<<"       vals = "<<qfSelectFromVector(init, error);
            qDebug()<<"      final = "<<final;*/

            qfSortVectorByPermutation(init, final, parameterValues);
            if (error) qfSortVectorByPermutation(init, final, error);

            // now we sort the diffusion fractions, using that ID_rhoI=ID_coeffI-1:
            for (int i=0; i<qMin(init.size(), final.size()); i++) {
                init[i]=init[i]-1.0;
                final[i]=final[i]-1.0;
            }
            /*qDebug()<<"   rhos:";
            qDebug()<<"      init  = "<<init;
            if (error) qDebug()<<"       vals = "<<qfSelectFromVector(init, error);
            if (fix) qDebug()<<"       vals = "<<qfSelectFromVector(init, fix);
            qDebug()<<"      final = "<<final;*/

            qfSortVectorByPermutation(init, final, parameterValues);
            if (error) qfSortVectorByPermutation(init, final, error);
            if (fix) qfSortVectorByPermutation(init, final, fix);
            //qDebug()<<"   fix = "<<fix<<"   error = "<<error;
        }

    }



}

void QFFitFunctionsSPIMFCSDiffC::calcParameter(double* data, double* error) const {
    //double D1=data[FCSSDiff_diff_coeff1];
    double eD1=0;
    double sigmaz=data[FCSSDiff_focus_height]/1.0e3;
    double esigmaz=0;
    //double wxy=data[FCSSDiff_focus_width]/1.0e3;
    double ewxy=0;
    double a=data[FCSSDiff_pixel_width]/1.0e3;
    double ea=0;
    //double offset=data[FCSSDiff_offset];
    double eoffset=0;

    int comp=data[FCSSDiff_n_components];
    double rho2=data[FCSSDiff_diff_rho2];
    double erho2=0;
    double rho3=data[FCSSDiff_diff_rho3];
    double erho3=0;
    double cps=data[FCSSDiff_count_rate];
    double ecps=0;
    double ecpm=0;
    double background=data[FCSSDiff_background];
    double ebackground=0;

    if (error) {
        eD1=error[FCSSDiff_diff_coeff1];
        esigmaz=error[FCSSDiff_focus_height]/1.0e3;
        ewxy=error[FCSSDiff_focus_width]/1.0e3;
        ea=error[FCSSDiff_pixel_width]/1.0e3;
        eoffset=error[FCSSDiff_offset];
        erho2=error[FCSSDiff_diff_rho2];
        erho3=error[FCSSDiff_diff_rho3];
        ecps=error[FCSSDiff_count_rate];
        ebackground=error[FCSSDiff_background];
    }


    // calculate rho1
    double rho1=1;
    double erho1=0;
    if (comp==2) {
        if (rho2>1.0) rho2=1.0;
        if (rho2<0.0) rho2=0.0;
        if (rho2>1.0) {
            rho1=0;
            rho2=0.5;
        }
        rho1=1.0-rho2;
        erho1=erho2;
    }
    if (comp==3) {
        if (rho2>1.0) rho2=1.0;
        if (rho2<0.0) rho2=0.0;
        if (rho3>1.0) rho3=1.0;
        if (rho3<0.0) rho3=0.0;
        if (rho2+rho3>1.0) {
            rho1=0;
            rho2=rho3=0.5;
        }
        rho1=1.0-rho2-rho3;
        erho1=sqrt(erho2*erho2+erho3*erho3);
    }

    data[FCSSDiff_diff_rho1]=rho1;
    data[FCSSDiff_diff_rho2]=rho2;
    data[FCSSDiff_diff_rho3]=rho3;
    if (error) {
        error[FCSSDiff_diff_rho1]=erho1;
        error[FCSSDiff_diff_rho2]=erho2;
        error[FCSSDiff_diff_rho3]=erho3;
    }


    // calculate Veff = 2 * a^2 * sigmaz
    data[FCSSDiff_focus_volume]=2.0*a*a*sigmaz;
    if (error) error[FCSSDiff_focus_volume]=sqrt(sqr(esigmaz*2.0*a*a)+sqr(ea*4.0*a*sigmaz));

    // calculate Aeff =  a^2
    data[FCSSDiff_focus_area]=a*a;
    if (error) error[FCSSDiff_focus_area]=sqrt(sqr(ea*2.0*a));



}

bool QFFitFunctionsSPIMFCSDiffC::isParameterVisible(int parameter, const double* data) const {
    int comp=data[FCSSDiff_n_components];
    switch(parameter) {
        case FCSSDiff_diff_rho1:  return comp>1;
        case FCSSDiff_diff_coeff1: return comp>0;
        case FCSSDiff_diff_rho2: case FCSSDiff_diff_coeff2: return comp>1;
        case FCSSDiff_diff_rho3: case FCSSDiff_diff_coeff3: return comp>2;
    }
    return true;
}

unsigned int QFFitFunctionsSPIMFCSDiffC::getAdditionalPlotCount(const double* params) const {

    Q_UNUSED(params);
    return 0;
}

QString QFFitFunctionsSPIMFCSDiffC::transformParametersForAdditionalPlot(int plot, double* params) const {


    Q_UNUSED(plot);
    Q_UNUSED(params);
    return "";
}
