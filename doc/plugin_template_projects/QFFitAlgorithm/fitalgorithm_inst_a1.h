#ifndef HEADER_A1_H
#define HEADER_A1_H
#include "qffitalgorithm.h"


/*! \brief QQFFitAlgorithm implementation
    \ingroup doxygen_GROUPNAME


*/
class QFFitAlgorithmInst_A1: public QFFitAlgorithm {
    protected:

        /** \copydoc QFFitAlgorithm::intFit() */
        virtual FitResult intFit(double* paramsOut, double* paramErrorsOut, const double* initialParams, Functor* model, const double* paramsMin, const double* paramsMax);


    public:
        /** \brief class constructor */
        QFFitAlgorithmInst_A1();
        /** \brief class destructor */
        virtual ~QFFitAlgorithmInst_A1() {}
        /** \copydoc QFFitAlgorithm::name() */
        virtual QString name() const { return QObject::tr("long fit algorithm name"); }
        /** \copydoc QFFitAlgorithm::shortName() */
        virtual QString shortName() const { return QObject::tr("short name"); }
        /** \copydoc QFFitAlgorithm::id() */
        virtual QString id() const { return QString("fa_id"); }
        /** \copydoc QFFitAlgorithm::get_supportsBoxConstraints() */
        virtual bool get_supportsBoxConstraints() const { return true; }
        /** \copydoc QFFitAlgorithm::displayConfig() */
        virtual bool displayConfig();
        /** \copydoc QFFitAlgorithm::isThreadSafe() */
        virtual bool isThreadSafe() const { return false; }
};


#endif // HEADER_A1_H
