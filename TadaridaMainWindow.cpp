#include "TadaridaMainWindow.h"
#include "param.h"
#include "etiquette.h"
#include "fenim.h"
#include "recherche.h"

RematchClass::RematchClass(QMainWindow *parent)
{
    _parent = parent;
}

RematchClass::~RematchClass()
{
}

void RematchClass::initialize()
{
    Ok = true;
}

int RematchClass::PreMatch(QString wavName,QString wavPath)
{
    _fileName = wavName;
    _wavPath = wavPath;
    int postab = _fileName.lastIndexOf(".");
    if(postab>0) _fileName = _fileName.left(postab);
    Fenim1 = new Fenim(_parent,_wavPath,_fileName,_baseDayDir,false,true,1,"1",0,0,0);
    //_fenim1->m_logStream << "Prematch 2" << endl;
    Nbc1=0; Nbe1=0;
    if(Fenim1->LoadCallsLabels()==false) return(0);

    //_fenim1->m_logStream << "Prematch 3" << "nb eti=" <<_fenim1->m_nbeti << endl;
    Nbc1=Fenim1->CallsNumber;
    Nbe1=Fenim1->LabelsNumber;
    return(Nbe1);
}

int RematchClass::PostMatch(bool initial,QString recupVersion,int *cpma)
{
    Nbe2=0;
    Nbc2=0;
    if(initial)
    {
        Fenim2 = new Fenim(_parent,_wavPath,_fileName,_baseDayDir,false,true,2,"2",0,0,0);
        if(Fenim2->LoadCallsLabels())
            Nbc2 = Fenim2->CallsNumber;
        else
        {
            Ok = false;
            return(0);
        }
    }
    if(Nbe1>0)
    {
        Nbe2 = Fenim2->MatchLabels(Fenim1,initial,recupVersion,cpma);
    }
    delete Fenim1;
    return(Nbe2);
}

int RematchClass::EndMatch()
{
    Fenim2->SaveLabels();
    delete Fenim2;
    return(0);
}

TadaridaMainWindow::TadaridaMainWindow(QWidget *parent) : QMainWindow(parent)
{
    _logFile.setFileName(QString("tadarida.log"));
    _logFile.open(QIODevice::WriteOnly | QIODevice::Text);
    _logText.setDevice(&_logFile);
    _iniPath = QString("/config/config.ini");
    CanContinue = true;
    _mustEnd = false;
    _mustCancel = false;
    _wavDirectory   = QDir::current();
    _lastWav = QString("");
    _tadaridaMode = SIMPLE; // mode simple
    _userVersion = 0;
    _programVersion = 22;
    // 21 : version avec enregistrement de _floagGoodCol...
    // 22 : version a partir de laquelle on enregistre modefreq dans les fichiers version.ini
    _baseDir   = QDir::current();
    _paramVersion = 2;
    _desactiveCorrectNoise = false;
    _tadaridaMode = SIMPLE;
    _maxThreads = 4; // (� remettre � 1)
    // attention _maxTheads pourra bouger par config.ini et aussi en cours de session
    _modifyFreqAuthorized = false;
    initThreads();

    IDebug = false;
   //
    DayPath = "";
    SearchDir1 = "";
    SearchDir2 = "";
    //
    _timeExpansionLeft = 10;
    _timeExpansionRight = 0;
    // multifreq :
    _modeFreq = 1;

    readConfigFile();
    //
    ResultSuffix = "ta";
    if(_tadaridaMode==ETIQUETAGE)
    {
        _previousWav = QString("");
        _nextWav = QString("");
        _imagesDirectory   = QDir::current();
        _baseUpToDate=false;
        _dayBaseUpToDate=false;
        _baseIniFile = QString("/version.ini");
        _savedTextsFile = QString("/config/saisies.ini");
        _canTag = tablesExists();
        FieldsList << "espece" << "type" << "indice" << "zone" << "site" << "commentaire"
         << "materiel" << "confidentiel" << "datenreg" << "auteur" << "etiqueteur";
        readLastTexts();
        TextsToSave = false;
        _baseProgramVersion = -1;
        _baseUserVersion = -1;
    }
    else
    {
        _canTag = false;
    }
    for(int i=0;i<MAXTHREADS;i++) _remObject[i] = new RematchClass(this);
    // -----------------------------------------------
    // -----------------------------------------------
    initializeGlobalParameters();
    createWindow();
    _isFenimWindowOpen = false;
    _isRechercheOpen = false;
    connect(_btnBrowse,SIGNAL(clicked()),this,SLOT(on_btnBrowse_clicked()));
    connect(_ledTreatedDirectory,SIGNAL(textChanged(const QString&)),this,SLOT(on_ledTreatedDirectory_textChanged(const QString&)));
    connect(_btnOk,SIGNAL(clicked()),this,SLOT(on_btnOk_clicked()));
    QObject::connect(_btnCancel, SIGNAL(clicked(bool)), this, SLOT(treatCancel()));
    _clock = new QTimer(this);
    connect(_clock,SIGNAL(timeout()),this,SLOT(manageDetecCall()));
    setWindowTitle("Tadarida");
    window()->setWindowTitle("TadaridaL   version 2.1");
    window()->setWindowIconText("Tadarida");
    show();
    if(_tadaridaMode==ETIQUETAGE)
    {
        connect(_btnParameters,SIGNAL(clicked()),this,SLOT(on_btnParam_clicked()));
        connect(_btnOpenWav,SIGNAL(clicked()),this,SLOT(on_btnOpenWav_clicked()));
        connect(_btnOpenPreviousWav,SIGNAL(clicked()),this,SLOT(on_btnOpenPreviousWav_clicked()));
        connect(_btnOpenNextWav,SIGNAL(clicked()),this,SLOT(on_btnOpenNextWav_clicked()));
        connect(_btnUpdateTags,SIGNAL(clicked()),this,SLOT(on_btnUpdateTags_clicked()));
        connect(_btnOpenBase,SIGNAL(clicked()),this,SLOT(on_btnOpenBase_clicked()));
        connect(_btnFind,SIGNAL(clicked()),this,SLOT(on_btnFind_clicked()));
        if(_canTag==true)
        {
            if(!manageBase(false,!_modifyFreqAuthorized)) {exitProgram(); return;}
            updateBaseVariables();
        }
        else
        {
            _lblPhase2Title->setEnabled(false);
            _lblBase->setVisible(false);
            _btnOpenBase->setEnabled(false);
            _btnOpenWav->setEnabled(false);
            _btnUpdateTags->setEnabled(false);
            _btnOpenPreviousWav->setEnabled(false);
            _btnOpenNextWav->setEnabled(false);
            _btnFind->setEnabled(false);
        }
    }
    connect(_sliderThreads,SIGNAL(valueChanged(int)),this,SLOT(modifyMaxThreads(int)));
    if(_modifyFreqAuthorized)
    connect(_freqLow,SIGNAL(toggled(bool)),this,SLOT(on_freqLow_toogled(bool)));
    _logText << "modefreq=" << _modeFreq << endl;
}

TadaridaMainWindow::~TadaridaMainWindow()
{
    for(int i=0;i<_nbDetecCreated;i++) delete _pDetec[i];
    for(int i=0;i<MAXTHREADS;i++) delete _remObject[i];
    if(_isFenimWindowOpen)
    {
        delete fenim;
    }
    if(_isRechercheOpen) delete _precherche;
    _logFile.close();
}

bool TadaridaMainWindow::tablesExists()
{
    QStringList tablesList;
    tablesList << "especes.txt" << "types.txt" << "zone.txt";
    QFile tableFile;
    int nlf = 0;
    QString err("");
    foreach(QString cod, tablesList)
    {
        tableFile.setFileName(QString(cod));
        if(tableFile.open(QIODevice::ReadOnly | QIODevice::Text))
            tableFile.close();
        else
        {
            nlf++;
            if(err=="") err = cod; else err+=", "+cod;
        }
    }
    if(nlf>0)
    {
        //fr QMessageBox::warning(this, "Attention !", QString("Table(s) manquante(s) :")+err+". Etiquetage non permis !",QMessageBox::Ok);
        QMessageBox::warning(this, "Error !", QString("Tables missing : ")+err+". not allowed labelling !",QMessageBox::Ok);

        return(false);
    }
    return(true);
}

void TadaridaMainWindow::updateBaseVariables()
{
    QDate dj=QDate::currentDate();
    QString sd=dj.toString("yyyyMMdd");
    _lblBase->SetText(QString("Database : ")+_baseDir.path()+" ("+sd+")");
    //
    _strDay = QDate::currentDate().toString("yyyyMMdd");
    QString dayPath = _baseDir.path() + "/" + _strDay;
    _baseDay.setPath(dayPath);
    if(_baseDay.exists()) DayPath = dayPath;
}

bool TadaridaMainWindow::manageBase(bool firstSelect,bool blockedFreq)
{
    if(!_modifyFreqAuthorized) blockedFreq = true;
    if(firstSelect)
    {
        QString basepath = selectBase();
        // 0) cas base absente
        if(basepath.isEmpty())
        {
            //fr if(QMessageBox::question(this, "Choix obligatoire !", "Le mode Etiquetage ne peut fonctionner si on ne choisit pas une base !",QMessageBox::Yes|QMessageBox::No)
            if(QMessageBox::question(this, "Required choice !", "Labelling mode can not function without a base !",QMessageBox::Yes|QMessageBox::No)
                    == QMessageBox::Yes) return(manageBase(true,blockedFreq));
            else return(false);
        }
        else _baseDir.setPath(basepath);
    }
    // test �tat de la base
    if(readBaseVersion())
    {
        // 1) cas type de frequence incoherent
        if(_modeFreqBase <1 || _modeFreqBase>2)
        {
            //fr QMessageBox::critical(this, "Arret du programme","Parametrage du type de frequence de la base incoherent !", QMessageBox::Ok);
            QMessageBox::critical(this, "Program stopped !","Wrong type of frequence !", QMessageBox::Ok);
            return(false);
        }
        // 2) type de frequence different
        if(_modeFreqBase != _modeFreq)
        {
            if(blockedFreq)
            {
                //fr QMessageBox::warning(this, "Cette base n'est pas du type de frequence voulu !","Selectionner une base correspondant au type voulu !", QMessageBox::Ok);
                QMessageBox::warning(this, "This base is not of the desired type of frequency !","Select a base with the right type of frequency !", QMessageBox::Ok);

                return(manageBase(true,true));
            }
            else
            {
                //fr if(QMessageBox::question(this, "Type de frequence different !", "Repondre oui pour modifier le type de fr�quence trait� ou non pour selectionner une autre base",
                if(QMessageBox::question(this, "Different type of frequency !", "Answer yes to change the type of frequency or no to select another base",
                                         QMessageBox::Yes|QMessageBox::No)
                        == QMessageBox::Yes)
                {
                    _modeFreq = _modeFreqBase;
                    cocheFreq();
                }
                else
                {
                    return(manageBase(true,!_modifyFreqAuthorized));
                }
            }
        }
        if(_baseProgramVersion != _programVersion || _baseUserVersion != _userVersion)
        {
            // 3) cas versions incoherentes
            if(_baseProgramVersion > _programVersion || _baseUserVersion > _userVersion)
            {
                //fr _logText << "versions incoherentes" << endl;
                _logText << "incompatible versions : " << _baseProgramVersion  << "," << _baseUserVersion << " et << " << _programVersion << "," << _userVersion << endl;
                //fr QMessageBox::critical(this, "Arret du programme","La base a une version en avance sur le logiciel !", QMessageBox::Ok);
                QMessageBox::critical(this, "Program stopped","The database version is more advanced than that of the software !", QMessageBox::Ok);
                return(false);
            }
            else
            {
                // 4) versions en retard
                updateBaseVariables();
                //fr _logText << "version en retard" << endl;
                _logText << "late version" << endl;
                _baseUpToDate=false;
                _dayBaseUpToDate=false;
                proposeGeneralReprocessing();
            }
        }
        else
        {
            // 5) base ok
            _baseUpToDate=true;
            _dayBaseUpToDate=true;
            updateBaseVariables();
        }
        return(true); // pour indiquer base existante
    }
    else
    {
        if(_baseDir == QDir::current())
        {
            //fr QMessageBox::warning(this,"Base non determnee", "Selectionner le repertoire de la base !", QMessageBox::Ok);
            QMessageBox::warning(this,"Undefined base", "Select the database folder !", QMessageBox::Ok);
             return(manageBase(true,blockedFreq));
        }
        // 6) base a initialiser
        // fr _logText << "base non initialisee : version.ini manquant" << endl;
         _logText << "Uninitialized database : config file is missingt" << endl;
        // fr if(QMessageBox::question(this, "Base vide", "Repondre oui pour initialiser cette base ou non pour en choisir une autre !",
        if(QMessageBox::question(this, "Empty database", "Answer yes to initialize this database or no to select an other one !",
                                 QMessageBox::Yes|QMessageBox::No)
                == QMessageBox::Yes)
        {
            createBase();

        }
        else return(manageBase(true,blockedFreq));
    }
}

bool TadaridaMainWindow::createBase()
{
    writeBaseVersion();
    _baseUpToDate = true;
    _dayBaseUpToDate = true;
    return(true);
}

void TadaridaMainWindow::exitProgram()
{
    CanContinue = false;
    close();
}

void TadaridaMainWindow::endTadarida()
{
    writeConfigFile();
    if(_tadaridaMode==ETIQUETAGE)
    {
        if(TextsToSave) writeLastTexts();
    }
}

void TadaridaMainWindow::closeEvent(QCloseEvent* event)
{
    if(countThreadsRunning()>0)
    {
        _mustEnd = true;
        treatCancel();
        event->ignore();
    }
    else
    {
        endTadarida();
        event->accept();
        QMainWindow::closeEvent(event);
    }
}

void TadaridaMainWindow::readConfigFile()
{
    if(!QFile::exists(QDir::currentPath() + _iniPath)) return;
    QSettings settings(QDir::currentPath() + _iniPath, QSettings::IniFormat);
    settings.beginGroup("path");
    QDir soundDir(settings.value("wavPath").toString());
    if(soundDir.exists()) _wavDirectory = soundDir;
    QString lastwav(settings.value("lastwav").toString());
    if(QFile::exists(lastwav)) _lastWav = lastwav;
    QString previouswav(settings.value("previouswav").toString());
    if(QFile::exists(previouswav)) _previousWav = previouswav;
    QString nextwav(settings.value("nextwav").toString());
    if(QFile::exists(nextwav)) _nextWav = nextwav;
    QDir imgDir(settings.value("imgPath").toString());
    if(imgDir.exists()) _imagesDirectory = imgDir;
    QDir baseDir(settings.value("basePath").toString());
    if(baseDir.exists()) _baseDir = baseDir;


    SearchDir1 = settings.value("searchDir1").toString();
    SearchDir2 = settings.value("searchDir2").toString();

    QString jourTraite(settings.value("jourTraite").toString());
    if(QDir(jourTraite).exists()) DayPath = jourTraite;
    settings.endGroup();
    settings.beginGroup("version");
    int verlog = settings.value("log").toInt();
    if(verlog != _programVersion)
    {
        // QMessageBox::warning(this, tr("Changement de version"), tr("Version logiciel differente dans config.ini : rectification "), QMessageBox::Ok);
        _logText << "Different versions - verlog=" << verlog << "  programVersion=" << _programVersion << endl;
    }
    _userVersion = settings.value("user").toInt();
    int mode = settings.value("mode").toInt();
    if(mode==ETIQUETAGE) _tadaridaMode = ETIQUETAGE;
    else _tadaridaMode = SIMPLE;
    _paramVersion = settings.value("paramVersion").toInt();
    if(_paramVersion==0 || _paramVersion>2) _paramVersion = 2;
    _timeExpansionLeft = settings.value("timeExpansionLeft").toInt();
    _timeExpansionRight = settings.value("timeExpansionRight").toInt();
    if(_timeExpansionLeft == 0 && _timeExpansionRight==0) _timeExpansionLeft = 10;
    int modifyFreqAuthorized = settings.value("modifyFreq").toInt();
    if(modifyFreqAuthorized == 1) _modifyFreqAuthorized  = true;
    else _modifyFreqAuthorized  = false;
    int modefreq = settings.value("modeFreq").toInt();
    if(modefreq != 2) _modeFreq = 1; else _modeFreq = 2;
    int threadAuthorized = settings.value("thread").toInt();
    if(threadAuthorized == 1) _threadAuthorized  = true;
    else _threadAuthorized  = false;
    _maxThreads = settings.value("threadNumber").toInt();
    if(_maxThreads < 1) _maxThreads =1;
    else
    {
    if(_maxThreads > MAXTHREADS) _maxThreads = MAXTHREADS;
    }
    //
    IDebug = settings.value("debug").toInt();
    settings.endGroup();
}

void TadaridaMainWindow::writeConfigFile()
{
    QFile cf(QDir::currentPath() + _iniPath);
    if(cf.open(QIODevice::WriteOnly)) cf.close();
    else
     //fr QMessageBox::warning(this, tr("Attention"), tr("Fichier config.ini inaccessible en Ecriture : fermer ce fichier s'il est ouvert !"), QMessageBox::Ok);
    QMessageBox::warning(this, "Warning", "Config.ini is not writable : close this file if it is open !", QMessageBox::Ok);
    QSettings settings(QDir::currentPath() + _iniPath, QSettings::IniFormat);
    settings.beginGroup("path");
    settings.setValue("wavPath",_wavDirectory.path());
    if(_tadaridaMode==ETIQUETAGE)
    {
        settings.setValue("lastWav",_lastWav);
        settings.setValue("previousWav",_previousWav);
        settings.setValue("nextWav",_nextWav);
        settings.setValue("imgPath",_imagesDirectory.path());
        settings.setValue("basePath",_baseDir.path());
        if(!DayPath.isEmpty()) settings.setValue("jourTraite",DayPath);
        settings.setValue("searchDir1",SearchDir1);
        settings.setValue("searchDir2",SearchDir2);
    }
    settings.endGroup();
    settings.beginGroup("version");
    settings.setValue("log", QString::number(_programVersion));
    settings.setValue("user",QString::number(_userVersion));
    settings.setValue("mode",QString::number(_tadaridaMode));
    settings.setValue("paramVersion",QString::number(_paramVersion));
    settings.setValue("timeExpansionLeft",QString::number(_timeExpansionLeft));
    settings.setValue("timeExpansionRight",QString::number(_timeExpansionRight));
    settings.setValue("modifyFreq",QString::number(_modifyFreqAuthorized));
    settings.setValue("modeFreq",QString::number(_modeFreq));
    settings.setValue("threadNumber",QString::number(_maxThreads));
    settings.setValue("debug",QString::number((int)IDebug));
    settings.endGroup();
}

void TadaridaMainWindow::readLastTexts()
{
    for(int i=0;i<NBFIELDS;i++) LastFields[i]="";
    if(!QFile::exists(QDir::currentPath() + _savedTextsFile)) return;
    QSettings settings(QDir::currentPath() + _savedTextsFile, QSettings::IniFormat);
    settings.beginGroup("saisies");
    for(int i=0;i<NBFIELDS;i++) LastFields[i] = settings.value(FieldsList[i]).toString();
    settings.endGroup();
}

void TadaridaMainWindow::writeLastTexts()
{
    QSettings settings(QDir::currentPath() + _savedTextsFile, QSettings::IniFormat);
    settings.beginGroup("saisies");
    for(int i=0;i<NBFIELDS;i++)
        settings.setValue(FieldsList[i],QVariant::fromValue(LastFields[i]));
    settings.endGroup();
}

bool TadaridaMainWindow::readBaseVersion()
{
    QString cbase = _baseDir.path()+ _baseIniFile;
    if(!QFile::exists(cbase))
    {
        _logText << "rvb  : " << cbase << "does not exist !" << endl;
        return(false);
    }
    QSettings settings(_baseDir.path()+ _baseIniFile, QSettings::IniFormat);
    settings.beginGroup("version");
    _baseProgramVersion = settings.value("log").toInt();
    _baseUserVersion = settings.value("user").toInt();
    _modeFreqBase = settings.value("modeFreq").toInt();
    if(_modeFreqBase==0 && _baseProgramVersion < 22) _modeFreqBase = 1;
    settings.endGroup();
    if(_baseProgramVersion < 0 || _baseUserVersion<0)
    {
        _logText << "rbv bpv<0 or buv<0" << endl;
        return(false);
    }
    else return(true);
}

void TadaridaMainWindow::writeBaseVersion()
{
    QSettings settings(_baseDir.path()+ _baseIniFile, QSettings::IniFormat);
    settings.beginGroup("version");
    settings.setValue("log", QVariant::fromValue(_programVersion));
    settings.setValue("user", QVariant::fromValue(_userVersion));
    settings.setValue("modeFreq", QVariant::fromValue(_modeFreq));
    settings.endGroup();
}

void TadaridaMainWindow::on_btnParam_clicked()
{
    param = new Param(this,18);
    param->ShowScreen();
    QStringList lnbo;
    lnbo << "1" << "2" << "4" << "8";
    QStringList lte;
    lte << "10" << "1" ;
    param->CreateParameter(QString("Detection threshold"),&_detectionThreshold,1,10,30);
    param->CreateParameter(QString("Stop threshold"),&_stopThreshold,1,5,25);
    param->CreateParameter(QString("Minimum frequency"),&_minimumFrequency,1,0,20);
    param->CreateParameter(QString("p_nbo"),&_overlapsNumber,4,0,0,0,0,&lnbo);
    param->CreateParameter(QString("coef. x"),&Divrl,1,100,100000);
    param->CreateParameter(QString("Treatment of silences"),&_useValflag ,3);
    param->CreateParameter(QString("Silence threshold"),&_jumpThreshold,1,10,50);
    param->CreateParameter(QString("Wide vertical band"),&_widthBigControl,1,10,500);
    param->CreateParameter(QString("Narrow vertical band"),&_widthLittleControl,1,1,20);
    param->CreateParameter(QString("High threshold"),&_highThresholdJB,1,9,20);
    param->CreateParameter(QString("Low threshold"),&_lowThresholdJB,1,-20,9);
    param->CreateParameter(QString("Second low threshold"),&_lowThresholdC,1,-5,10);
    param->CreateParameter(QString("Second high threshold"),&_highThresholdC,1,1,30);
    param->CreateParameter(QString("Percentage q5"),&_qR,1,1,20);
    param->CreateParameter(QString("Minimum pixel number q5"),&_qN,1,2,10);
    param->CreateParameter(QString("Parameters version"),&_paramVersion ,1,0,2);
    param->CreateParameter(QString("Time.csv"),&_withTimeCsv,3);
    param->CreateParameter(QString("Deactivate correctnoise"),&_desactiveCorrectNoise ,3);
    if(param->ParamOrderNumber > param->ParamsNumber) param->ParamOrderNumber = param->ParamsNumber;
    param->show();
}

void TadaridaMainWindow::on_btnBrowse_clicked()
{
    QString soundsPath  = QFileDialog::getExistingDirectory( this,
                            "Select the wav files folder",
                            _wavDirectory.path(),
                            QFileDialog::ShowDirsOnly);

    if(!soundsPath.isEmpty())
        _ledTreatedDirectory->setText(QDir::fromNativeSeparators(soundsPath));
}

void TadaridaMainWindow::on_ledTreatedDirectory_textChanged(const QString &txt)
{
    if(!txt.isEmpty())
    {
        _btnOk->setEnabled(true);
    }
    else
    {
        _btnOk->setEnabled(false);
    }
    _wavDirectory.setPath(txt);
}

void TadaridaMainWindow::on_btnOk_clicked()
{
    if(!GetDirectoryType(_wavDirectory.path()))
    {
        //fr QMessageBox::warning(this, "Erreur", "Ne pas lancer ce traitement sur un dossier de la base !", QMessageBox::Ok);
        QMessageBox::warning(this, "Error", "Do not start this treatment on a folder in the database !", QMessageBox::Ok);
        blockUnblock(true);
        return;
    }
    _mustCancel = false;
    directoryTreat(_wavDirectory,_chkSubDirectories->isChecked());
}


void TadaridaMainWindow::treatCancel()
{
    _mustCancel = true;
    if(!_directoriesRetreatMode)
    for(int i=0;i<_nbThreadsLaunched;i++) if(_threadRunning[i]) _pDetec[i]->MustCancel = true;
}

void TadaridaMainWindow::blockUnblock(bool acdesac)
{
    _ledTreatedDirectory->setEnabled(acdesac);
    _btnBrowse->setEnabled(acdesac);
    _btnOk->setEnabled(acdesac);
    _ledTreatedDirectory->setEnabled(acdesac);
    _ledTreatedDirectory->setVisible(acdesac);
    _lblTreatedDirectory->setVisible(!acdesac);
    _leftGroup->setEnabled(acdesac);
    _rightGroup->setEnabled(acdesac);
    _sliderThreads->setEnabled(acdesac);

    _btnCancel->setEnabled(!acdesac);
    if(_tadaridaMode==ETIQUETAGE)
    {
        bool condi = acdesac & _canTag;
        _btnParameters->setEnabled(condi);
        _btnOpenBase->setEnabled(condi);
        _btnOpenWav->setEnabled(condi);
        _btnUpdateTags->setEnabled(condi);
		_btnFind->setEnabled(condi);
        _btnOpenPreviousWav->setEnabled(condi);
        _btnOpenNextWav->setEnabled(condi);
    }
}

void TadaridaMainWindow::infoShow(QString mess)
{
    _lblPhase1Message->SetText(mess);
}

void TadaridaMainWindow::infoShow2(QString mess,bool withLog)
{
    _lblPhase1Message2->SetText(mess);
    if(withLog)  _logText << mess << endl;
}

void TadaridaMainWindow::infoShow3(QString mess,bool withLog)
{
    _lblPhase1Message3->SetText(mess);
    if(withLog)  _logText << mess << endl;
}

void TadaridaMainWindow::matchingInfoTreat(int nbeav,int nbeap,int nbere)
{
    _tagsNumberBefore += nbeav;
    _tagsNumberAfter  += nbeap;
    _tagsNumberFinal  += nbere;
}

void TadaridaMainWindow::detecInfoTreat(int iThread,int nbt,int nbe)
{
    QString mess,singpur,mess2;
    _nbTreated[iThread] = nbt;
    _nbError[iThread] = nbe;
    _nbTreatedTotal=0;
    _nbErrorTotal=0;

    for(int i=0;i<_nbThreadsLaunched;i++)
    {
        _nbTreatedTotal +=  _nbTreated[i];
        _nbErrorTotal     +=  _nbError[i];
        for(int k=0;k<NTERRORS;k++) _tabError[i][k] =_pDetec[i]->PDetecTreatment->TabErrors[k];
    }
    if(_nbTreatedTotal>0)
    {
        if(_nbTreatedTotal>1) singpur ="s"; else singpur = "";
        mess = QString::number(_nbTreatedTotal) + " treated file"+singpur;
    }
    if(_nbErrorTotal>0)
    {
        mess2 = createMessageErrors(_nbErrorTotal,_tabError);
        if(_nbTreatedTotal==0) mess = mess2; else mess += " - " + mess2;
    }
    if(_filesNumber>0)
    {
        updateProgBarValue((float)(_nbTreatedTotal+_nbErrorTotal)/(float)_filesNumber);

    }
    infoShow2(mess,((_nbTreatedTotal+_nbErrorTotal)==_filesNumber));
    _treatDirMess = mess;
}

QString TadaridaMainWindow::createMessageErrors(int netot,int tabe[][NTERRORS])
{
    QString m = "";
    if(netot>0)
    {
        QString singpur;
        if(netot>1) singpur =QString("s"); else singpur = QString("");
        m = QString::number(netot) + " not treated file"+singpur+" (";
        bool ffe = false;
        for(int i=0;i<NTERRORS;i++)
        {
            int nee=0;
            for(int j=0;j<_nbThreadsLaunched;j++) nee += tabe[j][i];
            if(nee>0)
            {
                if(ffe==false)   ffe=true; else m+=" - ";
                //fr if(i==FNREC) m+=" fichier son non reconnu";
                if(i==FNREC) m+=" unrecognized sound file";

                //fr if(i==MCNT) m+=" multi-channel non trait�";
                if(i==MCNT) m+=" multi-channel untreated";

                //fr if(i==DTP) m+=" duree trop petite";
                if(i==DTP) m+=" too small duration";

                //fr if(i==DTG) m+=" duree trop grande";
                if(i==DTG) m+=" too long duration";

                //fr  if(i==TNT) m+=" fact. temp. non defini";
                if(i==TNT) m+=" undefined time factor";

                if(nee<netot) m+=" : "+QString::number(nee);
            }
        }
        m+=")";
    }
    return(m);
}


void TadaridaMainWindow::detecFinished(int ithread)
{
    _stockNbTreated[ithread]+=_nbTreated[ithread];
    _stockNbTreatedTotal+=_nbTreated[ithread];
    _stockNbError[ithread]+=_nbError[ithread];
    _stockNbErrorTotal+=_nbError[ithread];
    for(int k=0;k<NTERRORS;k++)
    {
        //_tabError[ithread][k]+=_pDetec[ithread]->_detecTreatment->TabErrors[k];
        _stockTabError[ithread][k]+=_pDetec[ithread]->PDetecTreatment->TabErrors[k];
    }
    fusionErrors(ithread);
}

void TadaridaMainWindow::updateProgBarValue(float av)
{
    _prgProgression->setValue(av*10000);
}

void TadaridaMainWindow::on_btnOpenBase_clicked()
{
    if(!manageBase(true,_modifyFreqAuthorized)) {exitProgram(); return;}
    updateBaseVariables();
}

QString TadaridaMainWindow::selectBase()
{
    QString basePath  = QFileDialog::getExistingDirectory( this,
                            tr("Choisir le dossier de la base"),
                            _baseDir.path(),
                            QFileDialog::ShowDirsOnly);
    return(basePath);
}

void TadaridaMainWindow::on_btnOpenWav_clicked()
{
    //fr QString wavFile  = QFileDialog::getOpenFileName( this, tr("Choisir un fichier wav"),
    QString wavFile  = QFileDialog::getOpenFileName( this,"Select the database folder",
                                                   _wavDirectory.path(), "(*.wav)");
    if(!wavFile.isEmpty()) openWavTag(wavFile);
}

void TadaridaMainWindow::on_btnOpenPreviousWav_clicked()
{
    QString wavFile  = _previousWav;
    if(!wavFile.isEmpty()) openWavTag(wavFile);
    //fr else QMessageBox::warning(this, "Attention", "Premier fichier atteint", QMessageBox::Ok);
    else QMessageBox::warning(this, "Warning", "First file is already reached", QMessageBox::Ok);
}

void TadaridaMainWindow::on_btnOpenNextWav_clicked()
{
    QString wavFile  = _nextWav;
    if(!wavFile.isEmpty()) openWavTag(wavFile);
    else QMessageBox::warning(this, "Warning", "No next file", QMessageBox::Ok);
}

bool TadaridaMainWindow::openWavTag(QString wavFile)
{
    QString dirName = wavFile.left(wavFile.lastIndexOf(QString("/")));
    QString wavFileName  = wavFile.right(wavFile.length()-dirName.length()-1);
    QString wavShortName = wavFileName.left(wavFileName.length()-4);
    //
    QString da2File = dirName + "/dat/"+wavShortName + ".da2";
    if(!QFile::exists(da2File))
    {
        //fr QMessageBox::warning(this, "Fichier .da2 inexistant", da2File, QMessageBox::Ok);
        QMessageBox::warning(this, ".da2 file is missing", da2File, QMessageBox::Ok);
        return(false);
    }

    QString imaFile = dirName + "/ima/"+wavShortName + ".jpg";
    if(!QFile::exists(imaFile))
    {
        //fr QMessageBox::warning(this, "Attention", tr("Fichier image inexistant !"), QMessageBox::Ok);
        QMessageBox::warning(this, "Warning", "Image file is missing !", QMessageBox::Ok);
        return(false);
    }
    QString txtFile = dirName + "/txt/"+wavShortName + "."+ResultSuffix;
    if(!QFile::exists(txtFile))
    {
        QMessageBox::warning(this, "Warning",".ta file is missing !", QMessageBox::Ok);
        return(false);
    }
    if(consistencyCheck(wavFileName,da2File,txtFile)==false)
    {
        QMessageBox::warning(this, "Error", ".ta and .da2 file do not match : retreat the folder !", QMessageBox::Ok);
        return(false);
    }
    showPicture(dirName,wavShortName,true);
    if(_baseDay.exists()) DayPath = _baseDay.path();
    _lastWav = wavFile;
    QStringList wavsoundslist = _wavDirectory.entryList(QStringList("*.wav"), QDir::Files);
    int indice=-1,i=0;
    int nfic=wavsoundslist.size();
    foreach(QString wf, wavsoundslist)
    {
        if(wf==wavFileName) {indice=i; break;}
        i++;
    }
    _wavDirectory = QDir(dirName);
    _previousWav = QString("");
    _nextWav = QString("");
    if(indice>=0)
    {
        if(indice>0)  _previousWav = dirName + QString("/") + wavsoundslist.at(indice-1);
        if(indice<nfic-1) _nextWav = dirName + QString("/") + wavsoundslist.at(indice+1);
    }
    return(true);
}

void TadaridaMainWindow::on_btnFind_clicked()
{
    if(_isRechercheOpen==true) {delete _precherche; _isRechercheOpen=false;}
    _precherche  = new Recherche(this);
    _precherche->afficher_ecran();
    _precherche->show();
    _isRechercheOpen=true;
}

void TadaridaMainWindow::on_btnUpdateTags_clicked()
{
    //fr QString wavFile  = QFileDialog::getOpenFileName( this, tr("Choisir un fichier wav de la base"),
    QString wavFile  = QFileDialog::getOpenFileName( this, "Select a sound file in the database",
                                                   DayPath, "(*.wav)");
    if(!wavFile.isEmpty()) UpdateTags(wavFile);
}

void TadaridaMainWindow::UpdateTags(QString wavFile)
{
    QString dirName = wavFile.left(wavFile.lastIndexOf(QString("/")));
    QString wavShortName  = wavFile.right(wavFile.length()-dirName.length()-1);
    wavShortName = wavShortName.left(wavShortName.length()-4);
    QString datFile = dirName + "/dat/"+wavShortName + ".da2";
    if(!QFile::exists(datFile))
    {
        QMessageBox::warning(this, ".da2 file is missing", datFile, QMessageBox::Ok);
        return;
    }
    QString imaFile = dirName + "/ima/"+wavShortName + ".jpg";
    if(!QFile::exists(imaFile))
    {
        QMessageBox::warning(this, "Error", "Image file is missing !", QMessageBox::Ok);
        return;
    }
    QString etiFile = dirName + "/eti/"+wavShortName + ".eti";
    if(!QFile::exists(etiFile))
    {
        QMessageBox::warning(this,"Error", ".eti file is missing !", QMessageBox::Ok);
        return;
    }
    showPicture(dirName,wavShortName,false);
}

bool TadaridaMainWindow::GetDirectoryType(QString dirName)
{
    QString dirToExamine = dirName.right(dirName.length()-dirName.lastIndexOf(QString("/"))-1);
    bool typeA = true;
    if(dirToExamine.length()==8)
    {
        typeA=false;
        for(int i=0;i<8;i++)
        {
            char c=(char)dirToExamine[i].toLatin1();
            if(c<48 || c>57) {typeA = true; break;}
        }
    }
    return(typeA);
}

void TadaridaMainWindow::showPicture(QString wavDir,QString fileName,bool typeA)
{
    if(_isFenimWindowOpen==true) {delete fenim; _isFenimWindowOpen=false;}
    bool dirType = GetDirectoryType(wavDir);
    bool gettingMode = true;
    // case A : creating of a tag file from a simple wav file
    if(typeA==true)
    {
        if(dirType==false)
        {
            //fr QMessageBox::warning(this, tr("Erreur"), tr("Ne pas choisir par ce bouton un fichier dans la base !"), QMessageBox::Ok);
            QMessageBox::warning(this, "Error", "Do not select a file in the database !", QMessageBox::Ok);
            return;
        }
        if(_dayBaseUpToDate==false)
        {
            //fr QMessageBox::warning(this, "Error", "Choix impossible : retraiter au moins le dossier du de la base du jour !", QMessageBox::Ok);
            QMessageBox::warning(this, "Error", "Unauthorized choice : retreat at least the database today folder !", QMessageBox::Ok);
            return;

        }
    }
    // case B : updating a tag file of the base
    if(typeA==false)
    {
        if(dirType==true)
        {
            //fr QMessageBox::critical(this, "Error", "Ce n'est pas un fichier de la base !", QMessageBox::Ok);
            QMessageBox::critical(this, "Error", "This file is not part of the database !", QMessageBox::Ok);
            return;
        }
    }
    // --------------------------------------------------------------------------
    readDirectoryVersion(wavDir);
    if(_dirProgramVersion<_programVersion || _dirUserVersion < _userVersion)
    {
        QMessageBox::critical(this, "Error !", "The folder version is late !", QMessageBox::Ok);
        return;
    }
    if(_dirModeFreqVersion != _modeFreq)
    {
        if(_modifyFreqAuthorized)
        {

            //fr if(QMessageBox::question(this, "Type de frequence different !",
            //fr                          "Repondre oui pour modifier type de frequence traite ou non pour renoncer",
            if(QMessageBox::question(this, "Different frequency type !",
                                      "Answer yes to change the frequency type or no to cancel",
                                     QMessageBox::Yes|QMessageBox::No)
                    == QMessageBox::Yes)
            {
                _modeFreq = _dirModeFreqVersion;
                cocheFreq();
                if(_tadaridaMode==ETIQUETAGE)
                {
                    //fr QMessageBox::warning(this, "Base ne correspondant  pas au type de frequence voulu",
                    //fr                     "Choisir un autre dossier de base !",QMessageBox::Ok);
                    QMessageBox::warning(this, "Database with other frequency type",
                                         "Select an other database !",QMessageBox::Ok);
                    if(!manageBase(true,true)) {exitProgram(); return;}
                }
            }
            else return;
        }
        else
        {
            QMessageBox::warning(this,"Unauthorized !","Different frequency typet",QMessageBox::Ok);
            return;
        }
    }
    // --------------------------------------------------------------------------
    fenim = new Fenim(this,wavDir,fileName,_baseDay,typeA,false,0,"",_programVersion,_userVersion,_modeFreq);
    if(fenim->ShowFenim(gettingMode)) _isFenimWindowOpen=true;
    if(typeA) _wavDirectory.setPath(wavDir);
}

void TadaridaMainWindow::initializeGlobalParameters()
{
    _detectionThreshold = 26;
    _stopThreshold = 20;
    _minimumFrequency = 0;
    _overlapsNumber = 4;
    Divrl = 3000;
    _useValflag = true;
    _jumpThreshold = 20;
    _widthBigControl = 60;
    _widthLittleControl = 5;
    _highThresholdJB = 9;
    _lowThresholdJB = 7;
    _highThresholdC = 10;
    _lowThresholdC = -1;
    _qR = 5;
    _qN = 10;
    _withTimeCsv = false;
}

bool TadaridaMainWindow::proposeGeneralReprocessing()
{
    bool result = false;
    //fr if(QMessageBox::question(this, "Base en retard !", "Lancer le retraitement de la base ?",
    if(QMessageBox::question(this, "Base is late !", "Launch database retreatment ?",
                             QMessageBox::Yes|QMessageBox::No)
            == QMessageBox::Yes)
    {
        result  = generalReprocessing();
    }
    else
    {
        result =proposeDayReprocessing();
    }
    return(result );
}

bool TadaridaMainWindow::generalReprocessing()
{
    QStringList dayDirectoriesList = _baseDir.entryList(QStringList("*"), QDir::Dirs);
    _tagsNumberBefore=0; _tagsNumberAfter=0; _tagsNumberFinal=0;
    if(dayDirectoriesList.isEmpty())
    {
        //fr QString mess("Base vide - aucun traitement !");
        QString mess("Empty database - no treatment !");
        QMessageBox::warning(this, "Warning", mess, QMessageBox::Ok);
        return(true);
    }
    else
    {
        _logText << "Reprocessing of the database" << endl;
        _logText << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;
        blockUnblock(false);
        _lblTreatedDirectory->setVisible(true);
        foreach(QString dirName,dayDirectoriesList)
        {
            bool authorizedDirName = false;
            if(dirName.length()==8)
            {
                authorizedDirName = true;
                int annee=dirName.left(4).toInt();
                if(annee < 2014 || annee > 2099) authorizedDirName = false;
            }
            if(!authorizedDirName) dayDirectoriesList.removeOne(dirName);
        }
        int nrep=dayDirectoriesList.count();
        if(nrep>0)
        {
            _directoryRoot = _baseDir;
            _directoriesNumber=nrep;
            _directoryIndex=-1;
            _directoriesList=dayDirectoriesList;
            _directoriesRetreatMode = true;
            _isGeneralReprocessing=true;
            _oneDirProblem = false;
            beforeRetreating();
            _clock->start(100);
        }
        else
        {
            _dayBaseUpToDate = true;
             writeBaseVersion();
             blockUnblock(true);
        }
        return(true);
    }
    return(false);
}

void TadaridaMainWindow::beforeRetreating()
{
    QString retreatFilePath("retreat.log");
    RetreatFile.setFileName(retreatFilePath);
    RetreatFile.open(QIODevice::WriteOnly | QIODevice::Text);
    RetreatText.setDevice(&RetreatFile);
    _tagsNumberBefore=0; _tagsNumberAfter=0; _tagsNumberFinal=0;
}

void TadaridaMainWindow::afterRetreating()
{
    RetreatFile.close();
}

bool TadaridaMainWindow::directoryTreat(QDir repToTreat,bool subDirTreat)
{
    QStringList directoriesList("");
    _logText << "___ directorytreat nrep = " << directoriesList.count() << endl;
    _logText << "___ directorytreat reptotreat = " << repToTreat.path() << endl;
    bool premier = true;
    bool dirAuthorized = true;
    if(subDirTreat)
    {
        int ndt=0;
        QString drep,ajrep,ajrep2;
        while(ndt<directoriesList.count())
        {
            drep = repToTreat.path();
            ajrep = directoriesList.at(ndt);
            if(!premier) drep += "/" + ajrep;
            QDir drt(drep);
            QStringList srepList=drt.entryList(QStringList("*"), QDir::Dirs);
            foreach(QString dn,srepList)
            {
                if(dn!="." && dn!=".." && dn!="ima" && dn!="txt" && dn!="dat")
                {
                    if(ajrep=="") ajrep2 = dn;
                    else ajrep2 = ajrep +  '/' + dn;
                    directoriesList << ajrep2;
                    if(!GetDirectoryType(dn)) {dirAuthorized = false; break;}
                }
            }
            QStringList wavSoundsList = drt.entryList(QStringList("*.wav"), QDir::Files);
            if(wavSoundsList.isEmpty())
            {
                directoriesList.removeAt(ndt);
            }
            else ndt++;
            premier=false;
            if(directoriesList.count()>50) break;
        }
    }
    if(!dirAuthorized)
    {
        QMessageBox::warning(this, "Unauthorized treatment",
                 "Folder in the databse !",QMessageBox::Ok);
        blockUnblock(true);
        return(false);
    }
    blockUnblock(false);
    _ledTreatedDirectory->setVisible(false);
    _lblTreatedDirectory->setVisible(true);
    int dirNumber = directoriesList.count();
    if(dirNumber>0)
    {
        _directoryRoot = repToTreat;
        _directoriesNumber=dirNumber;
        _directoryIndex=-1;
        _directoriesList=directoriesList;
        _directoriesRetreatMode = false;
        _isGeneralReprocessing=false;
        _oneDirProblem = false;
        _nbTreatedTotal = 0;
        _nbErrorTotal = 0;
        _stockNbTreatedTotal = 0;
        _stockNbErrorTotal = 0;
        for(int j=0;j<_maxThreads;j++)
        {
            _stockNbTreated[j] = 0;
            _stockNbError[j] = 0;
            _nbTreated[j] = 0;
            _nbError[j] = 0;
            for(int k=0;k<NTERRORS;k++) _stockTabError[j][k]=0;
        }
        _clock->start(100);
    }
    else
    {
        QMessageBox::warning(this, "No treatment",
                 "No wav file !",QMessageBox::Ok);
        blockUnblock(true);
        return(false);
    }
    return(true);
}

void TadaridaMainWindow::clearThings()
{
    blockUnblock(true);
    _prgProgression->setValue(0);
    _lblPhase1Message->SetText("");
    _lblPhase1Message2->SetText("");
    _lblPhase1Message3->SetText("");
}

void TadaridaMainWindow::manageDetecCall()
{
    bool launch = false;
    if(_directoryIndex<0)
    {
        launch=true;
        _directoryIndex=0;
    }
    else
    {
        if(countThreadsRunning()==0)
        {
            _directoryIndex++;
            if(_errorFileOpen) {_errorFile.close(); _errorFileOpen=false;}
            if(_directoryIndex>=_directoriesList.count() || _oneDirProblem)
            {
                _clock->stop();
                clearThings();
                //blockUnblock(true);
                _ledTreatedDirectory->setVisible(true);
                _lblTreatedDirectory->setVisible(false);
                _logText << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;
                if(_directoriesRetreatMode)
                {
                    if(_oneDirProblem)
                    {
                        //fr QMessageBox::warning(this, "Error !","Fichier inaccessible : retraitement interrompu !",
                        QMessageBox::warning(this, "Error !","Unreachable file : retreatment is stopped !",
                                             QMessageBox::Ok);
                    }
                    else
                    {
                        _dayBaseUpToDate = true;
                        QString title;
                        //fr QString mess = QString::number(_tagsNumberAfter)+" etiquettes recuperees sur "+
                        //fr                             QString::number(_tagsNumberBefore)+"  dans le retraitement principal.";
                        QString mess = QString::number(_tagsNumberAfter)+" recovered labels out of "+
                                                     QString::number(_tagsNumberBefore)+"  in the main reprocessing.";
                        if(_tagsNumberFinal > 0)
                            //fr mess += "\nApres recuperation dans les versions precedantes, nombre total d'etiquettes : "
                            mess += "\nAfter recovering in older versions, total number of labels : "
                                    //+QString::number(_tagsNumberFinal)+".";
                                    +QString::number(_tagsNumberAfter+ _tagsNumberFinal)+".";
                        if(_isGeneralReprocessing)
                        {
                            //fr title = "Fin du retraitement general";
                            title = "End of general reprocessingl";
                            writeBaseVersion();
                        }
                        else
                        {
                            //fr title = "Fin du retraitement du dossier du jour";
                            title = "End of today folder reprocessing";
                            writeDirectoryVersion(_baseDay.path());
                        }
                        QMessageBox::warning(this,title,title+" : "+mess, QMessageBox::Ok);
                        RetreatText << title << endl << mess << endl;
                        _logText << mess << endl << title << endl;
                        afterRetreating();
                    }
                }
                else
                {
                    if(_oneDirProblem)
                    {
                        QMessageBox::warning(this, "Error !",
                                             "Unreachable file : treatment is stopped", QMessageBox::Ok);
                        _logText << endl << "Unreachable file : treatment is stopped" << endl;
                    }
                    else
                    {
                        if(!_mustCancel)
                        {
                            QString mess = "",singpur;
                            if((_stockNbTreatedTotal + _stockNbErrorTotal)>0)
                            {
                                if(_stockNbTreatedTotal>0)
                                {

                                    if(_stockNbTreatedTotal>1) singpur ="s"; else singpur = "";
                                    mess += QString::number(_stockNbTreatedTotal) + " treated file"+singpur;
                                }
                                if(_stockNbErrorTotal >0)
                                {
                                    mess += "  -  " + createMessageErrors(_stockNbErrorTotal,_stockTabError)+"   (see error.log)";
                                }
                                QMessageBox::warning(this,"End of treatment", mess, QMessageBox::Ok);
                                _logText << endl << mess << endl;
                            }
                        }
                    }

                }
                if(_mustEnd) close();
            }
            else
            {
                if(!_mustCancel)
                {
                    launch=true;
                }
                else
                {
                    _clock->stop();
                    clearThings();
                    if(_mustEnd && !_directoriesRetreatMode) close();
                    blockUnblock(true);
                }
            }
        }
    }
    if(launch==true)
    {
        QString dirName = _directoriesList.at(_directoryIndex);
        _lblPhase1Message->SetText(QString("Treated folder :  ")+dirName);
        _ledTreatedDirectory->setVisible(false);
        _lblTreatedDirectory->setVisible(true);
        QString dirToShow = dirName;
        if(dirToShow=="") dirToShow = _directoryRoot.path();
        int pos = dirToShow.lastIndexOf("/");
        if(pos>0) dirToShow = dirToShow.right(dirToShow.length()-pos-1);
         _lblTreatedDirectory->SetText(dirToShow);
        _prgProgression->setValue(0);
        dirTreat(dirName);
    }
}


void  TadaridaMainWindow::fusionErrors(int ithread)
{
    if(_errorFileOpen)
    {
        if(_nbError[ithread]>0)
        {
            if(_pDetec[ithread]->ErrorFileOpen)
            {
                _pDetec[ithread]->ErrorFileOpen = false;
                _pDetec[ithread]->ErrorFile.close();
                if(_pDetec[ithread]->ErrorFile.open(QIODevice::ReadOnly | QIODevice::Text))
                {
                    while(true)
                    {
                        QString line = _pDetec[ithread]->ErrorStream.readLine();
                        if(line.isNull() || line.isEmpty()) break;
                        _errorStream << line << endl;
                    }
                    _pDetec[ithread]->ErrorFile.close();
                }
                _pDetec[ithread]->ErrorFile.remove();
            }
        }
        else
        {
            if(_pDetec[ithread]->ErrorFileOpen) _pDetec[ithread]->ErrorFile.remove();
        }
    }
}

bool TadaridaMainWindow::proposeDayReprocessing()
{
    if(!_baseDay.exists())
    {
        _dayBaseUpToDate = true;
        return(true);
    }
    else
    {
        readDirectoryVersion(_baseDay.path());
        if(_dirProgramVersion == _programVersion && _dirUserVersion == _userVersion)
        {
            _dayBaseUpToDate = true;
            return(true);
        }
    }
    //fr if(QMessageBox::question(this, "Base en retard", "Lancer le retraitement du dossier du jour ?",
    if(QMessageBox::question(this, "Database is late", "Launch the reprocessing of the today folder ?",
                             QMessageBox::Yes|QMessageBox::No)
            == QMessageBox::Yes)
    {
        blockUnblock(false);
        QStringList dayDirectoriesList(_strDay);
        _directoryRoot = _baseDir;
        _directoriesNumber=1;
        _directoryIndex=-1;
        _directoriesList=dayDirectoriesList;
        _isGeneralReprocessing=false;
        _directoriesRetreatMode = true;
        _oneDirProblem = false;
        beforeRetreating();
        _clock->start(100);
    }
    else return(false);
    return(true);
}

bool TadaridaMainWindow::dirTreat(QString dirName)
{
    QString directoryPath = _directoryRoot.path()+"/"+dirName;
    QDir sdir(directoryPath);
    if(_isGeneralReprocessing)
    {
        readDirectoryVersion(directoryPath);
        if(_dirProgramVersion==_programVersion && _dirUserVersion==_userVersion)
        {
            return(true);
        }
    }
    if(sdir.exists())
    {
        _logText << "Treatment of the folder " << sdir.path() << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;
        _lblPhase1Message->SetText(QString("Treatment of the folder  ")+sdir.path()+" in progress");
        detecCall(sdir,_directoriesRetreatMode);
    }
    else
    {
        QMessageBox::warning(this,"Error", QString("Nonexistent folder : ")+directoryPath, QMessageBox::Ok);
        return(true);
    }
    return(true);
}

bool TadaridaMainWindow::detecCall(QDir dirToTreat,bool ReprocessingCase)
{
    _logText << endl <<  "treated folder : " << dirToTreat.path() << endl;
    if(!dirToTreat.exists())
    {
        //fr QMessageBox::critical(this, tr("Erreur"), tr("Le dossier des fichiers WAV n'existe pas !"), QMessageBox::Ok);
        QMessageBox::critical(this, "Error", "Nonexistent wav folder !", QMessageBox::Ok);
        blockUnblock(true);
        return(false);
    }
    _wavPath = dirToTreat.path();
    QStringList wavSoundsList;
    wavSoundsList = dirToTreat.entryList(QStringList("*.wav"), QDir::Files);
    if(wavSoundsList.isEmpty())
    {
        if(!ReprocessingCase)
        {
            // QString mess = QString("Il n'y a aucun fichier WAV dans le dossier ")+dirToTreat.path()+" !";
            QString mess = QString("No wav file in the folder : ")+dirToTreat.path()+" !";
            QMessageBox::critical(this, "Error", mess, QMessageBox::Ok);
            blockUnblock(true);
            return(false);
        }
    }
    bool generateImagesDat = false;
    if(_tadaridaMode==ETIQUETAGE)
    {
        if(_chkCreateImage->isChecked() || ReprocessingCase) generateImagesDat=true;
    }
    if(ReprocessingCase) previousVersionSave(wavSoundsList,dirToTreat.path());
    _logText << "before launching detec" << endl;
    _nbThreadsLaunched = _maxThreads;
    _filesNumber = wavSoundsList.size();
    if(_filesNumber < _nbThreadsLaunched) _nbThreadsLaunched = _filesNumber;
    //
    initThreadsLaunched(_nbThreadsLaunched);
    //
    int c=0;
    for(int j=0;j<_filesNumber;j++)
    {
        _pWavFileList[c].append(wavSoundsList.at(j));
        c++;
        if(c>=_nbThreadsLaunched) c=0;
    }
    //
    for(int i=0;i<_nbThreadsLaunched;i++)
    {
        if( _pDetec[i]->InitializeDetec(_pWavFileList[i], dirToTreat.path(),
                                   ReprocessingCase,_programVersion,_userVersion,generateImagesDat,_withTimeCsv,_remObject[i],_modeFreq))
        {
            _pDetec[i]->start();
        }
    }
    createErrorFile();
    _logText << "end of detecCall" << endl;
    return(true);
}

bool TadaridaMainWindow::readDirectoryVersion(QString dirpath)
{
    QString baseIniFile = dirpath + _baseIniFile;
    _dirProgramVersion = 0;
    _dirUserVersion = 0;
    _dirModeFreqVersion = 0;
    if(!QFile::exists(baseIniFile)) return(false);
    QSettings settings(baseIniFile, QSettings::IniFormat);
    settings.beginGroup("version");
    _dirProgramVersion = settings.value("log").toInt();
    _dirUserVersion = settings.value("user").toInt();
    _dirModeFreqVersion = settings.value("modeFreq").toInt();
    // if(_dirProgramVersion < 22 && _dirModeFreqVersion == 0) _dirModeFreqVersion = 1;
    if(_dirModeFreqVersion == 0) _dirModeFreqVersion = 1;
    settings.endGroup();
    return(true);
}

bool TadaridaMainWindow::writeDirectoryVersion(QString dirpath)
{
    QString baseIniFile = dirpath + _baseIniFile;
    QSettings settings(baseIniFile, QSettings::IniFormat);
    settings.beginGroup("version");
    settings.setValue("log", QVariant::fromValue(_programVersion));
    settings.setValue("user", QVariant::fromValue(_userVersion));
    settings.setValue("modeFreq", QVariant::fromValue(_modeFreq));
    settings.endGroup();
    return(true);
}

void TadaridaMainWindow::createWindow()
{
    setStyleSheet("background-image : url(tadarida.jpg);");
    _pmx = 2; _pmy=4;
    _margx = 5; _margy = 8;
    if(_tadaridaMode == SIMPLE)
    {
        _lt = 1100;
        _ltc = _lt/2;
    }
    else
    {
        _lt  = 900;
        _ltc = _lt;
    }
    _ht  = 600;
    _lcw = _lt-_pmx;
    _hcw = _ht - _pmy;
    _lg1= (_lcw-_pmx*4)/2;
    _hg1 = _hcw-_pmy*3;
    _lbou = 150; _hbou=30; _lbi = 30; _hbi = 30; _lbou2 = 90;
    // �����
    _hab1 = _hg1/(10-((int)(_tadaridaMode == SIMPLE)));
    int larl5= (_lg1-_margx*4)/5;
    //_hab1 = _hg1/10;
    resize(_ltc, _ht);
    QFont font1("Times New Roman",11);
    QFont font2("Times New Roman",10);
    QFont fontG("Arial",14);
    setMinimumSize(QSize(_ltc-_pmx, _ht-_pmy));
    setFont(font1);
    setMaximumSize(QSize(_ltc+_pmx, _ht+_pmy));
    QIcon icon;
    icon.addFile("bat.png");
    setWindowIcon(icon);
    setIconSize(QSize(10, 10));
    _mainWidget = new QWidget(this);
    _mainWidget->setMinimumSize(QSize(_lcw, _hcw));
    _mainWidget->setMaximumSize(QSize(_lg1+_margx, _hg1+_margy));
    _grpPhase1 = new QGroupBox(_mainWidget);
    _grpPhase1->setGeometry(_pmx,_pmy,_lg1,_hg1);
    _grpPhase1->setFont(font1);
    _grpPhase1->setStyleSheet("background-image : url(none);");
    _leftGroup = new QGroupBox(_grpPhase1);
    _leftGroup->setTitle(QString("Mono or left"));
    _rightGroup = new QGroupBox(_grpPhase1);
    _rightGroup->setTitle(QString("Right"));
    _left10 = new QRadioButton(QString("x 10"),_leftGroup);
   _left1 = new QRadioButton(QString("x 1"),_leftGroup);
   _left0 = new QRadioButton(QString("ignore"),_leftGroup);
   _right10 = new QRadioButton(QString("x10"),_rightGroup);
   _right1 = new QRadioButton(QString("x1"),_rightGroup);
   _right0 = new QRadioButton(QString("ignore"),_rightGroup);
   //
   int larl6,larlb;
   //
   if(_modifyFreqAuthorized)
    {
         larl6= (_lg1-_margx*4- _pmx*4)/9;
         larlb = (_lg1-_margx*4)/3;
        _freqGroup = new QGroupBox(_grpPhase1);
        _freqGroup->setGeometry(_margx+larlb*2+_margx*2,_hab1*2,larlb,(_hbou*3)/2);
        _freqGroup->setTitle(QString("Frequency type"));
        _freqHigh = new QRadioButton(QString("high"),_freqGroup);
        _freqLow = new QRadioButton(QString("low"),_freqGroup);
        _freqHigh->setGeometry(_pmx*2,_hbou/3,larl6,_hbou);
        _freqLow->setGeometry(_pmx*3+(larl6*3)/2,_hbou/3,larl6,_hbou);
        cocheFreq();
    }
    else
    {
        larl6= (_lg1-_margx*3-_pmx*8)/6;
        larlb = (_lg1-_margx*3)/2;
    }
   //
   _leftGroup->setGeometry(_margx,_hab1*2,larlb,(_hbou*3)/2);
   _rightGroup->setGeometry(larlb+_margx*2,_hab1*2,larlb,(_hbou*3)/2);
   _left10->setGeometry(_pmx,_hbou/3,larl6-1,_hbou);
   _left1->setGeometry(_pmx*2+larl6-1,_hbou/3,larl6-3,_hbou);
    _left0->setGeometry(_pmx*2+larl6*2-5,_hbou/3,larl6+2,_hbou);
    _right10->setGeometry(_pmx,_hbou/3,larl6-1,_hbou);
    _right1->setGeometry(_pmx*2+larl6-1,_hbou/3,larl6-3,_hbou);
    _right0->setGeometry(_pmx*2+larl6*2-5,_hbou/3,larl6+2,_hbou);
    if(_timeExpansionLeft==10)  _left10->setChecked(true);
    if(_timeExpansionLeft==1)     _left1->setChecked(true);
    if(_timeExpansionLeft==0)    _left0->setChecked(true);
    if(_timeExpansionRight==10)  _right10->setChecked(true);
    if(_timeExpansionRight==1)    _right1->setChecked(true);
    if(_timeExpansionRight==0)    _right0->setChecked(true);
    _lblWavDirectory = new MyQLabel(_grpPhase1);
    _lblWavDirectory->setGeometry(_margx,_hab1*4,larl5*2,_hbou);
    _lblWavDirectory->setFont(font2);
    _lblTreatedDirectory = new MyQLabel(_grpPhase1);
    _lblTreatedDirectory->setGeometry(_margx*2+larl5*2,_hab1*4,larl5*2,_hbou);
    _lblTreatedDirectory->setFont(font1);
    _ledTreatedDirectory = new QLineEdit(_grpPhase1);
    _ledTreatedDirectory->setGeometry(_margx*2+larl5*2,_hab1*4,larl5*2,_hbou);
    _ledTreatedDirectory->setFont(font1);
    _btnBrowse = new QPushButton(_grpPhase1);
    _btnBrowse->setGeometry(_margx*3+larl5*4,_hab1*4,larl5,_hbou);
    _btnBrowse->setFont(font1);
    int lar51 = _lg1-_margx*3-_lbi;
    _prgProgression = new QProgressBar(_grpPhase1);
    _prgProgression->setGeometry(_margx,_hab1*5,lar51,_hbou);
    _prgProgression->setMaximum(10000);
    _prgProgression->setValue(0);
    _prgProgression->setStyleSheet(QString::fromUtf8(""));
    _prgProgression->setTextVisible(false);
    _prgProgression->setInvertedAppearance(false);
    _prgProgression->setVisible(true);
    _chkSubDirectories = new QCheckBox(_grpPhase1);
    _chkSubDirectories->setGeometry(_margx*2+_lg1/2,_hab1*6,_lg1/2,_hbou);
    _chkSubDirectories->setEnabled(true);
    _chkSubDirectories->setChecked(false);
    _btnOk = new QPushButton(_grpPhase1);
    int mar6 = (_lg1 - _lbou2*4)/5;
    _btnOk->setGeometry(mar6,_hab1*7,_lbou2,_hbou);
    _btnOk->setFont(font1);
    _btnOk->setEnabled(false);
    _btnCancel = new QPushButton(_grpPhase1);
    _btnCancel->setGeometry(mar6*3 + _lbou2*2,_hab1*7,_lbou2,_hbou);
    _btnCancel->setFont(font1);
    _btnCancel->setEnabled(false);
    _lblPhase1Message = new MyQLabel(_grpPhase1);
    _lblPhase1Message->setGeometry(_margx,(_hab1*31)/4,_lg1-_margx*2,_hbou);
    _lblPhase1Message->setFont(font2);
    _lblPhase1Message->setVisible(false);
    _lblPhase1Message2 = new MyQLabel(_grpPhase1);
    _lblPhase1Message2->setGeometry(_margx,(_hab1*17)/2,_lg1-_margx*2,(_hbou*7)/4);
    _lblPhase1Message2->setFont(font2);
    _lblPhase1Message2->setWordWrap(true);
    _lblPhase1Message2->setVisible(false);
    _lblPhase1Message3 = new MyQLabel(_grpPhase1);
    _lblPhase1Message3->setGeometry(_margx,(_hab1*19)/2,_lg1-_margx*2,_hbou);
    _lblPhase1Message3->setFont(font2);
    _lblPhase1Message3->setWordWrap(true);
    _lblPhase1Message3->setVisible(false);
    _lblPhase1Title = new MyQLabel(_grpPhase1);
    _lblPhase1Title->SetText("Treatment of sound files");
    _lblPhase1Title->setFont(fontG);

    _lblPhase1Title->setGeometry(_lg1/4,(_hab1*7)/10,(_lg1*2)/3,_hbou);
    _labelImage = new MyQLabel(_grpPhase1);
    _labelImage->setGeometry(_margx,_margy,_hbou*3,_hbou*3);
    _labelImage->setPixmap(QPixmap("PictoVigieChiro.jpg"));
    _labelImage->show();
    if(_tadaridaMode==ETIQUETAGE)
    {
        _chkCreateImage = new QCheckBox(_grpPhase1);
        _chkCreateImage->setGeometry(_margx,_hab1*6,_lg1/2,_hbou);
        _chkCreateImage->setEnabled(true);
        _chkCreateImage->setChecked(true);
        _btnParameters = new QPushButton(_grpPhase1);
        _btnParameters->setGeometry(_margx,_hab1*3,larl5*2,_hbou);
        _btnParameters->setFont(font1);
        _grpPhase2 = new QGroupBox(_mainWidget);
        _grpPhase2->setGeometry(_pmx*2+_lg1,_pmy,_lg1,_hg1);
        _grpPhase2->setFont(font1);
        _grpPhase2->setStyleSheet("background-image : url(none);");
        int larw2 = (_lg1*3)/4;
        int mxw2 = _lg1/8;
        _lblPhase2Title = new MyQLabel(_grpPhase2);
        _lblPhase2Title->setGeometry(mxw2*3,(_hab1*7)/10,_lg1/3,_hbou);
        _lblPhase2Title->setFont(fontG);
        //fr _lblPhase2Title->setText("Etiquetage");
        _lblPhase2Title->SetText("Labelling");
        _lblBase = new MyQLabel(_grpPhase2);
        _lblBase->setGeometry(mxw2,_hab1*2,larw2,_hbou);
        _lblBase->setFont(font1);
        _lblBase->SetText("Database : ");
        _btnOpenBase = new QPushButton(_grpPhase2);
        _btnOpenBase->setGeometry(mxw2+larw2/3,(_hab1*8)/3,(larw2*2)/3,_hbou);
        _btnOpenBase->setFont(font2);
        _btnOpenWav = new QPushButton(_grpPhase2);
        _btnOpenWav->setGeometry(mxw2,_hab1*4-_hab1/8,larw2,_hbou);
        _btnOpenWav->setFont(font1);
        _btnOpenPreviousWav = new QPushButton(_grpPhase2);
        _btnOpenPreviousWav->setGeometry(mxw2,_hab1*5-_hab1/4,larw2/3,_hbou);
        _btnOpenPreviousWav->setFont(font1);
        _btnOpenNextWav = new QPushButton(_grpPhase2);
        _btnOpenNextWav->setGeometry(mxw2+larw2*2/3,_hab1*5-_hab1/4,larw2/3,_hbou);
        _btnOpenNextWav->setFont(font1);
        _btnUpdateTags = new QPushButton(_grpPhase2);
        _btnUpdateTags->setGeometry(mxw2,_hab1*6,larw2,_hbou);
        _btnUpdateTags->setFont(font1);
        _btnFind = new QPushButton(_grpPhase2);
        _btnFind->setGeometry(mxw2,_hab1*7,larw2,_hbou);
        _btnFind->setFont(font1);
    }
    _sliderThreads = new QSlider(_grpPhase1);
    _sliderThreads->setMinimum(1);
    _sliderThreads->setMaximum(MAXTHREADS);
    _sliderThreads->setValue(_maxThreads);
    _sliderThreads->setOrientation(Qt::Horizontal);
   _sliderThreads->move((larl5*13)/4+_margx*3,_hab1*3);
   _sliderThreads->resize((larl5*7)/4,_hbou);
   _sliderThreads->setValue(_maxThreads);
   _lblThreads = new MyQLabel(_grpPhase1);
   _lblThreads->setFont(font2);
   _lblThreads->setGeometry(_margx*2+(larl5*9)/4,_hab1*3,larl5,_hbou);
   _lblThreads->setText(QString("Parallelism : ")+QString::number(_maxThreads));
   updatesTexts();
}

void TadaridaMainWindow::updatesTexts()
{
    setWindowTitle("Tadarida");
    //fr _chkSubDirectories->setText("Inclure les sous-dossiers");
    _chkSubDirectories->setText("Include subfolders");
    //fr _lblWavDirectory->setText("Dossier des fichiers WAV");
    _lblWavDirectory->SetText("Folder containing wav files");
    // _btnBrowse->setText("Parcourir");
    _btnBrowse->setText("Browse");
    //_btnPause->setText(QString());
    _btnOk->setText("Treat");
    _btnCancel->setText("Cancel");
    if(_tadaridaMode==ETIQUETAGE)
    {
        //fr _btnParameters->setText(" Modifier les variables");
         _btnParameters->setText(" Modify variables");
        _chkCreateImage->setText("da2 and jpg files");
        _btnOpenWav->setText("Select a wav file");
        //fr _btnOpenBase->setText("Modifier le dossier de la base");
        _btnOpenBase->setText("Change database folder");
        //fr _btnUpdateTags->setText("Modifier l'Etiquetage d'un fichier");
        _btnUpdateTags->setText("Update file labelling");
        _btnOpenPreviousWav->setText("Previous");
        _btnOpenNextWav->setText("Next");
        _btnFind->setText("Search");
    }
}

bool TadaridaMainWindow::consistencyCheck(QString wavFileName,QString da2FileName,QString txtFileName)
{
    _logText << "consistencyCheck " << endl;
    _logText << da2FileName << endl;
    QFile da2File;
    da2File.setFileName(da2FileName);
    if(da2File.open(QIODevice::ReadOnly)==false)
    {
        _logText << "da2 file unreachable !!" << da2FileName << endl;
        QMessageBox::warning(this, "Error", "da2  file is missing !!!", QMessageBox::Ok);
        return(false);
    }
    QDataStream da2Stream;
    da2Stream.setDevice(&da2File);
    int numver=0,numveruser=0,nbcris;
    da2Stream >> numver;
    da2Stream >> numveruser;
    da2Stream >> nbcris;
    da2File.close();
    QFile txtFile;
    txtFile.setFileName(txtFileName);
    if(txtFile.open(QIODevice::ReadOnly)==false)
    {
        _logText << "ta file is unreachable  !!" << da2FileName << endl;
        QMessageBox::warning(this, "Error", "ta file is unreachable  !!", QMessageBox::Ok);
        return(false);
    }
    QTextStream txtStream;
    txtStream.setDevice(&txtFile);
    txtStream.readLine();
    QString txtLine,fwName;
    int nc=0;
    while(!txtStream.atEnd())
    {
        txtLine =txtStream.readLine();
        fwName = txtLine.section('\t',0,0);
        if(fwName==wavFileName) nc++;
    }
    txtFile.close();
    _logText << "nbcris = " << nbcris << " et nc = " << nc << endl;
    if(nc != nbcris)
    {
        _logText << "consistenceyCheck -->  number of calls in da2 file = " << nbcris << endl;
        _logText << "number of calls in ta file = " << nc << endl;
        return(false);
    }
    return(true);
}

void TadaridaMainWindow::previousVersionSave(QStringList wavFileList,QString wavPath)
{
    _baseDayDir.setPath(wavPath);
    QString nomrepcop = wavPath + "/ver" + QString::number(_dirProgramVersion)+"_"+QString::number(_dirUserVersion);
    QDir basecopie,basecopieDat,basecopieEti,basecopieTxt;
    basecopie.setPath(nomrepcop);
    if(!basecopie.exists())
    {
        basecopie.mkdir(nomrepcop);
        basecopieDat = QDir(nomrepcop+"/dat");
        basecopieEti = QDir(nomrepcop+"/eti");
        basecopieDat.mkdir(basecopieDat.path());
        basecopieEti.mkdir(basecopieEti.path());

        foreach(QString wavFile, wavFileList)
        {
            QString nomfic = wavFile;
            int postab = nomfic.lastIndexOf(".");
            if(postab>0) nomfic = nomfic.left(postab);
            QFile da2File(wavPath + "/dat/" + nomfic + ".da2");
            if(da2File.exists()) da2File.copy(basecopieDat.path()+"/"+nomfic+".da2");
            QFile etiFile(wavPath + "/eti/" + nomfic + ".eti");
            if(etiFile.exists()) etiFile.copy(basecopieEti.path()+"/"+nomfic+".eti");
        }
    }
}

void TadaridaMainWindow::treatDirProblem()
{
    _oneDirProblem = true;
    _logText << "dirProblem message" << endl;
}

void TadaridaMainWindow::initThreads()
{
    _nbThreadsLaunched = 0;
    _nbDetecCreated = 0;
    for(int i=0;i<MAXTHREADS;i++) _tabDetecCreated[i] = false;
}


void TadaridaMainWindow::affectTimeExpansions()
{
    if(_left10->isChecked()) _timeExpansionLeft = 10;
    if(_left1->isChecked()) _timeExpansionLeft = 1;
    if(_left0->isChecked()) _timeExpansionLeft = 0;

    if(_right10->isChecked()) _timeExpansionRight = 10;
    if(_right1->isChecked()) _timeExpansionRight = 1;
    if(_right0->isChecked()) _timeExpansionRight = 0;

}

void TadaridaMainWindow::cocheFreq()
{
    if(_modeFreq==2)  _freqLow->setChecked(true);
    else  _freqHigh->setChecked(true);
}

void TadaridaMainWindow::on_freqLow_toogled(bool checked)
{
    int precModeFreq = _modeFreq;
    if(checked) _modeFreq = 2; else _modeFreq = 1;
    if(_modeFreq != precModeFreq && _tadaridaMode==ETIQUETAGE)
    {
        QMessageBox::warning(this, "Database with an other frequency type",
                             "Select an other database !",QMessageBox::Ok);
        if(!manageBase(true,!_modifyFreqAuthorized)) {exitProgram(); return;}
    }
}

void TadaridaMainWindow::initThreadsLaunched(int nbLaunched)
{
    _nbThreadsLaunched = nbLaunched; // peut-�tre inutile : � voir ensuite
    if(nbLaunched>_nbDetecCreated) _nbDetecCreated= nbLaunched;
    int fh;
    for(int i=0;i<_nbThreadsLaunched;i++)
    {

        if(_tabDetecCreated[i]==false)
        {
            FftRes[i] 		= ( fftwf_complex* ) fftwf_malloc( sizeof( fftwf_complex ) * FFT_HEIGHT_MAX );
            ComplexInput[i]        = ( fftwf_complex* ) fftwf_malloc( sizeof( fftwf_complex ) * FFT_HEIGHT_MAX );
            // _logText << "adresse _complexInput[0]="<<  (qint64)_complexInput[i] << endl;
            for(int k=0;k<6;k++)
            {
                fh = pow(2,7+k);
                Plan[i][k] = fftwf_plan_dft_1d(fh, ComplexInput[i], FftRes[i], FFTW_FORWARD, FFTW_ESTIMATE );
            }
            //
            _pDetec[i] = new Detec(this,i);
            connectDetectSignals(i);
        }
        _pWavFileList[i].clear();
        _nbTreated[i]=0;
        _nbError[i]=0;
        for(int k=0;k<NTERRORS;k++) _tabError[i][k] = 0;
        _threadRunning[i]=true;
        affectTimeExpansions();
        _pDetec[i]->PDetecTreatment->SetGlobalParameters(_modeFreq,_timeExpansionLeft,_timeExpansionRight,_detectionThreshold,_stopThreshold,
                     _minimumFrequency,_overlapsNumber,
                     _useValflag,_jumpThreshold,_widthBigControl,_widthLittleControl,
                     _highThresholdJB,_lowThresholdJB,_lowThresholdC,_highThresholdC,_qR,_qN,_paramVersion,_desactiveCorrectNoise);
    }
}

void TadaridaMainWindow::createErrorFile()
{
    QString errorFilePath(_wavPath + "/txt/error.log");
    _errorFile.setFileName(errorFilePath);
    if(_errorFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
    _errorStream.setDevice(&_errorFile);
    _errorFileOpen = true;
    }
    else _errorFileOpen = false;
}


int TadaridaMainWindow::countThreadsRunning()
{

    int nbtr = _nbThreadsLaunched;
    nbtr = 0;
    for(int i=0;i<_nbThreadsLaunched;i++)
    {
        if(_threadRunning[i])
        {
            if(_pDetec[i]->isRunning()) nbtr++;
            else _threadRunning[i] = false;
        }
    }
    // _logText << nbtr << endl;
    return(nbtr);
}

void TadaridaMainWindow::modifyMaxThreads(int nt)
{
    if(nt>=1 && nt <= MAXTHREADS) 
    {
        _maxThreads = nt;
        _lblThreads->setText(QString("Parallelism : ")+QString::number(_maxThreads));
    }
}

void TadaridaMainWindow::connectDetectSignals(int iThread)
{
    connect(_pDetec[iThread], SIGNAL(threadFinished(int)),this, SLOT(detecFinished(int)));
    connect(_pDetec[iThread], SIGNAL(information(QString)),this, SLOT(infoShow(QString)));
    connect(_pDetec[iThread], SIGNAL(information2(QString,bool)),this, SLOT(infoShow2(QString,bool)));
    connect(_pDetec[iThread], SIGNAL(information2b(QString,bool)),this, SLOT(infoShow3(QString,bool)));
    connect(_pDetec[iThread], SIGNAL(information3(int,int,int)),this, SLOT(matchingInfoTreat(int,int,int)));
    connect(_pDetec[iThread], SIGNAL(information4(int,int,int)),this, SLOT(detecInfoTreat(int,int,int)));
    connect(_pDetec[iThread], SIGNAL(dirProblem()),this, SLOT(treatDirProblem()));
}
