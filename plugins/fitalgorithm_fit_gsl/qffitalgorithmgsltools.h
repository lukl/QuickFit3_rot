#ifndef QFFITALGORITHMGSLTOOLS_H
#define QFFITALGORITHMGSLTOOLS_H

#include <cmath>
#include "qffitalgorithm.h"
#include "qffitfunction.h"
#include "gsl/gsl_multimin.h"
#include "gsl/gsl_vector.h"
#include "gsl/gsl_blas.h"

struct QFFItAlgorithmGSL_evalData {
    QFFitAlgorithm::Functor* model;
    const double* paramsMin;
    const double* paramsMax;
    int pcount;
};

double QFFitAlgorithmGSL_f (const gsl_vector *v, void *params);
void QFFitAlgorithmGSL_df (const gsl_vector *v, void *params, gsl_vector * g);
void QFFitAlgorithmGSL_fdf (const gsl_vector *x, void *params, double *f, gsl_vector *df);

#endif // QFFITALGORITHMGSLTOOLS_H
