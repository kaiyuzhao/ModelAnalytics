#ifndef VOLATILITYMODEL_H
#define VOLATILITYMODEL_H

#include "ModelProfile.h"
//#include "DataModel.h"

class VolatilityModel : public ModelProfile
{
    Q_OBJECT
public:
    VolatilityModel(DataModel* dm, int w);
    ~VolatilityModel();

private:
    void initialize();

protected:
    //void updateGranularity(int g);
    void computeLocalEstimate();
};

#endif // VOLATILITYMODEL_H
