#include "etiquette.h"
#include "loupe.h"
#include "fenim.h"
#include "detectreatment.h"

const QString _baseIniFile = "/version.ini";

FenimWindow::FenimWindow(Fenim *pf,QMainWindow *parent) : QMainWindow(parent)
{
    PFenim = pf;
    _alreadyScaled = false;
    _countResize = 0;
    GBoxInput = new QGroupBox(this);
}


void FenimWindow::ValidateLabel() { PFenim->ValidateLabel(); }
void FenimWindow::SaveLabels() { PFenim->SaveLabels(); }
void FenimWindow::CloseFenim() { PFenim->CloseFenim(); }
void FenimWindow::NextCall() { PFenim->NextCall(); }
void FenimWindow::PreviousCall() { PFenim->PreviousCall(); }
void FenimWindow::EndCall() { PFenim->EndCall(); }
void FenimWindow::StartCall() { PFenim->StartCall(); }
void FenimWindow::SelectIndex(int a) { PFenim->SelectIndex(a); }
void FenimWindow::UpdateIndex(const QString& s) { PFenim->UpdateIndex(s); }
void FenimWindow::ClickConfi() { PFenim->ClickConfi(); }
void FenimWindow::Zoom() { PFenim->Zoom(); }
void FenimWindow::UnZoom() { PFenim->UnZoom(); }
void FenimWindow::ActivateGrid(int a) { PFenim->ActivateGrid(a); }
void FenimWindow::ActivateMasterPoints() { PFenim->ActivateMasterPoints(); }
void FenimWindow::ActivateCrests() { PFenim->ActivateCrests(); }
void FenimWindow::SelectEditedCalls() { PFenim->SelectEditedCalls(); }
void FenimWindow::SelectSpecTags(const QString& codsel) { PFenim->SelectSpecTags(codsel); }
void FenimWindow::SpecTagNext() { PFenim->SpecTagNext(); }
void FenimWindow::SpecTagPrevious() { PFenim->SpecTagPrevious(); }
void FenimWindow::SpecTagLast() { PFenim->SpecTagLast(); }
void FenimWindow::SpecTagFirst() { PFenim->SpecTagFirst(); }

void FenimWindow::CreateConnections()
{
    connect(BClose,SIGNAL(clicked()),this,SLOT(close()));
    connect(BRightArrow,SIGNAL(clicked()),this,SLOT(NextCall()));
    connect(BLeftArrow,SIGNAL(clicked()),this,SLOT(PreviousCall()));
    connect(BEndArrow,SIGNAL(clicked()),this,SLOT(EndCall()));
    connect(BStartArrow,SIGNAL(clicked()),this,SLOT(StartCall()));
    connect(BSaveOneLabel,SIGNAL(clicked()),this,SLOT(ValidateLabel()));
    connect(BSaveLabels,  SIGNAL(clicked()),this,SLOT(SaveLabels()));
    connect(PFenim->PFC[INDICE]->SLid, SIGNAL(valueChanged(int)), this, SLOT(SelectIndex(int)));
    connect(PFenim->PFC[INDICE]->LE,SIGNAL(textEdited(const QString&)),this,SLOT(UpdateIndex(const QString&)));
    connect(PFenim->PFC[CONFIDENTIEL]->ChB, SIGNAL(stateChanged(int)), this, SLOT(ClickConfi()));
    connect(BZoom,SIGNAL(clicked()),this,SLOT(Zoom()));
    connect(BUnZoom,SIGNAL(clicked()),this,SLOT(UnZoom()));
    connect(BCGrid,SIGNAL(stateChanged(int)),this,SLOT(ActivateGrid(int)));
    connect(BCMasterPoints,SIGNAL(stateChanged(int)),this,SLOT(ActivateMasterPoints()));
    connect(BCCalls,SIGNAL(stateChanged(int)),this,SLOT(ActivateCrests()));
    connect(EditCall,SIGNAL(editingFinished()),this,SLOT(SelectEditedCalls()));
    connect(ChbTagSpec,SIGNAL(activated(const QString&)), this, SLOT(SelectSpecTags(const QString&)));
    connect(BRightArrowSpec,SIGNAL(clicked()),this,SLOT(SpecTagNext()));
    connect(BLeftArrowSpec,SIGNAL(clicked()),this,SLOT(SpecTagPrevious()));
    connect(BEndArrowSpec,SIGNAL(clicked()),this,SLOT(SpecTagLast()));
    connect(BStartArrowSpec,SIGNAL(clicked()),this,SLOT(SpecTagFirst()));
}

FieldClass::FieldClass(QWidget *parent,Fenim *pf,QString title,int fieldType,bool obl,bool unic,QString fieldCode,bool allowAdd,int smin,int smax,QString englishLabel)
{
  PFenim = pf;
  FieldType = fieldType;
  Obl = obl;
  Unic = unic;
  Title = title;
  FieldCode = fieldCode;
  EnglishLabel = englishLabel;
  TitleLabel = new MyQLabel(parent);
  TitleLabel->SetText(title);
  EnglishLabel = englishLabel;
  if(FieldType==EC)
  {
      ECo = new EditCombo(parent,pf,FieldCode,EnglishLabel,allowAdd);
  }
  if(FieldType==SLI)
  {
      SMin = smin;
      SMax = smax;
  }
  if(FieldType==SLE || FieldType==SLI || FieldType==EC)
  {
     if(FieldType==SLE || FieldType==SLI)
       {
       SLEd = new Simple_LineEdit(parent,pf,fieldCode);
       LE = SLEd;
       }
     if(FieldType==EC)
       {
       LE = ECo->EcLe;
       }
     LE->setEnabled(true);

  }
  //enum FIELDTYPE {SLE,EC,SLI,CHB};
  if(FieldType==SLI)
  {
    SLid = new QSlider(parent);
    SLid->setMinimum(smin);
    SLid->setMaximum(smax);
    SLid->setValue(smax);
    SLid->setOrientation(Qt::Horizontal);
  }
  if(FieldType==CHB)
  {
    ChB = new QCheckBox(QString(""),parent);
  }
  
}

void FieldClass::Affect(QString text)
{
    if(FieldType==CHB)
    {
        if(text=="true") ChB->setChecked(true);
        else ChB->setChecked(false);
    }
    else
    {
        LE->setText(text);
        if(FieldType==EC) ECo->redoList("");
        if(FieldType == SLI) PFenim->UpdateIndex(text);
    }
}

QString FieldClass::GetText()
{
    QString resu = "";
    if(FieldType==CHB)
    {
        if(ChB->isChecked()) resu = "true"; else resu = "";
    }
    else resu = LE->text();
    return(resu);
}

void FieldClass::Colour(QString colorText)
{
    if(FieldType==CHB) TitleLabel->setStyleSheet(colorText);
    else LE->setStyleSheet(colorText);
}

Fenim::Fenim(QMainWindow *parent,QString repwav,QString nomfi,QDir basejour,bool casa,bool casretr,int typeretraitement,QString suffixe,int vl,int vu,int mf)
{
    _windowCreated = false;
    ParentWindow = parent;
    PMainWindow=(TadaridaMainWindow *)ParentWindow;
    ACase = casa;
    _reprocessingCase = casretr;
    _reprocessingType = typeretraitement;
    LogVersion = vl;
    UserVersion = vu;
    FrequencyType = mf;
    QString logFilePath(QString("fenim")+suffixe+".log");
    _logFile.setFileName(logFilePath);
    _logFile.open(QIODevice::WriteOnly | QIODevice::Text);
    LogStream.setDevice(&_logFile);
    _wavFolder = repwav;
    ReadFolderVersion();
    _mainFileName = nomfi;
    _dayBaseFolder = basejour;
    _dayDatFolder = QDir(basejour.path()+"/dat");
    _dayImaFolder = QDir(basejour.path()+"/ima");
    _dayEtiFolder = QDir(basejour.path()+"/eti");
    _dayTxtFolder = QDir(basejour.path()+"/txt");
    ImageName = _mainFileName + ".jpg";
    ImageFullName = repwav + "/ima/" + ImageName;
    _datFile = repwav + "/dat/" + _mainFileName + ".dat";
    _da2FileName = repwav + "/dat/" + _mainFileName + ".da2";
    InputToSave = false;
    _filesUpdated = false;
    _overwriteFile = false;
    ShowLoupe = false;
    FactorX = -1;
    _nliv=0;_nlih=0;_nte=0;
    NoMore=false;
    _firstCallSelected = 0;
    _lastCallSelected = 0;
    SpecTagNumber = 0;
    FlagGoodCol = new char[SONOGRAM_WIDTH_MAX];
    FlagGoodColInitial = new char[SONOGRAM_WIDTH_MAX];
    EnergyAverageCol = new char[SONOGRAM_WIDTH_MAX];
}


Fenim::~Fenim()
{
    delete[] FlagGoodCol;
    delete[] FlagGoodColInitial;
    delete[] EnergyAverageCol;

    ClearFenim();
    if(_windowCreated) delete PFenimWindow;
}

void FenimWindow::CreateFenimWindow(bool modeSaisie)
{
    setWindowTitle("Tadarida  -  Labelling");
    PolText=QFont("Arial",10,QFont::Normal);
    PolTitle=QFont("Arial",10,QFont::Bold);
    _labelImage = new MyQLabel(this);
    gBoxGen = new QGroupBox(this);
    _gGBoxButtons = new QGroupBox(this);
    LabelTitleG2 = new QLabel(gBoxGen);
    _labelMess = new MyQLabel(GBoxInput);
    LabelFolder = new MyQLabel(gBoxGen);
    _labelNbCalls = new MyQLabel(gBoxGen);
    LabelNbLabs = new MyQLabel(gBoxGen);
    LabelTagSpec = new MyQLabel(gBoxGen);
    ChbTagSpec = new QComboBox(gBoxGen);
    _labelCalls = new MyQLabel(gBoxGen);
    EditCall   = new MyQLineEdit((QWidget *)gBoxGen,PFenim,"cry");
    BRightArrow = new MyQPushButton(GBoxInput);
    BLeftArrow = new MyQPushButton(GBoxInput);
    BStartArrow = new MyQPushButton(GBoxInput);
    BEndArrow = new MyQPushButton(GBoxInput);
    BSaveLabels = new MyQPushButton(GBoxInput);
    BClose = new MyQPushButton(GBoxInput);
    BZoom = new MyQPushButton(_gGBoxButtons);
    BUnZoom = new MyQPushButton(_gGBoxButtons);
    //fr bcGrille = new QCheckBox(QString("Grille"),gbox_boutons);
    BCGrid = new QCheckBox(QString("Grid"),_gGBoxButtons);
    //fr bcPMaitres = new QCheckBox(QString("Pts maitres"),gbox_boutons);
    BCMasterPoints = new QCheckBox(QString("Master pts"),_gGBoxButtons);
    BCCalls = new QCheckBox(QString("Lines"),_gGBoxButtons);
    LabelX = new QLabel(_gGBoxButtons);
    LabelY = new QLabel(_gGBoxButtons);
    LabelR = new QLabel(_gGBoxButtons);
    BSaveOneLabel = new MyQPushButton(GBoxInput);
    BSaveOneLabel->setText("Validate the label(s)");
    PrgSessionEnd = new QProgressBar(gBoxGen);
    BRightArrowSpec = new MyQPushButton(gBoxGen);
    BLeftArrowSpec = new MyQPushButton(gBoxGen);
    BStartArrowSpec = new MyQPushButton(gBoxGen);
    BEndArrowSpec = new MyQPushButton(gBoxGen);
    if(!modeSaisie)
    {
        BSaveOneLabel->setEnabled(false);
        BSaveLabels->setEnabled(false);
    }
    _labelImage->setText(PFenim->ImageName);
    QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy1.setHeightForWidth(GBoxInput->sizePolicy().hasHeightForWidth());
    GBoxInput->setSizePolicy(sizePolicy1);
    GBoxInput->setStyleSheet(QString::fromUtf8(""));
    int typ;
    for(int iField=0;iField<NBFIELDS;iField++)
    {
        typ=PFenim->PFC[iField]->FieldType;
        if(typ==EC || typ==SLI || typ==SLE)
        {
            PFenim->PFC[iField]->LE->setText("");
        }
        if(PFenim->PFC[iField]->FieldType==EC)
        {
            PFenim->PFC[iField]->ECo->EcCo->setFrame(true);
        }
    }
    _labelMess->SetText("");
    BRightArrow->setIcon(QIcon("fleche_droite.jpg"));
    BLeftArrow->setIcon(QIcon("fleche_gauche.jpg"));
    BEndArrow->setIcon(QIcon("fleche_fin.jpg"));
    BStartArrow->setIcon(QIcon("fleche_debut.jpg"));
    BSaveLabels->setText("Save the labels file");
    BSaveLabels->setFont(QFont("Arial",10,QFont::Bold));
    BClose->setText("Close");
    gBoxGen->setSizePolicy(sizePolicy1);
    gBoxGen->setStyleSheet(QString::fromUtf8(""));
    PrgSessionEnd->setMaximum(10000);
    PrgSessionEnd->setValue(0);
    PrgSessionEnd->setTextVisible(false);
    _labelNbCalls->SetText(QString::number(PFenim->CallsNumber)+" Sound events");
    if(PFenim->LabelsNumber>1) LabelNbLabs->SetText(QString::number(PFenim->LabelsNumber)+" labels");
    else LabelNbLabs->SetText(QString::number(PFenim->LabelsNumber)+" label");
    BStartArrowSpec->setIcon(QIcon("fleche_debut.jpg"));
    BEndArrowSpec->setIcon(QIcon("fleche_fin.jpg"));
    BRightArrowSpec->setIcon(QIcon("fleche_droite.jpg"));
    BLeftArrowSpec->setIcon(QIcon("fleche_gauche.jpg"));
    _labelCalls->setText("Sound event(s)");
    EditCall->setText("");
    BZoom->setText("Zoom +");
    BUnZoom->setText("Zoom -");
    BCGrid->setChecked(true);
    BCMasterPoints->setChecked(true);
    BCCalls->setChecked(false);
    PFenim->ShowRatio();
    GBoxInput->setFont(PolText);
    gBoxGen->setFont(PolText);
    _labelCalls->setFont(PolTitle);
    LabelTitleG2->setFont(PolTitle);
    EditCall->setFont(PolTitle);
    //
    Fenima = new QImage;
    Fenima->load(PFenim->ImageFullName);
    Scene = new MyQGraphicsScene(PFenim,this,false); // création de la scene
    View = new MyQGraphicsView(this); // création de la view
    View->setScene(Scene);  // ajout de la scene dans la view
    PixMap=(Scene->addPixmap(QPixmap::fromImage(*Fenima))); // ajout du pixmap dans la scene
    View->setMouseTracking(true);
    PFenim->LogStream  << "fin CreateFenimWindow" << endl;
}

void Fenim::CreatFenimWindow(bool inputMode)
{
    LogStream << "CreatFenimWindow : création de PFenimWindow" << endl;
    PFenimWindow = new FenimWindow(this,ParentWindow);
    PFC[ESPECE]       = new FieldClass((QWidget *)PFenimWindow->GBoxInput,this,"Species",EC,true,false,"especes",true,0,0,"species");
    PFC[TYPE]         = new FieldClass((QWidget *)PFenimWindow->GBoxInput,this,"Type",EC,true,false,"types",false,0,0,"type");
    PFC[INDICE]       = new FieldClass((QWidget *)PFenimWindow->GBoxInput,this,"Confidence",SLI,true,false,"indice",false,1,5,"index");
    PFC[ZONE]         = new FieldClass((QWidget *)PFenimWindow->GBoxInput,this,"Region",EC,false,true,"zone",true,0,0,"region");
    PFC[SITE]         = new FieldClass((QWidget *)PFenimWindow->GBoxInput,this,"Site",SLE,false,true,"site",false,0,0,"site");
    PFC[COMMENTAIRE]  = new FieldClass((QWidget *)PFenimWindow->GBoxInput,this,"Comment",false,SLE,false,"comment",false,0,0,"comment");
    PFC[MATERIEL]     = new FieldClass((QWidget *)PFenimWindow->GBoxInput,this,"Recorder",EC,false,true,"materiel",true,0,0,"recorder");
    PFC[CONFIDENTIEL] = new FieldClass((QWidget *)PFenimWindow->GBoxInput,this,"Confidential",CHB,false,true,"confidential",false,0,0,"confidential");
    PFC[DATENREG]     = new FieldClass((QWidget *)PFenimWindow->GBoxInput,this,"Date",SLE,false,true,"datenreg",false,0,0,"date");
    PFC[AUTEUR]       = new FieldClass((QWidget *)PFenimWindow->GBoxInput,this,"Author",EC,false,true,"auteur",true,0,0,"author");
    PFC[ETIQUETEUR]   = new FieldClass((QWidget *)PFenimWindow->GBoxInput,this,"Labeller",EC,false,false,"etiqueteur",true,0,0,"labeller");
    PFenimWindow->CreateFenimWindow(inputMode);
    InputColors[0]=QString("QLineEdit {color: blue;}");
    InputColors[1]=QString("QLineEdit {color: red;}");
    InputColors[2]=QString("QLineEdit {color: green;}");
    InputColors[3]=QString("QLineEdit {color: orange;}");
    InputColors[4]=QString("QLineEdit {color: black;}");
    _windowCreated = true;
}

void FenimWindow::ResizeFenimWindow(bool firstCall)
{
    _countResize ++;
    if(_countResize < 2) return;
    _fWmx = 10 ; _fWmy=12;
    _fWbh = 18;
    _fWl = width();
    _fWh= height();
    WinRect = geometry();
    int hv = ((_fWh - _fWmy*4 - _fWbh)*2)/3;
    int lv = _fWl - _fWmx*2;
    View->move(_fWmx,_fWmy);
    View->setFixedSize(lv,hv);
    WidthRatio =   (float)_fWh * (1+PFenim->XHalf) / (float)(PFenim->PMainWindow->Divrl);
    HeightRatio = ((float)hv * PFenim->ImageHeight) / ((float)Fenima->height() * 160);
    if(!_alreadyScaled)
    {
        _alreadyScaled = true;
        PFenim->LogStream << "_fWl = " << _fWl << " et __fWh=" << _fWh << endl;
        PFenim->LogStream << "_ WidthRatio = " <<  WidthRatio << " et HeightRatio=" <<  HeightRatio << endl;
        PFenim->LogStream << "PFenim->ImageHeight = " << PFenim->ImageHeight << " et Fenima->height()=" << Fenima->height() << endl;
        View->SCALE(WidthRatio,HeightRatio);
        PFenim->ShowGrid(true);
        PFenim->ShowCalls();
        PFenim->ShowMasterPoints();
        PFenim->SelectCall(0,false);
    }
    int yc = PFenim->ImageHeight + PFenim->ImageHeight - 121;
    View->centerOn(0,yc);
    int hb = (_fWh - _fWmy*4 - _fWbh)/3;
    int lb = (_fWl - _fWmx*3)/2;
    int lbn = _fWl - _fWmx*2;
    int hbg = (hb-_fWmy)/3;
    int hbs = hb-_fWmy-hbg;
    int espgb = _fWmx*2;
    int larbl = ((lbn-espgb*5)*2)/9;
    _labelImage->move(_fWmy,_fWmy/2-7);
    _labelImage->resize(500,15);
    GBoxInput->resize(lbn,hbs);
    GBoxInput->move(_fWmx,_fWh-hb+hbg);
    _gGBoxButtons->resize(_fWl - _fWmx*2,_fWbh+_fWmy);
    _gGBoxButtons->move(_fWmx,_fWh-hb-_fWmy*2-_fWmy/2-_fWbh);
    int hpz = 17;
    int hgz = 21;
    int margx = 15;
    int margx2 =10;
    int larlabel = (lb-margx*4)/4;
    int laredit = ((lb-margx*4)*3)/8;
    int larcombo = laredit;
    int espy = (hbs- hpz*7)/10;
    int col,lig,typ;

    for(int iField=0;iField<NBFIELDS;iField++)
    {
        if(iField<(NBFIELDS+1)/2) {col=0;lig=iField;}
        else {col=1; lig=iField-(NBFIELDS+1)/2;}
        typ=PFenim->PFC[iField]->FieldType;
        PFenim->PFC[iField]->TitleLabel->move(margx+col*(lbn/2),hpz*lig+espy*(1+lig));
        PFenim->PFC[iField]->TitleLabel->resize(larlabel,hpz);
        if(typ==EC || typ==SLI || typ==SLE)
        {
            PFenim->PFC[iField]->LE->move(margx*2+larlabel+col*(lbn/2),hpz*lig+espy*(1+lig));
            PFenim->PFC[iField]->LE->resize(laredit,hpz);
        }
        if(PFenim->PFC[iField]->FieldType==EC)
        {
            PFenim->PFC[iField]->ECo->EcCo->move(margx*3+larlabel+laredit+col*(lbn/2),hpz*lig+espy*(1+lig));
            PFenim->PFC[iField]->ECo->EcCo->resize(larcombo,hpz);
        }
        if(PFenim->PFC[iField]->FieldType==SLI)
        {
            PFenim->PFC[iField]->SLid->move(margx*3+larlabel+laredit+col*(lbn/2),hpz*lig+espy*(1+lig));
            PFenim->PFC[iField]->SLid->resize(larcombo,hpz);
        }
        if(PFenim->PFC[iField]->FieldType==CHB)
        {
            PFenim->PFC[iField]->ChB->move(margx*3+larlabel+laredit+col*(lbn/2),hpz*lig+espy*(1+lig));
            PFenim->PFC[iField]->ChB->resize(larcombo,hpz);
        }
    }
    _labelMess->move(margx+lbn/2,hpz*5+espy*6);
    _labelMess->resize(larlabel+laredit+larcombo+margx*2,hpz);
    int lpb = hgz;
    BRightArrow->move(margx,hpz*6+espy*7);
    BRightArrow->resize(lpb,hgz);
    BLeftArrow->move(margx*2+lpb,hpz*6+espy*7);
    BLeftArrow->resize(lpb,hgz);
    BEndArrow->move(margx*3+lpb*2,hpz*6+espy*7);
    BEndArrow->resize(lpb,hgz);
    BStartArrow->move(margx*4+lpb*3,hpz*6+espy*7);
    BStartArrow->resize(lpb,hgz);
    BSaveLabels->move(espgb*3+larbl*2,hpz*6+espy*7);
    BSaveLabels->resize((larbl*3)/2,hgz);
    BSaveOneLabel->move(espgb*2+larbl,hpz*6+espy*7);
    BSaveOneLabel->resize(larbl,hgz);
    BClose->move(espgb*4+(larbl*7)/2,hpz*6+espy*7);
    BClose->resize(larbl,hgz);
    int espy2 = (hbg- hpz*3)/4;
    gBoxGen->resize(lbn,hbg);
    gBoxGen->move(_fWmx,_fWh-hb-_fWmy);
    LabelTitleG2->move(margx,espy2);
    LabelTitleG2->resize((lbn-margx*3)/2,hpz);
    LabelFolder->move(margx+lbn/2,espy2);
    LabelFolder->resize((lbn-margx*3)/2,hpz);
    PrgSessionEnd->move(margx+lbn/2,espy2);
    PrgSessionEnd->resize(0,0);
    _labelNbCalls->move(margx,hpz+espy2*2);
    _labelNbCalls->resize((lbn-margx*3)/2,hpz);
    LabelNbLabs->move(margx+lbn/2,hpz+espy2*2);
    LabelNbLabs->resize((lbn-margx*3)/2,hpz);
    int mxl = 5;
    int wts = (lbn-margx*3)/2-mxl*3;
    int lw = (wts*2)/9;
    int gw = wts/3;
    int db = gw/4;
    int xdep = margx+lbn/2;
    int yl= hpz+espy2*2;
    ChbTagSpec->move(xdep+lw+mxl,yl);
    ChbTagSpec->resize(lw,hpz);
    LabelTagSpec->move(xdep+lw*2+mxl*2,yl);
    LabelTagSpec->resize(lw,hpz);
    BStartArrowSpec->move(xdep+lw*3+mxl*3,yl);
    BStartArrowSpec->resize(lpb,hgz);
    BEndArrowSpec->move(xdep+lw*3+mxl*3+db,yl);
    BEndArrowSpec->resize(lpb,hpz);
    BRightArrowSpec->move(xdep+lw*3+mxl*3+db*2,yl);
    BRightArrowSpec->resize(lpb,hpz);
    BLeftArrowSpec->move(xdep+lw*3+mxl*3+db*3,yl);
    BLeftArrowSpec->resize(lpb,hpz);
    _labelCalls->move(margx,hpz*2+espy2*3);
    _labelCalls->resize((larlabel*2)/3,hgz);
    EditCall->move(margx*2+(larlabel*2)/3,hpz*2+espy2*3);
    EditCall->resize(lbn-margx*3-((larlabel*2)/3),hgz);
    int lzb = _gGBoxButtons->width();
    int larpw = (lzb-margx2*13)/12;
    BZoom->move(margx2,_fWmy/4);
    BZoom->resize(larpw,_fWbh);
    BUnZoom->move(margx2*2+larpw,_fWmy/4);
    BUnZoom->resize(larpw,_fWbh);
    BCGrid->move(margx2*3+larpw*2,_fWmy/4);
    BCGrid->resize(larpw,_fWbh);
    BCMasterPoints->move(margx2*4+larpw*3,_fWmy/4);
    BCMasterPoints->resize(larpw,_fWbh);
    BCCalls->move(margx2*5+larpw*4,_fWmy/4);
    BCCalls->resize(larpw,_fWbh);
    LabelX->move(lzb - margx2 * 2 - larpw * 2,_fWmy/4);
    LabelY->move(lzb - margx2 - larpw,_fWmy/4);
    LabelR->move(lzb - margx2  * 3 - larpw*3,_fWmy/4);
    LabelX->resize(larpw,_fWbh);
    LabelY->resize(larpw,_fWbh);
    LabelR->resize(larpw,_fWbh);
}

void Fenim::initInputs()
{
    for(int i=0;i<NBFIELDS;i++)
    {
        if(PFC[i]->Unic) FileFields[i]="";
    }
    for(int i=0;i<CallsNumber;i++)
    {
        if(!EtiquetteArray[i]->DataFields[ESPECE].isEmpty())
        {
            for(int j=0;j<NBFIELDS;j++)
            {
                if(PFC[j]->Unic)
                {
                    FileFields[j] = EtiquetteArray[i]->DataFields[j];
                    PMainWindow->LastFields[j] = FileFields[j];
                }
            }
            break;
        }
    }
    for(int i=0;i<NBFIELDS;i++)
    {
        PFC[i]->Affect(PMainWindow->LastFields[i]);
        PFC[i]->Colour(InputColors[0]);
    }
}

QRect Fenim::GetWindowRect()
{
    return(PFenimWindow->WinRect);
}

MyQGraphicsView* Fenim::GetView()
{
    return(PFenimWindow->View);
}

QImage * Fenim::GetImage()
{
    return(PFenimWindow->Fenima);
}

void Fenim::SelectEditedCalls()
{
    if(NoMore) return;
    QString cs=PFenimWindow->EditCall->text();
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
            if(n<1 || n>CallsNumber) ok=false;
        }
    }
    if(ok==false)
    {
        PFenimWindow->EditCall->setText(_callsString);
    }
    else
    {
        if(cs!=_callsString) SelectCall(n-1,false);
    }
}

void EditCombo::redoList(const QString& enteredText)
{
    EcCo->clear();
    if(enteredText.isEmpty()) EcCo->insertItems(0,*ListCodes);
    else
    {

        QStringList liste2;
        for(int i=0;i<ListCodes->size();i++)
        {
            if(ListCodes->at(i).contains(enteredText))
                liste2.append(ListCodes->at(i));
        }
        if(liste2.size()<1) EcCo->insertItems(0,*ListCodes);
        else EcCo->insertItems(0,liste2);
    }
}

void EditCombo::SelectCode()
{
    if(EcCo->count()>0) SelectCode(EcCo->itemText(0));
}

void EditCombo::SelectCode(const QString& selectedText)
{
    if(EcCo->count()<1) EcCo->insertItems(0,*ListCodes);
    EcLe->setText(selectedText);
    EcLe->setFocus(); // en attendant d'avoir le paramètre pour passer au champ suivant ZZZ
}


bool EditCombo::ConfirmAdd(QString& s)
{
    if(QMessageBox::question((QWidget *)PFenimWindow, (const QString &)QString("Question"),
                             //fr (const QString &)QString("Code ")+s+QString(" absent de la table : accepter ce code ?"),
                             (const QString &)QString("Code ")+s+QString(" code missing from the table : accept it ?"),
                             QMessageBox::Yes|QMessageBox::No)
            == QMessageBox::No) return(false);
    ListCodes->insert(0,s);
    ListCodes->sort();
    redoList(s);
    QFile fichier;
    QTextStream textefi;
    fichier.setFileName(CodeFi+".txt");
    if(fichier.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        textefi.setDevice(&fichier);
        for(int i=0;i<ListCodes->size();i++) textefi << ListCodes->at(i) << endl;
        fichier.close();
    }
    return(true);
}

void Fenim::SelectIndex(int n)
{
    PFC[INDICE]->LE->setText(QString::number(n));
    PFC[INDICE]->LE->setStyleSheet(InputColors[4]);
}

void Fenim::UpdateIndex(const QString& selectedIndex)
{
    bool convint;
    int nind = selectedIndex.toInt(&convint);
    if(convint)
    {
        if(nind>0 && nind<6)
            PFC[INDICE]->SLid->setValue(nind);
    }
}

void Fenim::ClickConfi()
{
    PFC[CONFIDENTIEL]->Colour(InputColors[4]);
}


bool EditCombo::Control(QString &s,QString &errorMessage,bool obl)
{
    bool bon = true; errorMessage="";
    s=EcLe->text();
    if(s.isNull() || s.isEmpty())
    {
        if(obl==true)
        {
            bon = false;
            errorMessage="You must entry code " + EnglishCode + " !";
        }
        else
        {
            bon=true;
        }
    }
    else
    {
        if(!ListCodes->contains(s))
        {
            if(!AllowAdd)
            {
                bon = false;
                errorMessage = "Unauthorized code !";
            }
            else
            {
                if(!ConfirmAdd(s))
                    bon = false;
            }
        }
    }
    if(!bon)
    {
        EcLe->clear();
        redoList("");
        EcLe->setFocus();
    }
    return(bon);
}

void Fenim::ValidateLabel()
{
    QString s,mess_err="";
    for(int i=0;i<NBFIELDS;i++)
    {
        s=PFC[i]->GetText();
        if(s.isEmpty() && PFC[i]->Obl==true)
        {
            //fr QMessageBox::warning(wfenim,"Saisie obligatoire",mess_err, QMessageBox::Ok);
            QMessageBox::warning(PFenimWindow,"Required entry",mess_err, QMessageBox::Ok);
            PFC[i]->LE->setFocus();
            return;
        }
        if(PFC[i]->FieldType==EC)
        {
            if(!PFC[i]->ECo->Control(s,mess_err,true))
            {
                QMessageBox::warning(PFenimWindow,"Incorrect entry",mess_err, QMessageBox::Ok);
                PFC[i]->ECo->EcLe->setFocus();
                return;
            }
        }
        if(PFC[i]->FieldType==SLI && !s.isEmpty())
        {
            bool convint;
            int nind = PFC[i]->GetText().toInt(&convint);
            if(convint)
            {
                if(nind<PFC[i]->SMin || nind>PFC[i]->SMax) convint=false;
            }

            if(!convint)
            {
                //fr QMessageBox::warning(wfenim,"Incorrect entry","Saisir un nombre entre 1 et 5 !", QMessageBox::Ok);
                QMessageBox::warning(PFenimWindow,"Incorrect entry","Enter a number between 1 and 5 !", QMessageBox::Ok);
                PFC[i]->LE->setFocus();
                return;
            }
        }
    }
    QString dae= PFC[DATENREG]->GetText();
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
            //j QMessageBox::warning(wfenim,"Saisir une date jj/mm/aaaa",mess_err, QMessageBox::Ok);
            QMessageBox::warning(PFenimWindow,"Enter a date dd/mm/yyyy",mess_err, QMessageBox::Ok);
            PFC[DATENREG]->LE->setFocus();
            return;
        }
    }
    // -----
    int premi=-1;
    for(int i=0;i<CallsNumber;i++)
    {
        if(SelectedCalls[i])
        {
            if(premi<0) premi=i;
            if(EtiquetteArray[i]->DataFields[ESPECE].isEmpty()) LabelsNumber++;
            for(int j=0;j<NBFIELDS;j++)
                EtiquetteArray[i]->DataFields[j]= this->PFC[j]->GetText();
            updateTagNSpec(i);
        }
    }
    if(!_reprocessingCase)
    {
        inputsSave(EtiquetteArray[premi]);
        greenPaint();
        treatUniqueFields();
        TadaridaMainWindow *tg=(TadaridaMainWindow *)ParentWindow;
        tg->TextsToSave = true;
        showNbLabels();
        updateChbTagSpec();
    }
    InputToSave = true; // on peut faire mieux
}

void Fenim::inputsSave(Etiquette *pLabel)
{
    for(int i=0;i<NBFIELDS;i++)
        PMainWindow->LastFields[i] = pLabel->DataFields[i];
}

void Fenim::treatUniqueFields()
{
    bool toExtend = false;

    for(int jField=0;jField<NBFIELDS;jField++)
    {
        if(PFC[jField]->Unic)
        {
            if(FileFields[jField] != PMainWindow->LastFields[jField])
            {
                FileFields[jField] = PMainWindow->LastFields[jField];
                toExtend = true;
            }
        }
    }
    if(toExtend)
    {
        for(int i=0;i<CallsNumber;i++)
        {
            if(!SelectedCalls[i] && !EtiquetteArray[i]->DataFields[ESPECE].isEmpty())
            {
                for(int jField=0;jField<NBFIELDS;jField++)
                    if(PFC[jField]->Unic) EtiquetteArray[i]->DataFields[jField] = FileFields[jField];
            }
        }
    }
}

void Fenim::greenPaint()
{
    for(int j=0;j<NBFIELDS;j++) PFC[j]->Colour(InputColors[2]);
}

bool Fenim::ShowFenim(bool inputMode)
{
    if(FolderLogVersion <LogVersion || FolderUserVersion < UserVersion)
    {
        //QMessageBox::warning(fparent,"Fichier inaccessible","Version en retard : retraiter d'abord le dossier !", QMessageBox::Ok);
        QMessageBox::warning(ParentWindow,"File is unreacheable","Version is late : reprocess first the folder !", QMessageBox::Ok);
        return(false);
    }
    initCalls();
    if(loadCallsMatrices(_da2FileName)==false)
    {
        QMessageBox::warning(ParentWindow,"File is unreacheable","Reprocess first the folder", QMessageBox::Ok);
        return(false);
    }
    _windowOpen = true;
    CreatFenimWindow(inputMode);
    if(loadLabels()==false)
    {
        if(_windowCreated) PFenimWindow->close();
        return(true);
    }

    showNbLabels();
    initInputs();
    initMasterPoints();
    initLines();
    PFenimWindow->CreateConnections();
    PFenimWindow->showMaximized();
    PFenimWindow->activateWindow();
    PFenimWindow->raise();
    return(true);
}

bool Fenim::LoadCallsLabels()
{
    if(!_reprocessingCase) initCalls();
    if(loadCallsMatrices(_da2FileName)==false)
    {
        _windowOpen = false;
        return(false);
    }
    else _windowOpen = true;
    if(loadLabels()==false) return(false);
    return(true);
}

void Fenim::Zoom()
{
    ZoomF(1.414f);
}
void Fenim::UnZoom()
{
    ZoomF(0.707f);
}
void Fenim::ZoomF(float iz)
{
    PFenimWindow->View->SCALE(iz,1);
    PFenimWindow->WidthRatio *= iz;
    ShowGrid(PFenimWindow->BCGrid->isChecked());
    ShowMasterPoints();
    ShowRatio();
}

void Fenim::initCalls()
{
    for(int i=0;i<CRESTSNUMBER;i++)
    {
        CallsMatrix[i].clear();
        for(int j=0;j<NSUPPL;j++) AddPointsVector[i][j].clear();
    }
    MasterPointsVector.clear();
    ContoursMatrices.clear();

}

void Fenim::ShowCalls()
{
    bool affichercri = PFenimWindow->BCCalls->isChecked();
    for(int i=0;i<CallsNumber;i++)
    {
        ShowOneCall(i,SelectedCalls[i],affichercri);
    }
}

void Fenim::ShowMasterPoints()
{
    bool afficherpm = PFenimWindow->BCMasterPoints->isChecked();
    for(int i=0;i<CallsNumber;i++) ShowOneMasterPoint(i,SelectedCalls[i],afficherpm);
}

void Fenim::ActivateGrid(int state)
{
    ShowGrid(state==Qt::Checked);
}

void Fenim::ActivateMasterPoints()
{
    ShowMasterPoints();
}

void Fenim::ActivateCrests()
{
    ShowCalls();
}

void Fenim::ShowGrid(bool toShow)
{
    if(_nliv>0) for(int i=0;i<_nliv;i++) delete PFenimWindow->GLiv[i];
    if(_nlih>0) for(int i=0;i<_nlih;i++) delete PFenimWindow->GLih[i];
    if(_nte>0) for(int i=0;i<_nte;i++) delete PFenimWindow->GTe[i];
    _nliv=0; _nlih=0; _nte=0;
    if(!toShow) return;

    float invsx = 1.0f/PFenimWindow->WidthRatio;
    float invsy = 1.0f/PFenimWindow->HeightRatio;
    QFont qf("Arial",8);
    int xmax=PFenimWindow->Fenima->width();
    float tmax = Getms(xmax-1);
    float igt=0;
    float incrt = 500;
    if(PFenimWindow->WidthRatio>0.25) incrt = 100;

    QPen qp = QPen(QColor(128,128,128),0);
    while(igt<tmax && _nliv < 500 && _nte<250)
    {
        float x=GetX(igt);
        PFenimWindow->GLiv[_nliv] = PFenimWindow->Scene->addLine(x,0,x,ImageHeight-1,qp);
        if((_nliv & 1)==0 && _nliv>0)
        {
            int nigt =(int)igt; int ns = nigt/1000;
            QString affi;
            if(nigt == ns * 1000) affi = QString::number(ns)+" sec";
            else affi = QString::number(nigt)+" ms";
            PFenimWindow->GTe[_nte] = PFenimWindow->Scene->addSimpleText(affi,qf);
            PFenimWindow->GTe[_nte]->setPos(x+invsx*2,ImageHeight-((float)30/PFenimWindow->HeightRatio));
            //gte[nte]->scale(invsx,invsy);
            PFenimWindow->GTe[_nte]->SCALE(invsx,invsy);
            _nte++;
        }
        _nliv++;
        igt += incrt;
    }
    float fmax = GetkHz(0);
    float igf=0;
    float incrf = 10;
    while(igf<fmax && _nlih < 500)
    {
        float y=GetY(igf);
        PFenimWindow->GLih[_nlih] = PFenimWindow->Scene->addLine(0,y,xmax-1,y,qp);
        if((_nlih & 1)==0 && _nlih>0)
        {
            PFenimWindow->GTe[_nte] = PFenimWindow->Scene->addSimpleText(QString::number(igf)+" kHz",qf);
            PFenimWindow->GTe[_nte]->setPos(1+invsx,y-8+PFenimWindow->HeightRatio);
            PFenimWindow->GTe[_nte]->SCALE(invsx,invsy);
            _nte++;
        }
        _nlih++; igf += incrf;
    }
}

void Fenim::ShowOneCall(int callNumber,bool selectedCall,bool showCall)
{
    if(!(callNumber<MAXCRI)) return;
    bool etiquette = !EtiquetteArray[callNumber]->DataFields[ESPECE].isEmpty();
    QPen qp = QPen(QColor(
                       (255-80*etiquette)*(!selectedCall)+20*selectedCall*etiquette,
                       (255-100*etiquette)*(!selectedCall),
                       255*(!selectedCall)+80*selectedCall*etiquette),
                       0);
    if(!showCall)
    {
        if(_ilc[callNumber])
        {
            delete PFenimWindow->GPlt[callNumber];
            _ilc[callNumber]=false;
        }
    }
    else
    {
        if(!_ilc[callNumber])
        {
            QPolygonF polygone;
            for(int j=0;j<CallsMatrix[0][callNumber].size();j++)
            {
                int x=CallsMatrix[0][callNumber][j].x()/(1+XHalf);
                int y=ImageHeight-CallsMatrix[0][callNumber][j].y()-1;
                polygone << QPointF(x,y);
            }
            QPainterPath path = QPainterPath();
            path.addPolygon(polygone);
            PFenimWindow->GPlt[callNumber] = new QGraphicsPathItem(path);
            PFenimWindow->GPlt[callNumber]->setPen(qp);
            PFenimWindow->Scene->addItem(PFenimWindow->GPlt[callNumber]);
            _ilc[callNumber]=true;
        }
        else
        {
            int counou=(int)etiquette*2+(int)selectedCall;
            if(counou!=_callColour[callNumber])
            {
                PFenimWindow->GPlt[callNumber]->setPen(qp);
                _callColour[callNumber]=counou;
            }
        }
    }
}

void Fenim::ShowOneMasterPoint(int callNumber,bool selectedCall,bool showMasterPoint)
{
    if(!(callNumber<MAXCRI)) return;
    int x = MasterPointsVector[callNumber].x()/(1+XHalf);
    int y = ImageHeight - MasterPointsVector[callNumber].y()-1;

    bool etiquette = !EtiquetteArray[callNumber]->DataFields[ESPECE].isEmpty();
    int nspec= (EtiquetteArray[callNumber]->SpecNumber);
    int rouge,vert,bleu,ajdec;
    bleu=0;
    ajdec=0;
    if(nspec>=8) nspec = nspec & 7;
    if(nspec>4) {nspec-=4; ajdec=1;}
    if(selectedCall)
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
    QPen qpm = QPen(QColor(rouge,vert,bleu),0);
    QBrush qb = QBrush(QColor(rouge,vert,bleu),Qt::SolidPattern);
    if(_ipmc[callNumber]==true)
    {
        delete PFenimWindow->GEpm[callNumber];
        _ipmc[callNumber]=false;
    }
    if(showMasterPoint)
    {
        if(_ipmc[callNumber]==false)
        {
            // float w=3.0f/m_rl+m_rl/16;
            // float h=2.0f/m_rh+m_rh/16;
            float w=6.0f/PFenimWindow->WidthRatio;
            float h=6.0f/PFenimWindow->HeightRatio;
            PFenimWindow->GEpm[callNumber]=PFenimWindow->Scene->addEllipse(x-w/2,y-h/2,w,h,qpm,qb);
            _ipmc[callNumber]=true;
        }
        else PFenimWindow->GEpm[callNumber]->setPen(qpm);
    }
}


bool Fenim::loadCallsMatrices(QString da2File)
{
    _da2File.setFileName(da2File);
    if(_da2File.open(QIODevice::ReadOnly)==false)
    {
        LogStream << "da2 file is missing : " << da2File << endl;
        if(!_reprocessingCase)
        QMessageBox::warning(PFenimWindow, "Error", "da2 file is missing", QMessageBox::Ok);
        return(false);
    }
    _da2Stream.setDevice(&_da2File);
    int numver=0,numveruser=0;
    _da2Stream >> numver;
    if(numver>3) _da2Stream >> numveruser;
    if(numver<11)
    {
        QMessageBox::warning(PFenimWindow, "Error", "da2 file is incompatible with software version", QMessageBox::Ok);
        return(false);
    }
    LogStream << "numver,m_verLog=" << numver << "," << LogVersion << endl;
    LogStream << "numveruser,m_verUser=" << numveruser << "," << UserVersion << endl;
    if(!_reprocessingCase)
    {
        if(numver < LogVersion || numveruser < UserVersion)
        {
            LogStream << "da2 file version is late" << endl;
            LogStream << "numveruser,m_verUser=" << numveruser << "," << UserVersion << endl;
            _da2File.close();
            return(false);
        }
    }
    _da2Stream >> CallsNumber;
    LogStream << " nb calls = " << CallsNumber << endl;
    _da2Stream >> ImageHeight;
    LogStream << " _imaHeight = " << ImageHeight << endl;
    if(numver > 1)
    {
        _da2Stream >> XHalf;
        LogStream << "m_xmoitie = " << XHalf << endl;
    }

    if(numver>2)
    {
        _da2Stream >> FactorX;
        _da2Stream >> FactorY;
        LogStream << "m_factory=" << FactorY << endl;
        LogStream << "m_factorx=" << FactorX << endl;
    }
    NumVer = numver;
    PtE = 10;
    if(numver>19)
    {
        _da2Stream >> PtE;
    }
    else
    {
        NumtE = FactorY * ImageHeight * 2000;
    }
    // -------------------
    int xpointm,ypointm;
    if(CallsNumber >0 && CallsNumber < MAXCRI)
    for(int i=0;i<CallsNumber;i++)
    {
        _da2Stream >> xpointm;
        _da2Stream >> ypointm;
        QPoint p;
        p.setX(xpointm);
        p.setY(ypointm);
        MasterPointsVector.push_back(p);
        int longCri;
        _da2Stream >> longCri;
        _contoursVector.clear();
        _energyVector.clear();
        int x,y;
        float e;
        for(int j=0;j<longCri;j++)
        {
            _da2Stream >> x >> y >> e;
            QPoint p;
            p.setX(x);
            p.setY(y);
            _contoursVector.push_back(p);
			
            _energyVector.push_back(e);
        }
        ContoursMatrices.push_back(_contoursVector);
        EnergyMatrix.push_back(_energyVector);
        int crestLength;
        for(int jcrete=0;jcrete<CRESTSNUMBER;jcrete++)
        {
            _callsVector.clear();
            _da2Stream >> crestLength;
            for(int j=0;j<crestLength;j++)
            {
                _da2Stream >> x >> y;
                QPoint p;
                p.setX(x);
                p.setY(y);
                _callsVector.push_back(p);
            }
            CallsMatrix[jcrete].push_back(_callsVector);

        }
        for(int jcrete=0;jcrete<CRESTSNUMBER;jcrete++)
            for(int k=0;k<NSUPPL;k++)
            {
                _da2Stream >> x >> y;
                QPoint p;
                p.setX(x);
                p.setY(y);
                AddPointsVector[jcrete][k].push_back(p);
            }
    }
    if(numver>20)
    {
        _da2Stream >> SonogramWidth;
        int f0;
        _da2Stream >> f0;
        WithSilence = (bool)f0;
        if(WithSilence)
        {
            LogStream << "withSilence=true" << endl;
            for(int j=0;j<SonogramWidth;j++)
            {
                qint8 f1,f2,f3;
                _da2Stream >> f1 >> f2 >> f3;
                FlagGoodCol[j] = f1;
                FlagGoodColInitial[j] = f2;
                EnergyAverageCol[j] = f3;
            }
        }
        else LogStream << "withSilence=false" << endl;
    }
    else WithSilence = false;
    _da2File.close();
    return(true);
}

int Fenim::MatchLabels(Fenim * fenim1,bool initial,QString recupVersion,int *cpma)
{
    // apriori : rien à changer pour la version contour !
    LogStream << "MatchLabels - m_nbcris = " << CallsNumber << endl;
    LogStream << "master points Number = " << MasterPointsVector.size() << endl;
    LogStream << "rematcheEtiquettes - fenim1 master points Number = " << fenim1->MasterPointsVector.size() << endl;
    // ajoutr le nombre d'étiquettes !
    int nrecup=0;
    QStringList listSpecrecup;
    for(int i=0;i<CallsNumber;i++)
    {
        if(!initial) if(!EtiquetteArray[i]->DataFields[ESPECE].isEmpty()) continue;
        QPoint p = this->MasterPointsVector.at(i);
        int x=p.x(),y=p.y();
        for(int j=0;j<fenim1->MasterPointsVector.size();j++)
        {
            QPoint p1 = fenim1->MasterPointsVector.at(j);
            int x1=p1.x(),y1=p1.y();
            *cpma += (x1+y1);
            int d=(x-x1)*(x-x1)+(y-y1)*(y-y1);
            if(d<20)
            {
                QString esp = fenim1->EtiquetteArray[j]->DataFields[ESPECE];
                if(!esp.isEmpty())
                {
                    LabelsNumber++;
                    EtiquetteArray[i]->EtiquetteClone(fenim1->EtiquetteArray[j]);
                    if(!initial)
                        if(!_listTaggedSpecies.contains(esp))
                            if(!listSpecrecup.contains(esp))
                                listSpecrecup.append(esp);
                    nrecup++;
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
            PMainWindow->RetreatText << "Recovered species " << esp << " in the file " << _mainFileName
                          << " of version " << recupVersion << endl;
        }
    }
    return(nrecup);
}

void Fenim::initMasterPoints()
{
    for(int i=0;i<CallsNumber && i<MAXCRI;i++)
        _ipmc[i]=false;
}

void Fenim::initLines()
{
    for(int i=0;i<CallsNumber && i<MAXCRI;i++)
    {
        _ilc[i]=false;
        _callColour[i]=0;
    }
}

void Fenim::updateTagNSpec(int i)
{
    int pos = _listTaggedSpecies.size();
    QString esp = EtiquetteArray[i]->DataFields[ESPECE];
    if(!_listTaggedSpecies.contains(esp)) _listTaggedSpecies.append(esp);
    else pos=_listTaggedSpecies.indexOf(esp);
    EtiquetteArray[i]->SpecNumber = pos;
}

void Fenim::updateChbTagSpec()
{
    PFenimWindow->ChbTagSpec->clear();
    PFenimWindow->ChbTagSpec->insertItems(0,_listTaggedSpecies);
}

bool Fenim::loadLabels()
{
    LogStream << "loadLabels 1 - nbcris=" << CallsNumber << endl;
    SelectedCalls = new bool[CallsNumber];
    _oldSelectedCalls = new bool[CallsNumber];
    EtiquetteArray = new Etiquette*[CallsNumber];
    for(int i=0;i<CallsNumber;i++)
    {
        SelectedCalls[i]=false;
        _oldSelectedCalls[i]=false;
        EtiquetteArray[i]=new Etiquette((int)i);
        EtiquetteArray[i]->EtiquetteClear();
    }
    LabelsFolderName = _wavFolder + "/eti";
    QDir direti(LabelsFolderName);
    if(!direti.exists()) direti.mkdir(LabelsFolderName);
    // pour version contour
    LabelsFileName = _mainFileName + ".eti";
    _labelsFullFileName = LabelsFolderName + "/" + LabelsFileName;
    _labelsFile.setFileName(_labelsFullFileName);
    _overwriteFile = false;
    int ncrilus=0;
    LabelsNumber = 0;
    if(_reprocessingCase && _reprocessingType == 2)
    {
        _overwriteFile = true;
    }
    else
    {
        if(_labelsFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            _labelsStream.setDevice(&_labelsFile);
            _labelsStream.readLine(); // pour éliminer ligne titre
            for(int i=0;i<=CallsNumber;i++)
            {
                if(_labelsStream.atEnd()) break;
                QString ligne = (_labelsStream.readLine());
                if(ligne.isNull() or ligne.isEmpty()) break;
                int postab = ligne.indexOf((char)'\t');
                if(postab>0)
                {
                    if(ligne.left(postab) != QString::number(i)) break;
                    ncrilus++;
                    if(i<=CallsNumber-1)
                    {
                        EtiquetteArray[i]->DataFields[ESPECE]=ligne.section('\t',1,1);
                        if(!EtiquetteArray[i]->DataFields[ESPECE].isEmpty())
                        {
                            updateTagNSpec(i);
                            LabelsNumber++;
                        }
                        EtiquetteArray[i]->DataFields[TYPE]=ligne.section('\t',2,2);
                        EtiquetteArray[i]->DataFields[INDICE]=ligne.section('\t',3,3);
                        EtiquetteArray[i]->DataFields[ZONE]=ligne.section('\t',4,4);
                        EtiquetteArray[i]->DataFields[SITE]=ligne.section('\t',5,5);
                        EtiquetteArray[i]->DataFields[COMMENTAIRE]=ligne.section('\t',6,6);
                        EtiquetteArray[i]->DataFields[MATERIEL]=ligne.section('\t',7,7);
                        EtiquetteArray[i]->DataFields[CONFIDENTIEL]=ligne.section('\t',8,8);
                        EtiquetteArray[i]->DataFields[DATENREG]=ligne.section('\t',9,9);
                        EtiquetteArray[i]->DataFields[AUTEUR]=ligne.section('\t',10,10);
                        EtiquetteArray[i]->DataFields[ETIQUETEUR]=ligne.section('\t',11,11);
                    }
                    else break;
                }
                else break;
            }
            _labelsFile.close();
            if(ncrilus != CallsNumber)
            {
                if(ncrilus>0)
                {
                    reinitLabels();
                    if(QMessageBox::question(PFenimWindow, "Question", "The labels file does not match: confirm overwriting ?",
                                             QMessageBox::Yes|QMessageBox::No)
                            == QMessageBox::No)
                    {
                        PFenimWindow->close();
                        return(false);
                    }
                }
                _overwriteFile = true;
            }
            else
            {
                if(!_reprocessingCase) updateChbTagSpec();
            }
        }
    }
    return(true);
}

void Fenim::showNbLabels()
{
    PFenimWindow->LabelTitleG2->setText(QString("File : ")+LabelsFileName);
    PFenimWindow->LabelFolder->SetText(QString("Folder : ")+LabelsFolderName);
    if(LabelsNumber<2)  PFenimWindow->LabelNbLabs->SetText(QString::number(LabelsNumber)+" label");
    else PFenimWindow->LabelNbLabs->SetText(QString::number(LabelsNumber)+" labels");
}

void Fenim::reinitLabels()
{
    LabelsNumber=0;
    for(int i=0;i<CallsNumber;i++)
        EtiquetteArray[i]->EtiquetteClear();
}

void Fenim::SaveLabels()
{
    LogStream << "SaveLabels" << endl;
    if(_overwriteFile) _labelsFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
    else _labelsFile.open(QIODevice::WriteOnly | QIODevice::Text);
    _labelsStream.setDevice(&_labelsFile);
    _labelsStream.setRealNumberNotation(QTextStream::FixedNotation);
    _labelsStream.setRealNumberPrecision(6);
    _labelsStream << "Cri\tEspece\tType\tIndice\tZone\tSite\tCommentaire\tMateriel\tConfidentiel\tDate\tAuteur\tEtiqueteur\n";
    for (int i = 0 ; i < CallsNumber ; i++)
    {
        _labelsStream << EtiquetteArray[i]->CallNumber << '\t';
        for(int j=0;j<NBFIELDS;j++) _labelsStream << EtiquetteArray[i]->DataFields[j] << '\t';
        _labelsStream << endl;
    }
    //
    _labelsFile.close();
    //
    _filesUpdated = true;
    InputToSave = false;
    _overwriteFile = false;
}

void Fenim::StockSelectedCalls()
{
    for(int i=0;i<CallsNumber;i++) _oldSelectedCalls[i]=SelectedCalls[i];
}

void Fenim::ShowSelections(bool specSelect)
{
    QString lsai[NBFIELDS];
    bool bsai[NBFIELDS],bsai2[NBFIELDS];
    for(int j=0;j<NBFIELDS;j++) {lsai[j]=""; bsai[j]=false; bsai2[j]=false;}
    _callsString = "";
    int nbcrisel = 0;
    if(specSelect) {SpecTagNumber=0; SpecTagSel=-1;}
    bool affichercri = PFenimWindow->BCCalls->isChecked();
    bool afficherpm = PFenimWindow->BCMasterPoints->isChecked();
    bool laffichercri = false;
    bool lafficherpm  = false;
    bool laffichersuppl  = false;
    if(ShowLoupe)
    {
        laffichercri = PLoupe->BcCalls->isChecked();
        lafficherpm  = PLoupe->BcMasterPoints->isChecked();
        laffichersuppl  = PLoupe->BcSuppl->isChecked();
    }
    for(int i=0;i<CallsNumber;i++)
    {
        if(_oldSelectedCalls[i] != SelectedCalls[i])
        {
            ShowOneCall(i,SelectedCalls[i],affichercri);
            ShowOneMasterPoint(i,SelectedCalls[i],afficherpm);
            if(ShowLoupe)
            {
                PLoupe->ShowOneCall(i,SelectedCalls[i],laffichercri);
                PLoupe->ShowOneMasterPoint(i,SelectedCalls[i],lafficherpm);
                PLoupe->ShowOneOtherCrestl(i,laffichersuppl);
                PLoupe->ShowOneOtherPoint(i,laffichersuppl);
            }
        }
        if(SelectedCalls[i])
        {
            nbcrisel++;
            if(nbcrisel == 1)
            {
                _callsString = QString::number(i+1);
                _firstCallSelected = i;
            }
            else _callsString += ","+QString::number(i+1);
            _lastCallSelected = i;
            if(specSelect) SpecTagList[SpecTagNumber++] = i;
            for(int jField=0;jField<NBFIELDS;jField++)
            {
                if(!EtiquetteArray[i]->DataFields[jField].isEmpty())
                {
                    if(bsai[jField]==false)
                    {
                        lsai[jField] = EtiquetteArray[i]->DataFields[jField];
                        PFC[jField]->Affect(lsai[jField]);
                        bsai[jField]=true;
                    }
                    else
                    {
                        if(!PFC[jField]->Unic)
                            if(lsai[jField] != EtiquetteArray[i]->DataFields[jField])
                                bsai2[jField]=true;
                    }
                }

            }
        }
    }
    PFenimWindow->EditCall->setText(_callsString);
    // ----------------------------------------------------------------
    int coul=0;
    for(int jField=0;jField<NBFIELDS;jField++)
    {
        if(bsai[jField])
        {
            if(bsai2[jField]) coul=1; else coul=2;
        }
        else coul=0;
        PFC[jField]->Colour(InputColors[coul]);
    }
}

void Fenim::SelectCall(int n,bool specSelect)
{
    StockSelectedCalls();
    for(int j=0;j<CallsNumber;j++)
        if(j==n) SelectedCalls[j]=true;
        else SelectedCalls[j]=false;
    if(!specSelect) clearSpecTagsSelection();
    ShowSelections(false);
    if(ShowLoupe)
    {
        int x = MasterPointsVector[n].x()/(1+XHalf);
        int y = ImageHeight - MasterPointsVector[n].y()-1;
        PLoupe->LoupeView->centerOn(x,y);
    }
}

void Fenim::NextCall()
{
    if(_lastCallSelected<CallsNumber-1)
   {
        SelectCall(_lastCallSelected+1,false);
    }
    else
    {
        SelectCall(_lastCallSelected,false);
    }
}

void Fenim::PreviousCall()
{
    if(_firstCallSelected>0) SelectCall(_firstCallSelected-1,false);
    else SelectCall(_firstCallSelected,false);
}

void Fenim::EndCall()
{
    SelectCall(CallsNumber-1,false);
}

void Fenim::StartCall()
{
    SelectCall(0,false);
}

void Fenim::SpecTagNext()
{
    if(SpecTagNumber>0)
    {
        if(SpecTagSel>-1 && SpecTagSel<SpecTagNumber-1)
        {
            SpecTagSel++;
            SelectCall(SpecTagList[SpecTagSel],true);
        }
    }
}

void Fenim::SpecTagPrevious()
{
    if(SpecTagNumber>0)
    {
        if(SpecTagSel>0)
        {
            SpecTagSel--;
            SelectCall(SpecTagList[SpecTagSel],true);
        }
    }
}

void Fenim::SpecTagLast()
{
    if(SpecTagNumber>0)
    {
        SpecTagSel=SpecTagNumber-1;
        SelectCall(SpecTagList[SpecTagSel],true);
        enableMoreArrows();
    }
}

void Fenim::SpecTagFirst()
{
    if(SpecTagNumber>0)
    {
        SpecTagSel=0;
        SelectCall(SpecTagList[SpecTagSel],true);
        enableMoreArrows();
    }
}


void Fenim::SelectCall(int x,int y,bool isCTRL)
{
    float distmax = 3000;
    int ntrouve = -1;
    int xr=x*(1+XHalf),yr=ImageHeight-y-1;
    for(int i=0;i<CallsNumber;i++)
    {
        float dist=pow(xr-MasterPointsVector[i].x(),2)+pow(yr-MasterPointsVector[i].y(),2);
        if(dist < distmax)
        {
            ntrouve = i;
            distmax = dist;
        }
    }
    if(ntrouve>=0)
    {
        StockSelectedCalls();
        if(isCTRL)
        {
            SelectedCalls[ntrouve]=!SelectedCalls[ntrouve];
        }
        else
        {
            // cas général
            for(int j=0;j<CallsNumber;j++) if(j==ntrouve) SelectedCalls[j]=true;
            else SelectedCalls[j]=false;
        }
        clearSpecTagsSelection();
        ShowSelections(false);
    }
}

QString Fenim::CalculateBubble(int x,int y)
{
    QString retour("");
    float distmax = 100;
    int ntrouve = -1;
    int xr=x,yr=ImageHeight-y-1;
    if(XHalf) xr*=2;
    for(int i=0;i<CallsNumber;i++)
    {
        float dist=pow(xr-MasterPointsVector[i].x(),2)+pow(yr-MasterPointsVector[i].y(),2);
        if(dist < distmax)
        {
            ntrouve = i;
            distmax = dist;
            //break;
        }
    }
    if(ntrouve>=0)
    {
        QString esp = EtiquetteArray[ntrouve]->DataFields[ESPECE];
        QString typ = EtiquetteArray[ntrouve]->DataFields[TYPE];
        QString ind = EtiquetteArray[ntrouve]->DataFields[INDICE];
        if(!esp.isEmpty() || !typ.isEmpty())
            retour=QString("Cry ")+QString::number(ntrouve+1)+" : "+esp+" - "+typ+ "    ("+ind+")";
        else
            retour=QString("Cry ")+QString::number(ntrouve+1)+" : without label";
        retour += " - ";
    }
    if(WithSilence && y<6)
    {
        int xr = x * (1+XHalf);
        if(xr>=0 && xr <SonogramWidth)
        retour += QString(" ( ") + QString::number((int)(EnergyAverageCol[xr])) + " )";

    }
    return(retour);
}

void Fenim::ShowBubble(QString bubbleString)
{
        QToolTip::showText(QCursor::pos(),bubbleString);
}


void Fenim::SelectCallsRect(int x1,int y1,int x2,int y2,bool isCTRL)
{
    y1 = ImageHeight-y1-1;
    y2 = ImageHeight-y2-1;
    if(XHalf) {x1*=2; x2*=2;}
    StockSelectedCalls();
    if(!isCTRL)
    for(int i=0;i<CallsNumber;i++) SelectedCalls[i]=false;
    for(int i=0;i<CallsNumber;i++)
    {
        int x = MasterPointsVector[i].x();
        int y = MasterPointsVector[i].y();
        if(x>=x1 && x<=x2 && y<=y1 && y>=y2)
        {
            if(isCTRL) SelectedCalls[i] = ! SelectedCalls[i];
            else SelectedCalls[i] = true;
        }
    }
    clearSpecTagsSelection();
    ShowSelections(false);
}

void Fenim::SelectAllCalls()
{
    StockSelectedCalls();
    for(int i=0;i<CallsNumber;i++) SelectedCalls[i]=true;
    clearSpecTagsSelection();
    ShowSelections(false);
}


void Fenim::SelectSpecTags(const QString& selectedCode)
{
    PFenimWindow->LabelTagSpec->SetText(selectedCode);
    StockSelectedCalls();
    for(int i=0;i<CallsNumber;i++) SelectedCalls[i]=false;
    for(int i=0;i<CallsNumber;i++)
        if(EtiquetteArray[i]->DataFields[ESPECE]==selectedCode) SelectedCalls[i] = true;
    ShowSelections(true);
    PFenimWindow->BStartArrowSpec->setEnabled(true);
    PFenimWindow->BEndArrowSpec->setEnabled(true);
}

void Fenim::clearSpecTagsSelection()
{
    PFenimWindow->LabelTagSpec->SetText("");
    SpecTagNumber=0;
    PFenimWindow->BStartArrowSpec->setEnabled(false);
    PFenimWindow->BEndArrowSpec->setEnabled(false);
    PFenimWindow->BRightArrowSpec->setEnabled(false);
    PFenimWindow->BLeftArrowSpec->setEnabled(false);
}

void Fenim::enableMoreArrows()
{
    PFenimWindow->BRightArrowSpec->setEnabled(true);
    PFenimWindow->BLeftArrowSpec->setEnabled(true);
}

void Fenim::ClearFenim()
{
    LogStream << "ClearFenim" << endl;
    NoMore=true;
    if(_windowOpen == true)
    {
        if(!_reprocessingCase)
        {
        delete PFenimWindow->Fenima;
        delete PFenimWindow->Scene;
        delete PFenimWindow->View;
        }
        delete SelectedCalls;
        delete _oldSelectedCalls;
        for(int i=0;i<CallsNumber;i++) delete EtiquetteArray[i];
        delete EtiquetteArray;
        if(ShowLoupe) delete PLoupe;
        _windowOpen = false;
    }
}

void Fenim::CloseFenim()
{
    //videfenim();
    PFenimWindow->close();
}

void Fenim::SessionFinish()
{
    if(_reprocessingCase) return;
    if(!ACase) return;
    bool version_a_ecrire = false;
    if(!_filesUpdated) return;
    if(!_dayBaseFolder.exists())
    {
        _dayBaseFolder.mkdir(_dayBaseFolder.path());
        version_a_ecrire = true;
    }
    if(!_dayDatFolder.exists()) _dayDatFolder.mkdir(_dayDatFolder.path());
    PMainWindow->DayPath = _dayBaseFolder.path();
    if(!_dayImaFolder.exists()) _dayImaFolder.mkdir(_dayImaFolder.path());
    if(!_dayEtiFolder.exists()) _dayEtiFolder.mkdir(_dayEtiFolder.path());
    if(!_dayTxtFolder.exists()) _dayTxtFolder.mkdir(_dayTxtFolder.path());
    QString nomFicWav = _mainFileName+".wav";
    QFile wavFile(_wavFolder+"/"+nomFicWav);
    wavFile.copy(_dayBaseFolder.path() + "/" + _mainFileName + ".wav");
    QString nomFicTxt = _mainFileName+".ta";
    QFile txtFile(_wavFolder+"/txt/"+nomFicTxt);
    txtFile.copy(_dayTxtFolder.path() + "/" + _mainFileName + ".ta");
    _da2File.copy(_dayDatFolder.path() + "/" + _mainFileName + ".da2");
    _labelsFile.copy(_dayEtiFolder.path() + "/" + _mainFileName + ".eti");
    QFile imaFile(ImageFullName);
    imaFile.copy(_dayImaFolder.path() + "/" + _mainFileName + ".jpg");
    //
    wavFile.remove();
    imaFile.remove();
    txtFile.remove();
    _callsFile.remove();
    _labelsFile.remove();
    _da2File.remove();
    if(version_a_ecrire) WriteFolderVersion();
}

void FenimWindow::resizeEvent(QResizeEvent *re)
{
    ResizeFenimWindow(false);
    show();
}

void FenimWindow::closeEvent(QCloseEvent *event)
 {
    if(PFenim->InputToSave)
        //fr if(QMessageBox::question(this, "Question", "Quitter sans sauvegarder les saisies ?",
        if(QMessageBox::question(this, "Question", "Exit without saving labels file ?",
                                 QMessageBox::Yes|QMessageBox::No)
                == QMessageBox::No)
        {
            event->ignore();
            return;
        }
    if(PFenim->ACase) PFenim->SessionFinish();
    PFenim->NoMore=true;
    PFenim->ClearFenim();
    event->accept();

 }

void Fenim::CreateLoupe(int x,int y)
{
    if(ShowLoupe) delete PLoupe;
    PLoupe = new Loupe(this,PFenimWindow,x,y);
    PLoupe->ShowLoupe();
    ShowLoupe = true;
}

float Fenim::GetRatio()
{
    return((PFenimWindow->HeightRatio * FactorX * (1+XHalf)) / (PFenimWindow->WidthRatio * FactorY));
}

void Fenim::ShowRatio()
{
    QString ratio;
    ratio.setNum(GetRatio(),'f',2);
    PFenimWindow->LabelR->setText(QString("r=")+ratio);
}

float Fenim::Getms(int x)
{
    return((FactorX * x) * (1+XHalf));
}

float Fenim::GetX(float t)
{
    return(t/(FactorX * (1+XHalf)));
}

float Fenim::GetkHz(int y)
{
    return(FactorY * (ImageHeight - y -1));
}

float Fenim::GetY(float f)
{
    return((float)ImageHeight - 1 - f/FactorY );
}

void MyQGraphicsScene::mouseMoveEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
    QPointF pos = mouseEvent->lastScenePos();
    int x = pos.x();
    int y = pos.y();
    if(Iloupe)
    {
        if(x>=0 && y>=0 && x<PFenim->GetImage()->width() && y<PFenim->GetImage()->height())
        {
            this->views().size();
            QRectF r = Ploupe->LoupeView->mapToScene(Ploupe->LoupeView->viewport()->geometry()).boundingRect();
            Ploupe->LastCenterX = r.left()+r.width()/2;
            Ploupe->LastCenterY = r.top()+r.height()/2;
        }

    }
    QString bubble = PFenim->CalculateBubble(x,y);
    int xr=x,yr=PFenim->ImageHeight-y-1;
    if(PFenim->XHalf) xr*=2;
    if(y > PFenim->GetImage()->height()-1) y=PFenim->GetImage()->height()-1;
    QString ms,khz;
    ms.setNum(PFenim->Getms(x),'f',0);
    khz.setNum(PFenim->GetkHz(y),'f',2);
    if(bubble.length()>0)
    {
        if(Iloupe) Ploupe->ShowBubble(bubble);
        else PFenim->ShowBubble(bubble);
    }
    PFenim->PFenimWindow->LabelX->setText(ms+" ms");
    PFenim->PFenimWindow->LabelY->setText(khz+" khz");
    if(Iloupe)
    {

        bool pointInShape = false,pIS2=false;
        float ener,ener2;
        int distmax = 10000;
        int nbcf = -1;
        int nbpt = 0;
        for(int i=0;i<PFenim->ContoursMatrices.size();i++)
        {
            float dist=pow(xr-PFenim->MasterPointsVector[i].x(),2)
                    +pow(yr-PFenim->MasterPointsVector[i].y(),2);
            if(dist < distmax)
            {
                QVector<QPoint> unemat = PFenim->ContoursMatrices.at(i);
                for(int j=0;j<unemat.size();j++)
                {
                    if(xr==unemat.at(j).x() && yr==unemat.at(j).y())
                    {
                        pointInShape = true;
                        nbcf = i+1;
                        ener = PFenim->EnergyMatrix[i][j];
                        nbpt++;
                        if(!PFenim->XHalf || nbpt==2) break;
                    }
                    if(PFenim->XHalf)
                    {
                        if(xr+1==unemat.at(j).x() && yr==unemat.at(j).y())
                        {
                            nbcf = i+1;
                            pIS2 = true;
                            ener2 = PFenim->EnergyMatrix[i][j];
                            nbpt++;
                            if(nbpt==2) break;
                        }
                    }
                }
            }
        }
        Ploupe->LabelX->setText(ms+" ms");
        Ploupe->LabelY->setText(khz+" khz");
        QString affi="";
        if(pointInShape || pIS2)
        {
            affi = QString::number(nbcf)+": ";
            if(pointInShape) affi += QString::number(ener);
            else affi += " - ";
            if(PFenim->XHalf)
            {
                affi += QString(" ; ");
                if(pIS2) affi += QString::number(ener2); else affi+= " - ";
            }
        }
        Ploupe->LabelEnergy->setText(affi);
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
    Qmaitre = parent;
    Iloupe=il;
    if(il)
    {
        Ploupe = (Loupe *)Qmaitre;
        PFenim = pf;
    }
    else
    {
        PFenim = pf;
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
    if(Iloupe)
    {
        Ploupe->Zoom(x,y);
    }
    else
    {
        PFenim->CreateLoupe(x,y);
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
        PFenim->SelectCall(pos.x(),pos.y(),isCTRL);
    else
    {
        int x1,x2,y1,y2;
        if(derx>pos.x()) {x1=pos.x(); x2=derx;}
        else {x1=derx; x2=pos.x();}
        if(dery>pos.y()) {y1=pos.y(); y2=dery;}
        else {y1=dery; y2=pos.y();}
        PFenim->SelectCallsRect(x1,y1,x2,y2,isCTRL);
    }
    derx = -1; dery = -1;
}

void MyQGraphicsScene::keyPressEvent(QKeyEvent* e)
{
    Qt::KeyboardModifiers keyMod = QApplication::keyboardModifiers ();
    if(keyMod.testFlag(Qt::ControlModifier) && e->key() == 'A') PFenim->SelectAllCalls();
}


MyQLineEdit::MyQLineEdit(QWidget *parent,Fenim *fen,QString cod):QLineEdit(parent)
{
    Qmaitre = parent;
    Pfenim = (Fenim *)fen;
    Codeedit=cod;
    setFont(QFont("Arial",10,QFont::Normal));
}

EC_LineEdit::EC_LineEdit(QWidget *parent,Fenim *fen,QString cod):MyQLineEdit(parent,fen,cod)
{
    _pFenim=fen;
    connect(this,SIGNAL(textEdited(const QString&)),this,SLOT(UpdateText(const QString&)));
}

Simple_LineEdit::Simple_LineEdit(QWidget *parent,Fenim *fen,QString cod):MyQLineEdit(parent,fen,cod)
{
    _pFenim=fen;
    connect(this,SIGNAL(textEdited(const QString&)),this,SLOT(UpdateText(const QString&)));
}

void EC_LineEdit::SetEcp(EditCombo *pec)
{
    Ecp=pec;
}

EC_ComboBoxEdit::EC_ComboBoxEdit(QWidget *parent,Fenim *fen,QString cod):MyQComboBox(parent,fen,cod)
{
    connect(this, SIGNAL(activated(const QString&)), this, SLOT(SelectCode(const QString&)));
}

void EC_ComboBoxEdit::SetEcp(EditCombo *pec)
{
    Ecp=pec;
}

EditCombo::EditCombo(QWidget *parent,Fenim *fen,QString cod,QString englishCode,bool autaj)
{
    PFenimWindow = fen->PFenimWindow;
    EcLe=new EC_LineEdit(parent,fen,cod);
    EcCo=new EC_ComboBoxEdit(parent,fen,cod);
    EcLe->SetEcp(this);
    EcCo->SetEcp(this);
    CodeFi = cod;
    EnglishCode = englishCode;
    AllowAdd=autaj;
    ListCodes = new QStringList();
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
                    else ListCodes->append(lte);
                }
            }
        }

    }
    redoList("");
}

void EC_LineEdit::UpdateText(const QString& s)
{
    Ecp->redoList(s);
    Ecp->EcLe->setStyleSheet(_pFenim->InputColors[4]);
}

void EC_ComboBoxEdit::SelectCode(const QString& s)
{
    Ecp->SelectCode(s);
    Ecp->EcLe->setStyleSheet(PFenim->InputColors[4]);
}

void EC_LineEdit::keyPressEvent(QKeyEvent* e)
{
    if(e->key()==Qt::Key_Return)
    {
        QString sc = Ecp->EcCo->currentText();
        QString se = text();
        if(se.length()>0 && sc.length()>0 && sc.contains(se))
            Ecp->SelectCode(sc);
    }
    MyQLineEdit::keyPressEvent(e);
}

void Simple_LineEdit::UpdateText(const QString& s)
{
    this->setStyleSheet(_pFenim->InputColors[4]);
}


void MyQLineEdit::keyPressEvent(QKeyEvent* e)
{
    Qt::KeyboardModifiers keyMod = QApplication::keyboardModifiers ();
    if(keyMod.testFlag(Qt::ControlModifier) && e->key() == 'A') Pfenim->SelectAllCalls();

    if(e->key()==Qt::Key_Down)
    {
        if(Codeedit=="cri") Pfenim->PFC[ESPECE]->LE->setFocus();
        else
        {
            for(int j=0;j<NBFIELDS;j++)
            {
                int ft = Pfenim->PFC[j]->FieldType;
                if(Codeedit==Pfenim->PFC[j]->FieldCode)
                {
                    if(ft==EC) Pfenim->PFC[j]->ECo->EcCo->setFocus();
                    else
                    {
                        if(j<NBFIELDS)
                        {
                            int ftb = Pfenim->PFC[j+1]->FieldType;

                            if(ftb==EC || ftb==SLI || ftb== SLE)
                                Pfenim->PFC[j+1]->LE->setFocus();
                        }
                    }
                }
            }
        }

    }
    if(e->key()==Qt::Key_Up)
    {
        if(Codeedit=="especes") Pfenim->PFenimWindow->EditCall->setFocus();
        else
        {
            for(int j=0;j<NBFIELDS;j++)
            {
                if(Codeedit==Pfenim->PFC[j]->FieldCode)
                {
                    if(j>0)
                    {
                        int ftb = Pfenim->PFC[j-1]->FieldType;
                        if(ftb==EC || ftb==SLI || ftb== SLE)
                            Pfenim->PFC[j-1]->LE->setFocus();
                    }
                }
            }
        }
    }
    QLineEdit::keyPressEvent(e);
}

MyQLabel::MyQLabel(QWidget *parent):QLabel(parent)
{
    QMaster = parent;
    setFont(QFont("Arial",10,QFont::Normal));
    setStyleSheet("background-color: #F8F8FE");
}

void MyQLabel::SetText(QString message)
{
    if(message.isEmpty())
    {
        if(isVisible()) setVisible(false);
    }
    else
    {
        if(!isVisible()) setVisible(true);
    }
    QLabel::setText(message);
}


MyQComboBox::MyQComboBox(QWidget *parent,Fenim *fen,QString esp):QComboBox(parent)
{
    Qmaitre = parent;
    PFenim = fen;
    Codecombo=esp;
    setFont(QFont("Arial",10,QFont::Normal));
}

void EC_ComboBoxEdit::keyPressEvent(QKeyEvent* e)
{
    if(e->key()==Qt::Key_Backspace || e->key()==Qt::Key_Delete)
    {
        Ecp->EcLe->setText("");
        Ecp->redoList("");
        showPopup();
    }
    if(e->key()==Qt::Key_Down)
    {
        this->showPopup();
    }
    if(e->key()==Qt::Key_Return)
    {
        Ecp->SelectCode(currentText());
    }
    MyQComboBox::keyPressEvent(e);
}

void MyQComboBox::keyPressEvent(QKeyEvent* e)
{
    if(PFenim!=0)
    {
        Qt::KeyboardModifiers keyMod = QApplication::keyboardModifiers ();
        if(keyMod.testFlag(Qt::ControlModifier) && e->key() == 'A') PFenim->SelectAllCalls();
    }
    QComboBox::keyPressEvent(e);
}


MyQPushButton::MyQPushButton(QWidget *parent):QPushButton(parent)
{
    QMaster = parent;
    setFont(QFont("Arial",10,QFont::Normal));
}


MyQGraphicsView::MyQGraphicsView(QMainWindow *parent):QGraphicsView(parent)
{
    qmaitre = parent;
}


bool Fenim::ReadFolderVersion()
{
    QString cbase = _wavFolder + _baseIniFile;
    FolderLogVersion = 0;
    FolderUserVersion = 0;
    if(!QFile::exists(cbase)) return(false);
    QSettings settings(_wavFolder + _baseIniFile, QSettings::IniFormat);
    settings.beginGroup("version");
    FolderLogVersion = settings.value("log").toInt();
    FolderUserVersion = settings.value("user").toInt();
    settings.endGroup();
    return(true);
}

void Fenim::WriteFolderVersion()
{
    QSettings settings(_dayBaseFolder.path() + _baseIniFile, QSettings::IniFormat);
    settings.beginGroup("version");
    settings.setValue("log", QVariant::fromValue(LogVersion));
    settings.setValue("user", QVariant::fromValue(UserVersion));
    settings.setValue("modeFreq", QVariant::fromValue(FrequencyType));
    settings.endGroup();
}



