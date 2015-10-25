#include "metric.h"
#include "ModelProfile.h"

#include <QtAlgorithms>
#include <QVector>
#include <utility>
#include <QDebug>


Metric::Metric(ModelProfile* mp, QObject *parent) :
    profile(mp),
    QObject(parent)
{
    //profile = mp;
    //initialize();
}

Metric::~Metric()
{

}

/**
 * @brief Metric::outlierScore
 * update outlier score;
 */
/*void Metric::outlierScore()
{
    //IQR range 1.5?
    //count percentage of data falls outside of the range.
    qSort(modelOutput.begin(),modelOutput.end());
    auto q = quartiles(modelOutput);
    float q1 = q.first;
    float q3 = q.second;
    float iqr = q3-q1;
    int cnt = 0;
    for(auto & val : modelOutput) {
        if(val < q1-1.5f*iqr || val > q3+1.5f*iqr) cnt++;
    }
    // the maximum number of outliers is less than 50% size of
    // original vector; normalized by 2;
    this->outlier = (float)cnt/(float)modelOutput.size()*2.0f;
}*/

/**
 * @brief Metric::clusterScore
 * update the cluster score using binning method
 * need fine tuning;
 */
/*void Metric::clusterScore()
{
    if(this->modelOutput.empty()) return;
    int num = 30; // hard coded;

    this->cluster = 0;

    while(num>3) {
        QVector<float> bins(num, 0);

        binning(modelOutput,bins,num);

        float current = 0;
        float mx = 0.0f;
        for(int i= 1; i<bins.size(); i++) {
            float c = fabs(bins[i]-bins[i-1]);
            current += c;
            mx = fmax(mx,c);
        }
        if(mx != 0.0f)
            this->cluster = fmax(cluster,current/mx);
        num = num>>1;
    }
    //cluster = cluster/5.0f;
    //if(cluster>1.0f) cluster = 1.0f;
    qDebug()<<"cluster "<<cluster;
}

void Metric::spreadScore()
{
    if(this->modelOutput.empty()) return;
    float mx = 0;
    float mi = 0;
    for(auto& val : this->modelOutput) {
        mx = fmax(mx,val);
        mi = fmin(mi,val);
    }
    this->spread = mx-mi;
    return;
}

void Metric::initialize()
{
    int n = profile->layout.size();
    //this->modelOutput.fill(n,0);
    this->cluster = 0;
    this->outlier = 0;
    this->spread = 0;
    for(auto v : profile->layout.keys()) {
        this->modelOutput.push_back(profile->layout.value(v));
    }
    this->spreadScore();
    this->outlierScore();
    this->clusterScore();
}*/

/*void Metric::binning(QVector<float> &output, QVector<float> &bins, int num)
{
    if(this->modelOutput.empty()) return;
    bins.fill(0,num);

    float mx = 1;
    float mi = -1;

    /*for(auto& val : this->modelOutput) {
        mx = std::max(mx,val);
        mi = std::min(mi,val);
    }//

    for(auto v : output) {
        if(v >= mx) {
            bins[num-1]++;
            continue;
        }
        if(v <= mi) {
            bins[0]++;
            continue;
        }
        int i = (v-mi)/(mx - mi) * num;
        bins[i]++;
    }
    return;

}*/

/*float Metric::median(QVector<float>::const_iterator begin, QVector<float>::const_iterator end)
{
    int len = end - begin;
    auto it = begin + len / 2;
    float m = *it;
    if ((len % 2) == 0) m = (m + *(--it)) / 2;
    return m;
}

std::pair<float, float> Metric::quartiles(const QVector<float> &v)
{
    auto it_second_half = v.begin() + v.size() / 2;
    auto it_first_half = it_second_half;
    if ((v.size() % 2) == 0) --it_first_half;

    float q1 = median(v.begin(), it_first_half);
    //float q2 = median(v.begin(), v.end());
    float q3 = median(it_second_half, v.end());
    return std::make_pair(q1, q3);
}


void Metric::metricUpdated()
{
}
*/
