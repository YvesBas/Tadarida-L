#include "loupe.h"
#include "math.h"

// Loupe class : this class is a graphic class used by Fenim class to create
// a second window zooming the picture
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
    BcCalls = new QCheckBox(QString("Lines"),_gboxButtons);
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
        _callColour[i]=0;
        _ilcs[i]=0;
        _ilps[i]=0;
    }
    for(int jcrete=0;jcrete<CRESTSNUMBER;jcrete++)
        _gplt[jcrete] = new QGraphicsPathItem *[MAXCRI];
}

Loupe::~Loupe()
{
    for(int jcrete=0;jcrete<CRESTSNUMBER;jcrete++)
        delete _gplt[jcrete];
    PFenim->ShowLoupe = false;
    delete LoupeScene;
    delete LoupeView;
}

// this is the entry point of the Loupe class
// it is called by the doubleclic event handler of the QGraphicsScene object of FenimWindow class
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
    _pix=(LoupeScene->addPixmap(QPixmap::fromImage(*Fenima)));
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
}

// this method locates and sizes buttons, labels, checkboxes of the Loupe window
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

// this method creates the grid of graduations
void Loupe::ShowGrid(bool toShow)
{
    if(_nliv>0) for(int i=0;i<_nliv;i++) delete _gliv[i];
    if(_nlih>0) for(int i=0;i<_nlih;i++) delete _glih[i];
    _nliv=0; _nlih=0;
    if(!toShow) return;
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

// these methods manage the visualization of all the sound events
void Loupe::ShowCalls()
{
    bool affichercri = BcCalls->isChecked();
    for(int i=0;i<PFenim->CallsNumber;i++) ShowOneCall(i,PFenim->SelectedCalls[i],affichercri);
}

void Loupe::ShowOneCall(int callNumber,bool selectedCall,bool showCall)
{
    if(!(callNumber<2000)) return;
    bool etiquette = !PFenim->EtiquetteArray[callNumber]->DataFields[ESPECE].isEmpty();
    QPen qp[CRESTSNUMBER];
    qp[0] = QPen(QColor((255-80*etiquette)*(!selectedCall)+20*selectedCall*etiquette,
                        (255-100*etiquette)*(!selectedCall),
                        255*(!selectedCall)+80*selectedCall*etiquette),
                       0);
    if(!showCall)
    {
        if(_ilc[callNumber])
        {
            for(int jcrete=0;jcrete<1;jcrete++)
            {
                delete _gplt[jcrete][callNumber];
                _ilc[callNumber]=false;
            }
        }
    }
    else
    {
        if(!_ilc[callNumber])
        {
            for(int jcrete=0;jcrete<1;jcrete++)
            {

                QPolygonF polygone;
                for(int j=0;j<PFenim->CallsMatrix[jcrete][callNumber].size();j++)
                {
                    float x=0.5f+(float)(PFenim->CallsMatrix[jcrete][callNumber][j].x()/(1+PFenim->XHalf));
                    float y=0.5f+(float)(PFenim->ImageHeight-PFenim->CallsMatrix[jcrete][callNumber][j].y()-1);
                    polygone << QPointF(x,y);
                }
                QPainterPath path = QPainterPath();
                path.addPolygon(polygone);
                _gplt[jcrete][callNumber] = new QGraphicsPathItem(path);
                _gplt[jcrete][callNumber]->setPen(qp[jcrete]);
                LoupeScene->addItem(_gplt[jcrete][callNumber]);
            }
            _ilc[callNumber]=true;
            _callColour[callNumber]=(int)etiquette*2+(int)selectedCall;
        }
        else
        {
            int newColour=(int)etiquette*2+(int)selectedCall;
            if(newColour!=_callColour[callNumber])
            {
                _gplt[0][callNumber]->setPen(qp[0]);
                _callColour[callNumber]=newColour;
            }
        }
    }
}

// these methods manage the visualization of all the master points
void Loupe::ShowMasterPoints()
{
    bool afficherpm = BcMasterPoints->isChecked();
    for(int i=0;i<PFenim->CallsNumber;i++) ShowOneMasterPoint(i,PFenim->SelectedCalls[i],afficherpm);
}

void Loupe::ShowOneMasterPoint(int callNumber,bool selectedCall,bool showMasterPoints)
{
    if(!(callNumber<2000)) return;
    float x = 0.5f+(PFenim->MasterPointsVector[callNumber].x()/(1+PFenim->XHalf));
    float y = 0.5f+(PFenim->ImageHeight - PFenim->MasterPointsVector[callNumber].y()-1.0f);
    bool etiquette = !PFenim->EtiquetteArray[callNumber]->DataFields[ESPECE].isEmpty();
    int nspec= PFenim->EtiquetteArray[callNumber]->SpecNumber+1;
    int rouge,vert,bleu;
    bleu=0;
    if(selectedCall)
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
    if(callNumber<2000)
    {
        if(_ipmc[callNumber]==true)
        {
            delete _gepm[callNumber];
            _ipmc[callNumber]=false;
        }
        if(showMasterPoints)
        {
            if(_ipmc[callNumber]==false)
            {
                float w=(10.0f+_liaj)/_lWl;
                float h=(10.0f+_liaj)/_lWh;
                _gepm[callNumber]=LoupeScene->addEllipse(x-w/2,y-h/2,w,h,qpm,qb);
                _ipmc[callNumber]=true;
            }
            else _gepm[callNumber]->setPen(qpm);
        }
    }
}

// these methods manage the visualization of the other crests
// (in addition to the main ridge: see description of settings in the manual)
void Loupe::ShowOtherCrests()
{
    bool affichersuppl = BcSuppl->isChecked();
    for(int i=0;i<PFenim->CallsNumber;i++) ShowOneOtherCrestl(i,affichersuppl);
}

void Loupe::ShowOneOtherCrestl(int callNumber,bool showSuppl)
{
    QColor qc2,qc1;
    QPen qp2;
    QBrush qb2;
    if(!(callNumber<2000)) return;
    QPen qp[CRESTSNUMBER];
    qp[1] = QPen(QColor(192,192,64));
    qp[2] = QPen(QColor(220,220,80));
    qp[3] = QPen(QColor(60,90,255));
    qp[4] = QPen(QColor(150,90,30));
    //
    if(!showSuppl)
    {
        if(_ilcs[callNumber])
        {
            for(int jcrete=1;jcrete<CRESTSNUMBER;jcrete++)
            {
                delete _gplt[jcrete][callNumber];
            }
            _ilcs[callNumber]=false;
        }
    }
    else
    {
        if(!_ilcs[callNumber])
        {
            for(int jcrete=1;jcrete<CRESTSNUMBER;jcrete++)
            {

                QPolygonF polygone;
                for(int j=0;j<PFenim->CallsMatrix[jcrete][callNumber].size();j++)
                {
                    float x=(float)(PFenim->CallsMatrix[jcrete][callNumber][j].x()/(1+PFenim->XHalf));
                    float y=(float)(PFenim->ImageHeight-PFenim->CallsMatrix[jcrete][callNumber][j].y()-1);
                    if(jcrete!=3) x+=0.5f; else x+=0.05f;
                    if(jcrete==1) y+=1.0f;
                    if(jcrete ==0 || jcrete>2) y+=0.5f;
                    polygone << QPointF(x,y);
                }
                QPainterPath path = QPainterPath();
                path.addPolygon(polygone);
                _gplt[jcrete][callNumber] = new QGraphicsPathItem(path);
                _gplt[jcrete][callNumber]->setPen(qp[jcrete]);
                LoupeScene->addItem(_gplt[jcrete][callNumber]);
                //
            } // next jcrete
            _ilcs[callNumber]=true;
        } // fin du ilcs[ncri]==false
    } // fin du else affichersuppl
}


// these methods manages the visualization of the other points
// (in addition to the master point: see description of settings in the manual)
void Loupe::ShowOtherPoints()
{
    bool affichersuppl = BcSuppl->isChecked();
    for(int i=0;i<PFenim->CallsNumber;i++) ShowOneOtherPoint(i,affichersuppl);
}

void Loupe::ShowOneOtherPoint(int ncri,bool affichersuppl)
{
    QColor qc2,qc1;
    QPen qp2;
    QBrush qb2;
    if(!(ncri<2000)) return;
    QPen qp[CRESTSNUMBER];
    qp[1] = QPen(QColor(192,192,64));
    qp[2] = QPen(QColor(220,220,80));
    qp[3] = QPen(QColor(60,90,255));
    qp[4] = QPen(QColor(150,90,30));
    //
    //
    if(_ilps[ncri])
    {
        for(int jcrete=0;jcrete<CRESTSNUMBER;jcrete++)
        {
            for(int k=0;k<NSUPPL;k++) delete _gepsu[jcrete][k][ncri];
            _ilps[ncri]=false;
        }
    }
    if(affichersuppl)
    {
        if(!_ilps[ncri])
        {

            for(int jcrete=0;jcrete<CRESTSNUMBER;jcrete++)
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
                }
            }
            _ilps[ncri]=true;
        }
    }
}

// this method shows a bubble when mouse is moved over the representation of a sound event
void Loupe::ShowBubble(QString bubbleString)
{
        QToolTip::showText(QCursor::pos(),bubbleString);
}

// these methods zoom (or unzoom) the picture
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

// this method shows ratio time/frequency, useful to analyse the meaning of the shape
// of the representation of sound events
void Loupe::showRatio()
{
    QString ratio;
    ratio.setNum(getRatio(),'f',2);
    LabelR->setText(QString("r=")+ratio);
}

// these methods are connected to signals (event handlers)
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

