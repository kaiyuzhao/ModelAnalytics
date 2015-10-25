#include "DataModel.h"
#include <QPair>

#include "DriftModel.h"
#include "MovingAvgModel.h"
#include "SeasonalModel.h"
#include "VolatilityModel.h"

DataModel::DataModel(RInside &r, QObject *parent):
    m_r(r)
{
    compNameList = new QStringListModel();
    compNameSelected = new QStringListModel();
    maxColorRange = 1;
    winNum = 5;
}

DataModel::~DataModel()
{
    if(compNameList) delete compNameList;
    if(compNameSelected) delete compNameSelected;
}

void DataModel::normalize()
{
    //this->normPriceBuff.clear();
    //if(compNameSelected->stringList().empty()) return;
    if(priceBuff.empty()) return;
    //int n = this->dm->priceBuff.values().size();
    int n = compNameSelected->stringList().size();

    for(int i = 0; i< n; i++){
        QString k = compNameSelected->stringList()[i];
        if(normPriceBuff.contains(k)) continue;
        if(priceBuff.value(k).empty()) continue;

        QVector<double> norm(priceBuff.value(k).size());
        //norm.resize(dm->priceBuff.value(k).size());
        double max = priceBuff.value(k)[0];
        double min = priceBuff.value(k)[0];

        for(int j = 0; j< priceBuff.value(k).size(); j++) {
            double d = priceBuff.value(k)[j];
            max = std::fmax(max, d);
            min = std::fmin(min, d);
        }
        //this->ranges.insert(k,qMakePair(min,max));
        if(max==min) {
            this->ranges.insert(k,qMakePair(min-10.0f,max+10.0f));
        }else{
            this->ranges.insert(k,qMakePair(min,max));
        }
        for(int j = 0; j< norm.size(); j++) {
            if(max == min) {  // norm[i] are all duplicates;
                norm[j] = 0.5f;
                continue;
            }
            norm[j] = (priceBuff.value(k)[j] - min)/(max - min);
        }
        normPriceBuff.insert(k,norm);
    }
    emit dataSelectionChanged();
}

void DataModel::updateWinNum(int x)
{
    this->winNum = x;
}

void DataModel::updateModelProfile()
{
    if(!m.tryLock()) return;
    for(ModelProfile* p : this->modelProfiles) {
        if(p) delete p;
    }
    this->modelProfiles.clear();
    this->tlSelsModel.clear();
    ModelProfile* driftModel = new DriftModel(this, winNum);
    ModelProfile* movingAvg = new MovingAvgModel(this, winNum);
    ModelProfile* season = new SeasonalModel(this, winNum);
    //ModelProfile* volatility = new VolatilityModel(this);
    this->modelProfiles.push_back(driftModel);
    this->modelProfiles.push_back(movingAvg);
    this->modelProfiles.push_back(season);
    //this->modelProfiles.push_back(volatility);
    m.unlock();
    emit timelinemodelChanged();
}
