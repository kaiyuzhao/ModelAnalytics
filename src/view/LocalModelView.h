#ifndef LOCALMODELVIEW_H
#define LOCALMODELVIEW_H

#include <QWidget>

class DataModel;
class MainWindow;
class ModelProfile;

class LocalModelView : public QWidget
{
    Q_OBJECT
public:
    explicit LocalModelView(MainWindow* mw, QWidget *parent = 0);

    ~LocalModelView();
    
    MainWindow* mainWin;

    DataModel* dataModel;

    ModelProfile* gProfile;

    int selectedProfileID;

    QVector<ModelProfile* > localProfiles; //derived local models;

    QVector<QColor> colorSchema;

    //QVector<double> distance;

    //double maxDist;
    //double minDist;

    //QVector< QVector<double> > infoGain;

    QVector<QRect> localRect;

    QVector<float> outliers;
    QVector<float> clusters;
    QVector<float> spreads;
    //QVector<QString> localName;
    void paintEvent(QPaintEvent *e);

    void mousePressEvent(QMouseEvent *e);

    void mouseMoveEvent(QMouseEvent *e);

    //ModelProfile* makeCopy(ModelProfile* profile);


signals:
    void localModelAdded();

public slots:
    //void updateLocalRelationView(int profileID, int g);

private:
    int margin;

    int topViewH;

    //void clearLocalProfiles();
    //void generateLocalProfiles(int granuality);

    //void redundancyMeasure();
    //double euclideanDistance(QList<double> l1, QList<double> l2);
    //double manhanttanDistance(QList<double> l1, QList<double> l2);
    //double consistencyDistance(QList<double> l1, QList<double> l2);
    //double cosineDistance(QList<double> l1, QList<double> l2);
    //double average(QVector<double> vals);
    //double stddev(QVector<double> vals, double avg);
    //QVector<double> calInfoGain(QList<double> l1, QList<double> l2);
};

#endif // LOCALMODELVIEW_H
