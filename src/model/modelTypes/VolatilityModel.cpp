#include "VolatilityModel.h"
#include "RInside.h"
#include "DataModel.h"
#include <QDebug>
#include <cmath>

VolatilityModel::VolatilityModel(DataModel *dm, int w):
    ModelProfile(dm, w)
{
    modelName = "Volatility";
    this->initialize();
}

VolatilityModel::~VolatilityModel()
{

}

void VolatilityModel::initialize()
{
    qDebug()<<"test";
//this->globLikeliSum = this->quartiles(this->globLikelihood);
}

/*void VolatilityModel::updateGranularity(int g)
{
    this->granularity = g;
    //re fresh the local estimate and local likelihood;
    this->computeLocalEstimate();
    emit localmodelChanged();
}*/

void VolatilityModel::computeLocalEstimate()
{

}
/*
void VolatilityModel::updateModelProfile()
{
    using namespace Rcpp;
    using namespace std;

    //if(this->metric) delete metric;

    profileName = "Volatility "+start.toString("MM/dd/yy")
            +" to "+end.toString("MM/dd/yy")
            +" segment "+QString::number(this->volInterval);

    QVector<double> segment;
    segment.resize(volInterval);
    for(QString str: selected){
        QVector<double> price = dataModel->priceBuff.value(str);
        QVector<QDate> date = dataModel->dateBuff.value(str);
        QVector<double> p;
        for(int i = 0; i<date.size(); i++){
            if(date[i]>=start && date[i]<=end){
                p.push_back(price[i]);
            }
        }
        //NumericVector val(p.size());
        typedef NumericVector::iterator NVit;
        //NVit it = val.begin();
        int j = 0; double avg = 0; double vol = 0; double sd = 0;

        QVector<double> vols;
        //vols.resize(volInterval);
        double allavg = average(p);
        vol = stddev(p,allavg);
        for(int i = 0; i<p.size();){
            if(j<segment.size()){
                segment[j++] = p[i];
                avg += p[i++]/(double)segment.size();
            }else{
                j = 0;
                //avg = avg/(double)segment.size();
                sd = stddev(segment,avg);
                //for(int k = 0; k<segment.size(); k++){
                //    sd+=(segment[k]-avg)*(segment[k]-avg)/(double)segment.size();
                //}
                //sd = sd/(double)segment.size();
                vols.push_back(sd);
                //vol = vol>sd?vol:sd;
                avg = 0; sd = 0;
            }
        }
        //qDebug()<<"vol "<<vol;
        dataModel->m_r["v"] = vol;
        double v = Rcpp::as<double>(dataModel->m_r.parseEval("atan(as.numeric(v))/pi*2;"));
        this->layout.insert(str,v);
        double confidence = conf(vols);
        this->fit.insert(str,confidence);
    }
    this->metric = new Metric(this);
    //qDebug()<<"volatility model "<<this->layout.size();
    //emit modelProfileExecuted();
}

QString VolatilityModel::whichObject(float v)
{
    float convert = v;
    typedef QMap<QString, double>::iterator LayoutIt;
    //QMap<QString, double>::iterator i;
    for(LayoutIt it = layout.begin();it!=layout.end(); ++it){
        if(fabs(it.value()-convert)<0.005){
            //qDebug()<<"key "<<it.key();
            return it.key();
        }
    }
    return "";
}

double VolatilityModel::average(QVector<double> vals)
{
    if(vals.empty()) return 0;
    double avg = 0;
    for(int i = 0; i<vals.size(); i++){
        avg+=vals[i]/(double)vals.size();
    }
    return avg;
}

double VolatilityModel::stddev(QVector<double> vals, double avg)
{
    if(vals.empty()) return 0;
    double sd = 0;
    for(int i = 0; i<vals.size(); i++){
        sd+=(vals[i]-avg)*(vals[i]-avg)/(double)vals.size();
    }
    return sd;
}

double VolatilityModel::conf(QVector<double> vols)
{
    using namespace Rcpp;
    double avgvol = average(vols);
    double sdvol = stddev(vols,avgvol);
    dataModel->m_r["conf"] = sdvol;
    double conf = Rcpp::as<double>(dataModel->m_r.parseEval("1-atan(as.numeric(conf))/pi*2;"));
    return conf;
}


*/
