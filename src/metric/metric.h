#ifndef METRIC_H
#define METRIC_H


#include <QObject>
#include <QVector>
#include <RInside.h>
#include <utility>
//#include "ModelProfile.h"


/**
 * @brief The Metric class
 *
 * for each model executed, this metric measures several
 * aspects of the model output, outlier, clustering, and spread;
 */

class ModelProfile;
//class QPair<float,float>;

class Metric : public QObject
{
    Q_OBJECT
public:
    explicit Metric(ModelProfile* mp, QObject *parent = 0);

    Metric(Metric *m);

    ~Metric();

    //RInside &m_r;

    /*QVector<float> modelOutput;

    float outlier;

    float cluster;

    float spread;

    void outlierScore();

    void clusterScore();

    void spreadScore();*/

private:
    ModelProfile* profile;

    /*void initialize();

    void binning(QVector<float>& output, QVector<float>& bins, int num = 10);

    float median(QVector<float>::const_iterator begin,
                QVector<float>::const_iterator end);

    std::pair<float, float> quartiles(const QVector<float>& v);*/

signals:
    void metricChanged();
    
public slots:
    /*void metricUpdated();*/
};

#endif // METRIC_H
