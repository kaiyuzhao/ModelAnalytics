#ifndef MODELEVALVIEW_H
#define MODELEVALVIEW_H


#include <QWidget>
#include <QVector>
#include <utility>
#include "ModelProfile.h"

class DataModel;
class MainWindow;
class ModelProfile;
class QComboBox;
class QHBoxLayout;
class QVBoxLayout;
class QGroupBox;
//class ModelProfile;


class ModelEvalView : public QWidget
{
    Q_OBJECT
public:
    MainWindow* mainWin;

    ///QWidget* par;

    DataModel* dataModel;

    explicit ModelEvalView(MainWindow* mw, QWidget *parent = 0);

    ~ModelEvalView();

    void paintEvent(QPaintEvent *e);

    void mouseMoveEvent(QMouseEvent *e);

    // return min and max;
    //static std::pair<float, float> normalize(QVector<float> & vec);
    //static std::pair<float, float> normalize(QVector<float> & vec, float l, float h);

signals:
    //void hopoverGlyph(int); //profile ID

public slots:
    //void updateModelEvalView();
    //void comboXSelected(int idx);
    //void comboYSelected(int idx);

    //void highlightGlyph(int); // profile ID
private:
    QVector<QColor> colorSchema;

    int top;
    int margin;
    int glyphSize;

    //int glyphHighlight;

    //QVector<QRect> glyphsVec;

    //QMap<ModelProfile*, QRect> glyphs;

    //

    //QComboBox* xAxis;
    //QComboBox* yAxis;

    //QVector<float> outliers;
    //QVector<float> clusters;
    //QVector<float> spreads;

    //QVector<float> xs;
    //QVector<float> ys;
    //std::pair<float,float> xrange;
    //std::pair<float,float> yrange;

    /*std::pair<float,float> oRange;  //0 to 1
    std::pair<float,float> cRange;  //0 to 5?
    std::pair<float,float> sRange;  //0 to 1

    QHBoxLayout* groupLayout;
    QGroupBox* groupBox;
    QVBoxLayout* mainLayout;

    int xAxSeleciton;
    int yAxSelection;
*/
    //void initialize();
    //int screenToData(int x, int y);

    // draw ticks on two axes
    // draw label on two axes
    // draw value ranges on two axes;
    //void drawAxes(QPainter &p);

    // draw linkage between global and local models;
    // void drawLinkage(QPainter &p);
};

#endif // MODELEVALVIEW_H
