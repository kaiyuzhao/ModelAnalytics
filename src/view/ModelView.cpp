#include "ModelView.h"
#include "mainwindow.h"
#include "DataModel.h"
#include "DriftModel.h"
#include "SeasonalModel.h"
#include "VolatilityModel.h"
#include "ModelProfile.h"
#include "Glyph.h"

#include <QDebug>
#include <QPainter>
#include <QRect>
#include <typeinfo>
#include <QPainterPath>
#include <QPointF>
#include <QPen>
#include <QMouseEvent>
#include <QElapsedTimer>
#include <QSet>
#include <QDate>


ModelView::ModelView(MainWindow *main, QWidget* parent) :
    QWidget(parent),dataModel(main->dataModel)
{
    margin = 20;

    margin2 = 25;

    margin3 = 20;

    //iMargin = 20;

    binSize = 10;

    maxColorRange = 1;
    colorRange = INT_MAX;

    QVector<QString> str = {"#ffffcc","#ffeda0","#fed976",
                            "#feb24c","#fd8d3c","#fc4e2a",
                            "#e31a1c","#bd0026","#800026"};
    for(auto &c : str) schemas.push_back(QColor(c));


    QVector<QString> ss = {
        "#7fc97f","#beaed4","#fdc086"
    };
    for(auto& s: ss) bgschema.push_back(QColor(s));

    isBoxPlot = false;

    mousepress = false;

    mouseDrag = false;

    brushBox = QRectF();

    setMouseTracking(true);
}

ModelView::~ModelView()
{

}

void ModelView::paintEvent(QPaintEvent *e) {
    QPainter p(this);
    p.fillRect(0,0,this->geometry().width(),
               this->geometry().height(),Qt::white);

    //divide the display to num of model strips vertically
    if(models.empty()) return;

    int w = this->geometry().width() - margin*2;
    int h = this->geometry().height() - margin*2;

    int n = this->models.size();
    float modelH = h/(float)n;
    for(int i = 0; i< n; i++) {
        models[i].bounding = QRectF(margin,margin+modelH*i,w,modelH);
        drawModel(p, models[i].bounding,
                  this->dataModel->modelProfiles[i],models[i],i);
    }
    p.save();
    p.setPen(QPen(Qt::red,1,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
    p.drawRect(brushBox);
    p.restore();
    drawSelections(p);
    drawHopover(p);
}

void ModelView::drawHopover(QPainter &p)
{
    if(highlightSelection.empty()) return;
    p.save();
    p.setPen(QPen(Qt::red,1,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
    p.drawRect(highlight);
    p.restore();

    int x = this->mPos.x()+20;
    int y = this->mPos.y()+20;

    int h = 300;

    QString text = "";

    int cont = 0;
    int charNum = 15;
    int rowCnt = 6;

    QFontMetrics fmetric(p.font());
    int rowH = fmetric.height()+1;
    int w = 0;
    for(auto& i : highlightSelection) {
        QString full = dataModel->compNameSelected->stringList()[i];
        QString name = full.size()>charNum?
                    (full.left(charNum)+"...\n"):(full+"\n");
        text += name;
        w = std::max(w,fmetric.width(name));
        if(++cont == rowCnt && cont<highlightSelection.size()) {
            QString last = "and "+
                    QString::number(highlightSelection.size()-cont)+" others";
            text+= last;
            w = std::max(w,fmetric.width(last));
            break;
        }
    }
    if(text[text.size()-1]=='\n') text = text.left(text.size()-1);
    h = std::min(h,std::min(rowH*(cont+1), rowH*highlightSelection.size()));

    QRect r(x,y,w,h);
    QColor c("#bebada");
    c.setAlpha(120);
    p.fillRect(r,QBrush(c));

    p.save();
    p.setPen(QPen(Qt::red,1,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
    p.drawText(r,Qt::AlignCenter,text);
    p.restore();

}

void ModelView::drawSelections(QPainter &p)
{
    p.save();
    p.setPen(QPen(Qt::red,1,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
    for(QRectF &r : this->selectionsRect) {
        p.drawRect(r);
    }
    for(QVector<QRectF> &vv : this->brushSelection) {
        for(QRectF& rr : vv) p.drawRect(rr);
    }
    p.restore();
}

void ModelView::mousePressEvent(QMouseEvent *e)
{
    if(!dataModel->m.tryLock()) return;
    QPointF p = e->posF();
    if(e->button() == Qt::RightButton) {
        //if(!selections.empty()) selections.pop();
        if(!selectionsRect.empty()) selectionsRect.pop();
        if(!dataModel->tlSelsModel.empty()) dataModel->tlSelsModel.pop();
        calculateIntersetion();
        this->dataModel->m.unlock();
        this->repaint();
        emit timelineSelectionChanged();
    }
    if(e->button() == Qt::LeftButton) {
        mousepress = true;
        brushBox = QRectF(e->posF(),QSizeF());
        for(int i = 0; i< models.size(); i++) {
            if(!models[i].bounding.contains(p)) continue;
            // find the histogram bar
            //int idx = 0;
            for(int idx = 0; idx< models[i].glHisto.rects.size(); idx++) {
                QRectF r = models[i].glHisto.rects[idx];
                if(!models[i].glHisto.bouding.contains(p)) continue;
                if(r.contains(p)) {
                    selectionsRect.push(models[i].glHisto.rects[idx]);
                    dataModel->tlSelsModel.push(models[i].glHisto.lookup[idx]);
                    calculateIntersetion();
                    dataModel->m.unlock();
                    this->repaint();
                    emit timelineSelectionChanged();
                    return;
                }
                //idx++;
            }
            //continue search on local models;

            for(int kk = 0; kk < models[i].locHisto.size(); kk++) {
                Histogram &hist = models[i].locHisto[kk];
                if(!hist.bouding.contains(p)) continue;
                int idx = 0;
                for(const QRectF& r : hist.rects) {
                    if(r.contains(p)) {
                        dataModel->tlSelsModel.push(hist.lookup[idx]);
                        selectionsRect.push(hist.rects[idx]);
                        calculateIntersetion();
                        dataModel->m.unlock();
                        this->repaint();
                        emit timelineSelectionChanged();
                        return;
                    }
                    idx++;
                }
            }
        }
    }
    dataModel->m.unlock();
    this->repaint();
}

void ModelView::mouseMoveEvent(QMouseEvent *e)
{
    if(!dataModel->m.tryLock()) return;
    //mutex protection needed here;

    if(mousepress) {
        mouseDrag = true;
        brushBox = QRectF(brushBox.topLeft(),e->posF());
        dataModel->m.unlock();
        this->repaint();
        return;
    }
    QPointF p = e->posF();
    highlightSelection.clear();
    highlight = QRectF();
    mPos = e->posF();
    for(int i = 0; i< models.size(); i++) {
        if(!models[i].bounding.contains(p)) continue;
        //qDebug()<<models[i].glHisto.rects<<"\n";
        for(int idx = 0; idx<models[i].glHisto.rects.size(); idx++) {
            QRectF r = models[i].glHisto.rects[idx];
            if(r.contains(p)) {
                highlightSelection = QSet<int>(models[i].glHisto.lookup[idx]);
                highlight = r;
                dataModel->m.unlock();
                this->repaint();
                return;
            }
        }
        //continue search on local models;
        for(int kk = 0; kk < models[i].locHisto.size(); kk++) {
            Histogram &hist =  models[i].locHisto[kk];
            if(!hist.bouding.contains(p)) continue;
            int idx = 0;
            for(const QRectF& r : hist.rects) {
                if(r.contains(p)) {
                    highlightSelection = QSet<int>(hist.lookup[idx]);
                    highlight = r;
                    dataModel->m.unlock();
                    this->repaint();
                    return;
                }
                idx++;
            }
        }
    }
    dataModel->m.unlock();
    this->repaint();
}

void ModelView::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button()==Qt::RightButton) {
        //pop brushed stack;
        if(!brushSelection.empty()) brushSelection.pop();
        if(!dataModel->tlBrushModel.empty()) dataModel->tlBrushModel.pop();
        calculateIntersetion();
        this->repaint();
        emit timelineSelectionChanged();
    }
    if(mouseDrag){
        //handel the selection here;
        //using the dimension of the brushBox
        mouseDrag = false;
        QVector<QRectF> tmpRect;
        QVector<QSet<int> > tmpBrush;
        int mpIdx = 0;
        int wIdx = -1;
        int wSize = -1;

        for(int i = 0; i< models.size(); i++) {
            if(!models[i].bounding.contains(brushBox)) continue;
            mpIdx = i;
            wSize = models[i].locHisto.size();
            if(models[i].glHisto.bouding.contains(brushBox)){
                //global model
                for(int k = 0; k< models[i].glHisto.rects.size(); k++){
                    if(brushBox.contains(models[i].glHisto.rects[k])){
                        tmpRect.push_back(models[i].glHisto.rects[k]);
                        tmpBrush.push_back(models[i].glHisto.lookup[k]);
                    }
                }
                break;
            }
            for(int j = 0; j<models[i].locHisto.size(); j++) {
                //local model
                if(!models[i].locHisto[j].bouding.contains(brushBox)) continue;
                wIdx = j;
                for(int k = 0; k < models[i].locHisto[j].rects.size(); k++){
                    if(brushBox.contains(models[i].locHisto[j].rects[k])){
                        tmpRect.push_back(models[i].locHisto[j].rects[k]);
                        tmpBrush.push_back(models[i].locHisto[j].lookup[k]);
                    }
                }
                break;
            }
        }
        if(!tmpRect.empty()) this->brushSelection.push(tmpRect);
        if(!tmpBrush.empty()) this->dataModel->tlBrushModel.push(tmpBrush);
        calculateIntersetion();
        if(!tmpBrush.empty()) {
            Glyph g;
            g.dToM = this->dToM;
            g.models = QVector<ModelProfile*>(this->dataModel->modelProfiles);
            g.wId = wIdx;
            g.wSize = wSize;
            g.brush = selectionModel;
            g.calFiveSum();
            g.whichModel = mpIdx;
            dataModel->glyphs.append(g);
        }
    }
    brushBox = QRectF();
    mousepress = false;
    this->repaint();
    emit timelineSelectionChanged();
    emit clusterAdded(); //added a goup based on model space;
}



void ModelView::drawModel(QPainter &p, QRectF bouding,
                          ModelProfile *mp, Model &m, int idx)
{
    //p.drawRect(bouding);
    float x = bouding.left();
    float y = bouding.top();
    float w = bouding.width();
    float h = bouding.height();
    // draw box plot;
    //mp = dataModel->modelProfiles[i];

    // one global model and several local models;
    int win = mp->winNum+1;

    // pecentage of space for histogram
    // space below it is for boxplot;
    double ratio = 0.75f;
    if(!isBoxPlot) ratio = 1.0f;

    double glHRatio = 1.00f; // size of box plot;

    float ww = w/(float)win;
    //draw glob histo and boxplot;
    QColor c = bgschema[idx];
    p.fillRect(x,y,ww,h, QBrush(c));
    m.glHisto.bouding = QRectF(x,y,ww,h*ratio);
    drawHistogram(p,m.glHisto.bouding,
                  m.glHisto,m.maxCnt);
    drawHistogramLabel(p,m.glHisto.bouding,mp);

    int n = dataModel->dateBuff.values()[0].size();

    if(m==models[0]) drawTimeTicks(p,bouding, m);
    //if(m == models[0]) drawLocalModelDates(p,m.glHisto.bouding,s,e,true);

    double q4 = std::get<4>(mp->globLikeliSum);
    double q0 = std::get<0>(mp->globLikeliSum);

    double L = q0-(q4-q0)*0.1f;
    double H = q4+(q4-q0)*0.1f;

    for(int i = 0; i< mp->locLikliSum.size(); i++) {
        double lq4 = std::get<4>(mp->locLikliSum[i]);
        double lq0 = std::get<0>(mp->locLikliSum[i]);
        L = std::fmin(L,lq0-(lq4-lq0)*0.1f);
        H = std::fmax(H,lq4+(lq4-lq0)*0.1f);
    }
    drawBoxPlot(p,QRectF(x,y+ratio*h,ww,h*(1.0-ratio)),
                mp->globLikeliSum,L,H,glHRatio);


    for(int i = 0; i< mp->winNum; i++) {
        m.locHisto[i].bouding = QRectF(x+(i+1)*ww,y,ww,h*ratio);

        QDate ss = dataModel->dateBuff.values()[0][i*n/mp->winNum];
        QDate ee = dataModel->dateBuff.values()[0][(i+1)*n/mp->winNum-1];

        //if(m == models[0])
            //drawLocalModelDates(p,m.locHisto[i].bouding,ss,ee,false);

        drawHistogram(p,m.locHisto[i].bouding,
                      m.locHisto[i],m.maxCnt);
        drawBoxPlot(p,QRectF(x+(i+1)*ww,y+ratio*h,ww,h*(1.0-ratio)),
                    mp->locLikliSum[i],L,H,glHRatio);

    }
    return;
}

void ModelView::drawTimeTicks(QPainter &p, QRectF bouding, Model &m)
{

    float glw = m.glHisto.bouding.width();

    float x1 = bouding.left()+margin+glw;
    float y1 = bouding.top();

    float x2 = x1 + (bouding.width()-glw) - margin*2;
    float y2 = y1;



    p.drawLine(x1,y1,x2,y2);

    int n = dataModel->dateBuff.values()[0].size();
    QDate s = dataModel->dateBuff.values()[0][0];
    QDate e = dataModel->dateBuff.values()[0][n-1];
    int days = s.daysTo(e);
    float steps = 20.0f;
    float add = (float)days/steps;
    float sec = (bouding.width()-glw-margin*2)/steps;
    QFontMetrics metc(p.font());
    for(int i = 0; i<=steps; i++) {
        float xx1 = x1+i*sec;
        float yy1 = y1;
        float xx2 = xx1;
        float yy2 = yy1-5;

        if(i%5==0) {
            QDate dt = s.addDays(add*i);
            QString strs = dt.toString("MMM/yy");
            QRectF rect(xx1-metc.width(strs)/2.0f,yy1,metc.width(strs),metc.height());
            p.drawText(rect,Qt::AlignCenter|Qt::AlignHCenter,strs);
        }
        p.drawLine(xx1,yy1,xx2,yy2);
    }
}

void ModelView::drawBoxPlot(QPainter &p, QRectF bouding,
                            const FiveNumberTuple t,
                            double minVal, double maxVal,
                            double widthScale)
{
    if(!isBoxPlot) return;

    bool vertical = false;

    float x = bouding.left();
    float y = bouding.top();
    float w = bouding.width();
    float h = bouding.height();

    double q4 = std::get<4>(t);
    double q3 = std::get<3>(t);
    double q2 = std::get<2>(t);
    double q1 = std::get<1>(t);
    double q0 = std::get<0>(t);

    float botm = y+h-margin3;
    //float right = x+w-margin3;

    if(maxVal==minVal) return;

    float maxBarW = w*widthScale;
    float maxBarH = h-margin3;// - margin3*2<=0? 0 : h - margin3*2;

    QFont f;
    f.setPointSize(7);
    QFontMetrics m(f);
    p.save();
    p.setFont(f);
    QString ttt = QString::number(minVal,'g');
    p.drawText(x+margin2, y+h-margin3 + m.height(),ttt);
    ttt = QString::number(maxVal,'g');
    p.drawText(x+w-margin2-m.width(ttt), y+h-margin3 + m.height(),ttt);
    // draw boxplot axes;
    // draw x axis;
    p.drawLine(x+margin2, y+h-margin3,
               x+ w - margin2, y+h-margin3);
    // draw y axis;
    p.drawLine(x+margin2, y,
               x+margin2, y+h-margin3);
    p.restore();
    ttt = "AIC";
    p.drawText(x+(w)*0.5-m.width(ttt)*0.5f,
               y+h-margin3 + 2*m.height(),ttt);

    if(vertical) {
        float q0d = botm - (q0-minVal)/(maxVal-minVal) *(h- margin3);
        float q1d = botm - (q1-minVal)/(maxVal-minVal) *(h- margin3);
        float q2d = botm - (q2-minVal)/(maxVal-minVal) *(h- margin3);
        float q3d = botm - (q3-minVal)/(maxVal-minVal) *(h- margin3);
        float q4d = botm - (q4-minVal)/(maxVal-minVal) *(h- margin3);

        // draw two boxes with alpha
        QColor c1(Qt::black);
        c1.setAlpha(100);
        p.fillRect(x+margin2, q0d, maxBarW, q4d-q0d, QBrush(c1));
        QColor c2(Qt::black);
        c2.setAlpha(150);
        p.fillRect(x+margin2, q1d, maxBarW, q3d-q1d, QBrush(c2));
        p.save();
        p.setPen(Qt::red);
        p.drawLine(x+margin2-1,q2d,x+maxBarW+margin2,q2d);
        p.restore();
    } else {
        float q0d = x + margin2 + (q0-minVal)/(maxVal-minVal) *(w- margin2*2);
        float q1d = x + margin2 + (q1-minVal)/(maxVal-minVal) *(w- margin2*2);
        float q2d = x + margin2 + (q2-minVal)/(maxVal-minVal) *(w- margin2*2);
        float q3d = x + margin2 + (q3-minVal)/(maxVal-minVal) *(w- margin2*2);
        float q4d = x + margin2 + (q4-minVal)/(maxVal-minVal) *(w- margin2*2);

        QColor c1(Qt::black);
        c1.setAlpha(100);
        p.fillRect(q0d, y + (h-margin3)*0.5f - maxBarH*0.5,
                   q4d-q0d, maxBarH, QBrush(c1));
        QColor c2(Qt::black);
        c2.setAlpha(150);
        p.fillRect(q1d, y + (h-margin3)*0.5f - maxBarH*0.5,
                   q3d-q1d, maxBarH, QBrush(c2));
        p.save();
        p.setPen(Qt::red);
        p.drawLine(q2d, y + (h-margin3)*0.5f - maxBarH*0.5 -1,
                   q2d, y + (h-margin3)*0.5f + maxBarH*0.5 -1);
        p.restore();
    }
}

void ModelView::drawHistogramLabel(QPainter &p,
                                   QRectF bouding, ModelProfile *mp)
{
    QString name = mp->modelName;
    QFont f;
    f.setPointSize(6);
    QFontMetrics m(f);
    double l = mp->globRange.first;
    double h = mp->globRange.second;

    QString high = QString::number(h,'g',2);
    QString low = QString::number(l,'g',2);

    // draw y axis;
    p.save();
    p.setFont(f);
    //m = QFontMetrics(p.font());
    p.translate(bouding.left()+margin2,bouding.top()+m.width(high)+margin2);
    p.rotate(-90);
    p.drawText(0,0,high);
    p.restore();

    p.save();
    p.setFont(f);
    //m = QFontMetrics(p.font());
    p.translate(bouding.left()+margin2,bouding.bottom()-margin2);
    p.rotate(-90);
    p.drawText(0,0,low);
    p.restore();

    m = QFontMetrics(p.font());
    p.save();
    p.translate(bouding.left()+m.height(),
                bouding.top()+0.5*bouding.height()+m.width(name)*0.5);
    p.rotate(-90);
    p.drawText(0,0,name);
    p.restore();
}

void ModelView::drawHistogram(QPainter &p, QRectF bouding,
                              Histogram &hist,
                              int maxCnt)
{
    float x = bouding.left();
    float y = bouding.top();
    float w = bouding.width();
    float h = bouding.height();

    float barH = (h - margin2*2) / binSize;
    int j = 0;


    for(int c : hist.histo) {
        float xx = x+margin2;
        float yy = y+ h - margin2 - j * barH - barH;
        float ww = c*(w-2*margin2)/maxColorRange;
        QRectF r(xx,yy,ww,barH-1);
        hist.rects[j] = QRectF(r);
        QSet<int> tmp(selectionModel);
        int cnt = tmp.intersect(hist.lookup[j]).size();  //hist.lookup[j].intersect(selectionModel).size();
        int s = schemas.size()* cnt/std::min(maxColorRange,colorRange);
        if(s>=schemas.size()) s = schemas.size()-1;
        p.fillRect(r,QBrush(this->schemas[s]));
        p.drawRect(r); //QBrush(Qt::black)
        j++;
        //qDebug()<<r;
    }

    // draw x axis;

    // min count of time lines in each bin;
    double xl = 0;
    // max count ...
    double xh = this->maxColorRange;
    p.save();
    QFont f;
    f.setPointSize(6);
    p.setFont(f);
    QFontMetrics mm(p.font());
    p.drawLine(x+margin2,y+h-margin2,x+w-margin2,y+h-margin2);
    p.drawText(x+margin2,y+h-margin2+mm.height(),QString::number(xl));
    QString strh = QString::number(xh);
    p.drawText(x+w-margin2-mm.width(strh),y+h-margin2+mm.height(),strh);
    QString title = "# Time Series";
    p.drawText(x+w*0.5-mm.width(title)*0.5,y+h-mm.height(),title);
    p.restore();
}

void ModelView::drawLocalModelDates(QPainter &p, QRectF bouding,
                                    QDate start, QDate end, bool g)
{
    int x1 = bouding.left();
    //int x2 = bouding.right();
    int y = bouding.top();
    QFont f;
    f.setPointSize(8);
    QFontMetrics m(f);
    p.save();
    p.setFont(f);
    QString s1 = start.toString("MM/dd/yy");
    QString s2 = end.toString("MM/dd/yy");
    QString s = s1+" to "+s2;
    p.drawText(x1+margin2,y+m.height(),s);
    p.restore();
    //p.drawText(x2-m.width(s2)-margin2,y+m.height(),s2);
}

void ModelView::fillValues()
{
    QElapsedTimer timer;
    timer.start();

    maxColorRange = 1;

    int n = dataModel->modelProfiles.size();
    models.clear();
    models.resize(n);
    for(int i = 0; i< n; i++) {
        models[i].glHisto.histo = QVector<int>(binSize,0);;
        models[i].glHisto.lookup = QVector<QSet<int> >(binSize,QSet<int>());
        models[i].glHisto.rects =  QVector<QRectF>(binSize,QRectF());
        int wNum = dataModel->modelProfiles[i]->locEstimates.size();
        if(wNum==0) continue;
        models[i].locHisto.clear();
        models[i].locHisto.resize(wNum);
    }

    QVector<int> sel;
    dToM.clear();
    for(auto in : this->selectionModel) sel.push_back(in);
    qSort(sel.begin(),sel.end());

    // fill glob histo
    for(int i = 0; i<n; i++) {
        models[i].maxCnt = 0;
        int j = 0;  // for each time line objects;
        for(double v : dataModel->modelProfiles[i]->normglobEstimates) {
            // range of v is 0 t 1;
            // bin size is binSize;
            int idx = binSize * v/1.0f;
            if(idx >= binSize) idx = binSize-1;
            int c = ++models[i].glHisto.histo[idx];
            models[i].glHisto.lookup[idx].insert(sel[j]);
            dToM.insert(sel[j],j);
            maxColorRange = std::max(c,maxColorRange);
            models[i].maxCnt = std::max(models[i].maxCnt,c);
            j++;
        }
    }

    //fill loc histo;
    for(int i = 0; i< n; i++) {
        int m = dataModel->modelProfiles[i]->winNum;
        //models[i].maxCnts.resize(m);// = 0;
        models[i].locHisto.clear();
        models[i].locHisto.resize(m);
        ModelProfile* mp = dataModel->modelProfiles[i];
        for(int j = 0; j< m; j++) {
            //models[i].maxCnt = 0;
            models[i].locHisto[j].histo = QVector<int>(binSize,0);//.resize(binSize);
            models[i].locHisto[j].lookup = QVector<QSet<int> >(binSize,QSet<int>());//resize(binSize);
            models[i].locHisto[j].rects = QVector<QRectF>(binSize,QRectF());
            int k = 0;
            for(double d : mp->normlocEstimates[j]) {
                int idx = binSize*d/1.0f;
                if(idx >= binSize) idx = binSize -1;

                //qDebug()<<"sel size "<<sel.size();
                //qDebug()<<"i "<<i<<" < "<<models.size();
                //qDebug()<<"j "<<j<<" < "<<models[i].locHisto.size();
                //qDebug()<<"idx "<<idx<<" < "<<models[i].locHisto[j].histo.size();

                int c = ++models[i].locHisto[j].histo[idx];
                //kth element of set dataModel->selectedTimeLines
                models[i].locHisto[j].lookup[idx].insert(sel[k]);
                //qDebug()<<"fill value "<<models[i].locHisto[j].lookup[idx];
                maxColorRange = std::max(c,maxColorRange);
                models[i].maxCnt = std::max(models[i].maxCnt,c);
                k++;
                //qDebug()<<"k "<<k<<" done out of "<<mp->normlocEstimates[j].size();
            }
            //qDebug()<<"j "<<j<<" done out of "<<m;
            //
        }
    }
    timer.elapsed();
    qDebug()<<"fill success, time spent "<<timer.elapsed()<<" ms";
    emit colorRangeChanged(maxColorRange);
}

void ModelView::calculateIntersetion()
{
    this->selectionModel = QSet<int>(dataModel->tlSelsListView);
    for(auto & sel: dataModel->tlSelsData) selectionModel.intersect(sel);
    for(auto & ss : dataModel->tlSelsModel) selectionModel.intersect(ss);

    for(auto & brush : dataModel->tlBrushModel) {
        QSet<int> tmp;
        for(auto & ss: brush) tmp.unite(ss);
        selectionModel.intersect(tmp);
    }
    return;
}

void ModelView::updateBinSize(int x)
{
    if(!dataModel->m.tryLock()) return;
    binSize = x;
    fillValues();
    dataModel->m.unlock();
    this->repaint();
}

void ModelView::updateModelView()
{
    //for(int i = 0; i< dataModel->compNameSelected->stringList().size(); i++)
    //    this->selectionModel.insert(i);
    this->selectionModel = QSet<int>(dataModel->tlSelsListView);
    for(auto & ss: dataModel->tlSelsData) selectionModel.intersect(ss);
    for(auto & ss : dataModel->tlSelsModel) selectionModel.intersect(ss);
    //selectionModel = QSet<int>(dataModel->selectedTimeLinesModel);
    //selectionData.clear();
    //for(int ii : this->dataModel->selectedTimeLines) selectionData.insert(ii);
    fillValues();
    this->repaint();
}

void ModelView::updateColors(int x)
{
    this->colorRange = x;
    this->repaint();
}

void ModelView::updateBoxPlot(bool b)
{
    this->isBoxPlot = b;
    this->repaint();
}

void ModelView::updateTimeLineSelection(QSet<int> idx)
{
    // updated selected indices;
}

//dataModel->modelProfiles[0]->globEstimates;
//dataModel->modelProfiles[0]->globLikeliSum;
// process the

/*this->repaint();
if(dataModel->modelProfiles.empty()) return;

int h = dataModel->modelProfiles.size()*(bandHeight+bandBetween);
int ph= this->m_parent->geometry().height();
int newh = h>ph?h:ph;
this->setMinimumHeight(newh);
this->repaint();*/



/*void ModelView::paintEvent(QPaintEvent *e)
{
    int bandWidth = m_parent->geometry().width()-margin*2-left;
    int originx = margin+left;
    int originy = margin;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing,true);
    painter.fillRect(this->geometry(),Qt::white);
    modelRect.clear();
    modelRect.resize(dataModel->modelProfiles.size());
    for(int i = 0; i< dataModel->modelProfiles.size(); i++){

        //draw vertical lines;
        int currentIndent = dataModel->modelProfiles[i]->isDerived*indent;
        int x = originx+currentIndent;
        int y = originy+i*(bandHeight+bandBetween);
        int w = bandWidth-currentIndent;
        int h = bandHeight;
        int c = x+0.5*w;

        //painter.drawRect(x,y,w,h);
        painter.drawLine(x,y+h,x+w,y+h);
        modelRect[i] = QRect(x,y,w,h);

        QRect nameRect(margin+currentIndent,y,
                       left-currentIndent,bandHeight);


        // change this to color rectangle and
        // use the start and end date map to the height

        painter.drawText(nameRect,Qt::AlignCenter|Qt::TextWordWrap,
                         dataModel->modelProfiles[i]->profileName);

        if(dataModel->modelProfiles[i]->isDerived){
            painter.drawLine(margin,y-bandBetween,
                             margin,y+0.5f*bandHeight);
            painter.drawLine(margin,y+0.5f*bandHeight,
                             margin+currentIndent,y+0.5f*bandHeight);
        }

        for(int j = 0; j<dataModel->modelProfiles[i]->layout.values().size(); j++){
            QString key = dataModel->modelProfiles[i]->layout.keys()[j];
            double d = dataModel->modelProfiles[i]->layout.values()[j];
            double conf = dataModel->modelProfiles[i]->fit.values()[j];
            int lineLen = conf*h;
            if(i==0){
                int xpos = c+d*w*0.5;
                int ypos = y;
                if(typeid(*dataModel->modelProfiles[i])==typeid(VolatilityModel)){
                    xpos = x+d*w;
                }
                QPainterPath* path = new QPainterPath(QPointF(xpos,ypos));
                path->lineTo(xpos,ypos+h);

                //painter.drawLine(xpos,ypos,xpos,ypos+h);
                painter.drawLine(xpos,ypos+h-lineLen,xpos,ypos+h);
                this->paths.insert(key,path);
            }else{
                int xpos = c+d*w*0.5;
                int ypos = y;
                if(typeid(*dataModel->modelProfiles[i])==typeid(VolatilityModel)){
                    xpos = x+d*w;
                }
                QPainterPath* path = NULL;
                if(paths.contains(key)){
                    path = paths.value(key);
                    int oldx = path->currentPosition().rx();
                    int oldy = path->currentPosition().ry();
                    path->cubicTo(oldx,ypos,xpos,oldy,xpos,ypos);
                    path->lineTo(xpos,ypos+h);
                    //painter.drawLine(xpos,ypos,xpos,ypos+h);
                    painter.drawLine(xpos,ypos+h-lineLen,xpos,ypos+h);
                }else{
                    path = new QPainterPath(QPointF(xpos,ypos));
                    path->lineTo(xpos,ypos+h);
                    paths.insert(key,path);
                }
            }
        }
    }
    for(QPainterPath *p: paths.values()){
        //painter.drawPath(*p);
    }

    if(pHighlight!=NULL){
        QPen pen(Qt::red,3,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin);
        painter.save();
        painter.setPen(pen);
        painter.drawPath(*pHighlight);
        painter.restore();
    }

    if(clickHighlight>=0 && clickHighlight<modelRect.size()){
        painter.fillRect(modelRect[clickHighlight],QBrush(QColor(128, 128, 255, 128)));
    }

    if(hopHilight>=0 && hopHilight<modelRect.size()) {
        painter.save();
        QPen pen(Qt::red,2,Qt::DashDotLine,Qt::RoundCap,Qt::RoundJoin);
        painter.setPen(pen);
        //painter.fillRect(modelRect[clickHighlight],QBrush(QColor(128, 128, 255, 128)));
        painter.drawRect(modelRect[hopHilight]);
        painter.restore();
    }
    painter.drawText(mouseRect,Qt::AlignLeft,highlightName);
}*/

/*void ModelView::mouseReleaseEvent(QMouseEvent *e)
{
    //if(dragging) return;
    /*QPoint dist = e->pos()-mousePress;
    if(dist.manhattanLength()>10) return;

    if(e->button()==Qt::RightButton){ //right click remove clicked profile
        for(int i = 0; i<this->modelRect.size(); i++){
            if(modelRect[i].contains(e->pos())){
                ModelProfile * p = this->dataModel->modelProfiles[i];
                if(p) delete p;
                this->dataModel->modelProfiles.remove(i);
                this->modelRect.clear();
                if(clickHighlight == i){
                    clickHighlight = -1;
                }
                this->repaint();
                //emit modelProfileRemoved();
                break;
            }
        }
    }

    if(e->button()==Qt::LeftButton){ // left click focus on clicked profile;
        clickHighlight = -1;
        for(int i = 0; i<this->modelRect.size(); i++){
            if(modelRect[i].contains(e->pos())){
                clickHighlight = i;
                this->repaint();
                //emit modelProfileClicked(i); //dataModel->modelProfiles[i]
                break;
            }
        }
    }
    return;
}*/

/*void ModelView::mousePressEvent(QMouseEvent *e)
{
    // set flat to check if a release involves dragging
    // detect brushing behavior
    //mousePress = e->pos();
}*/

/*void ModelView::dragEnterEvent(QDragEnterEvent *e)
{

}

void ModelView::dragMoveEvent(QDragMoveEvent *e)
{

}

void ModelView::dragLeaveEvent(QDragLeaveEvent *e)
{

}*/

/*void ModelView::mouseMoveEvent(QMouseEvent *e)
{
    this->pHighlight = NULL;
    highlightName = "";
    mouseRect = QRect(-10,-10,0,0);
    int idx = -1;
    if(modelRect.empty()) return;
    for(int i = 0; i<this->modelRect.size(); i++){
        if(modelRect[i].contains(e->pos())){
            idx = i;
            break;
        }
    }
    if(idx<0) return;
    hopHilight = idx;
    //send signal to model eval view;
    //emit this->modelProfileHopover(idx);

    QRect r = this->modelRect[idx];
    int x = e->pos().x();
    int offset = x - r.x();
    int width = r.width();

    float v = (float)offset/(float)width;
    QString name = dataModel->modelProfiles[idx]->whichObject(v);

    if(name.isEmpty()){
        pHighlight = NULL;
        this->repaint();
        return;
    }

    pHighlight = this->paths.value(name);
    mouseRect = QRect(e->pos().x()-50,e->pos().y()-15,
                      100,15);
    highlightName = name;
    this->repaint();
    return;
}
*/


/*void ModelView::clearPath(QMap<QString, QPainterPath *> &paths)
{
    QMap<QString, QPainterPath *>::iterator it;
    for(it = paths.begin();it!=paths.end();it++){
        QPainterPath * p = it.value();
        if(p) delete p;
    }
    paths.clear();
}*/


/*void ModelView::highlightPath(const QModelIndex &idx)
{
    QString pathname = idx.data().toString();
    if(this->paths.contains(pathname)){
        pHighlight = this->paths.value(pathname);
    }else{
        pHighlight = NULL;
    }
    this->repaint();
}

void ModelView::highlightProfile(int x)
{
    if(x<0) return;
    this->hopHilight = x;
    this->repaint();
}*/

