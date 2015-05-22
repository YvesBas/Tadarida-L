#include "loupe.h"
#include "math.h"

Loupe::Loupe(Fenim *pf,QMainWindow *parent,int x,int y) :
    QMainWindow(parent)
{
    //pfm=(Fenim *)parent;
    pfm=pf;
    l_zx=x;
    l_zy=y;
    vaf=false;
    gbox_boutons = new QGroupBox(this);
    bZoome = new MyQPushButton(gbox_boutons);
    bDezoome = new MyQPushButton(gbox_boutons);
    bZoomeH = new MyQPushButton(gbox_boutons);
    bDezoomeH = new MyQPushButton(gbox_boutons);
    bcGrille = new QCheckBox(QString("Grille"),gbox_boutons);
    bcPMaitres = new QCheckBox(QString("Pts maitres"),gbox_boutons);
    bcCris = new QCheckBox(QString("Cris"),gbox_boutons);
    bcSuppl = new QCheckBox(QString("Suppl"),gbox_boutons);

    labelx = new QLabel(gbox_boutons);
    labely = new QLabel(gbox_boutons);
    labelr = new QLabel(gbox_boutons);
    label_ener = new QLabel(gbox_boutons);
    l_bo=55;
    h_bo=20;
    setWindowTitle("Tadarida - Loupe");
    dercx = -1;
    nliv=0;nlih=0;
    for(int i=0;i<pfm->m_nbcris && i<MAXCRI;i++)
    {
        ipmc[i]=false;
        ilc[i]=false;
        coucri[i]=0;
        ilcs[i]=0;
        ilps[i]=0;
    }
    for(int jcrete=0;jcrete<NCRETES;jcrete++)
        gplt[jcrete] = new QGraphicsPathItem *[MAXCRI];

}

Loupe::~Loupe()
{
    for(int jcrete=0;jcrete<NCRETES;jcrete++)
        delete gplt[jcrete];
    pfm->affloupe = false;
    delete lscene;
    delete lview;
}


void Loupe::afficherLoupe()
{
    //m_mx = 30 ; m_my=30; // marges entre grandes parties de l'écran
    //m_logStream << "$$$ Fenim début afficher_image: " << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;
    QRect rfm=pfm->getFenetre();
    QRect rvm=pfm->getView()->geometry();
    resize(rvm.width()/2,rvm.height());
    move(rfm.left()+rvm.left()+rvm.width()/2,rfm.top()+rvm.top());
    fenima = pfm->getImage();
    lscene = new MyQGraphicsScene(pfm,this,true);
    lview = new MyQGraphicsView(this);
    lview->setScene(lscene);
    //lview->showMaximized();
    pix=(lscene->addPixmap(QPixmap::fromImage(*fenima))); // ajout du pixmap dans la scene
    //alpix = true;
    activateWindow();
    raise();
    show();
    l_lf=width();
    l_hf=height();
    lview->move(0,0);
    lview->setFixedSize(l_lf,l_hf-h_bo);
    // l_rl=pfm->m_rl*6;
    // l_rh=pfm->m_rh*1.5f;
    // edit yves - changer ratio
    l_rl=pfm->m_rl*10;
    l_rh=pfm->m_rh*1.5f;
    l_iaj=0.0f;

    lview->scale(l_rl,l_rh);
    lview->centerOn(l_zx,l_zy);
    dercx = l_zx;
    dercy = l_zy;
    vaf=true;
    lview->setMouseTracking(true);

    afficher_grille(true);

    afficher_boutons();

    bcGrille->setChecked(true);
    bcPMaitres->setChecked(true);
    bcCris->setChecked(true);
    bcSuppl->setChecked(false);

    afficher_cris();
    afficher_points_maitres();

    afficheratio();

    connect(bZoome,SIGNAL(clicked()),this,SLOT(zoome()));
    connect(bDezoome,SIGNAL(clicked()),this,SLOT(dezoome()));
    connect(bZoomeH,SIGNAL(clicked()),this,SLOT(zoomeh()));
    connect(bDezoomeH,SIGNAL(clicked()),this,SLOT(dezoomeh()));
    connect(bcGrille,SIGNAL(stateChanged(int)),this,SLOT(actdesactGrille(int)));
    connect(bcPMaitres,SIGNAL(stateChanged(int)),this,SLOT(actdesactPMaitres()));
    connect(bcCris,SIGNAL(stateChanged(int)),this,SLOT(actdesactCris()));
    connect(bcSuppl,SIGNAL(stateChanged(int)),this,SLOT(actdesactSuppl()));

    //
}

void Loupe::afficher_boutons()
{
    gbox_boutons->resize(l_lf,h_bo);
    gbox_boutons->move(1,l_hf-h_bo);

    int margx = 8;
    int lzb = gbox_boutons->width();
    l_bo = (lzb-margx*14)/13;

    bZoome->move(margx,1);
    bZoome->resize(l_bo,h_bo);
    bZoome->setText("Z +");

    bDezoome->move(margx*2+l_bo,1);
    bDezoome->resize(l_bo,h_bo);
    bDezoome->setText("Z -");

    bZoomeH->move(margx*3+l_bo*2,1);
    bZoomeH->resize(l_bo,h_bo);
    bZoomeH->setText("ZH +");

    bDezoomeH->move(margx*4+l_bo*3,1);
    bDezoomeH->resize(l_bo,h_bo);
    bDezoomeH->setText("ZH -");


    bcGrille->move(margx*5+l_bo*4,1);
    bcGrille->resize(l_bo,h_bo);

    bcPMaitres->move(margx*6+l_bo*5,1);
    bcPMaitres->resize(l_bo,h_bo);

    bcCris->move(margx*7+l_bo*6,1);
    bcCris->resize(l_bo,h_bo);

    bcSuppl->move(margx*8+l_bo*7,1);
    bcSuppl->resize(l_bo,h_bo);

    label_ener->move(lzb - l_bo*4 - margx*4,1);

    labelr->move(lzb - l_bo*3 - margx*3,1);
    labelx->move(lzb - l_bo*2 - margx*2,1);
    labely->move(lzb - l_bo - margx,1);

    label_ener->resize(l_bo,h_bo);
    labelr->resize(l_bo,h_bo);
    labelx->resize(l_bo,h_bo);
    labely->resize(l_bo,h_bo);


}

void Loupe::afficher_grille(bool afficher)
{
    if(nliv>0) for(int i=0;i<nliv;i++) delete gliv[i];
    if(nlih>0) for(int i=0;i<nlih;i++) delete glih[i];
    nliv=0; nlih=0;
    if(!afficher) return;

    int xmax = fenima->width();
    int ymax = fenima->height();
    float tmax = pfm->getms(xmax-1);
    float igt=0;
    float incrt = 500;
    if(l_rl>0.25) incrt = 100;
    QPen qp = QPen(QColor(128,128,128),0);
    while(igt<tmax && nliv < 500)
    {
        float x=pfm->getx(igt);
        gliv[nliv++] = lscene->addLine(x,0,x,ymax-1,qp);
        igt += incrt;
    }
    //
    float fmax = pfm->getkhz(0);
    float igf=0;
    float incrf = 10;
    while(igf<fmax && nlih < 500)
    {
        float y=pfm->gety(igf);
        glih[nlih++] = lscene->addLine(0,y,xmax-1,y,qp);
        igf += incrf;
    }
}


void Loupe::resizeEvent(QResizeEvent *re)
{
    QPointF pv = lview->mapToScene(1,1);
    lscene->activeWindow();
    if(!vaf) return;
    l_lf=width();
    l_hf=height();

    lview->setFixedSize(l_lf,l_hf - h_bo);
    afficher_boutons();
    lview->centerOn(dercx,dercy);
}

/*
void Loupe::rafraichit_image()
{
    pix->setPixmap(QPixmap::fromImage(*fenima));
    // reafficherCris();
}
*/

void Loupe::afficher_cris()
{
    bool affichercri = bcCris->isChecked();
    for(int i=0;i<pfm->m_nbcris;i++) afficher_un_cri(i,pfm->m_crisel[i],affichercri);
}

void Loupe::afficher_points_maitres()
{
    bool afficherpm = bcPMaitres->isChecked();
    for(int i=0;i<pfm->m_nbcris;i++) afficher_un_point_maitre(i,pfm->m_crisel[i],afficherpm);
}

void Loupe::afficher_cretes_suppl()
{
    bool affichersuppl = bcSuppl->isChecked();
    for(int i=0;i<pfm->m_nbcris;i++) afficher_une_crete_suppl(i,affichersuppl);
}

void Loupe::afficher_points_suppl()
{
    bool affichersuppl = bcSuppl->isChecked();
    for(int i=0;i<pfm->m_nbcris;i++) afficher_un_point_suppl(i,affichersuppl);
}
/*
void Loupe::afficher_suppl()
{
    bool affichersuppl = bcSuppl->isChecked();
    for(int i=0;i<pfm->m_nbcris;i++) afficher_un_suppl(i,affichersuppl);
}
*/

void Loupe::afficher_un_cri(int ncri,bool crisel,bool affichercri)
{
    // if(pfm->m_mode==2) return;
    if(!(ncri<2000)) return;
    bool etiquette = !pfm->lesEtiq[ncri]->DataFields[ESPECE].isEmpty();
    QPen qp[NCRETES];
    qp[0] = QPen(QColor((255-80*etiquette)*(!crisel)+20*crisel*etiquette,
                        (255-100*etiquette)*(!crisel),
                        255*(!crisel)+80*crisel*etiquette),
                       0);
    /*
    qp[1] = QPen(QColor(192,192,64));
    qp[2] = QPen(QColor(220,220,80));
    qp[3] = QPen(QColor(60,90,255));
    */
    if(!affichercri)
    {
        if(ilc[ncri])
        {
            for(int jcrete=0;jcrete<1;jcrete++)
            {
                delete gplt[jcrete][ncri];
                ilc[ncri]=false;
            }
        }
    }
    else
    {
        if(!ilc[ncri])
        {
            for(int jcrete=0;jcrete<1;jcrete++)
            {

                QPolygonF polygone;
                for(int j=0;j<pfm->m_matrixCalls[jcrete][ncri].size();j++)
                {
                    float x=0.5f+(float)(pfm->m_matrixCalls[jcrete][ncri][j].x()/(1+pfm->m_xmoitie));
                    float y=0.5f+(float)(pfm->_imaHeight-pfm->m_matrixCalls[jcrete][ncri][j].y()-1);
                    /*
                    if(jcrete<3) x+=0.5f; else x+=0.05f;
                    if(jcrete==1) y+=1.0f;
                    if(jcrete ==0 || jcrete>2) y+=0.5f;
                    */
                    polygone << QPointF(x,y);
                }
                QPainterPath path = QPainterPath();
                path.addPolygon(polygone);
                gplt[jcrete][ncri] = new QGraphicsPathItem(path);
                gplt[jcrete][ncri]->setPen(qp[jcrete]);
                lscene->addItem(gplt[jcrete][ncri]);
            }
            ilc[ncri]=true;
            coucri[ncri]=(int)etiquette*2+(int)crisel;
        }
        else
        {
            int counou=(int)etiquette*2+(int)crisel;
            if(counou!=coucri[ncri])
            {
                gplt[0][ncri]->setPen(qp[0]);
                coucri[ncri]=counou;
            }
        }
    }
}

void Loupe::afficher_une_crete_suppl(int ncri,bool affichersuppl)
{
    QColor qc2,qc1;
    QPen qp2;
    QBrush qb2;
    if(!(ncri<2000)) return;
    pfm->m_logStream << "acs - début - ncri=" << ncri << endl;
    QPen qp[NCRETES];
    qp[1] = QPen(QColor(192,192,64));
    qp[2] = QPen(QColor(220,220,80));
    qp[3] = QPen(QColor(60,90,255));
    qp[4] = QPen(QColor(150,90,30));
    //
    if(!affichersuppl)
    {
        pfm->m_logStream << "acs - 2 " << endl;
        if(ilcs[ncri])
        {
            pfm->m_logStream << "acs - 3 " << endl;
            for(int jcrete=1;jcrete<NCRETES;jcrete++)
            {
                delete gplt[jcrete][ncri];
            }
            pfm->m_logStream << "acs - 4 " << endl;
            ilcs[ncri]=false;
        }
    }
    else
    {
        pfm->m_logStream << "acs - 5 " << endl;
        if(!ilcs[ncri])
        {
            pfm->m_logStream << "acs - 6 " << endl;
            for(int jcrete=1;jcrete<NCRETES;jcrete++)
            {
                pfm->m_logStream << "acs - 7 - jcrete=" << jcrete << endl;

                QPolygonF polygone;
                for(int j=0;j<pfm->m_matrixCalls[jcrete][ncri].size();j++)
                {
                    pfm->m_logStream << "acs - 71 - jcrete=" << jcrete
                                     << "  j=" << j << endl;
                    float x=(float)(pfm->m_matrixCalls[jcrete][ncri][j].x()/(1+pfm->m_xmoitie));
                    float y=(float)(pfm->_imaHeight-pfm->m_matrixCalls[jcrete][ncri][j].y()-1);
                    pfm->m_logStream << "acs - 72 - jcrete=" << jcrete
                                     << "  x,y=" << x << "," << y << endl;
                    if(jcrete!=3) x+=0.5f; else x+=0.05f;
                    if(jcrete==1) y+=1.0f;
                    if(jcrete ==0 || jcrete>2) y+=0.5f;
                    polygone << QPointF(x,y);
                }
                pfm->m_logStream << "acs - 73 - jcrete=" << jcrete << endl;
                QPainterPath path = QPainterPath();
                path.addPolygon(polygone);
                pfm->m_logStream << "acs - 74 - jcrete=" << jcrete << endl;
                gplt[jcrete][ncri] = new QGraphicsPathItem(path);
                gplt[jcrete][ncri]->setPen(qp[jcrete]);
                lscene->addItem(gplt[jcrete][ncri]);
                pfm->m_logStream << "acs - 8 - jcrete=" << jcrete << endl;
                //
            } // next jcrete
            ilcs[ncri]=true;
            pfm->m_logStream << "acs - 9 " << endl;
        } // fin du ilcs[ncri]==false
    } // fin du else affichersuppl
    pfm->m_logStream << "acs - fin - ncri=" << ncri << endl;
}

void Loupe::afficher_un_point_suppl(int ncri,bool affichersuppl)
{
    QColor qc2,qc1;
    QPen qp2;
    QBrush qb2;
    if(!(ncri<2000)) return;
    pfm->m_logStream << "aps - début - ncri=" << ncri << endl;
    QPen qp[NCRETES];
    qp[1] = QPen(QColor(192,192,64));
    qp[2] = QPen(QColor(220,220,80));
    qp[3] = QPen(QColor(60,90,255));
    qp[4] = QPen(QColor(150,90,30));
    //
    pfm->m_logStream << "aps 2" << endl;
    //
    if(ilps[ncri])
    {
        for(int jcrete=0;jcrete<NCRETES;jcrete++)
        {
            for(int k=0;k<NSUPPL;k++) delete gepsu[jcrete][k][ncri];
            ilps[ncri]=false;
        }
    }
pfm->m_logStream << "aps 3" << endl;
    if(affichersuppl)
    {
        if(!ilps[ncri])
        {
pfm->m_logStream << "aps 4" << endl;

            for(int jcrete=0;jcrete<NCRETES;jcrete++)
            {
                pfm->m_logStream << "aps 5 jcrete=" << jcrete << endl;
                for(int kos=NSUPPL-1;kos>=0;kos--)
                {
                    pfm->m_logStream << "aps 6 kos=" << kos << endl;
                    int k=kos+(kos==0)-(kos==1);
                    pfm->m_logStream << "apm7 - jcrete,k=" << jcrete << "," << k << endl;
                    float x2=(float)(pfm->m_pointsSuppl[jcrete][k][ncri].x()/(float)(1+pfm->m_xmoitie));
                    float y2=(float)pfm->_imaHeight - (float)pfm->m_pointsSuppl[jcrete][k][ncri].y()-1.0f;
                    float w2=(6.0f+l_iaj+(float)kos)/l_rl;
                    float h2=(6.0f+l_iaj+(float)kos)/l_rh;

                    if(kos==0) {w2 /= 1.6f; h2 *= 1.6f;}
                    if(kos==2) {w2 *= 1.6f; h2 /= 1.6f;}
                    if(jcrete!=3) x2+=0.5f;  else x2+=0.5f;
                    if(jcrete==1) y2+=1.0f;
                    if(jcrete==0 || jcrete>2) y2+=0.5f;
                    x2+=(float)((float)kos-1.0f)/15.0f;
                    pfm->m_logStream << "apm52 - x2,y2 = " << x2 << " , " << y2 << endl;
                    pfm->m_logStream << "apm52,5 - w2,h2 = " << w2 << " , " << h2 << endl;
                    if(jcrete<3) qc2 = QColor(64+20*(jcrete+kos*3),64+20*(jcrete+kos*3),32*kos);
                    else qc2 = QColor(32*(kos+2),32*(kos+2),64+20*(1+kos*3));
                    if(jcrete<3) qc1 = QColor(32+10*(jcrete+kos*3),32+10*(jcrete+kos*3),16*kos);
                    else qc1 = QColor(16*(kos+2),16*(kos+2),32+10*(1+kos*3));
                    qp2 = QPen(qc1,0);
                    qb2 = QBrush(qc2,Qt::SolidPattern);
                    gepsu[jcrete][k][ncri]=lscene->addEllipse(x2-w2/2,y2-h2/2,w2,h2,qp2,qb2);;
                } // next kos
            } // next jcrete
            ilps[ncri]=true;
        } // fin du ilcs[ncri]==false
    } // fin du else affichersuppl
    pfm->m_logStream << "aps - fin - ncri=" << ncri << endl;
}


void Loupe::afficher_un_point_maitre(int ncri,bool crisel,bool afficherpm)
{
    pfm->m_logStream << "apm1 - ncri= " << ncri << endl;
    if(!(ncri<2000)) return;
    float x = 0.5f+(pfm->m_pointsMaitres[ncri].x()/(1+pfm->m_xmoitie));
    float y = 0.5f+(pfm->_imaHeight - pfm->m_pointsMaitres[ncri].y()-1.0f);
    //QPen qpm = QPen(QColor(96*(1-crisel),96*(1-crisel),96*(1-crisel)),0);
    //QBrush qb = QBrush(QColor(96*(1-crisel),96*(1-crisel),96*(1-crisel)),Qt::SolidPattern);
    //QPen qpm = QPen(QColor(255*(1-crisel),0,0,0));
    //QBrush qb = QBrush(QColor(255*(1-crisel),0,0),Qt::SolidPattern);
    bool etiquette = !pfm->lesEtiq[ncri]->DataFields[ESPECE].isEmpty();
    int nspec= pfm->lesEtiq[ncri]->SpecNumber+1;
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

    // QPen qpm = QPen(QColor(255*(1-crisel),128*etiquette*(1-crisel),0,0));
    QPen qpm = QPen(QColor(rouge,vert,bleu),0);
    QBrush qb = QBrush(QColor(rouge,vert,bleu),Qt::SolidPattern);
    //
    if(ncri<2000)
    {
        pfm->m_logStream << "apm2" << endl;
        if(ipmc[ncri]==true)
        {
            pfm->m_logStream << "apm21" << endl;
            delete gepm[ncri];
            pfm->m_logStream << "apm22" << endl;
            ipmc[ncri]=false;
        }
        if(afficherpm)
        {
            pfm->m_logStream << "apm3" << endl;
            if(ipmc[ncri]==false)
            {
                float w=(10.0f+l_iaj)/l_rl;
                float h=(10.0f+l_iaj)/l_rh;
                gepm[ncri]=lscene->addEllipse(x-w/2,y-h/2,w,h,qpm,qb);
                ipmc[ncri]=true;
            }
            else gepm[ncri]->setPen(qpm);
        }
    }
}

void Loupe::affbulle(QString sbulle)
{
        QToolTip::showText(QCursor::pos(),sbulle);
}

void Loupe::zoome()
{
    zoomef(1.414f,1.414f);
}
void Loupe::dezoome()
{
    zoomef(0.707f,0.707f);
}
void Loupe::zoomef(float izh,float izv)
{
    //QMessageBox::warning(this,"zoom","Passe dans zoome !", QMessageBox::Ok);
    lview->scale(izh,izv);
    lview->centerOn(dercx,dercy);
    l_rl *= izh;
    l_rh *= izv;
    float rapdeb=l_rl/pfm->m_rl;
    l_iaj=log(rapdeb);

    afficher_grille(bcGrille->isChecked());
    // afficher_cris();
    afficher_points_maitres();
    afficher_points_suppl();
    afficheratio();
}
void Loupe::zoome(int x,int y)
{
    zoomef(1.414f,1.414f);
    lview->centerOn(x,y);
    dercx=x;dercy=y;
}
void Loupe::zoomeh()
{
    zoomef(1.414f,1);
}
void Loupe::dezoomeh()
{
    zoomef(0.707f,1);
}

float Loupe::getratio()
{
    return((l_rh * pfm->m_factorX * (1+pfm->m_xmoitie)) / (l_rl * pfm->m_factorY));
}

void Loupe::afficheratio()
{
    QString ratio;
    ratio.setNum(getratio(),'f',2);
    labelr->setText(QString("r=")+ratio);
}


void Loupe::actdesactGrille(int state)
{
    afficher_grille(state==Qt::Checked);
}

void Loupe::actdesactPMaitres()
{
    afficher_points_maitres();
}

void Loupe::actdesactCris()
{
    //pfm->bcCris->setCheckState((Qt::CheckState)state);
    // rafraichit_image();
    afficher_cris();
}

void Loupe::actdesactSuppl()
{
    //pfm->bcCris->setCheckState((Qt::CheckState)state);
    // rafraichit_image();
    afficher_cretes_suppl();
    afficher_points_suppl();
}

