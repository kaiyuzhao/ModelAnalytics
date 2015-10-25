#ifndef SEASONALMODEL_H
#define SEASONALMODEL_H

#include "ModelProfile.h"
#include <QVector>
//#include "DataModel.h"

class SeasonalModel : public ModelProfile
{
    Q_OBJECT
public:
    SeasonalModel(DataModel* dm, int w);
    ~SeasonalModel();

private:
    //void initialize();

protected:
    //void updateGranularity(int g);

    void computeGlobEstimate();

    void computeLocalEstimate();

    void evalloc(const QString &s, const QString &e,
                 double &est, double &conf);
    void evalglob(double &est, double &conf);
};

#endif // SEASONALMODEL_H
