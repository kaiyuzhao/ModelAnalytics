#include <QButtonGroup>

#include "modelconfigdialog.h"
#include "ui_modelconfigdialog.h"
#include "ModelProfile.h"
#include "SeasonalModel.h"
#include "DriftModel.h"
#include "VolatilityModel.h"
#include "MovingAvgModel.h"

ModelConfigDialog::ModelConfigDialog(MainWindow *parent) :
    QDialog(parent),
    ui(new Ui::ModelConfigDialog)
{
    ui->setupUi(this);
    dataModel = parent->dataModel;
    btnGroup = new QButtonGroup(this);
    btnGroup->addButton(ui->rbtn_Serial);
    btnGroup->addButton(ui->rbtn_trend);
    btnGroup->addButton(ui->rbtn_volatility);

    btnGroup->setId(ui->rbtn_trend,0);
    btnGroup->setId(ui->rbtn_Serial,1);
    btnGroup->setId(ui->rbtn_volatility,2);

    ui->rbtn_trend->setChecked(true);

    initialize();
}

ModelConfigDialog::~ModelConfigDialog()
{
    delete ui;
}

/**
 * @brief ModelConfigDialog::initialize
 * initialize overview of different models;
 * with input parameters
 */
void ModelConfigDialog::initialize()
{
    //drift = new DriftModel(dataModel);
    //season = new SeasonalModel(dataModel);
    //moving = new MovingAvgModel(dataModel);

}

//after change configurations, emit modelProfileChanged;

/*void ModelConfigDialog::on_btn_Confirm_clicked()
{
    ModelProfile* mp;
    int id = btnGroup->checkedId();
    switch(id){
    case 0: //trend
        mp = new DriftModel(dataModel);
        mp->scale = ui->lineEdit_scale->text().toInt();
        break;
    case 1: //serial
        mp = new SeasonalModel(dataModel);
        mp->lag = ui->lineEdit_Lag->text().toInt();
        break;
    case 2: //volatility
        mp = new VolatilityModel(dataModel);
        mp->volInterval = ui->lineEdit_Segment->text().toInt();
        break;
    default:
        break;
    }
    mp->start = ui->dateEdit_start->date();
    mp->end = ui->dateEdit_end->date();
    mp->selected = dataModel->qstSelected;
    dataModel->modelProfiles.push_back(mp);
    mp->updateModelProfile();
    this->hide();
}*/
