#include "loupe.h"
#include "math.h"

Loupe::Loupe(Fenim *pf,QMainWindow *parent,int x,int y) :
    QMainWindow(parent)
{
    PFenim=pf;
    _lzx=x;
    _lzy=y;
    _vaf=false;
    _gboxButtons = new QGroupBox(this);
    _bZoom = new MyQPushButton(_gboxButtons);
    _bUnZoom = new MyQPushButton(_gboxButtons);
    _bZoomH = new MyQPushButton(_gboxButtons);
    _bUnZoomH = new MyQPushButton(_gboxButtons);
    _bcGrid = new QCheckBox(QString("Grid"),_gboxButtons);
    BcMasterPoints = new QCheckBox(QString("Master points"),_gboxButtons);
    BcCalls = new QCheckBox(QString("Cries"),_gboxButtons);
    BcSuppl = new QCheckBox(QString("Add."),_gboxButtons);

    LabelX = new QLabel(_gboxButtons);
    LabelY = new QLabel(_gboxButtons);
    LabelR = new QLabel(_gboxButtons);
    LabelEnergy = new QLabel(_gboxButtons);
    _lbo=55;
    _hbo=20;
    setWindowTitle("Tadarida - Zoom");
    LastCenterX = -1;
    _nliv=0;_nlih=0;
    for(int i=0;i<PFenim->CallsNumber && i<MAXCRI;i++)
    {
        _ipmc[i]=false;
        _ilc[i]=false;
        _coucri[i]=0;
        _ilcs[i]=0;
        _ilps[i]=0;
    }
    for(int jcrete=0;jcrete<NCRETES;jcrete++)
        _gplt[jcrete] = new QGraphicsPathItem *[MAXCRI];

}

Loupe::~Loupe()
{
    for(int jcrete=0;jcrete<NCRETES;jcrete++)
        delete _gplt[jcrete];
    PFenim->ShowLoupe = false;
    delete LoupeScene;
    delete LoupeView;
}


void Loupe::ShowLoupe()
{
    QRect rfm=PFenim->GetWindowRect();
    QRect rvm=PFenim->GetView()->geometry();
    resize(rvm.width()/2,rvm.height());
    move(rfm.left()+rvm.left()+rvm.width()/2,rfm.top()+rvm.top());
    Fenima = PFenim->GetImage();
    LoupeScene = new MyQGraphicsScene(PFenim,this,true);
    LoupeView = new MyQGraphicsView(this);
    LoupeView->setScene(LoupeScene);
    _pix=(LoupeScene->addPixmap(QPixmap::fromImage(*Fenima))); // ajout du pixmap dans la scene
    activateWindow();
    raise();
    show();
    _llf=width();
    _lhf=height();
    LoupeView->move(0,0);
    LoupeView->setFixedSize(_llf,_lhf-_hbo);
    _lWl=PFenim->PFenimWindow->WidthRatio * 10;
    _lWh=PFenim->PFenimWindow->HeightRatio * 1.5f;
    _liaj=0.0f;
    LoupeView->scale(_lWl,_lWh);
    LoupeView->centerOn(_lzx,_lzy);
    LastCenterX = _lzx;
    LastCenterY = _lzy;
    _vaf=true;
    LoupeView->setMouseTracking(true);
    ShowGrid(true);
    showButtons();
    _bcGrid->setChecked(true);
    BcMasterPoints->setChecked(true);
    BcCalls->setChecked(true);
    BcSuppl->setChecked(false);
    ShowCalls();
    ShowMasterPoints();
    showRatio();
    connect(_bZoom,SIGNAL(clicked()),this,SLOT(Zoom()));
    connect(_bUnZoom,SIGNAL(clicked()),this,SLOT(UnZoom()));
    connect(_bZoomH,SIGNAL(clicked()),this,SLOT(ZoomH()));
    connect(_bUnZoomH,SIGNAL(clicked()),this,SLOT(UnZoomH()));
    connect(_bcGrid,SIGNAL(stateChanged(int)),this,SLOT(ActivateGrid(int)));
    connect(BcMasterPoints,SIGNAL(stateChanged(int)),this,SLOT(ActivateMasterPoints()));
    connect(BcCalls,SIGNAL(stateChanged(int)),this,SLOT(ActivateCalls()));
    connect(BcSuppl,SIGNAL(stateChanged(int)),this,SLOT(ActivateOtherCrests()));

    //
}

void Loupe::showButtons()
{
    _gboxButtons->resize(_llf,_hbo);
    _gboxButtons->move(1,_lhf-_hbo);

    int margx = 8;
    int lzb = _gboxButtons->width();
    _lbo = (lzb-margx*14)/13;

    _bZoom->move(margx,1);
    _bZoom->resize(_lbo,_hbo);
    _bZoom->setText("Z +");

    _bUnZoom->move(margx*2+_lbo,1);
    _bUnZoom->resize(_lbo,_hbo);
    _bUnZoom->setText("Z -");

    _bZoomH->move(margx*3+_lbo*2,1);
    _bZoomH->resize(_lbo,_hbo);
    _bZoomH->setText("ZH +");

    _bUnZoomH->move(margx*4+_lbo*3,1);
    _bUnZoomH->resize(_lbo,_hbo);
    _bUnZoomH->setText("ZH -");


    _bcGrid->move(margx*5+_lbo*4,1);
    _bcGrid->resize(_lbo,_hbo);

    BcMasterPoints->move(margx*6+_lbo*5,1);
    BcMasterPoints->resize(_lbo,_hbo);

    BcCalls->move(margx*7+_lbo*6,1);
    BcCalls->resize(_lbo,_hbo);

    BcSuppl->move(margx*8+_lbo*7,1);
    BcSuppl->resize(_lbo,_hbo);

    LabelEnergy->move(lzb - _lbo*4 - margx*4,1);

    LabelR->move(lzb - _lbo*3 - margx*3,1);
    LabelX->move(lzb - _lbo*2 - margx*2,1);
    LabelY->move(lzb - _lbo - margx,1);

    LabelEnergy->resize(_lbo,_hbo);
    LabelR->resize(_lbo,_hbo);
    LabelX->resize(_lbo,_hbo);
    LabelY->resize(_lbo,_hbo);


}

void Loupe::ShowGrid(bool afficher)
{
    if(_nliv>0) for(int i=0;i<_nliv;i++) delete _gliv[i];
    if(_nlih>0) for(int i=0;i<_nlih;i++) delete _glih[i];
    _nliv=0; _nlih=0;
    if(!afficher) return;

    int xmax = Fenima->width();
    int ymax = Fenima->height();
    float tmax = PFenim->Getms(xmax-1);
    float igt=0;
    float incrt = 500;
    if(_lWl>0.25) incrt = 100;
    QPen qp = QPen(QColor(128,128,128),0);
    while(igt<tmax && _nliv < 500)
    {
        float x=PFenim->GetX(igt);
        _gliv[_nliv++] = LoupeScene->addLine(x,0,x,ymax-1,qp);
        igt += incrt;
    }
    //
    float fmax = PFenim->GetkHz(0);
    float igf=0;
    float incrf = 10;
    while(igf<fmax && _nlih < 500)
    {
        float y=PFenim->GetY(igf);
        _glih[_nlih++] = LoupeScene->addLine(0,y,xmax-1,y,qp);
        igf += incrf;
    }
}


void Loupe::resizeEvent(QResizeEvent *re)
{
    QPointF pv = LoupeView->mapToScene(1,1);
    LoupeScene->activeWindow();
    if(!_vaf) return;
    _llf=width();
    _lhf=height();

    LoupeView->setFixedSize(_llf,_lhf - _hbo);
    showButtons();
    LoupeView->centerOn(LastCenterX,LastCenterY);
}

/*
void Loupe::rafraichit_image()
{
    pix->setPixmap(QPixmap::fromImage(*fenima));
    // reafficherCris();
}
*/

void Loupe::ShowCalls()
{
    bool affichercri = BcCalls->isChecked();
    for(int i=0;i<PFenim->CallsNumber;i++) ShowOneCall(i,PFenim->SelectedCalls[i],affichercri);
}

void Loupe::ShowMasterPoints()
{
    bool afficherpm = BcMasterPoints->isChecked();
    for(int i=0;i<PFenim->CallsNumber;i++) ShowOneMasterPoint(i,PFenim->SelectedCalls[i],afficherpm);
}

void Loupe::ShowOtherCrests()
{
    bool affichersuppl = BcSuppl->isChecked();
    for(int i=0;i<PFenim->CallsNumber;i++) ShowOneOtherCrestl(i,affichersuppl);
}

void Loupe::ShowOtherPoints()
{
    bool affichersuppl = BcSuppl->isChecked();
    for(int i=0;i<PFenim->CallsNumber;i++) ShowOneOtherPoint(i,affichersuppl);
}

void Loupe::ShowOneCall(int ncri,bool crisel,bool affichercri)
{
    // if(pfm->m_mode==2) return;
    if(!(ncri<2000)) return;
    bool etiquette = !PFenim->EtiquetteArray[ncri]->DataFields[ESPECE].isEmpty();
    QPen qp[NCRETES];
    qp[0] = QPen(QColor((255-80*etiquette)*(!crisel)+20*crisel*etiquette,
                        (255-100*etiquette)*(!crisel),
                        255*(!crisel)+80*crisel*etiquette),
                       0);
    if(!affichercri)
    {
        if(_ilc[ncri])
        {
            for(int jcrete=0;jcrete<1;jcrete++)
            {
                delete _gplt[jcrete][ncri];
                _ilc[ncri]=false;
            }
        }
    }
    else
    {
        if(!_ilc[ncri])
        {
            for(int jcrete=0;jcrete<1;jcrete++)
            {

                QPolygonF polygone;
                for(int j=0;j<PFenim->CallsMatrix[jcrete][ncri].size();j++)
                {
                    float x=0.5f+(float)(PFenim->CallsMatrix[jcrete][ncri][j].x()/(1+PFenim->XHalf));
                    float y=0.5f+(float)(PFenim->ImageHeight-PFenim->CallsMatrix[jcrete][ncri][j].y()-1);
                    polygone << QPointF(x,y);
                }
                QPainterPath path = QPainterPath();
                path.addPolygon(polygone);
                _gplt[jcrete][ncri] = new QGraphicsPathItem(path);
                _gplt[jcrete][ncri]->setPen(qp[jcrete]);
                LoupeScene->addItem(_gplt[jcrete][ncri]);
            }
            _ilc[ncri]=true;
            _coucri[ncri]=(int)etiquette*2+(int)crisel;
        }
        else
        {
            int counou=(int)etiquette*2+(int)crisel;
            if(counou!=_coucri[ncri])
            {
                _gplt[0][ncri]->setPen(qp[0]);
                _coucri[ncri]=counou;
            }
        }
    }
}

void Loupe::ShowOneOtherCrestl(int ncri,bool affichersuppl)
{
    QColor qc2,qc1;
    QPen qp2;
    QBrush qb2;
    if(!(ncri<2000)) return;
    QPen qp[NCRETES];
    qp[1] = QPen(QColor(192,192,64));
    qp[2] = QPen(QColor(220,220,80));
    qp[3] = QPen(QColor(60,90,255));
    qp[4] = QPen(QColor(150,90,30));
    //
    if(!affichersuppl)
    {
        if(_ilcs[ncri])
        {
            for(int jcrete=1;jcrete<NCRETES;jcrete++)
            {
                delete _gplt[jcrete][ncri];
            }
            _ilcs[ncri]=false;
        }
    }
    else
    {
        if(!_ilcs[ncri])
        {
            for(int jcrete=1;jcrete<NCRETES;jcrete++)
            {

                QPolygonF polygone;
                for(int j=0;j<PFenim->CallsMatrix[jcrete][ncri].size();j++)
                {
                    float x=(float)(PFenim->CallsMatrix[jcrete][ncri][j].x()/(1+PFenim->XHalf));
                    float y=(float)(PFenim->ImageHeight-PFenim->CallsMatrix[jcrete][ncri][j].y()-1);
                    if(jcrete!=3) x+=0.5f; else x+=0.05f;
                    if(jcrete==1) y+=1.0f;
                    if(jcrete ==0 || jcrete>2) y+=0.5f;
                    polygone << QPointF(x,y);
                }
                QPainterPath path = QPainterPath();
                path.addPolygon(polygone);
                _gplt[jcrete][ncri] = new QGraphicsPathItem(path);
                _gplt[jcrete][ncri]->setPen(qp[jcrete]);
                LoupeScene->addItem(_gplt[jcrete][ncri]);
                //
            } // next jcrete
            _ilcs[ncri]=true;
        } // fin du ilcs[ncri]==false
    } // fin du else affichersuppl
}

void Loupe::ShowOneOtherPoint(int ncri,bool affichersuppl)
{
    QColor qc2,qc1;
    QPen qp2;
    QBrush qb2;
    if(!(ncri<2000)) return;
    QPen qp[NCRETES];
    qp[1] = QPen(QColor(192,192,64));
    qp[2] = QPen(QColor(220,220,80));
    qp[3] = QPen(QColor(60,90,255));
    qp[4] = QPen(QColor(150,90,30));
    //
    //
    if(_ilps[ncri])
    {
        for(int jcrete=0;jcrete<NCRETES;jcrete++)
        {
            for(int k=0;k<NSUPPL;k++) delete _gepsu[jcrete][k][ncri];
            _ilps[ncri]=false;
        }
    }
    if(affichersuppl)
    {
        if(!_ilps[ncri])
        {

            for(int jcrete=0;jcrete<NCRETES;jcrete++)
            {
                for(int kos=NSUPPL-1;kos>=0;kos--)
                {
                    int k=kos+(kos==0)-(kos==1);
                    float x2=(float)(PFenim->AddPointsVector[jcrete][k][ncri].x()/(float)(1+PFenim->XHalf));
                    float y2=(float)PFenim->ImageHeight - (float)PFenim->AddPointsVector[jcrete][k][ncri].y()-1.0f;
                    float w2=(6.0f+_liaj+(float)kos)/_lWl;
                    float h2=(6.0f+_liaj+(float)kos)/_lWh;

                    if(kos==0) {w2 /= 1.6f; h2 *= 1.6f;}
                    if(kos==2) {w2 *= 1.6f; h2 /= 1.6f;}
                    if(jcrete!=3) x2+=0.5f;  else x2+=0.5f;
                    if(jcrete==1) y2+=1.0f;
                    if(jcrete==0 || jcrete>2) y2+=0.5f;
                    x2+=(float)((float)kos-1.0f)/15.0f;
                    if(jcrete<3) qc2 = QColor(64+20*(jcrete+kos*3),64+20*(jcrete+kos*3),32*kos);
                    else qc2 = QColor(32*(kos+2),32*(kos+2),64+20*(1+kos*3));
                    if(jcrete<3) qc1 = QColor(32+10*(jcrete+kos*3),32+10*(jcrete+kos*3),16*kos);
                    else qc1 = QColor(16*(kos+2),16*(kos+2),32+10*(1+kos*3));
                    qp2 = QPen(qc1,0);
                    qb2 = QBrush(qc2,Qt::SolidPattern);
                    _gepsu[jcrete][k][ncri]=LoupeScene->addEllipse(x2-w2/2,y2-h2/2,w2,h2,qp2,qb2);;
                } // next kos
            } // next jcrete
            _ilps[ncri]=true;
        } // fin du ilcs[ncri]==false
    } // fin du else affichersuppl
}


void Loupe::ShowOneMasterPoint(int ncri,bool crisel,bool afficherpm)
{
    if(!(ncri<2000)) return;
    float x = 0.5f+(PFenim->MasterPointsVector[ncri].x()/(1+PFenim->XHalf));
    float y = 0.5f+(PFenim->ImageHeight - PFenim->MasterPointsVector[ncri].y()-1.0f);
    bool etiquette = !PFenim->EtiquetteArray[ncri]->DataFields[ESPECE].isEmpty();
    int nspec= PFenim->EtiquetteArray[ncri]->SpecNumber+1;
    int rouge,vert,bleu;
    bleu=0;
    if(crisel)
    {
        rouge=0;vert=0;
        if(etiquette && nspec>0 && nspec<8) {vert=8*nspec;rouge=4*nspec;}
    }
    else
    {
        rouge=255;vert=0;
        if(etiquette && nspec>0 && nspec<8) {vert=255-16*nspec;rouge=16*nspec;}
    }
    QPen qpm = QPen(QColor(rouge,vert,bleu),0);
    QBrush qb = QBrush(QColor(rouge,vert,bleu),Qt::SolidPattern);
    //
    if(ncri<2000)
    {
        if(_ipmc[ncri]==true)
        {
            delete _gepm[ncri];
            _ipmc[ncri]=false;
        }
        if(afficherpm)
        {
            if(_ipmc[ncri]==false)
            {
                float w=(10.0f+_liaj)/_lWl;
                float h=(10.0f+_liaj)/_lWh;
                _gepm[ncri]=LoupeScene->addEllipse(x-w/2,y-h/2,w,h,qpm,qb);
                _ipmc[ncri]=true;
            }
            else _gepm[ncri]->setPen(qpm);
        }
    }
}

void Loupe::ShowBubble(QString sbulle)
{
        QToolTip::showText(QCursor::pos(),sbulle);
}

void Loupe::Zoom()
{
    ZoomeF(1.414f,1.414f);
}
void Loupe::UnZoom()
{
    ZoomeF(0.707f,0.707f);
}
void Loupe::ZoomeF(float izh,float izv)
{
    LoupeView->scale(izh,izv);
    LoupeView->centerOn(LastCenterX,LastCenterY);
    _lWl *= izh;
    _lWh *= izv;
    float rapdeb=_lWl/PFenim->PFenimWindow->WidthRatio;
    _liaj=log(rapdeb);

    ShowGrid(_bcGrid->isChecked());
    // afficher_cris();
    ShowMasterPoints();
    ShowOtherPoints();
    showRatio();
}
void Loupe::Zoom(int x,int y)
{
    ZoomeF(1.414f,1.414f);
    LoupeView->centerOn(x,y);
    LastCenterX=x;LastCenterY=y;
}
void Loupe::ZoomH()
{
    ZoomeF(1.414f,1);
}
void Loupe::UnZoomH()
{
    ZoomeF(0.707f,1);
}

float Loupe::getRatio()
{
    return((_lWh * PFenim->FactorX * (1+PFenim->XHalf)) / (_lWl * PFenim->FactorY));
}

void Loupe::showRatio()
{
    QString ratio;
    ratio.setNum(getRatio(),'f',2);
    LabelR->setText(QString("r=")+ratio);
}


void Loupe::ActivateGrid(int state)
{
    ShowGrid(state==Qt::Checked);
}

void Loupe::ActivateMasterPoints()
{
    ShowMasterPoints();
}

void Loupe::ActivateCalls()
{
    ShowCalls();
}

void Loupe::ActivateOtherCrests()
{
    ShowOtherCrests();
    ShowOtherPoints();
}
