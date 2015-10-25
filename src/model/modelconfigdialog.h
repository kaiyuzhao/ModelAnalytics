#ifndef MODELCONFIGDIALOG_H
#define MODELCONFIGDIALOG_H

#include <QDialog>
#include "mainwindow.h"

class ModelProfile;

namespace Ui {
class ModelConfigDialog;
}

class QButtonGroup;
class ModelConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModelConfigDialog(MainWindow *parent = 0);
    ~ModelConfigDialog();

private:
    ModelProfile* drift;
    ModelProfile* moving;
    ModelProfile* season;

    //high and low boundary of model estimate;
    float high;
    float low;

    void initialize();

    Ui::ModelConfigDialog *ui;
    DataModel *dataModel;
    QButtonGroup* btnGroup;

signals:
    //void modelProfileChanged();
private slots:
    //void on_btn_Confirm_clicked();
};

#endif // MODELCONFIGDIALOG_H
