#include "ModelProfile.h"

#include "DataModel.h"

#include <QPair>
#include <QDebug>
#include <QTime>
#include <typeinfo>

ModelProfile::ModelProfile(DataModel *dm, int w) :
    QObject(), dataModel(dm), winNum(w)
{
    //this->computeGlobEstimate(); // watch for this fucker;
}

ModelProfile::~ModelProfile()
{

}

void ModelProfile::summarizeGlobEst()
{
    // count number of time line with drift below 25%
    // 25% to 50%, // 50% to 75% // above 75%
    if(globEstimates.empty()) return;

    double q2 = std::get<1>(this->globEstSum);
    double q3 = std::get<2>(this->globEstSum);
    double q4 = std::get<3>(this->globEstSum);

    for(int i = 0; i< globEstimates.size(); i++) {
        if(globEstimates[i] < q2 )
            std::get<0>(globReverseLookup).insert(i);
        if(globEstimates[i] >= q2 && globEstimates[i] < q3 )
            std::get<1>(globReverseLookup).insert(i);
        if(globEstimates[i] >= q3 && globEstimates[i] < q4 )
            std::get<2>(globReverseLookup).insert(i);
        if(globEstimates[i] >= q4)
            std::get<3>(globReverseLookup).insert(i);
    }
    return;
}

void ModelProfile::summarizeLocEst()
{
}

void ModelProfile::summarizeLocLikli()
{
    // count number of time line with drift below 25%
    // 25% to 50%, // 50% to 75% // above 75%
/*    if(locLikelihood.empty()) return;
    if(locLikelihood[0].empty()) return;
    int n = locLikelihood.size();
    int m = locLikelihood[0].size();

    locLikliSum.resize(n);
    locReverseLookup.resize(n);  // this may fail;
    for(int i = 0; i< n; i++) {
        double q2 = std::get<1>(locLikliSum[i]);
        double q3 = std::get<2>(locLikliSum[i]);
        double q4 = std::get<3>(locLikliSum[i]);

        for(int j = 0; j< m; j++) {
            if(globEstimates[i] < q2 )
                std::get<0>(locReverseLookup[i]).insert(i);
            if(globEstimates[i] >= q2 && globEstimates[i] < q3 )
                std::get<1>(locReverseLookup[i]).insert(i);
            if(globEstimates[i] >= q3 && globEstimates[i] < q4 )
                std::get<2>(locReverseLookup[i]).insert(i);
            if(globEstimates[i] >= q4)
                std::get<3>(locReverseLookup[i]).insert(i);
        }
    }
    return;*/
}

QPair<double, double> ModelProfile::findMinMax()
{
    if(globEstimates.empty()) return qMakePair(0.0,1.0);

    double mx = this->globEstimates[0];
    double mi = this->globEstimates[0];

    for(auto& d : globEstimates) {
        mx = std::fmax(mx,d);
        mi = std::fmin(mi,d);
    }

    for(auto& vec : locEstimates) {
        for(double&d : vec) {
            mx = std::fmax(mx,d);
            mi = std::fmin(mi,d);
        }
    }
    return qMakePair(mi,mx);
}

QPair<double,double> ModelProfile::normalizeGlobEst(double mi, double mx)
{
    if(globEstimates.empty()) return qMakePair(0.0,1.0);
    //double mx = this->globEstimates[0];
    //double mi = this->globEstimates[0];
    //for(auto& d : globEstimates) {
        //mx = std::fmax(mx,d);
        //mi = std::fmin(mi,d);
    //}

    int n = globEstimates.size();
    this->normglobEstimates = QVector<double>(n,0);
    for(int i = 0; i< n; i++ ) {
        if(mx == mi) {
            normglobEstimates[i] = 0.5f;
        }else{
            normglobEstimates[i] = (globEstimates[i]-mi) / (mx-mi);
        }
    }
    if(mi==mx) return qMakePair(0.0,1.0);
    return qMakePair(mi,mx);
}

void ModelProfile::updateGlobEstNorm()
{
    double mx = globRange.second;
    double mi = globRange.first;

    int n = globEstimates.size();
    this->normglobEstimates = QVector<double>(n,0);
    for(int i = 0; i< n; i++ ) {
        if(mx == mi) {
            normglobEstimates[i] = 0.5f;
        }else{
            normglobEstimates[i] = (globEstimates[i]-mi) / (mx-mi);
        }
    }
    qDebug()<<"nomalized updateGlobEstNorm";
    return;
}

QPair<double, double> ModelProfile::normalizeLocEst(double mi, double mx)
{
    if(locEstimates.empty()) return qMakePair(0.0,1.0);
    if(locEstimates[0].empty()) return qMakePair(0.0,1.0);

    //double mx = globRange.second;
    //double mi = globRange.first;
    //for(auto& vec : locEstimates) {
        //for(double d : vec) {
            //mx = std::fmax(mx,d);
            //mi = std::fmin(mi,d);
        //}
    //}

    int n = locEstimates.size();
    if(n <= 0) return globRange;
    int m = locEstimates[0].size();
    if(m <= 0) return globRange;
    if(mi == mx) {
        normlocEstimates = QVector<QVector<double> >
                (n, QVector<double>(m,0.5f));
    }else{
        normlocEstimates = QVector<QVector<double> >
                (n, QVector<double>(m,0.0f));
        for(int i = 0; i< n; i++ ) {

            for(int j = 0; j< m; j++) {
                if(locEstimates[i][j] < mi) {
                    qDebug()<<"why the f "<<locEstimates[i][j]<<"<"<<mi;
                }

                normlocEstimates[i][j] = (locEstimates[i][j]-mi) / (mx-mi);

            }
        }
    }
    if(mi==mx) return qMakePair(0.0,1.0);
    qDebug()<<"nomalized";
    return qMakePair(mi,mx);
}

double ModelProfile::median(QVector<double>::const_iterator begin,
                            QVector<double>::const_iterator end)
{
    int len = end - begin;
    auto it = begin + len / 2;
    float m = *it;
    if ((len % 2) == 0) m = (m + *(--it)) / 2;
    return m;
}

FiveNumberTuple ModelProfile::quartiles(QVector<double> v)
{
    if(v.empty()) return std::make_tuple(0,0,0,0,0);
    // compute quartiles from
    qSort(v.begin(),v.end());

    auto it_second_half = v.begin() + v.size() / 2;
    auto it_first_half = it_second_half;
    if ((v.size() % 2) == 0) --it_first_half;

    double q0 = v[0];
    double q1 = median(v.begin(), it_first_half);
    double q2 = median(v.begin(), v.end());
    double q3 = median(it_second_half, v.end());
    double q4 = v[v.size()-1];
    return std::make_tuple(q0,q1,q2,q3,q4);
}

/*ModelProfile::ModelProfile(ModelProfile *p):
    QObject()
{
    dataModel = p->dataModel;

    profileName = p->profileName; //name of the model, including model configurations: paras

    isDerived = p->isDerived;

    selected = p->selected; //selected company in the profile;

    start = p->start; //start date of local model
    end = p->end; //end date of local model;

    //scaling the estimated trend slop;
    scale = p->scale;

    par = p->par;

    //current = p->current;

    //for volatility model, interval for calculate local volatility
    volInterval = p->volInterval;

    //for serial model,
    lag = p->lag; //number of days for lagging

    //QMap<QString, double> confidence; //normalized confidence value;
    layout = p->layout; //positioning the objects to 0-1 range based on model discrimination
    fit = p->fit;    //fitness of model;

    metric = p->metric;

    connect(this,SIGNAL(modelProfileExecuted()),
            dataModel,SLOT(updateModelProfile()));

    //connect(this,SIGNAL(modelProfileExecuted()),
    //        dataModel,SLOT(updateModelProfile()));
}

ModelProfile::ModelProfile(ModelProfile &p):
    QObject()
{
    dataModel = p.dataModel;

    profileName = p.profileName; //name of the model, including model configurations: paras

    isDerived = p.isDerived;

    selected = p.selected; //selected company in the profile;

    start = p.start; //start date of local model
    end = p.end; //end date of local model;

    //scaling the estimated trend slop;
    scale = p.scale;

    par = p.par;

    //current = p.current;

    //for volatility model, interval for calculate local volatility
    volInterval = p.volInterval;

    //for serial model,
    lag = p.lag; //number of days for lagging

    //QMap<QString, double> confidence; //normalized confidence value;
    layout = p.layout; //positioning the objects to 0-1 range based on model discrimination
    fit = p.fit;    //fitness of model;

    metric = p.metric;

    //connect(this,SIGNAL(modelProfileExecuted()),
    //        dataModel,SLOT(updateModelProfile()));
    //connect(this,SIGNAL(modelProfileExecuted()),
    //       dataModel,SLOT(updateModelProfile()));
}*/


/*void ModelProfile::manualRefreshView(){
    //emit modelProfileExecuted();
}*/

