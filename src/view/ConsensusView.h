#ifndef CONSENSUSVIEW_H
#define CONSENSUSVIEW_H

#include <QWidget>
#include <QPainter>
#include <QRectF>

#include "macro.h"

class MainWindow;
class DataModel;

class ConsensusView : public QWidget
{
    Q_OBJECT


public:
    // radar representation;
    class GlyphRadar{
    public:
        double start;  // start degree of pie
        double end;   // end degree of pie for time range;
        QVector<FiveNumberTuple> est; // estimate of models mapped to radius;
        // currently selected time lines;
        QSet<int> indices;
        int whichModel;
        bool active;
    };

    //bool aggregate

    int glyphSize; // size of glyph; 50x50 by default

    int margin;  // 25 by default

    int space; // 5 default

    int inner; // min inner radius of the radar glyph;

    int outter; // max outter radius of the radar glyph; outter = size/2 - band

    int ring; // ring width for time range;

    explicit ConsensusView(MainWindow *mw, QWidget *parent = 0);

    void paintEvent(QPaintEvent *e);

    void mouseMoveEvent(QMouseEvent *e);

    void mousePressEvent(QMouseEvent *e);

private:
    DataModel *dm;

    MainWindow * mainWin;

    QVector<QRectF> glyphRects;

    QVector<GlyphRadar> glyphRadars;

    //distance to previous selections;
    QVector<double> similarity;

    void drawGlyph(GlyphRadar rd, QRectF r, QPainter &p);

    QVector<QColor> schema;

    QRectF highlight;

    int selectedGlyph;

    // threshold for similarity of user selection;
    double simThres;

    //update dist vector;
    void distanceFunc();

signals:
    void indexAggregated(QSet<int> idx);
    
public slots:
    void updateGlyphs();

    void updateThreshold(double d);

    void updateAggregation();
    
};

#endif // CONSENSUSVIEW_H
