#include "Glyph.h"
#include <QDebug>
Glyph::Glyph()
{

}

void Glyph::calFiveSum()
{
    if(this->wId == -1) {
        // use global estimate
        this->summary.resize(models.size());
        for(int i = 0; i< models.size(); i++) {
            QVector<double> vals;
            int n = brush.size();
            vals.resize(n);
            for(int j = 0; j< n; j++) {
                int k = brush.values()[j];
                int kk = dToM[k];
                //qDebug()<<"kk "<<kk<<" k "<<k << " n "<<n;
                vals[j] = models[i]->normglobEstimates[kk];
            }
            this->summary[i] = models[i]->quartiles(vals);
            //this->sdev = this->std(vals);
        }
    }else{
        this->summary.resize(models.size());
        for(int i = 0; i< models.size(); i++) {
            QVector<double> vals;
            int n = brush.size();
            vals.resize(n);
            for(int j = 0; j< n; j++) {
                int k = brush.values()[j];
                int kk = dToM[k];
                //qDebug()<<"kk "<<kk<<" k "<<k << " n "<<n;
                vals[j] = models[i]->normlocEstimates[wId][kk];
            }
            this->summary[i] = models[i]->quartiles(vals);
            //qDebug()<<std::get<0>(summary[i]) <<" "<<std::get<4>(summary[i]);
            //this->sdev = this->std(vals);
        }
    }
}

double Glyph::std(QVector<double> vec)
{
    double mean = 0;
    double dev = 0;
    for(double d:vec){
        mean+=d/(double)vec.size();
    }
    for(double d:vec) {
        dev += (d - mean)*(d - mean)/(double)vec.size();
    }
    return dev;
}
