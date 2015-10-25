#include "LocalModelView.h"
#include "mainwindow.h"

#include "ModelProfile.h"
#include "VolatilityModel.h"
#include "SeasonalModel.h"
#include "DriftModel.h"

#include "ModelView.h"
#include "ModelEvalView.h"

#include <QVector>
#include <QDebug>
#include <QPainter>
#include <QColor>
#include <QBrush>
#include <QMouseEvent>

LocalModelView::LocalModelView(MainWindow *mw, QWidget *parent) :
    QWidget(parent)
{
    mainWin = mw;

    dataModel = mainWin->dataModel;

    /*QPalette pal(palette());
    pal.setColor(QPalette::Background, Qt::white);
    setAutoFillBackground(true);
    setPalette(pal);*/

    margin = 15;
    topViewH = 15;


    QVector<QString> strs = {"#8dd3c7", "#ffffb3", "#bebada", "#fb8072",
                "#80b1d3", "#fdb462", "#b3de69", "#fccde5"};

    for(auto s : strs) colorSchema.push_back(QColor(s));

    //connect(this,SIGNAL(localModelAdded()),
    //        mainWin->modelView,SLOT(updateModelView()));

    this->setMouseTracking(true);
}

LocalModelView::~LocalModelView()
{
    //clearLocalProfiles();
}

void LocalModelView::paintEvent(QPaintEvent *e){
    /*localRect.clear();
    //localProfiles.clear();
    if(localProfiles.empty()) return;
    // three rows of bar plot
    int margin = 30;

    int w = this->geometry().width();
    int h = this->geometry().height();
    int n = this->localProfiles.size();



    QPainter p(this);
    localRect.resize(n);

    float hspace = 2.0f;
    float vspace = 10.0f;

    int barW = (w - 2*margin) / n;
    int barH = (h-2*margin) / 3.0f;

    for(int i = 0; i< n; i++) {
        int x = margin+barW*i;
        int y = margin+barH*0.5f;
        localRect[i] = QRect(x,y,barW,h);
        //p.drawRect(localRect[i])

        float l = this->outliers[i+1];
        float g = this->outliers[0];

        p.fillRect(x,y,barW/hspace, -(barH-vspace) * (l-g)*0.5, QBrush(colorSchema[0]));

        l = clusters[i+1];
        g = clusters[0];

        p.fillRect(x,y+barH, barW/hspace, -(barH-vspace) * (l-g)*0.5, QBrush(colorSchema[1]));


        l = spreads[i+1];
        g = spreads[0];

        p.fillRect(x,y+barH*2, barW/hspace, -(barH-vspace) * (l-g)*0.5, QBrush(colorSchema[2]));
    }
    p.save();
    QPen pen; //(Qt::red,1,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin);
    pen.setColor(Qt::red);
    p.setPen(pen);
    p.drawLine(margin, margin+barH*0.5f ,w-margin, margin+barH*0.5f);
    p.drawLine(margin, margin+barH*1.5f ,w-margin, margin+barH*1.5f);
    p.drawLine(margin, margin+barH*2.5f ,w-margin, margin+barH*2.5f);
    p.restore();

    p.save();
    QString title = "Outlier Score";
    p.translate(margin/2,margin+barH*0.5+title.size()*3.0f);
    p.rotate(-90);
    p.drawText(0,0,title);
    p.restore();

    p.save();
    title = "Cluster Score";
    p.translate(margin/2,margin+barH+barH*0.5f+title.size()*3.0f);
    p.rotate(-90);
    p.drawText(0,0,title);
    p.restore();

    p.save();
    title = "Spread Score";
    p.translate(margin/2,margin+2.0f*barH+barH*0.5f+title.size()*3.0f);
    p.rotate(-90);
    p.drawText(0,0,title);
    p.restore();
    */
}

/*
void LocalModelView::paintEvent(QPaintEvent *)
{
    // curtain view;
    int width = this->geometry().width();
    int height = this->geometry().height();

    QPainter p(this);
    //localRect.clear();
    //p.drawRect(margin,margin,w-margin*2,h-margin*2);

    p.drawLine(margin,margin+topViewH,
               width-margin,margin+topViewH);

    double seg = ((double)width-2.0f*margin)/(double)(distance.size()+1);
    localRect.resize(distance.size());
    for(int i = 0; i<distance.size(); i++){
        //int w = 0.6*seg*distance[i]/maxDist;
        //int h = topViewH;
        //int x = margin + seg*(i+1)-0.3*w;
        //int y = margin;

        int w = 0.8*seg;
        int h = topViewH*(distance[i]/maxDist);
        int x = margin + seg*(i+1.0f)-0.4f*seg;
        int y = margin + topViewH - h;

        p.fillRect(x,y,w,h,Qt::black);

        int vx = margin+seg*(i+1);

        p.drawLine(vx,y+h,vx,height-margin);

        localRect[i] = QRect(x,y,seg,height-margin*2);

        //testing;
        //p.drawRect(localRect[i]);

        int vlineH = height-margin*2-topViewH;

        for(int k =0; k<infoGain[i].size(); k++){
            double hlNum = infoGain[i].size();
            double intervalSize = (double)vlineH/(hlNum+1.0f);

            double val = fabs(infoGain[i][k]/2.0f);
            int vy = margin+topViewH+intervalSize*(k+1);
            double l = val*seg;
            p.save();
            if(infoGain[i][k]>0){
                QPen pen(Qt::green,2,Qt::SolidLine,Qt::SquareCap,Qt::MiterJoin);
                p.setPen(pen);
            }else{
                QPen pen(Qt::red,2,Qt::SolidLine,Qt::SquareCap,Qt::MiterJoin);
                p.setPen(pen);
            }
            p.drawLine(vx,vy,vx+l,vy);
            p.restore();

        }
    }

}
*/

void LocalModelView::mousePressEvent(QMouseEvent *e)
{
    /*int insertIdx = selectedProfileID;
    for(int i = 0; i<localRect.size(); i++){
        if(localRect[i].contains(e->pos())){
            ModelProfile* mp = NULL;
            mp = makeCopy(localProfiles[i]); // make copy first and then insert to global modelProfiles;
            this->dataModel->modelProfiles.insert(insertIdx+1,mp);
            mp->updateModelProfile();
            //emit localModelAdded();
        }
    }*/
}

void LocalModelView::mouseMoveEvent(QMouseEvent *e)
{

}

/*ModelProfile *LocalModelView::makeCopy(ModelProfile *profile)
{
    ModelProfile* mp = NULL;
    if(typeid(*profile)==typeid(VolatilityModel)){
        mp = new VolatilityModel(profile);
        mp->profileName = "";
    }
    if(typeid(*profile)==typeid(DriftModel)){
        mp = new DriftModel(profile);
        mp->profileName = "";
    }
    if(typeid(*profile)==typeid(SeasonalModel)){
        mp = new SeasonalModel(profile);
        mp->profileName = "";
    }
    return mp;
}*/

/*void LocalModelView::updateLocalRelationView(int profileID, int g)
{
    clearLocalProfiles();

    gProfile = dataModel->modelProfiles[profileID];
    selectedProfileID = profileID;

    this->localProfiles.clear();
    generateLocalProfiles(g);
}

void LocalModelView::clearLocalProfiles()
{
    QVector<ModelProfile* >::iterator it;

    for(it = localProfiles.begin();it!=localProfiles.end();it++){
        ModelProfile * mp = *it;
        if(mp) delete mp;
    }
    localProfiles.clear();
}

void LocalModelView::generateLocalProfiles(int granuality)
{
    QDate allstart = gProfile->start;
    QDate allend = gProfile->end;
    QDate start = allstart;
    QDate end = allstart.addDays(granuality-1);

    this->outliers.clear();
    this->clusters.clear();
    this->spreads.clear();

    this->outliers.push_back(gProfile->metric->outlier);
    this->clusters.push_back(gProfile->metric->cluster);
    this->spreads.push_back(gProfile->metric->spread);

    while(start<allend && end<=allend){
        ModelProfile* mp = NULL;
        mp = makeCopy(gProfile);
        mp->start = start;
        mp->end = end;
        mp->profileName+=" ";
        mp->profileName+=start.toString("MM/dd/yy");
        mp->profileName+=" to ";
        mp->profileName+=end.toString("MM/dd/yy");
        mp->isDerived = true;
        mp->par = gProfile;
        mp->updateModelProfile();
        this->localProfiles.push_back(mp);
        this->outliers.push_back(mp->metric->outlier);
        this->clusters.push_back(mp->metric->cluster);
        this->spreads.push_back(mp->metric->spread);
        start = start.addDays(granuality);
        end = end.addDays(granuality);
    }
    ModelEvalView::normalize(outliers,0,1);
    ModelEvalView::normalize(clusters,0,10);
    ModelEvalView::normalize(spreads,0,2);
}*/

/*
void LocalModelView::redundancyMeasure()
{
    if(localProfiles.empty()) return;
    distance.resize(localProfiles.size());
    QList<double> glist = gProfile->layout.values();
    infoGain.resize(localProfiles.size());

    maxDist = 0;

    for(int i = 0; i<localProfiles.size(); i++){
        QList<double> list = localProfiles[i]->layout.values();
        //distance[i] = euclideanDistance(glist,list);
        //distance[i] = cosineDistance(glist,list);
        //double tmp = euclideanDistance(glist,list);
        QVector<double> diff;
        //diff.resize(glist.size());

        distance[i] = this->consistencyDistance(glist,list);
        maxDist = maxDist>distance[i]?maxDist:distance[i];
        diff = calInfoGain(glist,list);
        this->infoGain[i] = diff;
    }
    return;
}

double LocalModelView::euclideanDistance(QList<double> l1, QList<double> l2)
{
    if(l1.size()!=l2.size()) return -1;
    double dist = 0;
    for(int i = 0; i<l1.size() && i<l2.size(); i++){
        dist += (l1[i]-l2[i])*(l1[i]-l2[i]);
    }
    return dist;
}

double LocalModelView::manhanttanDistance(QList<double> l1, QList<double> l2)
{
    if(l1.size()!=l2.size()) return -1;
    double dist = 0;
    for(int i = 0; i<l1.size() && i<l2.size(); i++){
        dist += fabs(l1[i]-l2[i]);
    }
    return dist;
}



double LocalModelView::consistencyDistance(QList<double> l1, QList<double> l2)
{
    if(l1.size()!=l2.size()) return -1;
    QVector<double> diff;
    diff.resize(l1.size());
    for(int i = 0; i<l1.size() && i<l2.size(); i++){
        diff[i] = l1[i]-l2[i];
    }
    double avg = this->average(diff);
    double sd = this->stddev(diff,avg);
    return sd;
}

double LocalModelView::cosineDistance(QList<double> l1, QList<double> l2)
{
    if(l1.size()!=l2.size()) return -1;
    double dist = 0; double len1 = 0; double len2 = 0;
    for(int i = 0; i<l1.size() && i<l2.size(); i++){
        dist += l1[i]*l2[i];
        len1 += l1[i]*l1[i];
        len2 += l2[i]*l2[i];
    }
    return dist/sqrt(len1)/sqrt(len2);
}

double LocalModelView::average(QVector<double> vals)
{
    if(vals.empty()) return 0;
    double avg = 0;
    for(int i = 0; i<vals.size(); i++){
        avg+=vals[i]/(double)vals.size();
    }
    return avg;
}

double LocalModelView::stddev(QVector<double> vals, double avg)
{

    if(vals.empty()) return 0;
    double sd = 0;
    for(int i = 0; i<vals.size(); i++){
        sd+=(vals[i]-avg)*(vals[i]-avg)/(double)vals.size();
    }
    return sd;
}

QVector<double> LocalModelView::calInfoGain(QList<double> l1, QList<double> l2)
{
    //l1 global; l2 local;
    //local - global > 0  green
    //local - global < 0 red;
    QVector<double> res;
    if(l1.size()!=l2.size()) return res;
    res.resize(l1.size());
    for(int i = 0; i<l1.size() && i<l2.size(); i++){
        res[i] = l2[i]-l1[i];
    }
    return res;
}

*/

