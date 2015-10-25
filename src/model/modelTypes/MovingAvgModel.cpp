#include "MovingAvgModel.h"
#include "RInside.h"
#include "DataModel.h"

#include <QDebug>


MovingAvgModel::MovingAvgModel(DataModel *dm, int w):
    ModelProfile(dm,w)
{
    modelName = QString("Moving Average");
    //this->initialize();
    qDebug()<<"MA model";
    this->computeGlobEstimate();
}


MovingAvgModel::~MovingAvgModel()
{

}

void MovingAvgModel::computeGlobEstimate()
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
            //NumericVector::iterator valit = val.begin();
            for(int i = 0; i< p.size() && i< val.size(); i++) val[i] = p[i];
            dataModel->m_r["val"] = val;
            double est = 0, conf = 0;
            //qDebug()<<typeid(this).name();
            dataModel->m_r.parseEval("fit<-Arima(val,order=c(0,0,1));");

            est = Rcpp::as<double>(dataModel->m_r.parseEval
                                   ("as.numeric(fit$coef[1]);"));
            conf = Rcpp::as<double>(dataModel->m_r.parseEval
                                    ("as.numeric(fit$aic);"));
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
    }
    catch (int e)
    {
        qDebug()<<"rcpp error "+e;
    }
}

void MovingAvgModel::computeLocalEstimate()
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

        //if(dataModel->priceBuff.values().empty()) return;
        //int len = dataModel->priceBuff.values()[0].size();

        locEstimates.clear();
        this->locEstimates.resize(winNum);

        locLikelihood.clear();
        locLikelihood.resize(winNum);

        locEstSum.clear();
        locEstSum.resize(winNum);

        locLikliSum.clear();
        locLikliSum.resize(winNum);

        QSet<int> curr = QSet<int> (dataModel->tlSelsListView);
        for(QSet<int>& s : dataModel->tlSelsData) curr.intersect(s);

        if(dataModel->priceBuff.values().empty()) return;
        int len = dataModel->priceBuff.values()[0].size();
        float winSize = (float)len/(float)winNum;

        for(int wIdx = 0; wIdx<winNum; wIdx++) {

            int start = wIdx*winSize;
            int end = (wIdx+1)*winSize;
            if(end>len) end = len;

            locEstimates[wIdx].clear();
            locEstimates[wIdx].resize(curr.size());

            locLikelihood[wIdx].clear();
            locLikelihood[wIdx].resize(curr.size());

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
                QString eval = "fit<-Arima(val["+s+":"+e+
                        "],order=c(0,0,1));";

                dataModel->m_r.parseEval(eval.toStdString());

                est = Rcpp::as<double>(dataModel->m_r.parseEval
                                       ("as.numeric(fit$coef[1]);"));
                conf = Rcpp::as<double>(dataModel->m_r.parseEval
                                        ("as.numeric(fit$aic);"));
                locEstimates[wIdx][idx] = est;
                locLikelihood[wIdx][idx] = std::fabs(conf);
                idx++;
            }
            locEstSum[wIdx] = quartiles(locEstimates[wIdx]);
            locLikliSum[wIdx] = quartiles(locLikelihood[wIdx]);
            //wIdx++;
        }
        //this->normalizeLocEst();
        //this->summarizeLocLikli();
        qDebug()<<"success, ma compute done in "<<ttt.elapsed()<<"ms";
    }
    catch (int e)
    {
        qDebug()<<"rcpp error "+e;
    }
}

/*void MovingAvgModel::initialize()
{
    using namespace Rcpp;
    using namespace std;
    try{
        dataModel->m_r.parseEval("require(forecast);");

        for(QString str: this->dataModel->compNameSelected->stringList()){
            std::vector<double> p = dataModel->
                    normPriceBuff.value(str).toStdVector();
            if(p.size()<2) return;
            NumericVector val(p.size());
            NumericVector::iterator valit = val.begin();
            for(int i = 0; i< p.size(); i++) valit[i] = p[i];
            dataModel->m_r["val"] = val;
            dataModel->m_r.parseEval("fit<-Arima(val,order=c(0,0,1));");
            double ma = Rcpp::as<double>(dataModel->m_r.parseEval
                                         ("as.numeric(fit$coef[1]);"));
            double loglik = Rcpp::as<double>(dataModel->m_r.parseEval
                                             ("as.numeric(fit$aic);"));
            this->globEstimates.push_back(ma);
            this->globLikelihood.push_back(-std::fabs(loglik));
        }
        globLikeliSum = quartiles(globLikelihood);
        globEstSum = quartiles(globEstimates);
        summarizeGlobEst();
        globRange = normalizeGlobEst();
        computeLocalEstimate();

        globRange = normalizeLocEst();
        updateGlobEstNorm();
    }
    catch (int e)
    {
        qDebug()<<"rcpp error "+e;
    }
}*/

/*void MovingAvgModel::updateGranularity(int g)
{
    this->granularity = g;
    //re fresh the local estimate and local likelihood;
    this->computeLocalEstimate();
    emit localmodelChanged();
}*/

void MovingAvgModel::evalloc(const QString &s, const QString &e, double &est, double &conf)
{
    try{

    }
    catch (int e)
    {
        qDebug()<<"rcpp error "+e;
    }
    return;
}

void MovingAvgModel::evalglob(double &est, double &conf)
{
    try{

    }
    catch (int e)
    {
        qDebug()<<"rcpp error "+e;
    }
}

/*void MovingAvgModel::computeLocalEstimate()
{
    using namespace Rcpp;
    using namespace std;
    try{
        //dataModel->m_r.parseEval("require(forecast)");
        int start = 0;
        int end = start + this->granularity;
        QStringList select = this->dataModel->compNameSelected->stringList();
        if(select.empty()) return;
        if(dataModel->normPriceBuff.values().empty()) return;
        int len = dataModel->normPriceBuff.values()[0].size();
        winNum = len/this->granularity;
        locEstimates.resize(winNum);
        locLikelihood.resize(winNum);

        locEstSum.resize(winNum);
        locLikliSum.resize(winNum);
        int wIdx = 0;
        for(; start< len && end<len; start+=granularity, end+=granularity) {
            if(wIdx>=winNum) wIdx = winNum-1;

            locEstimates[wIdx].resize(select.size());
            locLikelihood[wIdx].resize(select.size());

            int timLineIdx = 0;
            for(QString str: select){
                std::vector<double> p = dataModel->
                        normPriceBuff.value(str).toStdVector();

                if(p.size()<2) return;
                NumericVector val(p.size());
                NumericVector::iterator valit = val.begin();
                for(int i = 0; i< p.size(); i++) valit[i] = p[i];
                dataModel->m_r["val"] = val;

                QString s = QString::number(start+1);
                QString e = QString::number(end);
                QString eval = "fit<-Arima(val["+s+":"+e+
                        "],order=c(0,0,1));";

                double est = 0, conf = 0;

                eval

                locEstimates[wIdx][timLineIdx] = ma;
                locLikelihood[wIdx][timLineIdx] = -std::fabs(loglik);
                timLineIdx++;
            }
            locEstSum[wIdx] = quartiles(locEstimates[wIdx]);
            locLikliSum[wIdx] = quartiles(locLikelihood[wIdx]);
            wIdx++;
        }
    }
    catch (int e)
    {
        qDebug()<<"rcpp error "+e;
    }
}*/
