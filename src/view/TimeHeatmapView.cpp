#include "TimeHeatmapView.h"
#include "DataModel.h"
#include "mainwindow.h"

#include <QVector>
#include <QSet>
#include <QPainter>
#include <QBrush>

#include <QDebug>
#include <utility>
#include <QString>

#include <QMouseEvent>

TimeHeatmapView::TimeHeatmapView(MainWindow *mw, QWidget *parent) :
    QWidget(parent)
{
    dm = mw->dataModel;
    QVector<QString> c1 = {"#ffffd9","#edf8b1","#c7e9b4",
                           "#7fcdbb","#41b6c4","#1d91c0",
                           "#225ea8","#253494","#081d58"};
    for(QString s: c1) schemas.push_back(QColor(s));
    numIntvl = 24;
    numBins = 10;
    margin = 45;
    coord = qMakePair(-1,-1);

    colorMaxCnt = INT_MAX;

    connect(this,SIGNAL(subsetSelected()),
            this,SLOT(updateSelection()));

    setMouseTracking(true);
}

TimeHeatmapView::~TimeHeatmapView()
{
}

void TimeHeatmapView::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    p.fillRect(0,0,this->geometry().width(),
               this->geometry().height(),Qt::white);

    if(grids.empty()) return;

    int w  = this->geometry().width() - margin*2;
    int h = this->geometry().height() - margin*2;

    float gw = (float)w/(float)this->numIntvl;
    float gh = (float)h/(float)this->numBins;

    int m = sgrids.size();
    int n = sgrids[0].size();
    rects = QVector<QVector<QRect> >(m,QVector<QRect>(n, QRect()));

    for(int i = 0; i< sgrids.size(); i++) {
        for(int j = 0; j<sgrids[i].size(); j++) {
            int x = margin+ gw*i;
            int y = h+ margin - gh*j - gh;
            int cnt = sgrids[i][j];
            int tmp = std::min(colorMaxCnt,maxCount);
            int idx = cnt*9/tmp;
            if(idx>=9) idx = 8;
            rects[i][j] = QRect(x,y,gw-1,gh-1);
            p.fillRect(rects[i][j],QBrush(schemas[idx]));
        }
    }
    drawColorLegend(p);
    drawAxes(p);
    drawSelection(p);
    drawHopover(p);
}

void TimeHeatmapView::mousePressEvent(QMouseEvent *e)
{
    if(!dm->m.tryLock()) return;
    selected = QSet<int> (dm->tlSelsListView);
    //dm->tlSelsData.clear();

    if(e->button() == Qt::RightButton){
        if(!trace.empty()) trace.pop();
        if(!dm->tlSelsData.empty()) dm->tlSelsData.pop();

        //for(int i = 0; i< n; i++) selected.insert(i);
        /*for(QPair<int,int> p : trace) {
            //dm->tlSelsData.push(reverseLookup[p.first][p.second]);
            selected.intersect(reverseLookup[p.first][p.second]);
            dm->tlSelsData.push(reverseLookup[p.first][p.second]);
        }*/
        for(auto & ss : dm->tlSelsData) selected.intersect(ss);

        // aggregate result in model space;
        for(auto & ss : dm->tlSelsModel) selected.intersect(ss);

        qDebug()<<selected;
    }
    if(e->button()== Qt::LeftButton) {
        int x = e->pos().x();
        int y = e->pos().y();

        int w  = this->geometry().width() - margin*2;
        int h = this->geometry().height() - margin*2;

        float gw = (float)w/(float)this->numIntvl;
        float gh = (float)h/(float)this->numBins;

        int xpos = (x-margin)/gw;
        int ypos = (h-y+margin)/gh;

        this->coord = qMakePair(xpos,ypos);

        if(reverseLookup.empty()){
            dm->m.unlock();
            return;
        }

        if(coord.first<reverseLookup.size()
                && coord.first>=0
                && coord.second<reverseLookup[coord.first].size()
                && coord.second>=0) {
            trace.push(coord);
            dm->tlSelsData.push(reverseLookup[coord.first][coord.second]);
        }

        // aggregate result from data space
        for(auto & ss : dm->tlSelsData) selected.intersect(ss);
        // aggregate result from model space;
        for(auto & ss : dm->tlSelsModel) selected.intersect(ss);

    }
    dm->m.unlock();
    emit subsetSelected();
    return;
}

void TimeHeatmapView::mouseMoveEvent(QMouseEvent *e)
{
    if(!dm->m.tryLock()) return;

    int x = e->pos().x();
    int y = e->pos().y();

    int w  = this->geometry().width() - margin*2;
    int h = this->geometry().height() - margin*2;

    float gw = (float)w/(float)this->numIntvl;
    float gh = (float)h/(float)this->numBins;

    int xpos = (x-margin)/gw;
    int ypos = (h-y+margin)/gh;

    coord = qMakePair(xpos,ypos);

    if(cnt.empty()) {
        dm->m.unlock();
        return;
    }
    //if(this->selected.empty()) return;
    if(coord.first<cnt.size() && coord.first>=0 && coord.second<cnt[0].size() && coord.second>=0) {
        this->hopovered = QSet<int> (cnt[coord.first][coord.second]);
        mouseMove = e->pos();
    }else{
        this->hopovered.clear();
    }
    dm->m.unlock();
    this->repaint();
}

void TimeHeatmapView::drawAxes(QPainter &p)
{
    //draw 10 values for y axis;

    if(grids.empty()) return;

    int w  = this->geometry().width() - margin*2;
    int h = this->geometry().height() - margin*2;
    QFontMetrics m(p.font());
    for(int i = 0; i<= 10; i++) {
        QString lb = QString::number(i/10.0f,'g',2);
        int x = margin-m.width(lb);
        int y = this->geometry().height()-margin - h/10.0f*i;
        p.drawText(x,y,lb);
    }

    //draw 12 values for x axis;
    if(dm->dateBuff.values().empty()) return;
    int len = dm->dateBuff.values()[0].size();
    for(int i = 0; i<= 12; i++) {
        int x = margin + w/12*i;
        int y = h+margin*1.5f;
        int idx = len*i/12;
        if(idx >= len) idx = len-1;
        QDate d = dm->dateBuff.values()[0][idx];
        p.save();
        p.translate(x,y);
        p.rotate(25);
        p.drawText(0,0,d.toString("MM/dd/yy"));
        p.restore();
    }
}

void TimeHeatmapView::drawSelection(QPainter &p)
{
    if(trace.empty()) return;
    for(auto& coord : trace) {
        if(coord.first<numIntvl && coord.first>=0 && coord.second<numBins && coord.second>=0) {
            p.save();
            p.setPen(QPen(Qt::red,1,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
            p.drawRect(rects[coord.first][coord.second]);
            p.restore();
        }
    }
}

void TimeHeatmapView::drawHopover(QPainter &p)
{
    int x = this->mouseMove.x();
    int y = this->mouseMove.y();
    //int numDisp = this->hopovered.size();
    if(this->hopovered.empty()) return;
    //qDebug()<<hopovered.size();
    //int w = 300;
    int h = 300;

    QString text = "";
    int cont = 0;
    int charNum = 15;
    int rowCnt = 6;
    QFontMetrics fmetric(p.font());
    int rowH = fmetric.height()+1;
    int w = 0;
    for(auto& i : hopovered) {
        QString full = dm->compNameSelected->stringList()[i];
        QString name = full.size()>charNum?(full.left(charNum)+"...\n"):(full+"\n");
        text += name;
        w = std::max(w,fmetric.width(name));
        if(++cont == rowCnt && cont<hopovered.size()) {
            QString last = "and "+QString::number(hopovered.size()-cont)+" others";
            text+= last;
            w = std::max(w,fmetric.width(last));
            break;
        }
    }
    if(text[text.size()-1]=='\n') text = text.left(text.size()-1);
    h = std::min(h,std::min(rowH*(cont+1), rowH*hopovered.size()));
    //w = std::min(w,rowW*charNum);
    QRect r(x,y,w,h);
    QColor c("#bebada");
    c.setAlpha(120);
    p.fillRect(r,QBrush(c));

    p.save();
    p.setPen(QPen(Qt::red,1,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
    p.drawText(r,Qt::AlignCenter,text);
    p.restore();
}

void TimeHeatmapView::drawColorLegend(QPainter &p)
{
    if(grids.empty()) return;

    int n = this->schemas.size();
    int legSize = 10;  // 15 pixels;
    for(int i =0; i< n; i++){
        int x = this->geometry().width()-margin+legSize;
        int y = this->geometry().height()/2.0f + legSize * n /2.0f - i*legSize;
        p.fillRect(x,y,legSize,-legSize,QBrush(this->schemas[i]));
    }
    p.save();
    QFontMetrics m(p.font());  // center the legend number
    p.translate(this->geometry().width()-margin+legSize*2+m.height(),
                this->geometry().height()/2.0f - legSize * n /2.0f);
    p.rotate(-90);
    int tmp = std::min(colorMaxCnt,maxCount);
    //int idx = cnt*9/tmp;
    QString txt = QString::number(tmp);
    int offset = m.width(txt)/2.0f;
    p.drawText(-offset,0,txt);
    txt = QString::number(tmp/2);
    offset = m.width(txt)/2.0f;
    p.drawText(-legSize * n / 2.0f-offset,0,txt);
    txt = QString::number(0);
    offset = m.width(txt)/2.0f;
    p.drawText(-legSize * n-offset, 0, txt);
    p.restore();

    p.save();
    QString yName = "Normalized Price";
    p.translate(m.height(),this->geometry().height()/2.0f+m.width(yName)/2.0f);
    p.rotate(-90);
    p.drawText(0,0,yName);
    p.restore();

    QString title = "TiMov View";
    p.drawText(this->geometry().width()/2.0f-m.width(title)/2.0f,m.height()*2.0f,title);
}

void TimeHeatmapView::normalize(QVector<QVector<double> > &norm)
{
    norm.clear();
    if(dm->compNameSelected->stringList().empty()) return;
    if(dm->priceBuff.empty()) return;
    //int n = this->dm->priceBuff.values().size();
    int n = dm->compNameSelected->stringList().size();
    norm.clear();
    norm.resize(n);
    for(int i = 0; i<n; i++){
        QString k = dm->compNameSelected->stringList()[i];
        norm[i] = dm->normPriceBuff.value(k);
    }
}

int TimeHeatmapView::fillMap(int itvl, int bins, QVector<QVector<double> > &norm,
                             QVector<QVector<QSet<int> > > &reverse,
                             QVector<QVector<int> > &grids)
{
    int m = 0;
    grids = QVector<QVector<int> >(itvl, QVector<int>(bins,0));
    //QVector<QSet<int> > set = QVector<QSet<int> >(10, QSet<int>());
    reverse = QVector<QVector<QSet<int> > >(itvl, QVector<QSet<int> >(bins, QSet<int>()));

    for(int i = 0; i< norm.size(); i++){ // for each time line;
        for(int  j= 0; j< norm[i].size(); j++) {
            double val = norm[i][j];
            double range = 1.0f/(double)bins;
            double wSize = norm[i].size()/(double)itvl;
            int bId = val/range;
            int itvId = (double)j/(double)wSize;
            if(bId >= bins) bId = bins-1;
            if(itvId >= itvl) itvId = itvl -1;
            reverse[itvId][bId].insert(i);
            grids[itvId][bId] = reverse[itvId][bId].size();
            m = std::max(m,grids[itvId][bId]);
        }
    }
    return m==0?1:m;
}

void TimeHeatmapView::fillSelectedMap(int itvl, int bins,
                                      const QVector<QVector<double> > &norm,
                                      QVector<QVector<QSet<int> > > &cnt,
                                      QVector<QVector<int> > &sgrids)
{
    cnt = QVector<QVector<QSet<int> > >
            (itvl, QVector<QSet<int> >(bins, QSet<int>()));

    sgrids = QVector<QVector<int> >(itvl, QVector<int>(bins,0));

    //no selection in either data space or model space;
    if(dm->tlSelsData.empty() && dm->tlSelsModel.empty()) {
        for(int i = 0; i< numIntvl; i++){ // for each time line;
            for(int  j= 0; j< numBins; j++) {
                sgrids[i][j] = this->grids[i][j];
                cnt[i][j] = this->reverseLookup[i][j];
            }
        }
        return;
    }

    if(selected.empty()) {
        //nothing to display;
        for(int i = 0; i< numIntvl; i++){ // for each time line;
            for(int  j= 0; j< numBins; j++) {
                sgrids[i][j] = 0;
                cnt[i][j] = this->reverseLookup[i][j];
            }
        }
        return;
    }

    QList<int> l = selected.toList();
    for(int k = 0; k< l.size(); k++){ // for each time line;
        int i = l[k];
        for(int  j= 0; j< norm[i].size(); j++) {
            double val = norm[i][j];
            double range = 1.0f/(double)bins;
            double wSize = norm[i].size()/(double)itvl;
            int bId = val/range;
            int itvId = (double)j/(double)wSize;
            if(bId >= bins) bId = bins-1;
            if(itvId >= itvl) itvId = itvl -1;
            cnt[itvId][bId].insert(i);
            sgrids[itvId][bId] = cnt[itvId][bId].size();
        }
    }
    return;

}

void TimeHeatmapView::updateTimeHeatmapView()
{
    selected = QSet<int>(dm->tlSelsListView);
    for(auto & ss : dm->tlSelsData) selected.intersect(ss);
    for(auto & ss: dm->tlSelsModel) selected.intersect(ss);

    normalize(norm);
    this->coord = qMakePair(-1,-1);
    this->trace.clear();
    this->hopovered.clear();

    maxCount = fillMap(numIntvl, numBins, norm, reverseLookup, grids);
    fillSelectedMap(numIntvl, numBins, norm, cnt,sgrids);
    this->repaint();
    // the max cnt in each bin changed;
    emit colorRangeChanged(maxCount);
}

void TimeHeatmapView::updateBins(int b)
{
    if(!dm->m.tryLock()) return;
    numBins = b;
    this->coord = qMakePair(-1,-1);
    this->trace.clear();
    this->hopovered.clear();
    if(norm.empty()) {
        dm->m.unlock();
        return;
    }

    maxCount = fillMap(numIntvl, numBins, norm, reverseLookup, grids);
    fillSelectedMap(numIntvl, numBins, norm, cnt,sgrids);

    dm->m.unlock();
    emit colorRangeChanged(maxCount);
    this->repaint();
}

void TimeHeatmapView::updateIntvls(int i)
{
    numIntvl = i;
    this->coord = qMakePair(-1,-1);
    this->trace.clear();
    this->hopovered.clear();
    if(norm.empty()) return;

    maxCount = fillMap(numIntvl, numBins, norm, reverseLookup, grids);
    fillSelectedMap(numIntvl, numBins, norm, cnt,sgrids);
    this->repaint();

    // the max cnt in each bin changed;
    emit colorRangeChanged(maxCount);
}

void TimeHeatmapView::updateSelection()
{
    selected = QSet<int>(dm->tlSelsListView);
    for(auto & ss: dm->tlSelsData) selected.intersect(ss);
    for(auto & ss: dm->tlSelsModel) selected.intersect(ss);
    qDebug()<<selected;
    fillSelectedMap(numIntvl, numBins, norm, cnt, sgrids);
    this->repaint();
}

void TimeHeatmapView::updateColors(int x)
{
    this->colorMaxCnt = x;
    this->repaint();
}

void TimeHeatmapView::updateIndexSelection(QSet<int> idx)
{
    this->selected = QSet<int>(idx);
    this->repaint();
}
