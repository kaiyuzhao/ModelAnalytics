#ifndef MODELRELATIONVIEW_H
#define MODELRELATIONVIEW_H

#include "mainwindow.h"
#include <QWidget>
//#include <QGraphicsView>
//#include <QGraphicsObject>


//class QGraphicsView;

class ModelRelationView : public QWidget
{
    Q_OBJECT
public:
    explicit ModelRelationView(MainWindow* mw, QWidget *parent = 0);

    ~ModelRelationView();

    QWidget *m_parent;

    DataModel* dataModel;

    float margin;

    void paintEvent(QPaintEvent *e);

    void mousePressEvent(QMouseEvent *e);

    void mouseMoveEvent(QMouseEvent *e);

    //void resizeEvent(QResizeEvent *e);

    QMap<QString, QVector<double> > glyphProfile;

    QVector<QPointF> pos;

    QMap<QString, QRectF> profileRect;

    QString highlightName;
    int highlightRectIdx;
    QRectF mouseRect;

private:
    //void pcomp(QMap<QString, QVector<double> > &profile);
    //void drawProfile(QPainter &painter);
    //QGraphicsView *view;
    //QGraphicsScene *scene;

signals:

    
public slots:
    //void updateModelRelationView();
};

/*class QGraphicsGlyphItem: public QGraphicsObject{
    Q_OBJECT
public:
    QGraphicsGlyphItem(ModelRelationView* parent, QString objName, float xpos, float ypos);

    ModelRelationView* mrv;
    QString name;

    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    float x;
    float y;

    float w;
    float h;
public slots:

};*/

#endif // MODELRELATIONVIEW_H
