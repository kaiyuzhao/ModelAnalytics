#ifndef MACRO_H
#define MACRO_H
#include <QVector>
#include <QDate>
#include <QString>
#include <QMap>
#include <QHash>
#include <QSet>
#include <QColor>
#include <utility>
#include <tuple>
#include <QRectF>


typedef QMap<QString, QVector<double> > PriceVec;
typedef QMap<QString, QVector<QDate> > DateVec;

typedef std::tuple<double,double,double,double,double> FiveNumberTuple;

typedef std::tuple<QSet<int>,QSet<int>,QSet<int>,QSet<int> > QuartileHash;

typedef QVector<QRectF> HistoRect;

#endif // MACRO_H
