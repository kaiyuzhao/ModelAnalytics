#include "ModelEvalView.h"


#include "mainwindow.h"
#include "ModelProfile.h"
#include "VolatilityModel.h"
#include "SeasonalModel.h"
#include "DriftModel.h"


#include <QPainter>
#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QPair>
#include <Qvector>
#include <QLabel>
#include <QDebug>
#include <QMouseEvent>


ModelEvalView::ModelEvalView(MainWindow *mw, QWidget *parent) :
    QWidget(parent), mainWin(mw)
{
    //mainWin = mw;
    //par = parent;

    dataModel = mainWin->dataModel;

    //initialize();

    /*QPalette pal(palette());

    pal.setColor(QPalette::Background, Qt::white);
    setAutoFillBackground(true);
    setPalette(pal);
    */
    //xAxSeleciton = 0;
    //yAxSelection = 0;

    /*top = 30;
    margin = 30;
    glyphSize = 20;

    oRange = std::make_pair(0.0f,1.0f);
    cRange = std::make_pair(0.0f,5.0f);
    sRange = std::make_pair(0.0f,1.0f);

    glyphHighlight = -1;
*/
    QVector<QString> strs = {"#8dd3c7", "#ffffb3", "#bebada", "#fb8072",
                "#80b1d3", "#fdb462", "#b3de69", "#fccde5"};

    for(auto s : strs) colorSchema.push_back(QColor(s));

    this->setMouseTracking(true);
}

ModelEvalView::~ModelEvalView()
{
    /*if(mainLayout) delete mainLayout;
    if(groupLayout) delete groupLayout;
    if(groupBox) delete groupBox;*/
    //if(yAxis) delete yAxis;
    //if(xAxis) delete xAxis;
}


void ModelEvalView::paintEvent(QPaintEvent *e){
    QPainter p(this);
    p.fillRect(0,0,this->geometry().width(),this->geometry().height(),Qt::white);
    if(dataModel->modelProfiles.empty()) return;

}

// glyph profile with linkage between parent model and children models
/*
void ModelEvalView::paintEvent(QPaintEvent *e)
{
    if(dataModel->modelProfiles.empty()) return;

    // cavas top left is (margin+glyphSize, margin+glyphSize)
    // cavas width and height:
    int w = this->geometry().width() - margin*2- glyphSize *2;
    int h = this->geometry().height() - margin*2- glyphSize *2 - top;

    // draw boundary at (margin, margin) with this->geometry().width() - margin*2
    // and this->geometry().height() - margin*2;

    QPainter p(this);
    p.fillRect(this->geometry(),Qt::white);
    p.setRenderHint(QPainter::Antialiasing,true);

    p.drawRect(margin,margin+top,this->geometry().width()-margin*2,
               this->geometry().height()-margin*2-top);

    int n = dataModel->modelProfiles.size();
    if(ys.size()!=n) return;
    if(xs.size()!=n) return;

    glyphsVec.clear();
    glyphsVec.resize(n);
    glyphs.clear();
    for(int i = 0; i< n; i++) {
        //qDebug()<<"x "<<xs[i]<<" y "<<ys[i]<<endl;
        int x = xs[i] * w + margin + glyphSize;
        int y = (h- ys[i] * h) + margin+ glyphSize+top;
        QRect r(x-glyphSize/2, y-glyphSize/2, glyphSize, glyphSize);
        glyphs.insert(dataModel->modelProfiles[i],r);
        glyphsVec[i] = r;

        if(dataModel->modelProfiles[i]->isDerived) {
            p.setPen(Qt::DotLine);
            p.drawRect(r);
            p.setPen(Qt::SolidLine);
        } else { // global model
            QColor q(Qt::black);
            q.setAlpha(70);
            //p.setPen(QPen());
            //p.fillRect(r,QBrush(q));
            p.drawRect(r);
        }

        int h1 = glyphSize * outliers[i];//-oRange.first) / (oRange.second- oRange.first);
        int h2 = glyphSize* clusters[i];//-cRange.first) / (cRange.second- cRange.first);
        int h3 = glyphSize* spreads[i];//-sRange.first) / (sRange.second- sRange.first);
        int ww = glyphSize*0.2f;
        //qDebug()<<"h1 "<<outliers[i]<<"h2 "<<clusters[i]<<"h3 "<<spreads[i];
        //qDebug()<<oRange.first<<":"<<oRange.second<<" "<<cRange.first<<":"<<cRange.second<<" "<<sRange.first<<":"<<sRange.second;
        p.fillRect(x-glyphSize*0.5f, y+glyphSize*0.5f - h1, ww, h1, QBrush(this->colorSchema[0]));
        p.fillRect(x-glyphSize*0.5f + 2*ww, y+glyphSize*0.5f- h2, ww, h2, QBrush(this->colorSchema[1]));
        p.fillRect(x-glyphSize*0.5f + 4*ww, y+glyphSize*0.5f - h3, ww, h3, QBrush(this->colorSchema[2]));
    }
    if(glyphHighlight>=0 && glyphHighlight<glyphsVec.size()) {
        p.save();
        QPen pen(Qt::red,1,Qt::SolidLine);
        p.setPen(pen);
        p.drawRect(glyphsVec[glyphHighlight]);
        p.restore();
    }
    //qDebug()<<"\n";
    drawAxes(p);
    drawLinkage(p);
}*/

void ModelEvalView::mouseMoveEvent(QMouseEvent *e)
{
    /*int idx = 0;
    for(const QRect &r : glyphsVec) {
        if(r.contains(e->pos())) {
            glyphHighlight = idx;
            this->repaint();
            //emit this->hopoverGlyph(idx);
            return;
        }
        idx++;
    }
    glyphHighlight = -1;*/
}


/*void ModelEvalView::drawAxes(QPainter &p)
{

    if(dataModel->modelProfiles.empty()) return;

    QString xName = this->xAxis->currentText();
    QString yName = this->yAxis->currentText();

    int w = this->geometry().width() - margin*2;
    int h = this->geometry().height() - margin*2 - top;

    int x1 = margin/3.0f;
    int y1 = top + margin + h/2.0f + yName.size()*5.0f/2.0f;

    p.save();
    p.translate(x1,y1);
    p.rotate(-90);
    p.drawText(0,0,yName);
    p.restore();

    int x2 = margin+ w/2.0f - xName.size()*5.0f/2.0f;
    int y2 = this->geometry().height()-margin/4.0f;
    p.drawText(x2,y2,xName);

    //left bottom: x = margin, y = top+margin+h
    //int tickx = margin;
    int ticky = top+margin+h;
    //float start = xrange.first;
    //float end = xrange.second;
    int num = 10;
    for(int i = 0; i<= num; i++) {
        int xoffset = i*w/(float)num+margin;
        p.drawLine(xoffset,ticky,xoffset,ticky-5);
        double txt = xrange.first+i*(xrange.second-xrange.first)/(float)num;
        QString str = QString::number(txt,'g',2);
        p.drawText(xoffset-str.size()*2.0f,ticky+10, str);
    }

    for(int i = 0; i<=num; i++) {
        int yoffset = margin+top + h -  i*h/(float)num;
        p.drawLine(margin,yoffset,margin+5,yoffset);
        double txt = yrange.first+i*(yrange.second-yrange.first)/(float)num;
        QString str = QString::number(txt,'g',2);
        p.save();
        p.translate(margin-5,yoffset+str.size()*2.0f);
        p.rotate(-90);
        p.drawText(0,0,str);
        p.restore();
    }
}
*/
/*void ModelEvalView::drawLinkage(QPainter &p)
{
    if(this->dataModel->modelProfiles.empty()) return;

    p.save();
    for(auto m : this->dataModel->modelProfiles) {
        if(m->par == NULL) continue;
        QRect curr = glyphs.value(m);
        QRect par = glyphs.value(m->par);
        // draw line from center of par to curr;
        p.setPen(Qt::DotLine);
        p.drawLine(par.center(),curr.center());
    }
    p.restore();
}*/

/**
 * @brief ModelEvalView::updateModelEvalView
 *
 *  update xrange and yrange
 *  as well as the x values and y values;
 */
/*void ModelEvalView::updateModelEvalView()
{
    this->repaint();
    if(dataModel->modelProfiles.empty()) return;
    //by default set outlier as x and y axes;
    int n = dataModel->modelProfiles.size();
    this->outliers = QVector<float>(n,0.0f);
    this->clusters = QVector<float>(n,0.0f);
    this->spreads = QVector<float>(n,0.0f);

    for(int i = 0; i< n; i++) {
        outliers[i] = dataModel->modelProfiles[i]->metric->outlier;
        clusters[i] = dataModel->modelProfiles[i]->metric->cluster;
        spreads[i] = dataModel->modelProfiles[i]->metric->spread;
    }

    oRange = normalize(outliers, 0, 0);
    cRange = normalize(clusters, 0, 0);
    sRange = normalize(spreads, 0, 0);

    int xidx = this->xAxis->currentIndex();
    int yidx = this->yAxis->currentIndex();

    comboXSelected(xidx);
    comboYSelected(yidx);

    this->repaint();
}

void ModelEvalView::comboXSelected(int idx)
{
    if(idx==0) {
        xs = outliers;
        xrange = oRange;
    }
    if(idx==1) {
        xs = clusters;
        xrange = cRange;
    }
    if(idx==2) {
        xs = spreads;
        xrange = sRange;
    }
    this->repaint();
}

void ModelEvalView::comboYSelected(int idx)
{
    if(idx==0) {
        ys = outliers;
        yrange = oRange;
    }
    if(idx==1) {
        ys = clusters;
        yrange = cRange;
    }
    if(idx==2) {
        ys = spreads;
        yrange = sRange;
    }
    this->repaint();
}

void ModelEvalView::highlightGlyph(int x)
{
    if(x<0) return;
    this->glyphHighlight = x;
    this->repaint();
}

void ModelEvalView::initialize()
{
    this->xAxis = new QComboBox();
    this->yAxis = new QComboBox();

    this->xAxis->addItem("Outlier Score");
    this->xAxis->addItem("Cluster Score");
    this->xAxis->addItem("Spread Score");

    this->yAxis->addItem("Outlier Score");
    this->yAxis->addItem("Cluster Score");
    this->yAxis->addItem("Spread Score");

    this->groupBox = new QGroupBox();
    this->groupLayout = new QHBoxLayout(groupBox);
    groupLayout->addWidget(new QLabel("X Axis:"));
    groupLayout->addWidget(xAxis);
    groupLayout->addSpacerItem(new QSpacerItem(100,10));
    groupLayout->addWidget(new QLabel("Y Axis:"));
    groupLayout->addWidget(yAxis);

    this->mainLayout = new QVBoxLayout(this);
    this->mainLayout->setAlignment(Qt::AlignTop);
    mainLayout->addWidget(groupBox);

    this->xAxis->setCurrentIndex(0);
    this->yAxis->setCurrentIndex(0);

    connect(this->xAxis,SIGNAL(currentIndexChanged(int)),
            this,SLOT(comboXSelected(int)));
    connect(this->yAxis,SIGNAL(currentIndexChanged(int)),
            this,SLOT(comboYSelected(int)));
    connect(dataModel,SIGNAL(modelProfileChanged()),
            this,SLOT(updateModelEvalView()));
}*/

/**
 * @brief ModelEvalView::screenToData
 * @param x
 * @param y
 * @return  idx of modelprofile
 */
/*int ModelEvalView::screenToData(int x, int y)
{
    return 0;
}



std::pair<float,float> ModelEvalView::normalize(QVector<float> &vec)
{
    // normalize xs
    if(vec.empty()) return std::make_pair(0,5.0f);
    if(vec.size()==1) {
        if(vec[0]>5.0f)
            vec[0] = 1;
        else
            vec[0] = (vec[0] - 0) / (5.0f - 0);
        return std::make_pair(0,5.0f);
    }

    float mx = vec[0]; float mi = vec[0];
    for(auto val : vec) {
        mx = fmax(mx,val);
        mi = fmin(mi,val);
    }

    for(auto & val : vec) {
        if(fabs(mx-mi)<1E-9) {
            val = 0.5f;
        } else {
            val = (val - mi) / (mx - mi);
        }
    }
    if(fabs(mx-mi)<1E-9) return std::make_pair(0,1);
    return std::make_pair(mi,mx);
}

std::pair<float,float> ModelEvalView::normalize(QVector<float> &vec, float l, float h)
{
    if(vec.empty()) return std::make_pair(0,5.0f);

    if(vec.size()==1) {
        if(vec[0]> h)
            vec[0] = 1;
        else
            vec[0] = (vec[0] - 0) / (1 - 0);
        return std::make_pair(0,1);
    }

    if(l==h) {
        l = vec[0];
        h = vec[0];
        for(auto & val : vec) {
            l = fmin(val,l);
            h = fmax(val,h);
        }
    }

    for(auto & val : vec) {
        if(val>=h) {
            val = 1;
            continue;
        }
        if(val<=l) {
            val = 0;
            continue;
        }
        val = (val - l) / (h - l);
    }
    return std::make_pair(l,h);
}
*/
