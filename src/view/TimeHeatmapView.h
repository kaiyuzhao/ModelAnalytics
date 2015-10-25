#ifndef TIMEHEATMAPVIEW_H
#define TIMEHEATMAPVIEW_H

#include <QWidget>
#include <QPair>
#include <QStack>
#include <QPainter>
#include <QSet>

class DataModel;
class MainWindow;
class TimeHeatmapView : public QWidget
{
    Q_OBJECT
public:
    explicit TimeHeatmapView(MainWindow* mw, QWidget *parent = 0);
    ~TimeHeatmapView();


    void paintEvent(QPaintEvent *e);

    void mousePressEvent(QMouseEvent *e);

    void mouseMoveEvent(QMouseEvent *e);

private:
    void drawAxes(QPainter& p);

    void drawSelection(QPainter& p);

    void drawHopover(QPainter& p);

    void drawColorLegend(QPainter& p);


private:
    DataModel* dm;

    int numIntvl;  // number of grids horizontally
    int numBins;  //  number of grids vertically

    int numTimeLines; // total number of time series;
    int maxCount;  // max count;
    // color the grid map using this value and the maxCount;
    int colorMaxCnt;

    int margin;

    QVector<QColor> schemas;

    QVector<QVector<int> > grids;  // count of stocks in a particular grid;
    QVector<QVector<int> > sgrids;  // count of stocks in a particular grid; selected

    QVector<QVector<QRect> > rects;  // corresponding rect (x,y,w,h) of particular grid;

    QVector<QVector<QSet<int> > > reverseLookup; //indices of stock price in each grid
    QVector<QVector<QSet<int> > > cnt;  // copy to store indeces after selection;

    // selected time line indices in the list view;
    //QSet<int> selectedListView;

    // currently selected time line indicies;
    QSet<int> selected;

    // highlighted time line indicies;
    QSet<int> hopovered;

    QPoint mouseMove;

    QStack<QPair<int,int> > trace; // list of selections;
    QPair<int,int> coord; // coordinate of current highlighted gird;

    //normalized copy of stock price;
    QVector<QVector<double> > norm;
    // normalize stock price;
    // save it into norm;
    void normalize(QVector<QVector<double> > &norm);

    // fill heatmap with count of time lines
    // and index of time lines in each grid;
    // return max count of all grid cells
    int fillMap(int itvl, int bins, QVector<QVector<double> > &norm,
                 QVector<QVector<QSet<int> > >& reverse,
                 QVector<QVector<int> > &grids);

    void fillSelectedMap(int itvl, int bins,
                        const QVector<QVector<double> > &norm,
                        QVector<QVector<QSet<int> > >&cnt,
                        QVector<QVector<int> > &sgrids);

signals:
    void subsetSelected();
    void colorRangeChanged(int);

public slots:
    void updateTimeHeatmapView();
    void updateBins(int b);
    void updateIntvls(int i);
    void updateSelection();
    void updateColors(int);

    void updateIndexSelection(QSet<int> idx);
};

#endif // TIMEHEATMAPVIEW_H
