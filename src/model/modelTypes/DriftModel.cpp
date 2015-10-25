#include "DriftModel.h"
#include "DataModel.h"
#include "RInside.h"


#include <QDebug>
#include <QVector>

#include <vector>
#include <QTime>


DriftModel::DriftModel(DataModel *dataModel, int w):
    ModelProfile(dataModel,w)
{
    modelName = QString("Drift");
    //this->initialize();
    qDebug()<<"Drift model";
    this->computeGlobEstimate();
}

DriftModel::~DriftModel()
{
}

void DriftModel::computeGlobEstimate()
{
    using namespace Rcpp;
    using namespace std;
    try{
        dataModel->m_r.parseEval("require(forecast);");
        QStringList select = this->dataModel->compNameSelected->stringList();
        if(select.empty()) return;
        QVector<QString> names;
        for(auto str : select) names.push_back(str);
        if(dataModel->normPriceBuff.values().empty()) return;


        QSet<int> curr = QSet<int> (dataModel->tlSelsListView);
        for(QSet<int>& s : dataModel->tlSelsData) curr.intersect(s);

        for(int k = 0; k<names.size(); k++){
            if(!curr.contains(k)) continue;
            QString str = names[k];
            std::vector<double> p = dataModel->
                    priceBuff.value(str).toStdVector();
            if(p.size()<2) return;
            NumericVector val(p.size());
            NumericVector::iterator valit = val.begin();
            for(int i = 0; i< p.size(); i++) valit[i] = p[i];
            dataModel->m_r["val"] = val;
            double est = 0, conf = 0;
            //qDebug()<<typeid(this).name();
            dataModel->m_r.parseEval("fit<-Arima(val,order=c(0,1,0),"
                                     "include.drift=TRUE);");
            //qDebug()<<"eval in success 2";
            est = Rcpp::as<double>(dataModel->m_r.parseEval
                                            ("as.numeric(fit$coef[1]);"));
            conf = Rcpp::as<double>(dataModel->m_r.parseEval
                                             ("as.numeric(fit$aic);"));
            //qDebug()<<"eval success";
            this->globEstimates.push_back(est);
            this->globLikelihood.push_back(std::fabs(conf));
        }
        globLikeliSum = quartiles(globLikelihood);
        qDebug()<<"done1";
        //qDebug()<<"done2";
        computeLocalEstimate();
        qDebug()<<"done3";
        globRange = findMinMax();
        normalizeGlobEst(globRange.first,globRange.second);
        qDebug()<<"done4";
        normalizeLocEst(globRange.first,globRange.second);
        qDebug()<<"done5";
        //updateGlobEstNorm();
        //qDebug()<<"done6";
    }
    catch (int e)
    {
        qDebug()<<"rcpp error "+e;
    }
}

void DriftModel::computeLocalEstimate()
{
    using namespace Rcpp;
    using namespace std;
    try{
        //dataModel->m_r.parseEval("require(forecast)");
        QTime ttt;
        ttt.start();

        QStringList select = this->dataModel->compNameSelected->stringList();
        QVector<QString> names;
        for(auto str : select) names.push_back(str);
        //qDebug()<<select.size();
        if(select.empty()) return;
        if(names.empty()) return;

        this->locEstimates.clear();
        this->locEstimates.resize(winNum);
        this->locLikelihood.clear();
        this->locLikelihood.resize(winNum);

        locEstSum.clear();
        locEstSum.resize(winNum);
        locLikliSum.clear();
        locLikliSum.resize(winNum);

        QSet<int> curr = QSet<int> (dataModel->tlSelsListView);
        for(QSet<int>& s : dataModel->tlSelsData) curr.intersect(s);

        if(dataModel->priceBuff.values().empty()) return;
        int len = dataModel->priceBuff.values()[0].size();
        float winSize = (float)len/(float)winNum;
        //qDebug()<<winSize;

        for(int wIdx = 0; wIdx<winNum; wIdx++) {
        //for(; start< len && end<len; start+=granularity, end+=granularity) {
            //if(wIdx>=winNum) wIdx = winNum-1;
            int start = wIdx*winSize;
            int end = (wIdx+1)*winSize;
            if(end>len) end = len;

            locEstimates[wIdx].clear();
            locEstimates[wIdx].resize(curr.size());

            locLikelihood[wIdx].clear();
            locLikelihood[wIdx].resize(curr.size());
            //int timLineIdx = 0;
            // for each selected time line object
            // and I want only the active objects;
            int idx = 0;
            for(int timLineIdx = 0; timLineIdx< names.size(); timLineIdx++){
                //QString str: select
                if(!curr.contains(timLineIdx)) continue;
                QString str = names[timLineIdx];
                std::vector<double> p = dataModel->
                        priceBuff.value(str).toStdVector();
                if(p.size()<2) return;
                NumericVector val(p.size());
                NumericVector::iterator valit = val.begin();
                // for each value in a time line object;
                for(int i = 0; i< p.size(); i++) valit[i] = p[i];
                dataModel->m_r["val"] = val;
                QString s = QString::number(start+1);
                QString e = QString::number(end);
                double est = 0, conf = 0;
                QString evals = "fit<-Arima(val["+s+":"+e+
                        "],order=c(0,1,0),include.drift=TRUE);";

                dataModel->m_r.parseEval(evals.toStdString());

                est = Rcpp::as<double>(dataModel->m_r.parseEval
                                                ("as.numeric(fit$coef[1]);"));
                conf = Rcpp::as<double>(dataModel->m_r.parseEval
                                                 ("as.numeric(fit$aic);"));
                locEstimates[wIdx][idx] = est;
                locLikelihood[wIdx][idx] = std::fabs(conf);
                idx++;
                //qDebug()<<"loc eval 3";
            }
            locEstSum[wIdx] = quartiles(locEstimates[wIdx]);
            locLikliSum[wIdx] = quartiles(locLikelihood[wIdx]);
            //wIdx++;
        }
        //this->normalizeLocEst();
        //this->summarizeLocLikli();
        qDebug()<<"success, loc compute done in "<<ttt.elapsed()<<"ms";
    }
    catch (int e)
    {
        qDebug()<<"rcpp error "+e;
    }
}

/*void DriftModel::updateGranularity(int g)
{
    this->granularity = g;
    //re fresh the local estimate and local likelihood;
    computeLocalEstimate();
    emit localmodelChanged();
}*/

void DriftModel::evalloc(const QString &s, const QString &e,
                      double &est, double &conf)
{
    using namespace Rcpp;
    using namespace std;
    try{

        return;
    }
    catch (int e)
    {
        qDebug()<<"rcpp error "+e;
    }

}

void DriftModel::evalglob(double &est, double &conf)
{
    try{
        //qDebug()<<"eval in success 1";

    }
    catch (int e)
    {
        qDebug()<<"rcpp error "+e;
    }

}

/*void DriftModel::updateModelProfile()
{
    using namespace Rcpp;
    using namespace std;

    //if(this->metric) delete metric;

    profileName = "Trend "+start.toString("MM/dd/yy")
            +" to "+end.toString("MM/dd/yy")
            +" scale "+QString::number(this->scale);

    for(QString str: selected){
        QVector<double> p = dataModel->priceBuff.value(str);
        QVector<QDate> date = dataModel->dateBuff.value(str);

        QVector<double> local;
        for(int i = 0; i<date.size(); i++){
            if(date[i]>=start && date[i]<=end){
                local.push_back(p[i]);
            }
        }

        NumericVector yval(local.size());
        NumericVector xval(local.size());

        typedef NumericVector::iterator NVit;
        NVit yit = yval.begin();
        NVit xit = xval.begin();

        for(int j = 0; j<local.size(); j++){
            yit[j] = local[j];
            xit[j] = (double)j/scale;
        }

        dataModel->m_r["y"] = yval;
        dataModel->m_r["x"] = xval;
    // atan(as.numeric(lm(y~x)$coefficients[-1]))/pi*2;
        double trend = Rcpp::as<double>(dataModel->m_r.parseEval(
                                            "fit<-lm(y~x); atan(as.numeric(fit$coefficients[-1]))/pi*2;"));
        this->layout.insert(str,trend);

        double rsqr = Rcpp::as<double>(dataModel->m_r.parseEval("summary(fit)$r.squared;"));
        this->fit.insert(str,rsqr);
        //qDebug()<<"fit "<<rsqr;
    }
    this->metric = new Metric(this);
    //qDebug()<<"trend model "<<this->layout.size();
    //emit modelProfileExecuted();
}

QString DriftModel::whichObject(float v)
{
    float convert = (v-0.5f)*2.0f;

    typedef QMap<QString, double>::iterator LayoutIt;

    //QMap<QString, double>::iterator i;
    //qDebug()<<"convert "<<convert;
    for(LayoutIt it = layout.begin();it!=layout.end(); ++it){
        if(fabs(it.value()-convert)<0.005f){
            //qDebug()<<"key "<<it.key();
            return it.key();
        }
    }
    return "";
}*/
