#ifndef MODELVIEW_H
#define MODELVIEW_H

#include <QWidget>
#include <QVector>
#include <QMap>
#include <QModelIndex>
#include <QPainter>
#include <QSet>
#include <QStack>

#include "macro.h"
//#include <QPainterPath>


class QPainterPath;
class MainWindow;
class DataModel;
class ModelProfile;
class ModelView : public QWidget
{
    Q_OBJECT
public:
    // count of estimated parameters in a range;
    // for example 0 to 0.1;
    // this is for global estimate;
    class Histogram{
    public:
        QVector<int> histo;
        QVector<QSet<int> > lookup;
        HistoRect rects;  // rectangles for each bar of histogram;
        QRectF bouding;  // bouding box for histogram;
    };

    class Model{
    public:
        int maxCnt;
        Histogram glHisto;  // one global histogram
        QVector<Histogram> locHisto; // several local histogram;
        QRectF bounding;  // model bounding box;
        bool operator==(Model &m) {
            return this == &m;
        }
    };

    explicit ModelView(MainWindow *main, QWidget* parent = 0);
    ~ModelView();

    DataModel* dataModel;

    void paintEvent(QPaintEvent *e);

    void drawHopover(QPainter &p);

    void drawSelections(QPainter &p);

    void mousePressEvent(QMouseEvent *);

    void mouseMoveEvent(QMouseEvent *);

    void mouseReleaseEvent(QMouseEvent *e);


private:

    /**
     * @brief drawModel
     * @param p
     * @param bouding  bouding box of the drawing space;
     */
    void drawModel(QPainter &p, QRectF bouding, ModelProfile* mp, Model& m, int idx);

    /**
     * @brief drawTimeTicks
     * @param p
     * @param bouding
     */
    void drawTimeTicks(QPainter &p, QRectF bouding, Model &m);

    /**
     * @brief drawBoxPlot
     * @param p
     * @param bouding bouding box
     * @param t  five number summary
     * @param minVal  min value of five number summary of all parallel displays
     * @param maxVal  max value of ....
     * @param widthScale  width of box plot regarding the bounding box;
     */
    void drawBoxPlot(QPainter &p, QRectF bouding,
                     const FiveNumberTuple t,
                     double minVal = 0, double maxVal = 0,
                     double widthScale = 1.0f);

    void drawHistogramLabel(QPainter &p, QRectF bouding,
                            ModelProfile *mp);

    //void drawHistogram(QPainter &p, QRectF bouding,
                       //const Model &m);
    void drawHistogram(QPainter &p, QRectF bouding,
                       Histogram &hist,
                       int maxCnt);

    //draw local model range
    void drawLocalModelDates(QPainter&p, QRectF bouding,
                             QDate start, QDate end, bool g);

    int margin;

    float margin2; // margin for histogram

    float margin3;  // margin for boxplot

    QVector<Model> models;

    int binSize;

    void fillValues();

    QVector<QColor> schemas;

    QVector<QColor> bgschema;

    int maxColorRange;

    int colorRange;

    // highlight histogram bar;
    QRectF highlight;

    // highlight selected time lines;
    // draw their name while mouse over;
    QSet<int> highlightSelection;

    // mouse position when hop over histogram bars;
    QPointF mPos;

    // id of selected time lines in the dataView;
    //QSet<int> selectionData;
    QSet<int> selectionModel;

    // a stack of rectangles represents the selections;
    QStack<QRectF> selectionsRect;

    // brush selection of rectangles
    QStack<QVector<QRectF> > brushSelection;

    void calculateIntersetion();

    bool isBoxPlot;

    bool mouseDrag;

    bool mousepress;

    QRectF brushBox;

    QMap<int,int> dToM;

signals:
    void timelineSelectionChanged();
    void colorRangeChanged(int);
    void clusterAdded();

public slots:
    void updateBinSize(int x);
    void updateModelView();
    void updateColors(int x);
    void updateBoxPlot(bool);

    // update the rectangle selections here;
    void updateTimeLineSelection(QSet<int> idx);
};

#endif // MODELVIEW_H
