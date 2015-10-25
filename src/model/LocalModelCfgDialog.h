#ifndef LOCALMODELCFGDIALOG_H
#define LOCALMODELCFGDIALOG_H

#include <QDialog>

namespace Ui {
class LocalModelCfgDialog;
}

class MainWindow;
class ModelProfile;
class QButtonGroup;

class LocalModelCfgDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit LocalModelCfgDialog(int pID, MainWindow *parent = 0);
    ~LocalModelCfgDialog();

    MainWindow* mainwin;

    int profileID;

    //ModelProfile* profile; //global base model;
signals:
    void granualitySelected(int, int); //modelprofileID, granuality
    
private slots:
    void on_btn_add_clicked();

    void on_btn_cancel_clicked();

private:
    Ui::LocalModelCfgDialog *ui;

    QButtonGroup* btnGroup;
};

#endif // LOCALMODELCFGDIALOG_H
