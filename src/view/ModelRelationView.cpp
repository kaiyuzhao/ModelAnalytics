#include "ModelRelationView.h"

#include <QPainter>
#include <QDebug>
//#include <QGraphicsView>
//#include <QGraphicsScene>
#include <QPainterPath>
#include <QMouseEvent>
#include <QPen>

ModelRelationView::ModelRelationView(MainWindow* mw, QWidget *parent) :
    QWidget(parent)
{
    dataModel = mw->dataModel;
    m_parent = parent;

    QPalette pal(palette());
    margin = 20;

    highlightName = "";
    mouseRect = QRectF(-10,-10,0,0);
    highlightRectIdx = -1;

    //scene = new QGraphicsScene();
    //view = new QGraphicsView();
    //this->setScene(scene);
    //this->setAlignment(Qt::AlignLeft|Qt::AlignTop);
    //this->set(this);
    pal.setColor(QPalette::Background, Qt::white);
    setAutoFillBackground(true);
    setPalette(pal);

    setMouseTracking(true);
    setSizePolicy(QSizePolicy::Preferred,
                  QSizePolicy::Preferred);

    //this->resize();
    //connect(dataModel,SIGNAL(modelProfileChanged()),
    //        this,SLOT(updateModelRelationView()));
    //view->show();

}

ModelRelationView::~ModelRelationView()
{
    //if(view) delete view;
    // if(scene) delete scene;
}

void ModelRelationView::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    float w = this->geometry().width()-margin*2;
    float h = this->geometry().height()-margin*2;

    if(glyphProfile.empty()) return;
/*
    painter.drawRect(margin,margin,w,h);
    drawProfile(painter);

    QRectF highlight;
    if(highlightRectIdx<0 || highlightRectIdx>= profileRect.size()) return;

    highlight = this->profileRect.values()[highlightRectIdx];
    QPen pen(Qt::red,1,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin);

    painter.save();
    painter.setPen(pen);
    painter.drawEllipse(highlight.center(),
                        highlight.width()*0.5f,highlight.height()*0.5f);
    painter.drawText(mouseRect,Qt::AlignLeft,highlightName);
    painter.restore();
    */
}

/*void ModelRelationView::drawProfile(QPainter &painter)
{
    float glyphH = 30;
    float glyphW = glyphH;
    //float radius = glyphH/2.0f;

    float w = this->geometry().width()-margin*2;
    float h = this->geometry().height()-margin*2;

    h = h-glyphH;
    w = w-glyphW;

    profileRect.clear();

    for(int i = 0; i< pos.size(); i++){
        int x = (pos[i].rx()+1.0f)*0.5f*w+margin;
        int y = (pos[i].ry()+1.0f)*0.5f*h+margin;

        if(glyphProfile.isEmpty()) return;
        QVector<double> prof = this->glyphProfile.values()[i];
        QString name = this->glyphProfile.keys()[i];

        double dimSize = prof.size();
        //painter.drawRect(x,y,glyphH,glyphW);

        this->profileRect.insert(name,QRect(x,y,glyphH,glyphW));
        //QPainterPath p(QPointF(x,y));

        double angle = M_2PI/dimSize;

        for(int j = 0; j<prof.size(); j++){
            float l;
            if(prof[j]<0){
                l = glyphH*(1+prof[j])*0.5f;
            }else{
                l = glyphH*prof[j]*0.5f+0.5f;
            }
            float cx = x+0.5f*glyphW;
            float cy = y+0.5f*glyphH;
            float xpos = l*cos(angle*(double)j)+cx;
            float ypos = l*sin(angle*(double)j)+cy;
            painter.drawLine(xpos,ypos,cx,cy);

            int barW = glyphW/dimSize;
            int barH = -glyphH*0.5f*prof[j];
            int bary = y+barW*(double)j;
            int barx = x+glyphW*0.5f;

            if(j==0){
                //p.lineTo(x+barH,y);
                //continue;
            }

            //float oldx = x-(glyphH*0.5f*prof[j-1])*0.5f;
            //float oldy = y+barW*(double)(j-1);
            //p.cubicTo(oldx,bary,barx,oldy,barx,bary);
            //painter.drawPath(p);
        }

    }
}
*/
void ModelRelationView::mousePressEvent(QMouseEvent *e)
{

}

void ModelRelationView::mouseMoveEvent(QMouseEvent *e)
{
    /*mouseRect = QRectF(-10,-10,0,0);
    highlightName = "";
    highlightRectIdx = -1;
    if(profileRect.empty()) return;
    if(profileRect.values().empty()) return;
    for(int i = 0; i<profileRect.values().size(); i++){
        if(profileRect.values()[i].contains(e->pos())){
            highlightRectIdx = i;
            mouseRect = QRectF(e->pos().x()-50,e->pos().y()-20,100,20);
            highlightName = profileRect.keys()[i];
            this->repaint();
            return;
        }
    }*/
}

/*void ModelRelationView::resizeEvent(QResizeEvent *e)
{
    //qDebug()<<"resize event ";
}

void ModelRelationView::pcomp(QMap<QString, QVector<double> > &profile)
{
    using namespace Rcpp;
    if(profile.isEmpty()) return;

    int r = profile.size();
    int c = profile.begin().value().size();

    if(r==1){
        this->pos.resize(1);
        this->pos[0].setX(0);
        this->pos[0].setY(0);
        return;
    }

    if(r==2){
        this->pos.resize(2);
        this->pos[0].setX(-1);
        this->pos[0].setY(-1);

        this->pos[1].setX(1);
        this->pos[1].setY(1);
        return;
    }

    if(c==1){
        return;
    }
    if(c==2){
        this->pos.resize(r);
        for(int i = 0; i<r; i++){
            //qDebug()<<profile.values()[i];
            pos[i].setX(profile.values()[i][0]);
            pos[i].setY(profile.values()[i][1]);
        }
        return;
    }

    NumericMatrix pf(r,c);
    dataModel->m_r["pf"] = pf;
    for(int i = 0; i<r; i++){
        for(int j = 0; j<c; j++){
            pf(i,j) = profile.values()[i][j];
        }
    }

    QString eval = "fit <- cmdscale(dist(pf),eig=TRUE, k=2); fit$points;";

    SEXP ans = this->dataModel->m_r.parseEval(eval.toStdString());

    //to calculate 2d layout;
    NumericMatrix ansMtx(ans);

    pos.resize(ansMtx.nrow());

    for(int i = 0; i<ansMtx.nrow(); i++){
        float x = ansMtx(i,0)>1?1:ansMtx(i,0);
        x = x<-1?-1:x;
        float y = ansMtx(i,1)>1?1:ansMtx(i,1);
        y = y<-1?-1:y;
        pos[i].setX(x);
        pos[i].setY(y);
    }
}
*/
/*void ModelRelationView::updateModelRelationView()
{
    this->repaint();
    if(dataModel->modelProfiles.empty()) return;

    glyphProfile.clear();
    for(int i = 0; i<dataModel->modelProfiles.size(); i++){
        int currentSize = dataModel->modelProfiles[i]->layout.size();
        for(int j = 0; j<currentSize; j++){

            QString key = dataModel->modelProfiles[i]->layout.keys()[j];
            double d = dataModel->modelProfiles[i]->layout.values()[j];

            if(this->glyphProfile.contains(key)){
                glyphProfile[key].push_back(d);
            }else{
                this->glyphProfile[key].resize(i);
                this->glyphProfile[key].push_back(d);
            }
        }
        QMap<QString,QVector<double> >::iterator it;
        for(it=glyphProfile.begin();it!=glyphProfile.end(); it++){
            it.value().resize(i+1);
        }
    }
    this->pcomp(glyphProfile);
    */
    /*float w = this->geometry().width()-margin*3;
        float h = this->geometry().height()-margin*3;

        this->scene->clear();
        //this->view->setSceneRect(this->geometry());
        for(int k = 0; k< this->pos.size(); k++){
            float x = (pos[k].rx()+1.0f)*0.5f*w+margin;
            float y = (pos[k].ry()+1.0f)*0.5f*h+margin;

            QGraphicsGlyphItem *obj = new QGraphicsGlyphItem(this,glyphProfile.keys()[k],x,y);
            this->scene->addItem(obj);
        }
        //this->repaint();
        this->scene->update(this->geometry());

    this->repaint();
}

*/

/*void QGraphicsGlyphItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QVector<double> lyout = mrv->glyphProfile.value(name);
    painter->drawRect(x,y,w,h);
    double dimSize = lyout.size();
    for(int j = 0; j<lyout.size(); j++){
        float barW = w/dimSize;
        float barH = -h*0.5f*lyout[j];
        float barx = x+barW*(double)j;
        float bary = y+h*0.5f;
        painter->fillRect(QRectF(barx,bary,barW-1,barH),Qt::black);
    }
}


QGraphicsGlyphItem::QGraphicsGlyphItem(ModelRelationView *parent, QString objName, float xpos, float ypos)
    :x(xpos),y(ypos)
{
    this->w = 20;
    this->h = w;
    this->mrv = parent;
    this->name = objName;

    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsMovable);
}

QRectF QGraphicsGlyphItem::boundingRect() const
{
    return QRectF(x,y,w,h);
}*/
