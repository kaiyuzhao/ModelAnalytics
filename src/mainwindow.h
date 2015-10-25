#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <RInside.h>
#include <QMainWindow>
#include "DataModel.h"

class ModelConfigDialog;
class ModelView;
class QHBoxLayout;
class QVBoxLayout;
class QScrollArea;
class ModelRelationView;
class LocalModelCfgDialog;
class ModelProfile;
class LocalModelView;
class ModelEvalView;
class TimeHeatmapView;
class ConsensusView;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    RInside &m_r;
    explicit MainWindow(RInside &r,QWidget *parent = 0);
    ~MainWindow();
    DataModel* dataModel;

    ModelConfigDialog *dlg;
    LocalModelCfgDialog *localdlg;

    ModelView* modelView;
    ModelRelationView* relationView;
    LocalModelView* localView;
    //ModelEvalView* metricView;
    TimeHeatmapView* timeLineView;
    ConsensusView* concesusView;

    QScrollArea* scrollArea;// = new QScrollArea();

    QHBoxLayout* containerLayout; //model detail view container;
    QHBoxLayout* mrvLayout;  // model relation view aggregated similarity
    QHBoxLayout* localLayout;  // model local view
    QHBoxLayout* metricLayout;
    QHBoxLayout* concesusLayout;

    //QVBoxLayout* scrollLayout;

private slots:
    void on_actionOpen_triggered();

    void on_lineEdit_textChanged(const QString &arg1);

    void on_btn_Add_clicked();

    void on_listView_clicked(const QModelIndex &index);

    void on_btn_Remove_clicked();

    void on_listView_selected_entered(const QModelIndex &index);

    void on_pushButton_clicked();

    void on_thresholdSlder_valueChanged(int value);

private:
    Ui::MainWindow *ui;

signals:
    //void dataLoaded();
    //void modelProfileAdded();

    void timeLineChanged();
    //void dataModelChanged();
    //void modelProfilesChanged();
    void itemHighlighted(const QModelIndex& idx);

    void simThresChanged(double d);

    void aggregationChanged();



public slots:
    //void updateModelSpaceView();
    //void updateHighlightPathName(QString str);
    void showLocalModelCfg(int profileID);

    void updateColorSliderRange(int);

    void updateModelColorSliderRange(int);

};

#endif // MAINWINDOW_H
