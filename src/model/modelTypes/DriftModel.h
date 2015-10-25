#ifndef DRIFTMODEL_H
#define DRIFTMODEL_H

#include "ModelProfile.h"
//#include "DataModel.h"

class DriftModel : public ModelProfile
{
    Q_OBJECT
public:
    DriftModel(DataModel* dataModel, int w);
    ~DriftModel();
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

#endif // DRIFTMODEL_H
