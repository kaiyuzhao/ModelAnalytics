#ifndef DATAMODEL_H
#define DATAMODEL_H

#include "RInside.h"
#include "macro.h"
#include "ModelProfile.h"

#include <QObject>
#include <QStringListModel>
#include <QStack>
#include <QMutex>
#include <Glyph.h>

class DataModel: public QObject
{
    Q_OBJECT
public:
    explicit DataModel(RInside &r, QObject *parent = 0);

    ~DataModel();

    RInside &m_r;

    //stores price values
    PriceVec priceBuff;

    //normalized vlaues;
    PriceVec normPriceBuff;

    //range of prices before normalization;
    QMap<QString, QPair<double,double> > ranges;

    //stores date value
    DateVec dateBuff;

    //company name list model
    QStringListModel* compNameList;

    //model parameters; how to store the model properties
    //store confidence value as well?
    QString dataViewComp;

    QMap<QString,bool> dataSelected;

    //right pannel selected companies;
    QStringListModel* compNameSelected;

    //QStringList rightPannel;

    //QVector<double> dataViewPrice;
    QVector<ModelProfile*> modelProfiles;

    // user selected objects in the in the data view;
    QStack< QSet<int> > tlSelsData;

    // user selected objects in the model view;
    QStack< QSet<int> > tlSelsModel;

    // user brushed objects;
    QStack< QVector< QSet<int> > > tlBrushData;

    // user brushed objects;
    QStack< QVector< QSet<int> > > tlBrushModel;

    QList<Glyph> glyphs;

    // original selections in the list view;
    // full set;
    QSet<int> tlSelsListView;

    int maxColorRange;

    QMutex m;

    // user selected objects in the whole session;
    // in data view;
    //QSet<int> selectedTimeLinesData;

    //QSet<int> selectedTimeLines;

    // user selected objects in the whole session;
    // in model view;
    //QSet<int> selectedTimeLinesModel;

signals:
    // data changed;
    void dataSelectionChanged();

    //time line model executed;
    void timelinemodelChanged();

public slots:
    void updateModelProfile();
    void normalize();
    void updateWinNum(int);

private:
    int winNum;
};

#endif // DATAMODEL_H
