// 17-11-2014 : 2930 lignes avant grosses modifs
#include "etiquette.h"
#include "loupe.h"
#include "fenim.h"

const QString _baseIniFile = "/version.ini";

FenetreFenim::FenetreFenim(Fenim *pf,QMainWindow *parent) : QMainWindow(parent)
{
    pfenim = pf;
}

void FenetreFenim::etablit_connexions()
{
    connect(pfenim->bFlecheDroite,SIGNAL(clicked()),this,SLOT(crisuiv()));
    connect(pfenim->bFlecheGauche,SIGNAL(clicked()),this,SLOT(criprec()));
    connect(pfenim->bFlecheFin,SIGNAL(clicked()),this,SLOT(crifin()));
    connect(pfenim->bFlecheDebut,SIGNAL(clicked()),this,SLOT(crideb()));
    connect(pfenim->bsaveUneEtiq,SIGNAL(clicked()),this,SLOT(enreg_etiq()));
    connect(pfenim->bEnregEtiq,SIGNAL(clicked()),this,SLOT(EnregEtiquettes()));
    connect(pfenim->bClose,SIGNAL(clicked()),this,SLOT(fermeFenim()));
    connect(pfenim->pfc[INDICE]->_sli, SIGNAL(valueChanged(int)), this, SLOT(selectionne_indice(int)));
    connect(pfenim->pfc[INDICE]->_le,SIGNAL(textEdited(const QString&)),this,SLOT(modif_indice(const QString&)));
    connect(pfenim->pfc[CONFIDENTIEL]->_chb, SIGNAL(stateChanged(int)), this, SLOT(clique_confi(int)));
    connect(pfenim->bZoome,SIGNAL(clicked()),this,SLOT(zoome()));
    connect(pfenim->bDezoome,SIGNAL(clicked()),this,SLOT(dezoome()));
    connect(pfenim->bcGrille,SIGNAL(stateChanged(int)),this,SLOT(actdesactGrille(int)));
    connect(pfenim->bcPMaitres,SIGNAL(stateChanged(int)),this,SLOT(actdesactPMaitres()));
    connect(pfenim->bcCris,SIGNAL(stateChanged(int)),this,SLOT(actdesactCris()));
    connect(pfenim->editCri,SIGNAL(editingFinished()),this,SLOT(choisit_cri()));
    //
    connect(pfenim->chbTagSpec,SIGNAL(activated(const QString&)), this, SLOT(selectSpecTags(const QString&)));
    connect(pfenim->bFlecheDroiteSpec,SIGNAL(clicked()),this,SLOT(specTagNext()));
    connect(pfenim->bFlecheGaucheSpec,SIGNAL(clicked()),this,SLOT(specTagPrevious()));
    connect(pfenim->bFlecheFinSpec,SIGNAL(clicked()),this,SLOT(specTagLast()));
    connect(pfenim->bFlecheDebutSpec,SIGNAL(clicked()),this,SLOT(specTagFirst()));
}

// privisoire : ces méthodes doivent (au moins certaines) être complètement traitées dans classe FenetreFenim
void FenetreFenim::enreg_etiq() { pfenim->enreg_etiq(); }
void FenetreFenim::EnregEtiquettes() { pfenim->EnregEtiquettes(); }
void FenetreFenim::fermeFenim() { pfenim->fermeFenim(); }
void FenetreFenim::selectionne_indice(int a) { pfenim->selectionne_indice(a); }
void FenetreFenim::clique_confi(int s) { pfenim->clique_confi(s); }
void FenetreFenim::modif_indice(const QString&s) { pfenim->modif_indice(s); }
void FenetreFenim::zoomef(float f) { pfenim->zoomef(f); }
void FenetreFenim::zoome() { pfenim->zoome(); }
void FenetreFenim::dezoome() { pfenim->dezoome(); }
void FenetreFenim::actdesactGrille(int a) { pfenim->actdesactGrille(a); }
void FenetreFenim::actdesactPMaitres() { pfenim->actdesactPMaitres(); }
void FenetreFenim::actdesactCris() { pfenim->actdesactCris(); }
void FenetreFenim::choisit_cri() { pfenim->choisit_cri(); }
void FenetreFenim::crisuiv() { pfenim->crisuiv(); }
void FenetreFenim::criprec() { pfenim->criprec(); }
void FenetreFenim::crifin() { pfenim->crifin(); }
void FenetreFenim::crideb() { pfenim->crideb(); }

void FenetreFenim::selectSpecTags(const QString& codsel) { pfenim->selectSpecTags(codsel); }
void FenetreFenim::specTagNext() { pfenim->specTagNext(); }
void FenetreFenim::specTagPrevious() { pfenim->specTagPrevious(); }
void FenetreFenim::specTagLast() { pfenim->specTagLast(); }
void FenetreFenim::specTagFirst() { pfenim->specTagFirst(); }



FieldClass::FieldClass(QWidget *parent,Fenim *pf,QString title,int fieldType,bool obl,bool unic,QString fieldCode,bool autaj,int smin,int smax)
{
  pfenim = pf;
  _fieldType = fieldType;
  Obl = obl;
  Unic = unic;
  _title = title;
  _fieldCode = fieldCode;
  _titleLabel = new MyQLabel(parent);
  _titleLabel->setText(title);
  if(_fieldType==EC)
  {
      _ec = new EditCombo(parent,pf,fieldCode,autaj);
  }
  if(_fieldType==SLI)
  {
      _Smin = smin;
      _Smax = smax;
  }
// TODO : bien voir l'utilsation de fieldcode... dans les traitements d'événement : par exemple : rique de double utilisation...
  if(_fieldType==SLE || _fieldType==SLI || _fieldType==EC)
  {
     if(_fieldType==SLE || _fieldType==SLI)
       {
       _sle = new Simple_LineEdit(parent,pf,fieldCode);
       _le = _sle;
       }
     if(_fieldType==EC)
       {
       _le = _ec->ec_le;
       }
     _le->setEnabled(true);

  }
  //enum FIELDTYPE {SLE,EC,SLI,CHB};
  if(_fieldType==SLI) 
  {
	_sli = new QSlider(parent);
	_sli->setMinimum(smin);
	_sli->setMaximum(smax);
	_sli->setValue(smax);
    _sli->setOrientation(Qt::Horizontal);
  }
  if(_fieldType==CHB) 
  {
	_chb = new QCheckBox(QString(""),parent);
  }
  
}
// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
void FieldClass::affect(QString text)
{
    //pfenim->m_logStream << "ZF aff 1" << endl;
    if(_fieldType==CHB)
    {
        //pfenim->m_logStream << "ZF aff 2" << endl;
        if(text=="true") _chb->setChecked(true);
        else _chb->setChecked(false);
        //pfenim->m_logStream << "ZF aff 3" << endl;
    }
    else
    {
        //pfenim->m_logStream << "ZF aff 4" << endl;
        _le->setText(text);
        //pfenim->m_logStream << "ZF aff 5" << endl;
        if(_fieldType==EC) _ec->realim_liste("");
        //pfenim->m_logStream << "ZF aff 6" << endl;
        if(_fieldType == SLI) pfenim->modif_indice(text);
        //pfenim->m_logStream << "ZF aff 7" << endl;
    }
    //pfenim->m_logStream << "ZF aff 8" << endl;
}

QString FieldClass::getText()
{
    QString resu = "";
    if(_fieldType==CHB)
    {
        if(_chb->isChecked()) resu = "true"; else resu = "";
    }
    else resu = _le->text();
    return(resu);
}

void FieldClass::colour(QString colorText)
{
    if(_fieldType==CHB) _titleLabel->setStyleSheet(colorText);
    else _le->setStyleSheet(colorText);
}

Fenim::Fenim(QMainWindow *parent,QString repwav,QString nomfi,QDir basejour,bool casa,bool casretr,int typeretraitement,QString suffixe,int vl,int vu)
{
    crefen = false;
    fparent = parent;
    tgui=(TadaridaMainWindow *)fparent;
    casA = casa;
    m_casRetraitement = casretr;
    m_typeretraitement = typeretraitement;
    m_verLog = vl;
    m_verUser = vu;
    //
    QString logFilePath(QString("fenim")+suffixe+".log");
    m_logFile.setFileName(logFilePath);
    m_logFile.open(QIODevice::WriteOnly | QIODevice::Text);
    m_logStream.setDevice(&m_logFile);

    //m_logStream << "$$$ Fenim début : " << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;

    repWav = repwav;

    readVersionRep();

    nomFic = nomfi;
    baseJour = basejour;
    baseJourDat = QDir(basejour.path()+"/dat");
    baseJourIma = QDir(basejour.path()+"/ima");
    baseJourEti = QDir(basejour.path()+"/eti");
    baseJourTxt = QDir(basejour.path()+"/txt");
    nomImage = nomFic + ".jpg";
    imaNom = repwav + "/ima/" + nomImage;
    datFile = repwav + "/dat/" + nomFic + ".dat";
    da2File = repwav + "/dat/" + nomFic + ".da2";
    saisie_a_sauver = false;
    bouge_fichiers = false;
    m_ecraserFichier = false;
    autorise_ajout_type = false;
    type_ajoute = false;
    affloupe = false;
    m_factorX = -1;
    nliv=0;nlih=0;nte=0;
    neplus=false;
    npremcrisel = 0;
    ndercrisel = 0;
    nlt=0;
    specTagNumber = 0;
    //m_logStream << "Fenim fin constructeur " << endl;
}




Fenim::~Fenim()
{
    videfenim();
    if(crefen) delete wfenim;
}


void Fenim::cree_fenetre(bool modesaisie)
{
    //m_logStream << "Fenim cree_fenetre" << endl;
    wfenim = new FenetreFenim(this,fparent);
    wfenim->setWindowTitle("Tadarida - Image et Etiquettes");
    m_poltexte=QFont("Arial",10,QFont::Normal);
    m_poltitre=QFont("Arial",10,QFont::Bold);
    labelImage = new MyQLabel(wfenim);
    gbox_saisie = new QGroupBox(wfenim);
    gbox_gen = new QGroupBox(wfenim);
    gbox_boutons = new QGroupBox(wfenim);
    labelTitreG2 = new QLabel(gbox_gen);
    //m_logStream << "ZFenim avant création des pfc" << endl;
    pfc[ESPECE]       = new FieldClass((QWidget *)gbox_saisie,this,"Espece",EC,true,false,"especes",true,0,0);
    pfc[TYPE]         = new FieldClass((QWidget *)gbox_saisie,this,"Type",EC,true,false,"types",false,0,0);
    pfc[INDICE]       = new FieldClass((QWidget *)gbox_saisie,this,"Indice",SLI,true,false,"indice",false,1,5);
    pfc[ZONE]         = new FieldClass((QWidget *)gbox_saisie,this,"Zone",EC,false,true,"zone",true,0,0);
    pfc[SITE]         = new FieldClass((QWidget *)gbox_saisie,this,"Site",SLE,false,true,"site",false,0,0);
    pfc[COMMENTAIRE]  = new FieldClass((QWidget *)gbox_saisie,this,"Commentaire",false,SLE,false,"commentaire",false,0,0);
    pfc[MATERIEL]     = new FieldClass((QWidget *)gbox_saisie,this,"Materiel",EC,false,true,"materiel",true,0,0);
    pfc[CONFIDENTIEL] = new FieldClass((QWidget *)gbox_saisie,this,"Confidentiel",CHB,false,true,"confidentiel",false,0,0);
    pfc[DATENREG]     = new FieldClass((QWidget *)gbox_saisie,this,"Date",SLE,false,true,"datenreg",false,0,0);
    pfc[AUTEUR]       = new FieldClass((QWidget *)gbox_saisie,this,"Auteur",EC,false,true,"auteur",true,0,0);
    pfc[ETIQUETEUR]   = new FieldClass((QWidget *)gbox_saisie,this,"Etiqueteur",EC,false,false,"etiqueteur",true,0,0);
    //m_logStream << "ZFenim après création des pfc" << endl;
    //
    labelMess = new MyQLabel(gbox_saisie);
    labelRep = new MyQLabel(gbox_gen);
    labelNbcri = new MyQLabel(gbox_gen);
    labelNbeti = new MyQLabel(gbox_gen);
    labelTagSpec = new MyQLabel(gbox_gen);
    chbTagSpec = new QComboBox(gbox_gen);
    labelCris = new MyQLabel(gbox_gen);
    editCri   = new MyQLineEdit((QWidget *)gbox_gen,this,"cri");
    bFlecheDroite = new MyQPushButton(gbox_saisie);
    bFlecheGauche = new MyQPushButton(gbox_saisie);
    bFlecheDebut = new MyQPushButton(gbox_saisie);
    bFlecheFin = new MyQPushButton(gbox_saisie);
    bsaveUneEtiq = new MyQPushButton(gbox_saisie);
    bEnregEtiq = new MyQPushButton(gbox_saisie);
    bClose = new MyQPushButton(gbox_saisie);
    bZoome = new MyQPushButton(gbox_boutons);
    bDezoome = new MyQPushButton(gbox_boutons);
    bcGrille = new QCheckBox(QString("Grille"),gbox_boutons);
    bcPMaitres = new QCheckBox(QString("Pts maitres"),gbox_boutons);
    bcCris = new QCheckBox(QString("Cris"),gbox_boutons);
    labelx = new QLabel(gbox_boutons);
    labely = new QLabel(gbox_boutons);
    labelr = new QLabel(gbox_boutons);
    crefen = true;
    if(!modesaisie)
    {
        bsaveUneEtiq->setEnabled(false);
        bEnregEtiq->setEnabled(false);
    }
    //m_logStream << "Fenim cree_fenetre fin" << endl;
    couleur[0]=QString("QLineEdit {color: blue;}");
    couleur[1]=QString("QLineEdit {color: red;}");
    couleur[2]=QString("QLineEdit {color: green;}");
    couleur[3]=QString("QLineEdit {color: orange;}");
    couleur[4]=QString("QLineEdit {color: black;}");
    _prgSessionEnd = new QProgressBar(gbox_gen);
    //m_logStream << "ZFenim  fin crefen" << endl;
    bFlecheDroiteSpec = new MyQPushButton(gbox_gen);
    bFlecheGaucheSpec = new MyQPushButton(gbox_gen);
    bFlecheDebutSpec = new MyQPushButton(gbox_gen);
    bFlecheFinSpec = new MyQPushButton(gbox_gen);
}


void Fenim::afficher_ecran()
{
    //m_logStream << "afficher_ecran 1" << endl;
    int hb = (m_h - m_my*4 - m_hbou)/3;
    int lb = (m_l - m_mx*3)/2;

    int lbn = m_l - m_mx*2;
    int hbg = (hb-m_my)/3;
    int hbs = hb-m_my-hbg;

    int espgb = m_mx*2;
    int larbl = ((lbn-espgb*5)*2)/9;


    labelImage->setText(nomImage);
    labelImage->move(m_my,m_my/2-7);
    labelImage->resize(500,15);

    QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy1.setHeightForWidth(gbox_saisie->sizePolicy().hasHeightForWidth());
    gbox_saisie->setSizePolicy(sizePolicy1);
    gbox_saisie->setStyleSheet(QString::fromUtf8(""));

    gbox_saisie->resize(lbn,hbs);
    gbox_saisie->move(m_mx,m_h-hb+hbg);

    gbox_boutons->resize(m_l - m_mx*2,m_hbou+m_my);
    gbox_boutons->move(m_mx,m_h-hb-m_my*2-m_my/2-m_hbou);

    int hpz = 17;
    int hgz = 21;
    int margx = 15;
    int margx2 =10;

    int larlabel = (lb-margx*4)/4;
    int laredit = ((lb-margx*4)*3)/8;
    int larcombo = laredit;
    int espy = (hbs- hpz*7)/10;
    int col,lig,typ;
    //m_logStream << "ZFenim afficher_ecran avant placement des champs" << endl;
    for(int iField=0;iField<NBFIELDS;iField++)
    {
        if(iField<(NBFIELDS+1)/2) {col=0;lig=iField;}
        else {col=1; lig=iField-(NBFIELDS+1)/2;}
        typ=pfc[iField]->_fieldType;
        pfc[iField]->_titleLabel->move(margx+col*(lbn/2),hpz*lig+espy*(1+lig));
        pfc[iField]->_titleLabel->resize(larlabel,hpz);
        if(typ==EC || typ==SLI || typ==SLE)
        {
            pfc[iField]->_le->move(margx*2+larlabel+col*(lbn/2),hpz*lig+espy*(1+lig));
            if(typ==SLI) pfc[iField]->_le->resize(laredit+larcombo+margx,hpz);
            else pfc[iField]->_le->resize(laredit,hpz);
            pfc[iField]->_le->setText("");
        }
        if(pfc[iField]->_fieldType==EC)
        {
            pfc[iField]->_ec->ec_co->move(margx*3+larlabel+laredit+col*(lbn/2),hpz*lig+espy*(1+lig));
            pfc[iField]->_ec->ec_co->resize(larcombo,hpz);
            pfc[iField]->_ec->ec_co->setFrame(true);
        }
        if(pfc[iField]->_fieldType==SLI)
        {
            pfc[iField]->_sli->move(margx*3+larlabel+laredit+col*(lbn/2),hpz*lig+espy*(1+lig));
            pfc[iField]->_sli->resize(larcombo,hpz);
        }
        if(pfc[iField]->_fieldType==CHB)
        {
            pfc[iField]->_chb->move(margx*3+larlabel+laredit+col*(lbn/2),hpz*lig+espy*(1+lig));
            pfc[iField]->_chb->resize(larcombo,hpz);
        }
    }
    labelMess->move(margx+lbn/2,hpz*5+espy*6);
    labelMess->resize(larlabel+laredit+larcombo+margx*2,hpz);
    labelMess->setText("");
    //
    int lpb = hgz;
    bFlecheDroite->move(margx,hpz*6+espy*7);
    bFlecheDroite->resize(lpb,hgz);
    bFlecheDroite->setIcon(QIcon("fleche_droite.jpg"));
    //
    bFlecheGauche->move(margx*2+lpb,hpz*6+espy*7);
    bFlecheGauche->resize(lpb,hgz);
    bFlecheGauche->setIcon(QIcon("fleche_gauche.jpg"));
    //
    bFlecheFin->move(margx*3+lpb*2,hpz*6+espy*7);
    bFlecheFin->resize(lpb,hgz);
    bFlecheFin->setIcon(QIcon("fleche_fin.jpg"));
    //
    bFlecheDebut->move(margx*4+lpb*3,hpz*6+espy*7);
    bFlecheDebut->resize(lpb,hgz);
    bFlecheDebut->setIcon(QIcon("fleche_debut.jpg"));
    //
    //bsaveUneEtiq->move(margx*2+larlabel*2,hpz*6+espy*7);
    bsaveUneEtiq->move(espgb*2+larbl,hpz*6+espy*7);
    bsaveUneEtiq->resize(larbl,hgz);
    bsaveUneEtiq->setText("Valider l'Etiquette");

    bEnregEtiq->move(espgb*3+larbl*2,hpz*6+espy*7);
    bEnregEtiq->resize((larbl*3)/2,hgz);
    bEnregEtiq->setText("Sauvegarder le fichier d'Etiquettes");
    bEnregEtiq->setFont(QFont("Arial",10,QFont::Bold));
    //
    bClose->move(espgb*4+(larbl*7)/2,hpz*6+espy*7);
    bClose->resize(larbl,hgz);
    bClose->setText("Fermer");
    // ----------------------------------
    //int espy2 = (hb- hgz*2-hpz*3)/6;
    int espy2 = (hbg- hpz*3)/4;
    //
    gbox_gen->setSizePolicy(sizePolicy1);
    gbox_gen->setStyleSheet(QString::fromUtf8(""));

    //gbox_saisie->move(m_mx,m_h-hb+hbg);
    //gbox_gen->resize(lb,hb);
    gbox_gen->resize(lbn,hbg);
    // gbox_gen->move(lb+m_mx*2,m_h-hb-m_my);
    gbox_gen->move(m_mx,m_h-hb-m_my);
    //
    //
    labelTitreG2->setText(QString("Fichier : ")+nomEti);
    labelTitreG2->move(margx,espy2);
    labelTitreG2->resize((lbn-margx*3)/2,hpz);
    //
    labelRep->setText(QString("Dossier : ")+repEti);
    labelRep->move(margx+lbn/2,espy2);
    labelRep->resize((lbn-margx*3)/2,hpz);
    //
    _prgSessionEnd->move(margx+lbn/2,espy2);
    //_prgSessionEnd->resize((lbn-margx*3)/2,hpz);
    _prgSessionEnd->resize(0,0);
    _prgSessionEnd->setMaximum(10000);
    _prgSessionEnd->setValue(0);
    _prgSessionEnd->setTextVisible(false);
    //
    // reprendre ici :
    labelNbcri->setText(QString::number(m_nbcris)+" cris");
    //labelNbcri->move(margx,hgz+hpz+espy2*3);
    labelNbcri->move(margx,hpz+espy2*2);
    //labelNbcri->resize(lb-margx*2,hpz);
    labelNbcri->resize((lbn-margx*3)/2,hpz);
    //
    labelNbeti->setText(QString::number(m_nbeti)+" etiquettes");
    //labelNbeti->move(margx,hgz+hpz*2+espy2*4);
    labelNbeti->move(margx+lbn/2,hpz+espy2*2);
    //labelNbeti->resize(lb-margx*2,hpz);
    labelNbeti->resize((lbn-margx*3)/2,hpz);
    //
    int mxl = 5;
    int wts = (lbn-margx*3)/2-mxl*3;
    int lw = (wts*2)/9;
    int gw = wts/3;
    int db = gw/4;
    int xdep = margx+lbn/2;
    int yl= hpz+espy2*2;
    chbTagSpec->move(xdep+lw+mxl,yl);
    chbTagSpec->resize(lw,hpz);
    labelTagSpec->move(xdep+lw*2+mxl*2,yl);
    labelTagSpec->resize(lw,hpz);
    // $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
    bFlecheDebutSpec->move(xdep+lw*3+mxl*3,yl);
    bFlecheDebutSpec->resize(lpb,hgz);
    bFlecheDebutSpec->setIcon(QIcon("fleche_debut.jpg"));
    //
    bFlecheFinSpec->move(xdep+lw*3+mxl*3+db,yl);
    bFlecheFinSpec->resize(lpb,hpz);
    bFlecheFinSpec->setIcon(QIcon("fleche_fin.jpg"));
    //
    bFlecheDroiteSpec->move(xdep+lw*3+mxl*3+db*2,yl);
    bFlecheDroiteSpec->resize(lpb,hpz);
    bFlecheDroiteSpec->setIcon(QIcon("fleche_droite.jpg"));
    //
    bFlecheGaucheSpec->move(xdep+lw*3+mxl*3+db*3,yl);
    bFlecheGaucheSpec->resize(lpb,hpz);
    bFlecheGaucheSpec->setIcon(QIcon("fleche_gauche.jpg"));
    // $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
    labelCris->setText("Cri(s)");
    labelCris->move(margx,hpz*2+espy2*3);
    labelCris->resize(larlabel/2,hgz);
    //
    editCri->move(margx*2+larlabel/2,hpz*2+espy2*3);
    editCri->resize(lbn-margx*3-larlabel/2,hgz);
    editCri->setText("");

    int lzb = gbox_boutons->width();
    int larpw = (lzb-margx2*13)/12;


    bZoome->move(margx2,m_my/4);
    bZoome->resize(larpw,m_hbou);
    bZoome->setText("Zoom +");

    bDezoome->move(margx2*2+larpw,m_my/4);
    bDezoome->resize(larpw,m_hbou);
    bDezoome->setText("Zoom -");

    bcGrille->move(margx2*3+larpw*2,m_my/4);
    bcGrille->resize(larpw,m_hbou);
    bcGrille->setChecked(true);

    bcPMaitres->move(margx2*4+larpw*3,m_my/4);
    bcPMaitres->resize(larpw,m_hbou);
    bcPMaitres->setChecked(true);

    bcCris->move(margx2*5+larpw*4,m_my/4);
    bcCris->resize(larpw,m_hbou);
    bcCris->setChecked(false);


    labelx->move(lzb - margx2 * 2 - larpw * 2,m_my/4);
    labely->move(lzb - margx2 - larpw,m_my/4);
    labelr->move(lzb - margx2  * 3 - larpw*3,m_my/4);

    labelx->resize(larpw,m_hbou);
    labely->resize(larpw,m_hbou);
    labelr->resize(larpw,m_hbou);
    afficheratio();
    gbox_saisie->setFont(m_poltexte);
    gbox_gen->setFont(m_poltexte);
    labelCris->setFont(m_poltitre);
    labelTitreG2->setFont(m_poltitre);
    editCri->setFont(m_poltitre);
    //m_logStream << "afficher_ecran 2" << endl;
    //
    wfenim->etablit_connexions();
    //m_logStream << "ZFenim afficher_ecran fin" << endl;
}

void Fenim::initialiseZones()
{
    //m_logStream << "ZFenim initialiseZones 1" << endl;
    // 1)
    for(int i=0;i<NBFIELDS;i++)
    {
        if(pfc[i]->Unic) FileFields[i]="";
    }
    //
    //m_logStream << "ZFenim initialiseZones 2" << endl;
    // 2)
    //m_logStream << "m_nbcris=" << m_nbcris << endl;
    for(int i=0;i<m_nbcris;i++)
    {
        if(!lesEtiq[i]->DataFields[ESPECE].isEmpty())
        {
            // dès qu'on a une étiquette non vide : on prend les valeurs pour ces champs
            for(int j=0;j<NBFIELDS;j++)
            {
                if(pfc[j]->Unic)
                {
                    FileFields[j] = lesEtiq[i]->DataFields[j];
                    tgui->LastFields[j] = FileFields[j];
                }
            }
            break;
        }
    }
   // m_logStream << "ZFenim initialiseZones 3" << endl;
    // 3)
    for(int i=0;i<NBFIELDS;i++)
    {
        //m_logStream << "ZF IZ 1 - iField=" << i << endl;
        pfc[i]->affect(tgui->LastFields[i]);
        //m_logStream << "ZF IZ 2" << endl;
        pfc[i]->colour(couleur[0]);
        //m_logStream << "ZF IZ 3" << endl;
    }
    //m_logStream << "ZFenim initialiseZones fin" << endl;
}

QRect Fenim::getFenetre()
{
    return(m_rf);
}

MyQGraphicsView* Fenim::getView()
{
    return(view);
}

QImage * Fenim::getImage()
{
    return(fenima);
}

void Fenim::choisit_cri()
{
    //QMessageBox::warning(wfenim,"ri saisi",editCri->text(), QMessageBox::Ok);
    if(neplus) return;
    QString cs=editCri->text();
    bool ok=true;
    int n=1;
    if(cs.length()==0) ok=false;
    else
    {
        bool convint;
        n = cs.toInt(&convint);
        if(convint==false) ok=false;
        else
        {
            if(n<1 || n>m_nbcris) ok=false;
        }
    }
    if(ok==false)
    {
        editCri->setText(m_lesCris);
    }
    else
    {
        if(cs!=m_lesCris) selectionneCri(n-1,false);
    }
}
// $$$$$$$$$$$$$$$$$$$$$$$$$

void EditCombo::realim_liste(const QString& codsai)
{
    ec_co->clear();
    if(codsai.isEmpty()) ec_co->insertItems(0,*liste_codes);
    else
    {

        QStringList liste2;
        for(int i=0;i<liste_codes->size();i++)
        {
            if(liste_codes->at(i).contains(codsai))
                liste2.append(liste_codes->at(i));
        }
        if(liste2.size()<1) ec_co->insertItems(0,*liste_codes);
        else ec_co->insertItems(0,liste2);
    }
}

void EditCombo::selectionne_code()
{
    if(ec_co->count()>0) selectionne_code(ec_co->itemText(0));
}

void EditCombo::selectionne_code(const QString& codsel)
{
    if(ec_co->count()<1) ec_co->insertItems(0,*liste_codes);
    ec_le->setText(codsel);
    ec_le->setFocus(); // en attendant d'avoir le paramètre pour passer au champ suivant ZZZ
}


bool EditCombo::confirme_ajout(QString& s)
{
    if(QMessageBox::question((QWidget *)fenpar, (const QString &)QString("Attention"),
                             (const QString &)QString("Code ")+s+QString(" absent de la table : accepter ce code ?"),
                             QMessageBox::Yes|QMessageBox::No)
            == QMessageBox::No) return(false);
    liste_codes->insert(0,s);
    liste_codes->sort();
    realim_liste(s);
    QFile fichier;
    QTextStream textefi;
    fichier.setFileName(codefi+".txt");
    if(fichier.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        textefi.setDevice(&fichier);
        for(int i=0;i<liste_codes->size();i++) textefi << liste_codes->at(i) << endl;
        fichier.close();
    }
    return(true);
}

void Fenim::selectionne_indice(int n)
{
    pfc[INDICE]->_le->setText(QString::number(n));
    pfc[INDICE]->_le->setStyleSheet(couleur[4]);
}

void Fenim::modif_indice(const QString& indsai)
{
    bool convint;
    int nind = indsai.toInt(&convint);
    if(convint)
    {
        if(nind>0 && nind<6)
            pfc[INDICE]->_sli->setValue(nind);
    }
}

void Fenim::clique_confi(int s)
{
    pfc[CONFIDENTIEL]->colour(couleur[4]);
}


bool EditCombo::controle(QString &s,QString &mess_err,bool obl)
{
    bool bon = true; mess_err="";
    s=ec_le->text();
    if(s.isNull() || s.isEmpty())
    {
        if(obl==true)
        {
            bon = false;
            mess_err="Il faut saisir le code " + codefi + " !";
        }
        else
        {
            bon=true;
        }
    }
    else
    {
        if(!liste_codes->contains(s))
        {
            if(!autorise_ajout)
            {
                bon = false;
                mess_err = "Code incorrect !";
            }
            else
            {
                if(!confirme_ajout(s))
                    bon = false;
            }
        }
    }
    if(!bon)
    {
        ec_le->clear();
        realim_liste("");
        ec_le->setFocus();
    }
    return(bon);
}

void Fenim::enreg_etiq()
{
    QString s,mess_err="";
    for(int i=0;i<NBFIELDS;i++)
    {
        s=pfc[i]->getText();
        if(s.isEmpty() && pfc[i]->Obl==true)
        {
            QMessageBox::warning(wfenim,"Saisie obligatoire",mess_err, QMessageBox::Ok);
            pfc[i]->_le->setFocus();
            return;
        }
        if(pfc[i]->_fieldType==EC)
        {
            if(!pfc[i]->_ec->controle(s,mess_err,true))
            {
                QMessageBox::warning(wfenim,"Saisie incorrecte",mess_err, QMessageBox::Ok);
                pfc[i]->_ec->ec_le->setFocus();
                return;
            }
        }
        if(pfc[i]->_fieldType==SLI && !s.isEmpty())
        {
            bool convint;
            int nind = pfc[i]->getText().toInt(&convint);
            if(convint)
            {
                if(nind<pfc[i]->_Smin || nind>pfc[i]->_Smax) convint=false;
            }

            if(!convint)
            {
                QMessageBox::warning(wfenim,"Saisie incorrecte","Saisir un nombre entre 1 et 5 !", QMessageBox::Ok);
                pfc[i]->_le->setFocus();
                return;
            }
        }
    }
    QString dae= pfc[DATENREG]->getText();
    if(!dae.isEmpty())
    {
        QString sj=dae.section("/",0,0);
        QString sm=dae.section("/",1,1);
        QString sa=dae.section("/",2,2);
        bool convdate=false;
        bool convj,convm,conva;
        int j = sj.toInt(&convj);
        int m = sm.toInt(&convm);
        int a = sa.toInt(&conva);
        if(convj && convm && conva)
        {
            convdate=QDate(a,m,j).isValid();
        }
        if(!convdate)
        {
            QMessageBox::warning(wfenim,"Saisir une date jj/mm/aaaa",mess_err, QMessageBox::Ok);
            pfc[DATENREG]->_le->setFocus();
            return;
        }
    }
    // -----
    int premi=-1;
    for(int i=0;i<m_nbcris;i++)
    {
        if(m_crisel[i])
        {
            if(premi<0) premi=i;
            if(lesEtiq[i]->DataFields[ESPECE].isEmpty()) m_nbeti++;
            for(int j=0;j<NBFIELDS;j++)
                lesEtiq[i]->DataFields[j]= this->pfc[j]->getText();
            // $$$$$$$$$$$$$$$$$
            // 05-03-2015
            // if(!m_casRetraitement) updateTagNSpec(i);
            updateTagNSpec(i);
        }
    }
    if(!m_casRetraitement)
    {
        conserveSaisies(lesEtiq[premi]);
        repeint_en_vert();
        traite_champs_uniques();
        TadaridaMainWindow *tg=(TadaridaMainWindow *)fparent;
        tg->TextsToSave = true;
        affiche_nbeti();
        updateChbTagSpec();
    }
    saisie_a_sauver = true; // on peut faire mieux
}

void Fenim::conserveSaisies(Etiquette *peti)
{
    for(int i=0;i<NBFIELDS;i++)
        tgui->LastFields[i] = peti->DataFields[i];
}

void Fenim::traite_champs_uniques()
{
    bool a_etendre = false;

    for(int jField=0;jField<NBFIELDS;jField++)
    {
        if(pfc[jField]->Unic)
        {
            if(FileFields[jField] != tgui->LastFields[jField])
            {
                FileFields[jField] = tgui->LastFields[jField];
                a_etendre = true;
            }
        }
    }
    if(a_etendre)
    {
        for(int i=0;i<m_nbcris;i++)
        {
            if(!m_crisel[i] && !lesEtiq[i]->DataFields[ESPECE].isEmpty())
            {
                for(int jField=0;jField<NBFIELDS;jField++)
                    if(pfc[jField]->Unic) lesEtiq[i]->DataFields[jField] = FileFields[jField];
            }
        }
    }
}

void Fenim::repeint_en_vert()
{
    // on vient de valider une étiquette ou un groupe d'étiquette : on passe les champs simples
    // en vert
    for(int j=0;j<NBFIELDS;j++) pfc[j]->colour(couleur[2]);
}

bool Fenim::afficher_image(bool modesaisie)
{
    //m_logStream << "ZFenim afficher_image debut" << endl;

    if(m_verRepLog <m_verLog || m_verRepUser < m_verUser)
    {
        QMessageBox::warning(fparent,"Fichier inaccessible","Version en retard : retraiter d'abord le dossier !", QMessageBox::Ok);
        return(false);
    }
    initialise_cris();
    if(loadMatriceCris2(da2File)==false)
    {
        QMessageBox::warning(fparent,"Fichier inaccessible","Retraiter d'abord le dossier", QMessageBox::Ok);
        return(false);
    }
    //m_logStream << "ZFenim afficher_image appelle cree_fenetre" << endl;
    cree_fenetre(modesaisie);
    //m_logStream << "ZFenim afficher_image retour de cree_fenetre" << endl;
    m_mx = 10 ; m_my=12; // marges entre grandes parties de l'écran
    m_hbou = 18;
    wfenim->resize(1150,630);
    fenima = new QImage;
    fenima->load(imaNom);
    scene = new MyQGraphicsScene(this,wfenim,false); // création de la scene
    view = new MyQGraphicsView(wfenim); // création de la view
    view->setScene(scene);  // ajout de la scene dans la view

    //
    //
    if(_imaHeight<=128)
    {
        view->setFixedSize(1060,fenima->height()+200);
    }
    else
    {
        view->setFixedSize(1060,fenima->height()+72);
    }
    view->move(m_mx,m_my);
    fenouv = true;
    pix=(scene->addPixmap(QPixmap::fromImage(*fenima))); // ajout du pixmap dans la scene
    //m_logStream << "ZFenim afficher_image 11" << endl;
    // ------------------- 29/1/2014
    wfenim->showMaximized();
    m_l = wfenim->width();
    m_h= wfenim->height();
    m_rf = wfenim->geometry();
    wfenim->activateWindow();
    wfenim->raise();
    //int hv = ((m_h - m_my*3)*2)/3;
    int hv = ((m_h - m_my*4 - m_hbou)*2)/3;

    int lv = m_l - m_mx*2;
    view->setFixedSize(lv,hv);
    // float rl = 50000f / ((h x 16) /9);
    // ratio à revoir
    //m_rl =   (float)m_h * (1+m_xmoitie) / (float)28125;
    //m_rh = ((float)hv * m_iSizeFFTHalf) / ((float)fenima->height() * 120);

    // edit yves - changer echelle xy
    m_rl =   (float)m_h * (1+m_xmoitie) / (float)(tgui->Divrl);
    m_rh = ((float)hv * _imaHeight) / ((float)fenima->height() * 160);

    // m_rl *= 2;
    view->SCALE(m_rl,m_rh);
    //view->setAlignment(Qt::AlignBottom);
    //int dy = m_iSizeFFTHalf - 121;
    int yc = _imaHeight + _imaHeight - 121;
    // yc : point central pour forcer le scrolling
    view->centerOn(0,yc); // yes : cela marche

    //m_logStream << "m_iSizeFFTHalf = " << m_iSizeFFTHalf << endl;
    //m_logStream << "m_rh = " << m_rh << endl;
    //m_logStream << "m_rl = " << m_rl << endl;
    // -----------------------------
    // --
    //m_logStream << "avant chargeEtiquettes" << endl;
    if(chargeEtiquettes()==false)
    {
        if(crefen) wfenim->close();
        return(true);
    }
    //m_logStream << "apres chargeEtiquettes et avant afficher_ecran" << endl;

    afficher_ecran();
    //m_logStream << "ZFenim après afficher_ecran et avant iz" << endl;

    initialiseZones();
    //m_logStream << "ZFenim après initialiseZones" << endl;

    //m_logStream << "après afficher_ecran" << endl;
    cree_points_maitres();
    initialise_traits();
    //m_logStream << "ai 12" << endl;
    //
    //m_logStream << "$$$ Fenim afficher_image apres chargement etiquettes : " << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;
    //if(m_iSizeFFTHalf<=128) view->scale(1,2);
    afficher_grille(true);
    afficher_cris();
    afficher_points_maitres();
    //m_logStream << "$$$ Fenim afficher_image après affichage des cris : " << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;
    // afficher_ecran();
    view->setMouseTracking(true);

    selectionneCri(0,false);

    //QMessageBox::warning(wfenim,"Fenim","affim 4", QMessageBox::Ok);
    //m_logStream << "$$$ Fenim fin afficher_image : " << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;
    return(true);
}

bool Fenim::chargeCrisEtiquettes()
{
    //m_logStream << "Fenim charge_cris_etiquettes" << endl;
    if(!m_casRetraitement) initialise_cris();
    if(loadMatriceCris2(da2File)==false)
    {
        fenouv = false;
        return(false);
    }
    else fenouv = true;
    if(chargeEtiquettes()==false) return(false);
    //m_logStream << "Fenim charge_cris_etiquettes fin" << endl;
    return(true);
}

void Fenim::zoome()
{
    //QMessageBox::warning(wfenim,"zoom","Passe dans zoome !", QMessageBox::Ok);
    zoomef(1.414f);
}
void Fenim::dezoome()
{
    zoomef(0.707f);
}
void Fenim::zoomef(float iz)
{
    //QMessageBox::warning(wfenim,"zoom","Passe dans zoome !", QMessageBox::Ok);
    view->SCALE(iz,1);
    m_rl *= iz;
    /*
    rafraichit_image();
    afficher_grille(bcGrille->isChecked());
    reafficher_cris();
    */
    // rafraichit_tout(); retiré le 26/2/2014
    afficher_grille(bcGrille->isChecked());
    afficher_points_maitres();
    afficheratio();
}

void Fenim::initialise_cris()
{
    for(int i=0;i<NCRETES;i++)
    {
        m_matrixCalls[i].clear();
        for(int j=0;j<NSUPPL;j++) m_pointsSuppl[i][j].clear();
    }
    m_pointsMaitres.clear();
    m_matrixContours.clear();

}

void Fenim::afficher_cris()
{
    // pour version contour
    // if(m_mode==2) return;
    bool affichercri = bcCris->isChecked();
    for(int i=0;i<m_nbcris;i++) afficher_un_cri(i,m_crisel[i],affichercri);
}

void Fenim::afficher_points_maitres()
{
    bool afficherpm = bcPMaitres->isChecked();
    for(int i=0;i<m_nbcris;i++) afficher_un_point_maitre(i,m_crisel[i],afficherpm);
}

void Fenim::rafraichit_image()
{
    pix->setPixmap(QPixmap::fromImage(*fenima));
}

void Fenim::actdesactGrille(int state)
{
    afficher_grille(state==Qt::Checked);
}

void Fenim::actdesactPMaitres()
{
    afficher_points_maitres();
}

void Fenim::actdesactCris()
{
    afficher_cris();
}

void Fenim::afficher_grille(bool afficher)
{
    if(nliv>0) for(int i=0;i<nliv;i++) delete gliv[i];
    if(nlih>0) for(int i=0;i<nlih;i++) delete glih[i];
    if(nte>0) for(int i=0;i<nte;i++) delete gte[i];
    nliv=0; nlih=0; nte=0;
    if(!afficher) return;

    float invsx = 1.0f/m_rl;
    float invsy = 1.0f/m_rh;
    QFont qf("Arial",8);
    int xmax=fenima->width();
    //m_logStream << "afficher_grille" <<  endl;
    float tmax = getms(xmax-1);
    float igt=0;
    float incrt = 500;
    if(m_rl>0.25) incrt = 100;

    QPen qp = QPen(QColor(128,128,128),0);
    while(igt<tmax && nliv < 500 && nte<250)
    {
        float x=getx(igt);
        gliv[nliv] = scene->addLine(x,0,x,_imaHeight-1,qp);
        if((nliv & 1)==0 && nliv>0)
        {
            int nigt =(int)igt; int ns = nigt/1000;
            QString affi;
            if(nigt == ns * 1000) affi = QString::number(ns)+" sec";
            else affi = QString::number(nigt)+" ms";
            gte[nte] = scene->addSimpleText(affi,qf);
            gte[nte]->setPos(x+invsx*2,_imaHeight-((float)30/m_rh));
            //gte[nte]->scale(invsx,invsy);
            gte[nte]->SCALE(invsx,invsy);
            nte++;
        }
        nliv++;
        igt += incrt;
    }
    //m_logStream << "m_rl = " << m_rl <<  endl;
    //
    float fmax = getkhz(0);
    float igf=0;
    float incrf = 10;

    while(igf<fmax && nlih < 500)
    {
        float y=gety(igf);
        // for(int k=x;k<x+nbl;k++)
        glih[nlih] = scene->addLine(0,y,xmax-1,y,qp);
        if((nlih & 1)==0 && nlih>0)
        {
            gte[nte] = scene->addSimpleText(QString::number(igf)+" kHz",qf);
            gte[nte]->setPos(1+invsx,y-8+m_rh);
            gte[nte]->SCALE(invsx,invsy);
            nte++;
        }
        //m_logStream << "grille x = " << x << " nliv = " << nliv <<  endl;
        nlih++; igf += incrf;
        //m_logStream << "grille y = " << y << " igf = " << igf <<  endl;
    }
    //m_logStream << "m_rh = " << m_rh <<  endl;
}

void Fenim::afficher_un_cri(int ncri,bool crisel,bool affichercri)
{
    // pour version contour
    // if(m_mode==2) return;
    // prévoir un affichage lors de la sélection

    if(!(ncri<MAXCRI)) return;
    bool etiquette = !lesEtiq[ncri]->DataFields[ESPECE].isEmpty();

    QPen qp = QPen(QColor(
                       (255-80*etiquette)*(!crisel)+20*crisel*etiquette,
                       (255-100*etiquette)*(!crisel),
                       255*(!crisel)+80*crisel*etiquette),
                       0);

    //m_logStream << "afficher_un_cri pour ncri = " << ncri << endl;
    if(!affichercri)
    {
        //m_logStream << "cas 1 - suppression "  << endl;
        if(ilc[ncri])
        {
            delete gplt[ncri];
            ilc[ncri]=false;
        }
    }
    else
    {
        if(!ilc[ncri])
        {
            //m_logStream << "cas 2 - création"  << endl;
            QPolygonF polygone;
            for(int j=0;j<m_matrixCalls[0][ncri].size();j++)
            {
                int x=m_matrixCalls[0][ncri][j].x()/(1+m_xmoitie);
                int y=_imaHeight-m_matrixCalls[0][ncri][j].y()-1;
                polygone << QPointF(x,y);
            }
            QPainterPath path = QPainterPath();
            path.addPolygon(polygone);
            gplt[ncri] = new QGraphicsPathItem(path);
            gplt[ncri]->setPen(qp);
            scene->addItem(gplt[ncri]);
            ilc[ncri]=true;
        }
        else
        {
            int counou=(int)etiquette*2+(int)crisel;
            if(counou!=coucri[ncri])
            {
                //m_logStream << "cas 3a - changement de couleur"  << endl;
                gplt[ncri]->setPen(qp);
                coucri[ncri]=counou;
            }
        }
    }
}

void Fenim::afficher_un_point_maitre(int ncri,bool crisel,bool afficherpm)
{
    if(!(ncri<MAXCRI)) return;
    int x = m_pointsMaitres[ncri].x()/(1+m_xmoitie);
    int y = _imaHeight - m_pointsMaitres[ncri].y()-1;
    //QPen qpm = QPen(QColor(160-80*crisel,128*crisel,255-160*crisel),0);
    //QBrush qb = QBrush(QColor(160-80*crisel,128*crisel,255-160*crisel),Qt::SolidPattern);
    //QPen qpm = QPen(QColor(96*(1-crisel),96*(1-crisel),96*(1-crisel)),0);
    //QBrush qb = QBrush(QColor(96*(1-crisel),96*(1-crisel),96*(1-crisel)),Qt::SolidPattern);

    bool etiquette = !lesEtiq[ncri]->DataFields[ESPECE].isEmpty();
    int nspec= (lesEtiq[ncri]->SpecNumber);
    int rouge,vert,bleu,ajdec;
    bleu=0;
    ajdec=0;
    if(nspec>=8) nspec = nspec & 7;
    if(nspec>4) {nspec-=4; ajdec=1;}
    if(crisel)
    {
        rouge=0;vert=0;
        if(etiquette) {vert=16*nspec;rouge=8*nspec;}
    }
    else
    {
        rouge=255;vert=0;
        if(etiquette)
        {vert=255-50*nspec+32*ajdec;rouge=50*nspec+32*ajdec;}
    }
    //
    // QPen qpm = QPen(QColor(255*(1-crisel),128*etiquette*(1-crisel),0,0));
    QPen qpm = QPen(QColor(rouge,vert,bleu),0);
    QBrush qb = QBrush(QColor(rouge,vert,bleu),Qt::SolidPattern);

    if(ipmc[ncri]==true)
    {
        delete gepm[ncri];
        ipmc[ncri]=false;
    }
    if(afficherpm)
    {
        if(ipmc[ncri]==false)
        {
            // float w=3.0f/m_rl+m_rl/16;
            // float h=2.0f/m_rh+m_rh/16;
            float w=6.0f/m_rl;
            float h=6.0f/m_rh;
            gepm[ncri]=scene->addEllipse(x-w/2,y-h/2,w,h,qpm,qb);
            ipmc[ncri]=true;

        }
        else gepm[ncri]->setPen(qpm);
    }
    // ----------------------------------------------------------
}


bool Fenim::loadMatriceCris2(QString da2File)
{

    //m_logStream << "loadMatriceCris2 " << endl;
    //m_logStream << " fichier cris :  " << da2File << endl;
    m_cris2File.setFileName(da2File);
    if(m_cris2File.open(QIODevice::ReadOnly)==false)
    {
        m_logStream << "fichier cris inexistant : " << da2File << endl;
        if(!m_casRetraitement)
        QMessageBox::warning(wfenim, "Fin", "Fichier de cris inexistant", QMessageBox::Ok);
        return(false);
    }
    // gérer les retours...
    m_cris2Stream.setDevice(&m_cris2File);

    // numéro de version
    int numver=0,numveruser=0;

    m_cris2Stream >> numver;


    if(numver>3) m_cris2Stream >> numveruser;
    if(numver<11)
    {
        QMessageBox::warning(wfenim, "Fin", "Fichier de cris incompatible avec la version du logiciel", QMessageBox::Ok);
        return(false);
    }

    m_logStream << "numver,m_verLog=" << numver << "," << m_verLog << endl;
    m_logStream << "numveruser,m_verUser=" << numveruser << "," << m_verUser << endl;

    if(!m_casRetraitement)
    {
        if(numver < m_verLog || numveruser < m_verUser)
        {
            m_logStream << "version du dat en retard" << endl;
            m_logStream << "numveruser,m_verUser=" << numveruser << "," << m_verUser << endl;
            m_cris2File.close();
            return(false);
        }
    }

    m_logStream << "version " << numver << endl;

    // nombre de cris
    m_cris2Stream >> m_nbcris;
    m_logStream << "loadmatricecris2  -->  nbre cris = " << m_nbcris << endl;

    m_cris2Stream >> _imaHeight;
    m_logStream << "loadmatricecris2  -->  _imaHeight " << _imaHeight << endl;
    if(numver > 1) m_cris2Stream >> m_xmoitie;
    //m_logStream << "2 m_xmoitie=" << m_xmoitie << endl;

    if(numver>2)
    {
        m_cris2Stream >> m_factorX;
        m_cris2Stream >> m_factorY;
    }

    // £££ 27/05/2015
    _numVer = numver;
    _tE = 10;
    if(numver>19)
    {
        m_cris2Stream >> _tE;
    }
    else
    {
        _numtE = m_factorY * _imaHeight * 2000;
    }
    // fin £££ 27/05/2015

    // m_logStream << "nbre de cris : " << nbcris << endl;
    // m_logStream << "sizeffthalf = " << m_iSizeFFTHalf << endl;
    // m_logStream << "m_xmoitie = " << m_xmoitie << endl;
    // -------------------
    // pour chaque cri

    int xpointm,ypointm;
    if(m_nbcris >0 && m_nbcris < MAXCRI)

    for(int i=0;i<m_nbcris;i++)
    {
        //m_logStream << endl;
        //m_logStream << "Cri n° " << i << endl;
        m_cris2Stream >> xpointm;
        m_cris2Stream >> ypointm;

        QPoint p;
        p.setX(xpointm);
        p.setY(ypointm);
        m_pointsMaitres.push_back(p);

        int longCri;
        m_cris2Stream >> longCri;

        //m_logStream << "loncri = " << longCri << endl;

        m_vectorContours.clear();
        m_vectorEnergie.clear();
		

        int x,y;
        float e;
        for(int j=0;j<longCri;j++)
        {
            m_cris2Stream >> x >> y >> e;
            QPoint p;
            p.setX(x);
            p.setY(y);
            m_vectorContours.push_back(p);
			
            m_vectorEnergie.push_back(e);
			
        }
        m_matrixContours.push_back(m_vectorContours);
        m_matrixEnergie.push_back(m_vectorEnergie);
        //
        int longCrete;
        //
        for(int jcrete=0;jcrete<NCRETES;jcrete++)
        {
            m_vectorCalls.clear();
            m_cris2Stream >> longCrete;
            for(int j=0;j<longCrete;j++)
            {
                m_cris2Stream >> x >> y;
                QPoint p;
                p.setX(x);
                p.setY(y);
                m_vectorCalls.push_back(p);
            }
            m_matrixCalls[jcrete].push_back(m_vectorCalls);

        }
        //
        //m_cris2Stream >> lowfc >> hifc >> fc3;
        for(int jcrete=0;jcrete<NCRETES;jcrete++)
            for(int k=0;k<NSUPPL;k++)
            {
                m_cris2Stream >> x >> y;
                QPoint p;
                p.setX(x);
                p.setY(y);
                m_pointsSuppl[jcrete][k].push_back(p);
            }

    }
    m_cris2File.close();
    return(true);
}

int Fenim::rematcheEtiquettes(Fenim * fenim1,bool initial,QString recupVersion,int *cpma)
{
    // apriori : rien à changer pour la version contour !
    m_logStream << "rematcheEtiquettes - m_nbcris = " << m_nbcris << endl;
    m_logStream << "rematcheEtiquettes - nb points maitres = " << m_pointsMaitres.size() << endl;
    m_logStream << "rematcheEtiquettes - fenim1 nb points maitres = " << fenim1->m_pointsMaitres.size() << endl;
    // ajoutr le nombre d'étiquettes !
    int nrecup=0;
    QStringList listSpecrecup;
    for(int i=0;i<m_nbcris;i++)
    {
        //m_logStream << "i = " << i << endl;
        if(!initial) if(!lesEtiq[i]->DataFields[ESPECE].isEmpty()) continue;
        QPoint p = this->m_pointsMaitres.at(i);
        int x=p.x(),y=p.y();
        for(int j=0;j<fenim1->m_pointsMaitres.size();j++)
        {
            //m_logStream << "j = " << j << endl;
            QPoint p1 = fenim1->m_pointsMaitres.at(j);
            int x1=p1.x(),y1=p1.y();
            *cpma += (x1+y1);
            int d=(x-x1)*(x-x1)+(y-y1)*(y-y1);
            if(d<20)
            {
                QString esp = fenim1->lesEtiq[j]->DataFields[ESPECE];
                if(!esp.isEmpty())
                {
                    m_nbeti++;
                    lesEtiq[i]->recopie(fenim1->lesEtiq[j]);
                    if(!initial)
                        if(!_listTaggedSpecies.contains(esp))
                            if(!listSpecrecup.contains(esp))
                                listSpecrecup.append(esp);

                    nrecup++;
                    // m_logStream << "copie faite" << endl;
                }
                break;
            }
        }
    }
    if(listSpecrecup.size()>0)
    {
        for(int i=0;i<listSpecrecup.size();i++)
        {
            QString esp = listSpecrecup.at(i);
        tgui->RetreatText << "Recuperaton de l'espèce " << esp << " pour le fichier " << nomFic
                          << " dans le dossier de la version " << recupVersion << endl;
        }
    }
    return(nrecup);
}

void Fenim::cree_points_maitres()
{
    for(int i=0;i<m_nbcris && i<MAXCRI;i++)
        ipmc[i]=false;
}

void Fenim::initialise_traits()
{
    // on laisse car cela doit reservir et ne coute pas cher
    for(int i=0;i<m_nbcris && i<MAXCRI;i++)
    {
        ilc[i]=false;
        coucri[i]=0;
    }
}

void Fenim::updateTagNSpec(int i)
{
    int pos = _listTaggedSpecies.size();
    QString esp = lesEtiq[i]->DataFields[ESPECE];
    if(!_listTaggedSpecies.contains(esp)) _listTaggedSpecies.append(esp);
    else pos=_listTaggedSpecies.indexOf(esp);
	lesEtiq[i]->SpecNumber = pos;
    m_logStream<< "updateTagNSpec : i=" << i << " - esp=" << esp << endl;
    //m_logStream<< "updateTagNSpec : liste=" << _listTaggedSpecies << endl;
    m_logStream<< "updateTagNSpec : pos=" << pos << endl << endl;

}

void Fenim::updateChbTagSpec()
{
    chbTagSpec->clear();
    chbTagSpec->insertItems(0,_listTaggedSpecies);
}

bool Fenim::chargeEtiquettes()
{
    //
    // pour version contours : mise en rem: m_nbcris est tj chargé dans loadmatricecris
    //m_nbcris = m_matrixCalls.size();
    m_logStream << "chargeEtiquettes 1 - m__nbcris=" << m_nbcris << endl;
    m_crisel = new bool[m_nbcris];
    m_excrisel = new bool[m_nbcris];
    lesEtiq = new Etiquette*[m_nbcris];
    for(int i=0;i<m_nbcris;i++)
    {
        m_crisel[i]=false;
        m_excrisel[i]=false;
        lesEtiq[i]=new Etiquette((int)i);
        lesEtiq[i]->vide();
    }
    repEti = repWav + "/eti";
    QDir direti(repEti);
    if(!direti.exists()) direti.mkdir(repEti);
    // pour version contour
    nomEti = nomFic + ".eti";
    m_etiNom = repEti + "/" + nomEti;
    m_etiFile.setFileName(m_etiNom);
    m_ecraserFichier = false;
    int ncrilus=0;
    m_nbeti = 0;
    if(m_casRetraitement && m_typeretraitement == 2)
    {
        m_ecraserFichier = true;
    }
    else
    {
        if(m_etiFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            m_etiStream.setDevice(&m_etiFile);
            m_etiStream.readLine(); // pour éliminer ligne titre
            for(int i=0;i<=m_nbcris;i++)
            {
                if(m_etiStream.atEnd()) break;
                QString ligne = (m_etiStream.readLine());
                if(ligne.isNull() or ligne.isEmpty()) break;
                int postab = ligne.indexOf((char)'\t');
                if(postab>0)
                {
                    if(ligne.left(postab) != QString::number(i)) break;
                    ncrilus++;
                    if(i<=m_nbcris-1)
                    {
                        lesEtiq[i]->DataFields[ESPECE]=ligne.section('\t',1,1);
                        if(!lesEtiq[i]->DataFields[ESPECE].isEmpty())
                        {
                            // 05-03-2015
                            // if(!m_casRetraitement) updateTagNSpec(i);
                            updateTagNSpec(i);
                            m_nbeti++;
                        }
                        lesEtiq[i]->DataFields[TYPE]=ligne.section('\t',2,2);
                        lesEtiq[i]->DataFields[INDICE]=ligne.section('\t',3,3);
                        lesEtiq[i]->DataFields[ZONE]=ligne.section('\t',4,4);
                        lesEtiq[i]->DataFields[SITE]=ligne.section('\t',5,5);
                        lesEtiq[i]->DataFields[COMMENTAIRE]=ligne.section('\t',6,6);
                        lesEtiq[i]->DataFields[MATERIEL]=ligne.section('\t',7,7);
                        lesEtiq[i]->DataFields[CONFIDENTIEL]=ligne.section('\t',8,8);
                        lesEtiq[i]->DataFields[DATENREG]=ligne.section('\t',9,9);
                        lesEtiq[i]->DataFields[AUTEUR]=ligne.section('\t',10,10);
                        lesEtiq[i]->DataFields[ETIQUETEUR]=ligne.section('\t',11,11);
                    }
                    else break;
                }
                else break;
            }
            m_etiFile.close();
            if(ncrilus != m_nbcris)
            {
                if(ncrilus>0)
                {
                    reinitialise_etiquettes();
                    if(QMessageBox::question(wfenim, "Attention", "Le fichier d'Etiquettes ne correspond pas : confirmer son ecrasement ?",
                                             QMessageBox::Yes|QMessageBox::No)
                            == QMessageBox::No)
                    {
                        wfenim->close();
                        return(false);
                    }

                }
                m_ecraserFichier = true;
            }
            else
            {
                if(!m_casRetraitement) updateChbTagSpec();
            }
        } // fin du else % cas retraitement
    }
    return(true);
}

void Fenim::affiche_nbeti()
{
    labelNbeti->setText(QString::number(m_nbeti)+" etiquettes");
}

void Fenim::reinitialise_etiquettes()
{
    m_nbeti=0;
    for(int i=0;i<m_nbcris;i++)
        lesEtiq[i]->vide();
}

void Fenim::EnregEtiquettes()
{
    m_logStream << "EnregEtiquettes - debut" << endl;
    if(m_ecraserFichier) m_etiFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
    else m_etiFile.open(QIODevice::WriteOnly | QIODevice::Text);
    m_etiStream.setDevice(&m_etiFile);
    m_etiStream.setRealNumberNotation(QTextStream::FixedNotation);
    m_etiStream.setRealNumberPrecision(6);
    m_etiStream << "Cri\tEspece\tType\tIndice\tZone\tSite\tCommentaire\tMateriel\tConfidentiel\tDate\tAuteur\tEtiqueteur\n";
    m_logStream << "EnregEtiquettes - avant boucle - m_nbcris  = " << m_nbcris << endl;
    for (int i = 0 ; i < m_nbcris ; i++)
    {
        m_etiStream << lesEtiq[i]->e_numCri << '\t';
        for(int j=0;j<NBFIELDS;j++) m_etiStream << lesEtiq[i]->DataFields[j] << '\t';
        m_etiStream << endl;
    }
    //
    m_etiFile.close();
    //
    bouge_fichiers = true;
    saisie_a_sauver = false;
    m_ecraserFichier = false;
}

void Fenim::archive_crisel()
{
    for(int i=0;i<m_nbcris;i++) m_excrisel[i]=m_crisel[i];
}

bool Fenim::affiche_selections(bool specSelect)
{
    QString lsai[NBFIELDS];
    bool bsai[NBFIELDS],bsai2[NBFIELDS];
    for(int j=0;j<NBFIELDS;j++) {lsai[j]=""; bsai[j]=false; bsai2[j]=false;}
    m_lesCris = "";
    int nbcrisel = 0;
    if(specSelect) {specTagNumber=0; specTagSel=-1;}
    bool affichercri = bcCris->isChecked();
    bool afficherpm = bcPMaitres->isChecked();
    bool laffichercri = false;
    bool lafficherpm  = false;
    bool laffichersuppl  = false;
    if(affloupe)
    {
        laffichercri = floupe->bcCris->isChecked();
        lafficherpm  = floupe->bcPMaitres->isChecked();
        laffichersuppl  = floupe->bcSuppl->isChecked();
    }
    for(int i=0;i<m_nbcris;i++)
    {
        if(m_excrisel[i] != m_crisel[i])
        {
            afficher_un_cri(i,m_crisel[i],affichercri);
            afficher_un_point_maitre(i,m_crisel[i],afficherpm);
            if(affloupe)
            {
                floupe->afficher_un_cri(i,m_crisel[i],laffichercri);
                floupe->afficher_un_point_maitre(i,m_crisel[i],lafficherpm);
                floupe->afficher_une_crete_suppl(i,laffichersuppl);
                floupe->afficher_un_point_suppl(i,laffichersuppl);
            }
        }
        if(m_crisel[i])
        {
            nbcrisel++;
            if(nbcrisel == 1)
            {
                m_lesCris = QString::number(i+1);
                npremcrisel = i;
            }
            else m_lesCris += ","+QString::number(i+1);
            ndercrisel = i;
            if(specSelect) specTagList[specTagNumber++] = i;
            for(int jField=0;jField<NBFIELDS;jField++)
            {
                if(!lesEtiq[i]->DataFields[jField].isEmpty())
                {
                    if(bsai[jField]==false)
                    {
                        lsai[jField] = lesEtiq[i]->DataFields[jField];
                        pfc[jField]->affect(lsai[jField]);
                        bsai[jField]=true;
                    }
                    else
                    {
                        if(!pfc[jField]->Unic)
                            if(lsai[jField] != lesEtiq[i]->DataFields[jField])
                                bsai2[jField]=true;
                    }
                }

            }
        }
    }
    editCri->setText(m_lesCris);
    // ----------------------------------------------------------------
    // affichage en rouge ou vert des champs
    int coul=0;
    for(int jField=0;jField<NBFIELDS;jField++)
    {
        if(bsai[jField])
        {
            if(bsai2[jField]) coul=1; else coul=2;
        }
        else coul=0;
        pfc[jField]->colour(couleur[coul]);
    }
}

void Fenim::selectionneCri(int n,bool specSelect)
{
    archive_crisel();
    for(int j=0;j<m_nbcris;j++)
        if(j==n) m_crisel[j]=true;
        else m_crisel[j]=false;
    if(!specSelect) clearSpecTagsSelection();
    affiche_selections(false);
    if(affloupe)
    {
        int x = m_pointsMaitres[n].x()/(1+m_xmoitie);
        int y = _imaHeight - m_pointsMaitres[n].y()-1;
        floupe->lview->centerOn(x,y);
    }
}

void Fenim::crisuiv()
{
    if(ndercrisel<m_nbcris-1) selectionneCri(ndercrisel+1,false);
    else selectionneCri(ndercrisel,false);
}

void Fenim::criprec()
{
    if(npremcrisel>0) selectionneCri(npremcrisel-1,false);
    else selectionneCri(npremcrisel,false);
}

void Fenim::crifin()
{
    selectionneCri(m_nbcris-1,false);
}

void Fenim::crideb()
{
    selectionneCri(0,false);
}

void Fenim::specTagNext()
{
    if(specTagNumber>0)
    {
        if(specTagSel>-1 && specTagSel<specTagNumber-1)
        {
            specTagSel++;
            selectionneCri(specTagList[specTagSel],true);
        }
    }
}

void Fenim::specTagPrevious()
{
    if(specTagNumber>0)
    {
        if(specTagSel>0)
        {
            specTagSel--;
            selectionneCri(specTagList[specTagSel],true);
        }
    }
}

void Fenim::specTagLast()
{
    if(specTagNumber>0)
    {
        specTagSel=specTagNumber-1;
        selectionneCri(specTagList[specTagSel],true);
        enableMoreArrows();
    }
}

void Fenim::specTagFirst()
{
    if(specTagNumber>0)
    {
        specTagSel=0;
        selectionneCri(specTagList[specTagSel],true);
        enableMoreArrows();
    }
}


void Fenim::selectionneCri(int x,int y,bool isCTRL)
{
    float distmax = 3000;
    int ntrouve = -1;
    int xr=x*(1+m_xmoitie),yr=_imaHeight-y-1;
    for(int i=0;i<m_nbcris;i++)
    {
        float dist=pow(xr-m_pointsMaitres[i].x(),2)+pow(yr-m_pointsMaitres[i].y(),2);
        if(dist < distmax)
        {
            ntrouve = i;
            distmax = dist;
        }
    }
    if(ntrouve>=0)
    {
        archive_crisel();
        if(isCTRL)
        {
            m_crisel[ntrouve]=!m_crisel[ntrouve];
        }
        else
        {
            // cas général
            for(int j=0;j<m_nbcris;j++) if(j==ntrouve) m_crisel[j]=true;
            else m_crisel[j]=false;
        }
        clearSpecTagsSelection();
        affiche_selections(false);
    }
}

QString Fenim::calculebulle(int x,int y)
{
    QString retour("");
    float distmax = 2000;
    int ntrouve = -1;
    int xr=x,yr=_imaHeight-y-1;
    if(m_xmoitie) xr*=2;
    for(int i=0;i<m_nbcris;i++)
    {
        float dist=pow(xr-m_pointsMaitres[i].x(),2)+pow(yr-m_pointsMaitres[i].y(),2);
        if(dist < distmax)
        {
            ntrouve = i;
            distmax = dist;
            break;
        }
    }
    if(ntrouve>=0)
    {
        QString esp = lesEtiq[ntrouve]->DataFields[ESPECE];
        QString typ = lesEtiq[ntrouve]->DataFields[TYPE];
        QString ind = lesEtiq[ntrouve]->DataFields[INDICE];
        if(!esp.isEmpty() || !typ.isEmpty())
            retour=QString("Cri ")+QString::number(ntrouve+1)+" : "+esp+" - "+typ+ "    ("+ind+")";
        else
            retour=QString("Cri ")+QString::number(ntrouve+1)+" : cri sans Etiquette";
    }
    return(retour);
}

void Fenim::affbulle(QString sbulle)
{
        QToolTip::showText(QCursor::pos(),sbulle);
}


void Fenim::selectionneRectCri(int x1,int y1,int x2,int y2,bool isCTRL)
{
    y1 = _imaHeight-y1-1;
    y2 = _imaHeight-y2-1;
    if(m_xmoitie) {x1*=2; x2*=2;}
    archive_crisel();
    if(!isCTRL)
    for(int i=0;i<m_nbcris;i++) m_crisel[i]=false;
    for(int i=0;i<m_nbcris;i++)
    {
        int x = m_pointsMaitres[i].x();
        int y = m_pointsMaitres[i].y();
        if(x>=x1 && x<=x2 && y<=y1 && y>=y2)
        {
            if(isCTRL) m_crisel[i] = ! m_crisel[i];
            else m_crisel[i] = true;
        }
    }
    clearSpecTagsSelection();
    affiche_selections(false);
}

void Fenim::selectAllCalls()
{
    archive_crisel();
    for(int i=0;i<m_nbcris;i++) m_crisel[i]=true;
    clearSpecTagsSelection();
    affiche_selections(false);
}


void Fenim::selectSpecTags(const QString& codsel)
{
    labelTagSpec->setText(codsel);
    archive_crisel();
    for(int i=0;i<m_nbcris;i++) m_crisel[i]=false;
    for(int i=0;i<m_nbcris;i++)
        if(lesEtiq[i]->DataFields[ESPECE]==codsel) m_crisel[i] = true;
    affiche_selections(true);
    bFlecheDebutSpec->setEnabled(true);
    bFlecheFinSpec->setEnabled(true);
}

void Fenim::clearSpecTagsSelection()
{
    labelTagSpec->setText("");
    specTagNumber=0;
    bFlecheDebutSpec->setEnabled(false);
    bFlecheFinSpec->setEnabled(false);
    bFlecheDroiteSpec->setEnabled(false);
    bFlecheGaucheSpec->setEnabled(false);
}

void Fenim::enableMoreArrows()
{
    bFlecheDroiteSpec->setEnabled(true);
    bFlecheGaucheSpec->setEnabled(true);
}

void Fenim::videfenim()
{
    m_logStream << "debut videfenim" << endl;
    neplus=true;
    if(fenouv == true)
    {
        m_logStream << "v1" << endl;
        if(!m_casRetraitement)
        {
            m_logStream << "v2" << endl;
        delete fenima;
            m_logStream << "v3" << endl;
        delete scene;
            m_logStream << "v4" << endl;
        delete view;
            m_logStream << "v5" << endl;
        }
        delete m_crisel;
        m_logStream << "v6" << endl;
        delete m_excrisel;
        m_logStream << "v7" << endl;
        m_logStream << "m_nbcris = " << m_nbcris << endl;
        for(int i=0;i<m_nbcris;i++) delete lesEtiq[i];
        m_logStream << "v8" << endl;
        delete lesEtiq;
        m_logStream << "v9" << endl;
        if(affloupe) delete floupe;
        m_logStream << "v10" << endl;
        fenouv = false;
    }
}

void Fenim::fermeFenim()
{
    //videfenim();
    wfenim->close();
}

void Fenim::termine_session()
{
    if(m_casRetraitement) return;
    if(!casA) return;
    // cette méthode doit passer les fichiers du répertoire de travail dans la base
    // Bien distinguer les 2 cas :
    // cas a : création e fichier d'étiquette sur répertoire de travail
    // cas b : modif de fichier d'étiquettes directement dans la base
    // et bien éliminer le cas du retraitement !
    //
    //m_logStream << "ts1" << endl;

    bool version_a_ecrire = false;
    // 0) tester s'il y a eu enregistrement - sinon retour
    if(!bouge_fichiers) return;
    //
    // 1) positionnement sur le répertoire du jour de la base
    if(!baseJour.exists())
    {
        baseJour.mkdir(baseJour.path());
        version_a_ecrire = true;
    }
    if(!baseJourDat.exists()) baseJourDat.mkdir(baseJourDat.path());
    tgui->DayPath = baseJour.path();
    //
    if(!baseJourIma.exists()) baseJourIma.mkdir(baseJourIma.path());
    if(!baseJourEti.exists()) baseJourEti.mkdir(baseJourEti.path());
    if(!baseJourTxt.exists()) baseJourTxt.mkdir(baseJourTxt.path());
    //
    //m_logStream << "ts1" << endl;
    // 2) recopie des fichiers wav-dat-ima-eti dans ce répertoire
    QString nomFicWav = nomFic+".wav";
    QFile wavFile(repWav+"/"+nomFicWav);
    wavFile.copy(baseJour.path() + "/" + nomFic + ".wav");

    QString nomFicTxt = nomFic+".ta";
    QFile txtFile(repWav+"/txt/"+nomFicTxt);
    txtFile.copy(baseJourTxt.path() + "/" + nomFic + ".ta");

    m_cris2File.copy(baseJourDat.path() + "/" + nomFic + ".da2");
    //
    m_etiFile.copy(baseJourEti.path() + "/" + nomFic + ".eti");


    QFile imaFile(imaNom);
    imaFile.copy(baseJourIma.path() + "/" + nomFic + ".jpg");
    //
    //m_logStream << "ts2" << endl;
    // 3) effacement du répertoire source des fichiers : wav,dat,ima,eti
    wavFile.remove();
    imaFile.remove();
    txtFile.remove();
    m_crisFile.remove();
    m_etiFile.remove();
    // pour version contour
    m_cris2File.remove();
    //
    //m_logStream << "ts3" << endl;
    //
    // 4) écriture du fichier version.ini s'il n'existait pas
    if(version_a_ecrire) writeVersionRep();
    m_logStream << "ts_fin" << endl;
}


void FenetreFenim::closeEvent(QCloseEvent *event)
 {
    if(pfenim->saisie_a_sauver)
        if(QMessageBox::question(this, "Attention", "Quitter sans sauvegarder les saisies ?",
                                 QMessageBox::Yes|QMessageBox::No)
                == QMessageBox::No)
        {
            event->ignore();
            return;
        }
    //
    if(pfenim->casA) pfenim->termine_session();
    //

    pfenim->neplus=true;
    pfenim->videfenim();
    event->accept();

 }

void Fenim::creeloupe(int x,int y)
{
    if(affloupe) delete floupe;
    floupe = new Loupe(this,wfenim,x,y);
    floupe->afficherLoupe();
    affloupe = true;
}

float Fenim::getratio()
{
    // return((m_rh * m_factorY) / (m_rl * m_factorX * (1+m_xmoitie)));
    //return((m_rh * m_factorX * (1+m_xmoitie)) / (m_rl * m_factorY));
    return((m_rh * m_factorX * (1+m_xmoitie)) / (m_rl * m_factorY));
}

void Fenim::afficheratio()
{
    QString ratio;
    ratio.setNum(getratio(),'f',2);
    labelr->setText(QString("r=")+ratio);
}

float Fenim::getms(int x)
{
    return((m_factorX * x) * (1+m_xmoitie));
}

float Fenim::getx(float t)
{
    return(t/(m_factorX * (1+m_xmoitie)));
}

float Fenim::getkhz(int y)
{
    return(m_factorY * (_imaHeight - y -1));
}

float Fenim::gety(float f)
{
    return((float)_imaHeight - 1 - f/m_factorY );
}

/*
void MyQComboBox::mousePressEvent ( QMouseEvent *e )
{
    //if(count()<2 && arealim) insertItems(0,*ql);
}
*/

void MyQGraphicsScene::mouseMoveEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
    QPointF pos = mouseEvent->lastScenePos();
    int x = pos.x();
    int y = pos.y();
    if(iloupe)
    {
        if(x>=0 && y>=0 && x<pfenim->getImage()->width() && y<pfenim->getImage()->height())
        {
            this->views().size();
            QRectF r = ploupe->lview->mapToScene(ploupe->lview->viewport()->geometry()).boundingRect();
            ploupe->dercx = r.left()+r.width()/2;
            ploupe->dercy = r.top()+r.height()/2;
        }

    }
    // ly->setText(QString::number(y));

    // revoir le calcul de bulle en mode contour : n'afficher que lorsqu'on est au-dessus du contour !

    QString bulle = pfenim->calculebulle(x,y);
    int xr=x,yr=pfenim->_imaHeight-y-1;
    if(pfenim->m_xmoitie) xr*=2;
    if(iloupe) ploupe->affbulle(bulle);
    else pfenim->affbulle(bulle);
    if(y > pfenim->getImage()->height()-1) y=pfenim->getImage()->height()-1;
    QString ms,khz;
    ms.setNum(pfenim->getms(x),'f',0);
    khz.setNum(pfenim->getkhz(y),'f',2);

    pfenim->labelx->setText(ms+" ms");
    pfenim->labely->setText(khz+" khz");

    // --------------
    // ZZZ 08/07/2014
    if(iloupe)
    {

        bool pointdansforme = false,pdf2=false;
        float ener,ener2;
        int distmax = 10000;
        int nbcf = -1;
        int nbpt = 0;
        for(int i=0;i<pfenim->m_matrixContours.size();i++)
        {
            float dist=pow(xr-pfenim->m_pointsMaitres[i].x(),2)
                    +pow(yr-pfenim->m_pointsMaitres[i].y(),2);
            if(dist < distmax)
            {
                QVector<QPoint> unemat = pfenim->m_matrixContours.at(i);
                for(int j=0;j<unemat.size();j++)
                {
                    if(xr==unemat.at(j).x() && yr==unemat.at(j).y())
                    {
                        pointdansforme = true;
                        nbcf = i+1;
                        ener = pfenim->m_matrixEnergie[i][j];
                        nbpt++;
                        if(!pfenim->m_xmoitie || nbpt==2) break;
                    }
                    if(pfenim->m_xmoitie)
                    {
                        if(xr+1==unemat.at(j).x() && yr==unemat.at(j).y())
                        {
                            nbcf = i+1;
                            pdf2 = true;
                            ener2 = pfenim->m_matrixEnergie[i][j];
                            nbpt++;
                            if(nbpt==2) break;
                        }
                    }
                }
                //if(pointdansforme || pdf2) break;
            }
        }

        ploupe->labelx->setText(ms+" ms");
        ploupe->labely->setText(khz+" khz");
        QString affi="";
        //
        if(pointdansforme || pdf2)
        {
            affi = QString::number(nbcf)+": ";
            if(pointdansforme) affi += QString::number(ener);
            else affi += " - ";
            if(pfenim->m_xmoitie)
            {
                affi += QString(" ; ");
                if(pdf2) affi += QString::number(ener2); else affi+= " - ";
            }
        }
        ploupe->label_ener->setText(affi);
    }

    if(derx>0)
    {
        QPen qp = QPen(QColor(0,0,255),0);
        if(algri) this->removeItem((QGraphicsItem *)gri);
        gri = this->addRect(derx,dery,x-derx,y-dery,qp);
        algri = true;
    }
}


MyQGraphicsScene::MyQGraphicsScene(Fenim *pf,QMainWindow *parent,bool il):QGraphicsScene(parent)
{
    qmaitre = parent;
    iloupe=il;
    if(il)
    {
        ploupe = (Loupe *)qmaitre;
        pfenim = pf;
    }
    else
    {
        pfenim = pf;
    }

    derx=-1;
    dery=-1;
    algri = false;
}

void MyQGraphicsScene::mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
//    if(iloupe) return;
    QPointF pos = mouseEvent->lastScenePos();
    derx = pos.x();
    dery = pos.y();
}

void MyQGraphicsScene::mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
    mousePressEvent(mouseEvent);
    QPointF pos = mouseEvent->lastScenePos();
    int x = pos.x();
    int y = pos.y();
    if(iloupe)
    {
        ploupe->zoome(x,y);
    }
    else
    {
        pfenim->creeloupe(x,y);
    }
}

void MyQGraphicsScene::mouseReleaseEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
    if(derx < 0 || dery <0) return;
    if(algri) {this->removeItem((QGraphicsItem *)gri); algri=false;}
    QPointF pos = mouseEvent->lastScenePos();
    Qt::KeyboardModifiers keyMod = QApplication::keyboardModifiers ();
    bool isCTRL = keyMod.testFlag(Qt::ControlModifier);
    if(abs(pos.x()-derx)+abs(pos.y()-dery) < 5)
        pfenim->selectionneCri(pos.x(),pos.y(),isCTRL);
    else
    {
        // rectangle
        int x1,x2,y1,y2;
        if(derx>pos.x()) {x1=pos.x(); x2=derx;}
        else {x1=derx; x2=pos.x();}
        if(dery>pos.y()) {y1=pos.y(); y2=dery;}
        else {y1=dery; y2=pos.y();}
        pfenim->selectionneRectCri(x1,y1,x2,y2,isCTRL);
    }
    derx = -1; dery = -1;
}

void MyQGraphicsScene::keyPressEvent(QKeyEvent* e)
{
    Qt::KeyboardModifiers keyMod = QApplication::keyboardModifiers ();
    if(keyMod.testFlag(Qt::ControlModifier) && e->key() == 'A') pfenim->selectAllCalls();
}


MyQLineEdit::MyQLineEdit(QWidget *parent,Fenim *fen,QString cod):QLineEdit(parent)
{
    qmaitre = parent;
    pfenim = (Fenim *)fen;
    codeedit=cod;
    setFont(QFont("Arial",10,QFont::Normal));
}

EC_LineEdit::EC_LineEdit(QWidget *parent,Fenim *fen,QString cod):MyQLineEdit(parent,fen,cod)
{
    pfenim=fen;
    connect(this,SIGNAL(textEdited(const QString&)),this,SLOT(modifie_texte(const QString&)));
}

Simple_LineEdit::Simple_LineEdit(QWidget *parent,Fenim *fen,QString cod):MyQLineEdit(parent,fen,cod)
{
    pfenim=fen;
    connect(this,SIGNAL(textEdited(const QString&)),this,SLOT(modifie_texte(const QString&)));
}

void EC_LineEdit::setecp(EditCombo *pec)
{
    ecp=pec;
}

EC_ComboBoxEdit::EC_ComboBoxEdit(QWidget *parent,Fenim *fen,QString cod):MyQComboBox(parent,fen,cod)
{
    connect(this, SIGNAL(activated(const QString&)), this, SLOT(selectionne_code(const QString&)));
}

void EC_ComboBoxEdit::setecp(EditCombo *pec)
{
    ecp=pec;
}

EditCombo::EditCombo(QWidget *parent,Fenim *fen,QString cod,bool autaj)
{
    fenpar = fen->wfenim;
    ec_le=new EC_LineEdit(parent,fen,cod);
    ec_co=new EC_ComboBoxEdit(parent,fen,cod);
    ec_le->setecp(this);
    ec_co->setecp(this);
    // alimentation de la liste
    codefi = cod;
    autorise_ajout=autaj;
    liste_codes = new QStringList();
    QFile fichier;
    fichier.setFileName(cod+".txt");
    QTextStream textefi;
    if(fichier.open(QIODevice::ReadOnly)==true)
    {
        textefi.setDevice(&fichier);
        QString lte;
        bool trcod =  true;
        while(trcod)
        {
            if(textefi.atEnd()) trcod=false;
            else
            {
                lte = (textefi.readLine());
                if(lte.isNull()) trcod = false;
                else
                {
                    if(lte.isEmpty()) trcod = false;
                    else liste_codes->append(lte);
                }
            }
        }

    }
    realim_liste("");
}

void EditCombo::set_autor(bool autor)
{
    autorise_ajout = autor;
}


void EC_LineEdit::modifie_texte(const QString& s)
{
    ecp->realim_liste(s);
    ecp->ec_le->setStyleSheet(pfenim->couleur[4]);
}

void EC_ComboBoxEdit::selectionne_code(const QString& s)
{
    ecp->selectionne_code(s);
    ecp->ec_le->setStyleSheet(pfenim->couleur[4]);
}

void EC_LineEdit::keyPressEvent(QKeyEvent* e)
{
    if(e->key()==Qt::Key_Return)
    {

        QString sc = ecp->ec_co->currentText();
        QString se = text();
        if(se.length()>0 && sc.length()>0 && sc.contains(se))
            ecp->selectionne_code(sc);
    }
    MyQLineEdit::keyPressEvent(e);
}

void Simple_LineEdit::modifie_texte(const QString& s)
{
    this->setStyleSheet(pfenim->couleur[4]);
}


void MyQLineEdit::keyPressEvent(QKeyEvent* e)
{
    Qt::KeyboardModifiers keyMod = QApplication::keyboardModifiers ();
    if(keyMod.testFlag(Qt::ControlModifier) && e->key() == 'A') pfenim->selectAllCalls();

    if(e->key()==Qt::Key_Down)
    {
        if(codeedit=="cri") pfenim->pfc[ESPECE]->_le->setFocus();
        else
        {
            for(int j=0;j<NBFIELDS;j++)
            {
                int ft = pfenim->pfc[j]->_fieldType;
                if(codeedit==pfenim->pfc[j]->_fieldCode)
                {
                    if(ft==EC) pfenim->pfc[j]->_ec->ec_co->setFocus();
                    else
                    {
                        if(j<NBFIELDS)
                        {
                            int ftb = pfenim->pfc[j+1]->_fieldType;

                            if(ftb==EC || ftb==SLI || ftb== SLE)
                                pfenim->pfc[j+1]->_le->setFocus();
                        }
                    }
                }
            }
        }

    }
    if(e->key()==Qt::Key_Up)
    {
        if(codeedit=="especes") pfenim->editCri->setFocus();
        else
        {
            for(int j=0;j<NBFIELDS;j++)
            {
                if(codeedit==pfenim->pfc[j]->_fieldCode)
                {
                    if(j>0)
                    {
                        int ftb = pfenim->pfc[j-1]->_fieldType;
                        if(ftb==EC || ftb==SLI || ftb== SLE)
                            pfenim->pfc[j-1]->_le->setFocus();
                    }
                }
            }
        }
    }
    QLineEdit::keyPressEvent(e);
}

MyQLabel::MyQLabel(QWidget *parent):QLabel(parent)
{
    qmaitre = parent;
    setFont(QFont("Arial",10,QFont::Normal));
    setStyleSheet("background-color: #F8F8FE");
}

MyQComboBox::MyQComboBox(QWidget *parent,Fenim *fen,QString esp):QComboBox(parent)
{
    qmaitre = parent;
    pfenim = fen;
    codecombo=esp;
    setFont(QFont("Arial",10,QFont::Normal));
}

/*
void MyQComboBox:: focusInEvent(QFocusEvent *e)
{
}
*/

void EC_ComboBoxEdit::keyPressEvent(QKeyEvent* e)
{
    if(e->key()==Qt::Key_Backspace || e->key()==Qt::Key_Delete)
    {
        ecp->ec_le->setText("");
        ecp->realim_liste("");
        showPopup();
    }
    if(e->key()==Qt::Key_Down)
    {
        this->showPopup();
    }
    if(e->key()==Qt::Key_Return)
    {
        ecp->selectionne_code(currentText());
    }
    MyQComboBox::keyPressEvent(e);
}

void MyQComboBox::keyPressEvent(QKeyEvent* e)
{
    Qt::KeyboardModifiers keyMod = QApplication::keyboardModifiers ();
    if(keyMod.testFlag(Qt::ControlModifier) && e->key() == 'A') pfenim->selectAllCalls();
    QComboBox::keyPressEvent(e);
}


MyQPushButton::MyQPushButton(QWidget *parent):QPushButton(parent)
{
    qmaitre = parent;
    setFont(QFont("Arial",10,QFont::Normal));
}


MyQGraphicsView::MyQGraphicsView(QMainWindow *parent):QGraphicsView(parent)
{
    qmaitre = parent;
}

void MyQGraphicsView::resizeEvent(QResizeEvent *re)
{
}

bool Fenim::readVersionRep()
{
    QString cbase = repWav + _baseIniFile;
    m_verRepLog = 0;
    m_verRepUser = 0;
    m_modeRep = 1; // si pas de mode : on suppose répertoire resté en mode 1
    if(!QFile::exists(cbase)) return(false);
    QSettings settings(repWav + _baseIniFile, QSettings::IniFormat);
    settings.beginGroup("version");
    m_verRepLog = settings.value("log").toInt();
    m_verRepUser = settings.value("user").toInt();
    // pour version rep
    m_modeRep = settings.value("mode").toInt();
    settings.endGroup();
    return(true);
}

void Fenim::writeVersionRep()
{
    QSettings settings(baseJour.path() + _baseIniFile, QSettings::IniFormat);
    settings.beginGroup("version");
    settings.setValue("log", QVariant::fromValue(m_verLog));
    settings.setValue("user", QVariant::fromValue(m_verUser));
    // pour version rep
    settings.endGroup();
}



