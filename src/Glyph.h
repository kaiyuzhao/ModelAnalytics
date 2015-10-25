#ifndef GLYPH_H
#define GLYPH_H


#include "ModelProfile.h"
#include "macro.h"

#include <QVector>
#include <QSet>


class Glyph{
public:
    Glyph();
    QVector<ModelProfile*> models;
    int wId;  // which local partition; -1 means global
    int wSize;  // number of partitoins;
    int whichModel; // drift 0, MA 1, season 2;
    QSet<int> brush;
    QMap<int,int> dToM;

    double sdev;
    QVector<FiveNumberTuple> summary;
    void calFiveSum();
    double std(QVector<double> vec);
};

#endif // GLYPH_H
