#include "LocalModelCfgDialog.h"
#include "ui_LocalModelCfgDialog.h"

#include "mainwindow.h"
#include "ModelProfile.h"

#include "DriftModel.h"
#include "SeasonalModel.h"
#include "VolatilityModel.h"
#include "LocalModelView.h"

#include <QButtonGroup>

LocalModelCfgDialog::LocalModelCfgDialog(int pID, MainWindow *parent) :
    QDialog(parent),
    ui(new Ui::LocalModelCfgDialog)
{
    ui->setupUi(this);

    mainwin = parent;
    profileID = pID;
    //profile = mainwin->dataModel->modelProfiles[pID];


    btnGroup = new QButtonGroup();
    btnGroup->addButton(ui->rbt_quarter);
    btnGroup->addButton(ui->rbt_month);
    btnGroup->addButton(ui->rbt_week);
    btnGroup->addButton(ui->rbt_custom);


    btnGroup->setId(ui->rbt_quarter,0);
    btnGroup->setId(ui->rbt_month,1);
    btnGroup->setId(ui->rbt_week,2);
    btnGroup->setId(ui->rbt_custom,3);



    connect(this,SIGNAL(granualitySelected(int,int)),
            mainwin->localView,SLOT(updateLocalRelationView(int,int)));
}

LocalModelCfgDialog::~LocalModelCfgDialog()
{
    delete ui;
}

void LocalModelCfgDialog::on_btn_add_clicked()
{
    //generate local models; and update model redundancy view;

    //which local granuality?
    int id = btnGroup->checkedId();
    int granuality = 90;
    switch(id){
    case 0: //quarter
        granuality = 90;
        break;
    case 1://month
        granuality = 30;
        break;
    case 2://week
        granuality = 7;
        break;
    case 3: //custom
        granuality = ui->lineEdit->text().toInt();
        break;
    default:
        granuality = 90;
        break;
    }
    //this->clearLocalProfiles();
    //emit granualitySelected(profileID,granuality);
    this->hide();
}

void LocalModelCfgDialog::on_btn_cancel_clicked()
{
    this->hide();
}

