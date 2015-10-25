#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ModelConfigDialog.h"
#include "ModelView.h"
#include "ModelRelationView.h"
#include "LocalModelCfgDialog.h"
#include "LocalModelView.h"
#include "ModelEvalView.h"
#include "TimeHeatmapView.h"
#include "ConsensusView.h"

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDataStream>
#include <QStringList>
#include <QFileDialog>
#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>



MainWindow::MainWindow(RInside &r, QWidget *parent) :
    QMainWindow(parent),
    m_r(r),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    dataModel = new DataModel(r);
    dlg = NULL;
    localdlg = NULL;


    //relationView = new ModelRelationView(this,ui->modelRelationContainer);
    //mrvLayout = new QHBoxLayout(ui->modelRelationContainer);
    //ui->modelRelationContainer->layout()->setMargin(0);
    //mrvLayout->addWidget(this->relationView);


    modelView = new ModelView(this,ui->modelContainer);
    containerLayout = new QHBoxLayout(ui->modelContainer);
    ui->modelContainer->layout()->setMargin(0);
    containerLayout->addWidget(this->modelView);

    /*scrollArea = new QScrollArea();
    containerLayout->addWidget(scrollArea);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(modelView);
    scrollArea->show();*/

    ui->listView_selected->setMouseTracking(true);

    //localView = new LocalModelView(this,ui->LocalModelContainer);
    //localLayout = new QHBoxLayout(ui->LocalModelContainer);
    //ui->LocalModelContainer->layout()->setMargin(0);
    //localLayout->addWidget(localView);

    timeLineView = new TimeHeatmapView(this,ui->modelMetric);
    metricLayout = new QHBoxLayout(ui->modelMetric);
    ui->modelMetric->layout()->setMargin(0);
    metricLayout->addWidget(timeLineView);

    concesusView = new ConsensusView(this,ui->rightWidget);
    concesusLayout = new QHBoxLayout(ui->rightWidget);
    ui->rightWidget->layout()->setMargin(0);
    concesusLayout->addWidget(concesusView);

    // double link interaction between model view and model eval view;
    /*connect(metricView,SIGNAL(hopoverGlyph(int)),
            modelView,SLOT(highlightProfile(int)));
    connect(modelView,SIGNAL(modelProfileHopover(int)),
            metricView,SLOT(highlightGlyph(int)));
*/

    connect(this, SIGNAL(timeLineChanged()),
            dataModel,SLOT(normalize()));

    //connect(this, SIGNAL(timeLineChanged()),
            //timeLineView, SLOT(updateTimeHeatmapView()));

    connect(dataModel, SIGNAL(dataSelectionChanged()),
            timeLineView, SLOT(updateTimeHeatmapView()));

    connect(ui->binSlider,SIGNAL(valueChanged(int)),
            timeLineView,SLOT(updateBins(int)));

    connect(ui->intvlSlider,SIGNAL(valueChanged(int)),
            timeLineView,SLOT(updateIntvls(int)));

    connect(ui->btnAddModel, SIGNAL(clicked()),
            dataModel,SLOT(updateModelProfile()));

    connect(dataModel, SIGNAL(timelinemodelChanged()),
                modelView, SLOT(updateModelView()));

    connect(this->modelView, SIGNAL(timelineSelectionChanged()),
            this->timeLineView,SLOT(updateSelection()));

    connect(this->timeLineView,SIGNAL(colorRangeChanged(int)),
            this, SLOT(updateColorSliderRange(int)));

    connect(this->ui->colorSlider,SIGNAL(valueChanged(int)),
            this->timeLineView,SLOT(updateColors(int)));

    connect(this->ui->binModSlidr,SIGNAL(valueChanged(int)),
            modelView,SLOT(updateBinSize(int)));

    connect(this->ui->intvlModSlider,SIGNAL(valueChanged(int)),
            dataModel,SLOT(updateWinNum(int)));

    connect(this->ui->colorModSlider,SIGNAL(valueChanged(int)),
            modelView,SLOT(updateColors(int)));

    connect(modelView,SIGNAL(colorRangeChanged(int)),
            this,SLOT(updateModelColorSliderRange(int)));

    connect(this->ui->binModSlidr,SIGNAL(valueChanged(int)),
            this->ui->lblModBins,SLOT(setNum(int)));

    connect(this->ui->intvlModSlider,SIGNAL(valueChanged(int)),
            this->ui->lblModIntvls,SLOT(setNum(int)));

    connect(this->ui->colorModSlider,SIGNAL(valueChanged(int)),
            this->ui->lblModColorRange,SLOT(setNum(int)));

    connect(this->ui->cBoxConf,SIGNAL(toggled(bool)),
            modelView,SLOT(updateBoxPlot(bool)));

    connect(this->modelView,SIGNAL(clusterAdded()),
            this->concesusView,SLOT(updateGlyphs()));

    connect(this,SIGNAL(simThresChanged(double)),
            this->concesusView,SLOT(updateThreshold(double)));

    connect(this->concesusView,SIGNAL(indexAggregated(QSet<int>)),
            this->modelView,SLOT());
    connect(this->concesusView,SIGNAL(),
            this->timeLineView,SLOT());
}

MainWindow::~MainWindow()
{
    delete ui;
    if(dataModel) delete dataModel;
    if(dlg) delete dlg;
    if(localdlg) delete localdlg;
}

void MainWindow::on_actionOpen_triggered()
{
    //open file;
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File..."),QString(),
                                                    tr("CSV-Files(*.csv);;All Files (*)"),0,
                                                    QFileDialog::DontUseNativeDialog);

    if(fileName.isEmpty()){
        return;
    }
    QFile file(fileName);

    if(!file.open(QIODevice::ReadOnly)){
        return;
    }

    QTextStream in(&file);
    QStringList eachRow;

    QString header = in.readLine();
    QVector<QString> lines;
    //qDebug()<<"start";
    //QString all = in.readAll();
    while(!in.atEnd()) lines.push_back(in.readLine());
    file.close();
    qDebug()<<"io done "<<lines.size();
    //QStringList lines = all.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
    for(QString line: lines){
        eachRow = line.split(","); //first element name, second element date, third element price;
        double p = eachRow[2].toDouble();
        QStringList dt = eachRow[1].split("/");
        //dt[0]; //month;
        //dt[1]; //day
        //dt[2]; //year
        QDate d(dt[2].toInt(),dt[0].toInt(),dt[1].toInt());
        if(dataModel->priceBuff.contains(eachRow[0])){
            PriceVec::iterator i = dataModel->priceBuff.find(eachRow[0]);
            i.value().push_back(p);
        }else{
            QVector<double> vec;
            vec.push_back(p);
            dataModel->priceBuff.insert(eachRow[0],vec);
        }

        if(dataModel->dateBuff.contains(eachRow[0])){
            DateVec::iterator i = dataModel->dateBuff.find(eachRow[0]);
            i.value().push_back(d);
        }else{
            QVector<QDate> vec;
            vec.push_back(d);
            dataModel->dateBuff.insert(eachRow[0],vec);
        }
    }

    QList<QString> tmp = dataModel->priceBuff.keys();
    dataModel->compNameList->setStringList(QStringList(tmp));
    ui->listView->setModel(dataModel->compNameList);
    ui->listView->setSelectionMode( QAbstractItemView::ExtendedSelection );
    ui->listView->update();
}

void MainWindow::on_lineEdit_textChanged(const QString &arg1)
{
    QStringList qlist = dataModel->compNameList->stringList();
    //filter

}

void MainWindow::on_btn_Add_clicked()
{
    dataModel->dataSelected.clear();
    QModelIndexList lst = ui->listView->selectionModel()->selectedIndexes();
    for(QModelIndex idx :lst){
        dataModel->dataSelected.insert(idx.data().toString(),1);
    }
    QList<QString> selected = dataModel->dataSelected.keys();
    QStringList tmp(selected);
    //dataModel->qstSelected = tmp;
    dataModel->compNameSelected->setStringList(tmp);
    ui->listView_selected->setModel(dataModel->compNameSelected);
    ui->listView_selected->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->listView_selected->update();

    dataModel->tlSelsListView.clear();
    int s = selected.size();
    for(int i = 0; i<s; i++) dataModel->tlSelsListView.insert(i);

    emit timeLineChanged();
}

void MainWindow::on_listView_clicked(const QModelIndex &index)
{
    //QModelIndex a = ui->listView->selectedIndexes().back();
    QVariant selected = dataModel->compNameList->data(index,Qt::DisplayRole);
    dataModel->dataViewComp = selected.toString();
    QVector<double> dataViewPrice = dataModel->priceBuff.value(selected.toString());
    QVector<double> xval;
    QVector<double> baseVal;
    if(dataViewPrice.empty()) return;
    double min = dataViewPrice[0];
    for(auto &i : dataViewPrice) min = std::fmin(min,i);
    for(int i = 0; i<dataViewPrice.size(); i++){
        xval<<i;
        baseVal<<min;
    }
    QCustomPlot* p = ui->qcpDataView;;
    p->clearGraphs();

    QCPGraph* timeline = new QCPGraph(p->xAxis,p->yAxis);
    timeline->addData(xval,dataViewPrice);
    QCPGraph* base = new QCPGraph(p->xAxis,p->yAxis);
    base->setData(xval,baseVal);

    p->addPlottable(timeline);
    p->addPlottable(base);

    timeline->setChannelFillGraph(base);
    timeline->setPen(QPen(QColor(0,0,0,0)));
    timeline->setBrush(QColor(0,0,255,100));

    //QVector<double> ticks;
    QVector<QString> labels;

    int L = this->dataModel->dateBuff.values()[0].size()-1;
    int len = 6;
    float step = (float)L/(float)len;
    for(int i = 0; i<= len; i++){
        labels<<this->dataModel->dateBuff.values()[0][i*step].toString("MM/yy");
    }

    p->xAxis->setTickVectorLabels(labels);
    p->xAxis->setAutoTicks(true);
    p->xAxis->setAutoTickLabels(false);

    p->xAxis->setTickLabelRotation(-60);
    //p->xAxis->setSubTickCount(0);
    //p->xAxis->setTickLength(0, len-1);

    p->xAxis->grid()->setVisible(true);
    //p->xAxis->setRange(0, len-2);
    //p->xAxis->setTickVector(ticks);

    p->rescaleAxes();
    p->replot();
}

/*void MainWindow::on_addProfile_clicked()
{
    if(!this->dlg)
        this->dlg = new ModelConfigDialog(this);
    dlg->show();
    //pop up dialog box and allow change model types;
    //change model configurations
}*/

//number of models, model type, model covered range, model fit coefficient, model fitness;
//loop over all selected companies
//build models and get the info above
//store the information somewhere in some format
//update the model space view;

//question what kind of interactions do we support?
/*void MainWindow::updateModelSpaceView()
{
    //loop over QVector<ModelProfile> modelProfiles;
    //emit dataModelChanged(); //not a good practice, using controller instead;
}*/

/*void MainWindow::updateHighlightPathName(QString str)
{
    //ui->lbl_Highlight->setText(str);
}*/

void MainWindow::showLocalModelCfg(int profileID)
{
    //ModelProfile* p = this->dataModel->modelProfiles[profileID];
    if(!this->localdlg){
        this->localdlg = new LocalModelCfgDialog(profileID,this);
    }else{
        //localdlg->profile = p;
        this->localdlg->profileID = profileID;
    }
    localdlg->show();
}

void MainWindow::updateColorSliderRange(int x)
{
    if(x<1) return;
    ui->colorSlider->blockSignals(true);
    ui->colorSlider->setMaximum(x);
    ui->colorSlider->setMinimum(1);
    ui->colorSlider->setValue(x);
    ui->colorSlider->blockSignals(false);
    ui->lblColorRange->setText(QString::number(x));
}

void MainWindow::updateModelColorSliderRange(int x)
{
    if(x<1) return;
    ui->colorModSlider->blockSignals(true);
    ui->colorModSlider->setMaximum(x);
    ui->colorModSlider->setMinimum(1);
    ui->colorModSlider->setValue(x);
    ui->colorModSlider->blockSignals(false);
    ui->lblModColorRange->setText(QString::number(x));
}

void MainWindow::on_btn_Remove_clicked()
{
    QModelIndexList qlst = ui->listView_selected->selectionModel()->selectedIndexes();

    ui->listView_selected->setUpdatesEnabled(false);
    //QModelIndexList indexes = ui->listView->selectionModel()->selectedIndexes();
    qSort(qlst.begin(), qlst.end());
    for(int i = qlst.count() - 1; i > -1; --i)
        dataModel->compNameSelected->removeRow(qlst.at(i).row());
    //qDebug()<<dataModel->compNameSelected->stringList().size();
    int s = dataModel->compNameSelected->stringList().size();
    dataModel->tlSelsListView.clear();
    for(int i = 0; i<s; i++) dataModel->tlSelsListView.insert(i);
    ui->listView_selected->setUpdatesEnabled(true);
    emit timeLineChanged();
}

void MainWindow::on_listView_selected_entered(const QModelIndex &index)
{
    //QString highlightItem = index.data().toString();
    //qDebug()<<"company name "<<index.data().toString();
    emit itemHighlighted(index);
}

void MainWindow::on_pushButton_clicked()
{
    // emit button clicked;
}

void MainWindow::on_thresholdSlder_valueChanged(int value)
{
    emit simThresChanged((double)value/(double)ui->thresholdSlder->maximum());
}
