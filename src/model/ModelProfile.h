#ifndef MODELPROFILE_H
#define MODELPROFILE_H

#include <QObject>
#include <QVector>
#include <QStringList>
#include <QMap>
#include <QDate>
#include "metric.h"
#include "macro.h"

#include <tuple>

class DataModel;

/**
 * @brief The ModelProfile class
 * redesign since April 3, 2015
 * the new design saves:
 * 1. global estimate and log likelihood for selected time lines;
 * 2. local estimates and log likelihoods with a granuality selection;
 */
class ModelProfile : public QObject
{
    Q_OBJECT
public:

    ModelProfile(DataModel *dm, int w);//parent is the datamodel;

    ~ModelProfile();

    //bool isDerived;

    DataModel *dataModel;

    //QStringList selected; //selected company in the profile;

    // window size of the local partitions;
    int granularity;


    // number of sub windows on the time line;
    int winNum;

    // maintian a list of estimate for selected companies;
    QVector<double> globEstimates;
    // maintian a list of estimate for selected companies;
    QVector<double> normglobEstimates;
    // globestimate range;
    QPair<double,double> globRange;

    // summary of estimate;
    FiveNumberTuple globEstSum;

    // maintain values of likelihood for selected time lines;
    QVector<double> globLikelihood;

    // five number summary of globLikelihood;
    FiveNumberTuple globLikeliSum;

    //look up quartile related indices;
    QuartileHash globReverseLookup;

    // for each local range, maintain a list of estimate for each selected company
    QVector< QVector<double> > locEstimates;
    // for each local range, maintain a list of estimate for each selected company
    QVector< QVector<double> > normlocEstimates;

    // summarization of local estimate in each local partition;
    QVector< FiveNumberTuple > locEstSum;

    // maintain a likelihood value for each local partition of each time line;
    QVector< QVector<double> > locLikelihood;

    // five num summary of local estimate;
    QVector< FiveNumberTuple > locLikliSum;

    // look up companies within each boxplot;
    QVector<QuartileHash> locReverseLookup;

    QString modelName;

protected:
    // create five number summary for global estimate;
    void summarizeGlobEst();

    // create five number summary for each local partition;
    void summarizeLocEst();

    // create five number summary for each local partition;
    void summarizeLocLikli();

    QPair<double, double> findMinMax();

    // normalize estimate;
    QPair<double,double> normalizeGlobEst(double mi, double mx);

    void updateGlobEstNorm();

    // normalize local estimate;
    QPair<double,double> normalizeLocEst(double mi, double mx);

signals:
    void localmodelChanged();

public slots:
    // update granularity and refresh the local estimate and local likelihood;
    //virtual void updateGranularity(int g) = 0;

protected:
    virtual void computeGlobEstimate() = 0;

    // initialize the local estimate and local likelihood
    //virtual void initialize() = 0;
    virtual void computeLocalEstimate() = 0;

    // evaluate sub class specific r script
    // and return two values
    //virtual void evalloc(const QString &s, const QString &e,
                 //double &est, double &conf) = 0;
    //virtual void evalglob(double &est, double &conf) = 0;

    double median(QVector<double>::const_iterator begin,
                  QVector<double>::const_iterator end);
public:
    FiveNumberTuple quartiles(QVector<double> vals);
};

#endif // MODELPROFILE_H
