#ifndef MOVINGAVGMODEL_H
#define MOVINGAVGMODEL_H

#include "ModelProfile.h"
//#include "DataModel.h"


class MovingAvgModel : public ModelProfile
{
    Q_OBJECT
public:
    MovingAvgModel(DataModel *dm, int w);
    ~MovingAvgModel();
private:
    //void initialize();
protected:
    //void updateGranularity(int g);

    void computeGlobEstimate();

    void computeLocalEstimate();

    //void computeLocalEstimate();
    void evalloc(const QString &s, const QString &e,
                 double &est, double &conf);
    void evalglob(double &est, double &conf);


};

#endif // MOVINGAVGMODEL_H
