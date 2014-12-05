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

#include "qffitfunction.h"
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include "qftools.h"
#include "statistics_tools.h"




QVector<double> QFFitFunction::multiEvaluate(const QVector<double> &x, const double *parameters) const
{
    QVector<double> y(x.size(), NAN);
    multiEvaluate(y.data(), x.constData(), x.size(), parameters);
    return y;
}

QVector<double> QFFitFunction::multiEvaluate(const double *x, long long N, const double *parameters) const
{
    QVector<double> y(N, NAN);
    multiEvaluate(y.data(), x, N, parameters);
    return y;

}

void QFFitFunction::multiEvaluate(double *y, const double *x, uint64_t N, const double *parameters) const
{
    for (uint64_t i=0; i<N; i++) {
        y[i]=evaluate(x[i], parameters);
    }
}

void QFFitFunction::evaluateDerivatives(double *derivatives, double x, const double *parameters) const
{

}





bool QFFitFunction::estimateInitial(double *params, const double *dataX, const double *dataY, long N, const bool *fix)
{
    return false;
}

void QFFitFunction::evaluateNumericalDerivatives(double *derivatives, double x, const double *parameters, double stepsize) const
{
    int N=m_parameters.size();
    double* ptemp=duplicateArray(parameters, N);
    for (int i=0; i<N; i++) {
        derivatives[i]=0;
        const double oldpi=ptemp[i];
        ptemp[i]=oldpi+2.0*stepsize;
        const double fp2h=evaluate(x, ptemp);
        ptemp[i]=oldpi+stepsize;
        const double fp1h=evaluate(x, ptemp);
        ptemp[i]=oldpi-stepsize;
        const double fm1h=evaluate(x, ptemp);
        ptemp[i]=oldpi-2.0*stepsize;
        const double fm2h=evaluate(x, ptemp);
        ptemp[i]=oldpi;
        derivatives[i]=(-fp2h+8.0*fp1h-8.0*fm1h+fm2h)/(12.0*stepsize);
    }
    qfFree(ptemp);
}

void QFFitFunction::evaluateNumericalParameterErrors(double *errors, double x, const double *parameters, double residualSigma2, double stepsize) const
{
    const int pcount=paramCount();
    QVector<double> cov, g;
    cov.reserve(pcount*pcount);
    g.resize(pcount);
    //double* cov=(double*)qfMalloc(pcount*pcount*sizeof(double));
    //double* g=(double*)qfMalloc(pcount*sizeof(double));
    if (get_implementsDerivatives()) {
        evaluateDerivatives(g, x, parameters);
    } else {
        evaluateNumericalDerivatives(g, x, parameters, stepsize);
    }
    for (int i=0; i<pcount; i++) {
        for (int j=0; j<pcount; j++) {
            cov[i*pcount+j]=g[i]*g[j];
        }
    }
    statisticsMatrixInversion(cov.data(), pcount);

    for (int i=0; i<pcount; i++) {
        errors[i]=sqrt(cov[i*pcount+i]*residualSigma2);
    }
    //qfFree(g);
    //qfFree(cov);
}



QFFitStatistics QFFitFunction::calcFitStatistics(long N, const double* tauvals, const double* corrdata, const double* weights, int datacut_min, int datacut_max, const double* fullParams, const double* /*errors*/, const bool* paramsFix, int runAvgWidth, int residualHistogramBins) const {
    int fitparamN=0;
    const int pcount=paramCount();
    for (int i=0; i<pcount; i++) {
        if (isParameterVisible(i, fullParams) && (!paramsFix[i]) && getDescription(i).fit) {
            fitparamN++;
        }
    }

    QVector<double> model=multiEvaluate(tauvals, N, fullParams);
    /*for (int i=0; i<N; i++) {
        model[i]=evaluate(tauvals[i], fullParams);
    }*/

    return calculateFitStatistics(N, tauvals, model.constData(), corrdata, weights, datacut_min, datacut_max, fitparamN, runAvgWidth, residualHistogramBins);
}












