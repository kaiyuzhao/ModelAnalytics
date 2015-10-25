#include "ConsensusView.h"
#include "mainwindow.h"
#include "DataModel.h"

#include <math.h>
#include <QPair>
#include <tuple>
#include <QColor>
#include <QMouseEvent>

ConsensusView::ConsensusView(MainWindow *mw, QWidget *parent) :
    QWidget(parent)
{
    mainWin = mw;
    dm = mw->dataModel;

    glyphSize = 130;

    margin = 15;

    inner = 5;

    ring = 5;

    space = 20;

    selectedGlyph = -1;

    outter = glyphSize/2 - ring-space;

    simThres = 1;

    QVector<QString> ss = {
        "#7fc97f","#beaed4","#fdc086"
    };
    for(auto& s: ss) schema.push_back(QColor(s));
    //this->setsetRenderHints(QPainter::Antialiasing,false);
    this->setMouseTracking(true);
}

void ConsensusView::paintEvent(QPaintEvent *e)
{
    //double maxthick = 5;
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing,true);
    p.fillRect(this->geometry(),Qt::white);

    if(this->glyphRadars.empty()) return;
    QFontMetrics mm(p.font());
    for(int i = 0; i< glyphRadars.size();i++) {
        this->drawGlyph(this->glyphRadars[i],this->glyphRects[i],p);
        QColor c(Qt::white);
        double alfa = similarity[i]>=simThres?1:similarity[i]/simThres;
        c.setAlphaF(1-alfa);
        QBrush b(c);
        QRectF f(glyphRects[i].left()-space,glyphRects[i].top()-space,
                 glyphRects[i].width()+space*2, glyphRects[i].height()+space*2);
        p.fillRect(f,b);
        QString str = QString::number(this->similarity[i],'g',2);
        p.drawText(f.left(),f.top()+mm.height(),str);
    }
    p.save();
    p.setPen(QPen(Qt::red,1,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
    p.drawRect(highlight);
    /*for(int i = 0; i< glyphRadars.size(); i++) {
        double thick = maxthick*dist[i];
        p.setPen(QPen(Qt::red,thick,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
        p.drawRect(this->glyphRects[i]);
    }*/
    p.restore();
}

void ConsensusView::mouseMoveEvent(QMouseEvent *e)
{

}

void ConsensusView::mousePressEvent(QMouseEvent *e)
{
    highlight = QRectF();
    selectedGlyph = -1;
    //this->repaint();
    if(this->glyphRadars.empty()) return;
    for(int i = 0; i< glyphRadars.size(); i++) {
        if(glyphRects[i].contains(e->pos())){
            highlight = glyphRects[i];
            selectedGlyph = i;
            break;
        }
    }
    this->distanceFunc();
    this->repaint();

}

void ConsensusView::updateGlyphs()
{
    int numRow = (this->geometry().height()-margin*2)/glyphSize;
    int numCol = (this->geometry().width()-margin*2)/glyphSize;
    //get a list of rect;
    int n = numRow*numCol;
    this->glyphRects.resize(n);
    for(int i = 0; i< n; i++) {
        int r = i%numRow;
        int c = i/numRow;
        glyphRects[i] = QRectF(margin+glyphSize*c + space,
                               margin+glyphSize*r + space,
                               glyphSize- 2* space,glyphSize- 2* space);
    }
    if(dm->dateBuff.empty()) return;
    if(dm->dateBuff.values().empty()) return;

    while(dm->glyphs.size()>numRow*numCol) {
        dm->glyphs.removeFirst();
    }
    this->glyphRadars.resize(dm->glyphs.size());

    this->similarity = QVector<double>(this->glyphRadars.size(),1.0f);

    for(int i = 0; i< this->dm->glyphs.size(); i++){

        glyphRadars[i].indices = dm->glyphs[i].brush;
        glyphRadars[i].whichModel = dm->glyphs[i].whichModel;
        glyphRadars[i].active = false;

        if(dm->glyphs[i].wId<0){
            glyphRadars[i].start = 0;
            glyphRadars[i].end = 360;
        }else{
            glyphRadars[i].start = (float)dm->glyphs[i].wId /
                    (float)dm->glyphs[i].wSize * 360.0f;
            glyphRadars[i].end = (float)(dm->glyphs[i].wId +1) /
                    (float)dm->glyphs[i].wSize * 360.0f;
        }

        //loop over different models, drift, ma, seasonal;
        this->glyphRadars[i].est.resize(dm->glyphs[i].models.size());
        for(int j = 0; j<dm->glyphs[i].models.size(); j++){
            FiveNumberTuple t = this->dm->glyphs[i].summary[j];
            QPair<double,double> pair = dm->glyphs[i].models[j]->globRange;
            double mi = 0;
            double mx = 1;

            double q4 = std::get<4>(t);
            double q3 = std::get<3>(t);
            double q2 = std::get<2>(t);
            double q1 = std::get<1>(t);
            double q0 = std::get<0>(t);

            if(mx != mi) {
                double r0 = (q0 - mi)/(mx-mi) * (this->outter - this->inner)
                        + this->inner;
                double r1 = (q1 - mi)/(mx-mi) * (this->outter - this->inner)
                        + this->inner;
                double r2 = (q2 - mi)/(mx-mi) * (this->outter - this->inner)
                        + this->inner;
                double r3 = (q3 - mi)/(mx-mi) * (this->outter - this->inner)
                        + this->inner;
                double r4 = (q4 - mi)/(mx-mi) * (this->outter - this->inner)
                        + this->inner;
                 this->glyphRadars[i].est[j] = std::make_tuple(r0,r1,r2,r3,r4);
            }else{
                double r0 = (outter - inner)/2.0+ inner;
                double r1 = (outter - inner)/2.0+ inner;
                double r2 = (outter - inner)/2.0+ inner;
                double r3 = (outter - inner)/2.0+ inner;
                double r4 = (outter - inner)/2.0+ inner;
                 this->glyphRadars[i].est[j] = std::make_tuple(r0,r1,r2,r3,r4);
            }
        }
    }
    this->repaint();
}

void ConsensusView::updateThreshold(double d)
{
    //do the filtering selection here;
    this->simThres = d;

    for(int i = 0; i< similarity.size(); i++) {
        this->glyphRadars[i].active = false;
        if(similarity[i]>=d) {
            this->glyphRadars[i].active = true;
        }
    }

    this->repaint();
}

void ConsensusView::updateAggregation()
{
    //this->dm->dataSelected;
    QSet<int> agg;

    for(int i = 0; i< this->glyphRadars.size(); i++) {
        agg.unite(glyphRadars[i].indices);
    }

    emit indexAggregated(agg);
}

void ConsensusView::drawGlyph(GlyphRadar rd, QRectF r, QPainter &p)
{
    // create a smaller rectangle;
    // for inner circle
    QRect sm(r.left()+ring,r.top()+ring,
             r.width()-2*ring, r.height()-2*ring);

    //draw clock ticks;
    QPointF c = r.center();
    double s = 20.0f;
    double fontRect = 20;
    double all = PI*2.0;

    p.save();
    p.setPen(QPen(Qt::black,1,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));

    for(int i = 0; i< s; i++){
        double theta = all/s*i;
        float x1 = c.x()+r.width()/2.0*sin(theta);
        float y1 = c.y()+r.width()/2.0*cos(theta);

        float x2 = c.x()+sm.width()/2.0*sin(theta);
        float y2 = c.y()+sm.width()/2.0*cos(theta);
        p.drawLine(x1,y1,x2,y2);

        QFont f;
        f.setPixelSize(9);
        p.setFont(f);
        if(i%5==0) {

            float x3 = c.x()+(r.width()/2.0+fontRect/2.0)*sin(PI-theta);
            float y3 = c.y()+(r.width()/2.0+fontRect/2.0)*cos(PI-theta);

            QRectF rec(x3-fontRect/2.0f,y3-fontRect/2.0f,fontRect,fontRect);

            QString str = QString::number(i/s,'g',2);
            p.drawText(rec,Qt::AlignCenter|Qt::AlignHCenter,str);
        }

    }
    p.restore();

    //need time range // start to end;
    //need value range of drift, ma and season corr
    // that is a pair of triplet 6 numbers

    QPainterPath path;
    path.moveTo(r.center());
    //path.arcMoveTo(r,-180);
    //path.arcMoveTo(r,rd.start);
    path.arcTo(r,360-rd.start+90,rd.start-rd.end);

    //path.arcMoveTo();
    path.arcTo(sm,360-rd.end+90,rd.end-rd.start);
    //path.closeSubpath();
    //QConicalGradient grad;
    //grad.setAngle(90);
    //grad.setCenter(r.center());
    //grad.setColorAt(0,Qt::red);
    //grad.setColorAt(1,Qt::yellow);
    QColor cc(Qt::red);
    cc.setAlpha(120);
    p.fillPath(path,QBrush(cc));

    for( int i = 0; i< rd.est.size(); i++) {
        //double s = 90;
        double sweep = -360.0f/rd.est.size();

        double s = 360.0f - i*360.0f/rd.est.size()+90;
        double e = 360.0f - (i+1)*360.0f/rd.est.size()+90;

        FiveNumberTuple t = rd.est[i];

        double q4 = std::get<4>(t);
        double q3 = std::get<3>(t);
        double q2 = std::get<2>(t);
        double q1 = std::get<1>(t);
        double q0 = std::get<0>(t);

        QPainterPath pat;

        pat.moveTo(r.center());

        QRectF r3(r.left()+(r.width()-q3*2)*0.5f,
                  r.top()+(r.height()-q3*2)*0.5f,q3*2,q3*2);

        QRectF r1(r.left()+(r.width()-q1*2)*0.5f,
                  r.top()+(r.height()-q1*2)*0.5f,q1*2,q1*2);


        QRectF r4(r.left()+(r.width()-q4*2)*0.5f,
                  r.top()+(r.height()-q4*2)*0.5f,q4*2,q4*2);

        QRectF r0(r.left()+(r.width()-q0*2)*0.5f,
                  r.top()+(r.height()-q0*2)*0.5f,q0*2,q0*2);

        QRectF r2(r.left()+(r.width()-q2*2)*0.5f,
                  r.top()+(r.height()-q2*2)*0.5f,q2*2,q2*2);


        pat.arcTo(r4,s,sweep);
        pat.arcTo(r0,e,-sweep);

        p.save();
        /*QPainterPath pat2;
        pat2.moveTo(r.center());
        pat2.arcTo(r4,s,sweep);
        pat2.arcTo(r0,e,-sweep);*/

        schema[i].setAlphaF(0.8f);
        p.fillPath(pat,QBrush(schema[i]));

        schema[i].setAlphaF(0.8f);
        //p.fillPath(pat2,QBrush(schema[i]));
        if(i==rd.whichModel){
            QColor c(Qt::blue);
            c.setAlphaF(0.8);
            p.setPen(QPen(c,2,Qt::DashDotLine));
            double theta1 = i*PI*2.0f/rd.est.size();
            double theta2 = (i+1)*PI*2.0f/rd.est.size();
            double x11 = r0.center().x()+r0.width()*0.5f*sin(theta1);
            double y11 = r0.center().y()-r0.width()*0.5f*cos(theta1);
            double x12 = r4.center().x()+r4.width()*0.5f*sin(theta1);
            double y12 = r4.center().y()-r4.width()*0.5f*cos(theta1);
            p.drawLine(x11,y11,x12,y12);
            double x21 = r0.center().x()+r0.width()*0.5f*sin(theta2);
            double y21 = r0.center().y()-r0.width()*0.5f*cos(theta2);
            double x22 = r4.center().x()+r4.width()*0.5f*sin(theta2);
            double y22 = r4.center().y()-r4.width()*0.5f*cos(theta2);
            p.drawLine(x21,y21,x22,y22);
            p.drawArc(r0,s*16,sweep*16);
            p.drawArc(r4,s*16,sweep*16);
        }else{
            p.setPen(QPen(schema[i]));
            p.drawArc(r0,s*16,sweep*16);
            p.drawArc(r4,s*16,sweep*16);
        }


        //QColor blk(Qt::black);
        schema[i].setAlphaF(1);

        p.setPen(QPen(schema[i]));
        //p.drawArc(r1,s*16,sweep*16);
        //p.drawArc(r3,s*16,sweep*16);
        p.restore();
    }

    //p.save();
    //p.setPen(QPen(Qt::black,1,Qt::DashDotLine,Qt::RoundCap,Qt::RoundJoin));
    //p.drawRect(r);
    //p.restore();
}

void ConsensusView::distanceFunc()
{
    //this->dist.clear();
    this->similarity = QVector<double>(this->glyphRadars.size(),1.0f);

    if(glyphRadars.empty()) return;
    if(this->selectedGlyph <0 || this->selectedGlyph>this->glyphRadars.size())
        return;

    for(int i = 0; i< this->glyphRadars.size(); i++){//GlyphRadar gr : this->glyphRadars) {
        QSet<int> sel = glyphRadars[selectedGlyph].indices;
        int intCnt = sel.intersect(glyphRadars[i].indices).size();
        sel = glyphRadars[selectedGlyph].indices;
        int untCnt = sel.unite(glyphRadars[i].indices).size();
        similarity[i] = (float)intCnt/(float)untCnt;
    }
}
