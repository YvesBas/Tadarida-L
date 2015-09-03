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
    _fenim1 = new Fenim(_parent,_wavPath,_fileName,_baseDayDir,false,true,1,"1",0,0);
    //_fenim1->m_logStream << "Prematch 2" << endl;
    nbc1=0; nbe1=0;
    if(_fenim1->chargeCrisEtiquettes()==false) return(0);

    //_fenim1->m_logStream << "Prematch 3" << "nb eti=" <<_fenim1->m_nbeti << endl;
    nbc1=_fenim1->m_nbcris;
    nbe1=_fenim1->m_nbeti;
    return(nbe1);
}

int RematchClass::PostMatch(bool initial,QString recupVersion,int *cpma)
{
    nbe2=0;
    nbc2=0;
    TadaridaMainWindow *tgui = (TadaridaMainWindow *)_parent;
    if(initial)
    {
        _fenim2 = new Fenim(_parent,_wavPath,_fileName,_baseDayDir,false,true,2,"2",0,0);
        if(_fenim2->chargeCrisEtiquettes())
            nbc2 = _fenim2->m_nbcris;
        else
        {
            Ok = false;
            return(0);
        }
    }
    if(nbe1>0)
    {
        nbe2 = _fenim2->rematcheEtiquettes(_fenim1,initial,recupVersion,cpma);
    }
    delete _fenim1;
    return(nbe2);
}

int RematchClass::EndMatch()
{
    TadaridaMainWindow *tgui = (TadaridaMainWindow *)_parent;
    _fenim2->EnregEtiquettes();
    delete _fenim2;
    return(0);
}

TadaridaMainWindow::TadaridaMainWindow(QWidget *parent) : QMainWindow(parent)
{
    _logFile.setFileName(QString("tadarida.log"));
    _logFile.open(QIODevice::WriteOnly | QIODevice::Text);
    _logText.setDevice(&_logFile);
    _iniPath = QString("/config/config.ini");
    CanContinue = true;
    MustEnd = false;
    MustCancel = false;
    _wavDirectory   = QDir::current();
    _lastWav = QString("");
    _tadaridaMode = SIMPLE; // mode simple
    _userVersion = 0;
    _programVersion = 21;
    // 21 : version avec enregistrement de _floagGoodCol...
    _baseDir   = QDir::current();
    _paramVersion = 2;
    _tadaridaMode = SIMPLE;
    _maxThreads = 4; // (à remettre à 1)
    // attention _maxTheads pourra bouger par config.ini et aussi en cours de session
    initThreads();
    _transferAuthorized = false;
    _transLastTransfer = true;
    _transDirPath = "";
    TransSuffix = "";
    TransAddress="";
    TransLogin="";
    TransPassword="";
    TransCommand="";
    TransChangeDirectory="";
    TransGetCommand = false;
    //
    _transLastTransferedNumber = 0;
    _transLastFilesNumber = 0;
    _pMessageBox = 0;

    _alreadyMessage = false;
    IDebug = false;
   //
    DayPath = "";
    SearchDir1 = "";
    SearchDir2 = "";
    //
    _timeExpansionLeft = 10;
    _timeExpansionRight = 0;

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
    if(_transferAuthorized) 
	{
		connect(_btnTransfer,SIGNAL(clicked()),this,SLOT(on_btnTransfer_clicked()));
		connect(_btnResume,SIGNAL(clicked()),this,SLOT(on_btnResume_clicked()));
	}

    QObject::connect(_btnCancel, SIGNAL(clicked(bool)), this, SLOT(treatCancel()));
    //connect(_btnPause,SIGNAL(toggled(bool)),this,SLOT(on_btnPause_toggled(bool)));
    _clock = new QTimer(this);
    connect(_clock,SIGNAL(timeout()),this,SLOT(manageDetecCall()));
    setWindowTitle("Tadarida");
    window()->setWindowTitle("Tadarida   version 1.0");
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
            if(testBase()==false)
            {
                if(!CanContinue) return;
                QMessageBox::warning(this, "Base inexitante",
                                     "Choisir le dossier de la base !",QMessageBox::Ok);
                QString basepath = "";
                basepath=selectBase();
                if(basepath.isEmpty())
                {
                    QMessageBox::critical(this, "Arret du programme",
                    "Le mode Etiquetage ne peut fonctionner si on ne chosit pas une base !", QMessageBox::Ok);
                    exitProgram();
                    return;
                }
                _baseDir.setPath(basepath);
                writeConfigFile();
                if(testBase()==false)
                {
                    if(!CanContinue)
                    {
                        _logText << "retour brutal après 2eme testbase negatif" << endl;
                        return;
                    }
                    else
                    {
                        _logText << "enregistrement de la nouvelle base sur testbase negatif" << endl;
                        createBase();

                    }
                }
            }
            QDate dj=QDate::currentDate();
            QString sd=dj.toString("yyyyMMdd");
            _lblBase->setText(QString("Base : ")+_baseDir.path()+" ("+sd+")");
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
    else
    {
        //connect(_comboTemps,SIGNAL(activated(const QString&)), this, SLOT(selectTemps(const QString&)));
    }
    connect(_sliderThreads,SIGNAL(valueChanged(int)),this,SLOT(modifyMaxThreads(int)));

	// todo : utilisation de la possibilité de transférer à mettre sous condition de paramètre dans config.ini
    _transState = TRANSNOTRANS;
    _transParameters = 0;
    if(_transferAuthorized)
    {
        _transCancel = false;
        if(!_transLastTransfer) _btnResume->setEnabled(true);
        _transClock = new QTimer(this);
        connect(_transClock,SIGNAL(timeout()),this,SLOT(manageTransfer()));
    }
    // ---
}

TadaridaMainWindow::~TadaridaMainWindow()
{
    //delete _detec;
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
        QMessageBox::warning(this, "Attention !",
                             QString("Table(s) manquante(s) :")+err+". Etiquetage non permis !",QMessageBox::Ok);
        return(false);
    }
    return(true);
}

void TadaridaMainWindow::updateBaseVariables()
{
    _strDay = QDate::currentDate().toString("yyyyMMdd");
    QString dayPath = _baseDir.path() + "/" + _strDay;
    _baseDay.setPath(dayPath);
    if(_baseDay.exists()) DayPath = dayPath;
}

bool TadaridaMainWindow::testBase()
{
    if(readBaseVersion())
    {
        if(_baseProgramVersion != _programVersion || _baseUserVersion != _userVersion)
        {
            if(_baseProgramVersion > _programVersion || _baseUserVersion > _userVersion)
            {
                _logText << "versions incoherentes" << endl;
                QMessageBox::critical(this, "Arret du programme",
                "La base a une version en avance sur le logiciel !", QMessageBox::Ok);
                exitProgram();
                return(false);
            }
            else
            {
                updateBaseVariables();
                _logText << "version en retard" << endl;
                _baseUpToDate=false;
                _dayBaseUpToDate=false;
                proposeGeneralReprocessing();
            }
        }
        else
        {
            _baseUpToDate=true;
            _dayBaseUpToDate=true;
            updateBaseVariables();
        }
        return(true); // pour indiquer base existante
    }
    else
    {
        _logText << "base impropre : version.ini manquant" << endl;
        return(false);
    }
}

bool TadaridaMainWindow::createBase()
{
    _logText << "creebase appele writebaseversion" << endl;
    //if(!_baseDir.exists()) _baseDir.mkdir(_baseDir.path());
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
    //if(_detec->isRunning())
    if(countThreadsRunning()>0)
    {
        MustEnd = true;
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
        // QMessageBox::warning(this, tr("Changement de version"), tr("Version logiciel différente dans config.ini : rectification "), QMessageBox::Ok);
        _logText << "Changement de version - verlog=" << verlog << endl;
        _logText << "programVersion=" << _programVersion << endl;
    }
    _userVersion = settings.value("user").toInt();
    int mode = settings.value("mode").toInt();
    if(mode==ETIQUETAGE) _tadaridaMode = ETIQUETAGE;
    else _tadaridaMode = SIMPLE;
    _paramVersion = settings.value("paramVersion").toInt();
    _timeExpansionLeft = settings.value("timeExpansionLeft").toInt();
    _timeExpansionRight = settings.value("timeExpansionRight").toInt();
    if(_timeExpansionLeft == 0 && _timeExpansionRight==0) _timeExpansionLeft = 10;
    //
    int transferAuthorized = settings.value("transfer").toInt();
    if(transferAuthorized == 1) _transferAuthorized  = true;
    else _transferAuthorized  = false;
    _transLastTransfer= settings.value("transLastTransfer").toInt();
    _transDirPath = settings.value("transDirPath").toString();
    TransSuffix = settings.value("transSuffix").toString();
    TransAddress = settings.value("transAddress").toString();
    TransLogin = settings.value("transLogin").toString();
    TransPassword = settings.value("transPassword").toString();
    if(TransLogin.isEmpty()) StockLogin = false; else StockLogin = true;
    if(TransPassword.isEmpty()) StockPassword = false; else StockPassword = true;
    //
    int transCommand = settings.value("transCommand").toInt();
    if(transCommand == 1) TransGetCommand  = true;
    else TransGetCommand  = false;
    //
    TransChangeDirectory = settings.value("transChangeDirectory").toString();
    //
    _transLastTransferedNumber = settings.value("transLastTransferedNumber").toInt();
    _transLastFilesNumber = settings.value("transLastFilesNumber").toInt();
    //
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
    else QMessageBox::warning(this, tr("Attention"), tr("Fichier config.ini inaccessible en Ecriture : fermer ce fichier s'il est ouvert !"), QMessageBox::Ok);
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

    settings.setValue("transfer",QString::number((int)_transferAuthorized));
    settings.setValue("transLastTransfer",QString::number((int)_transLastTransfer));
    settings.setValue("transDirPath",_transDirPath);
    settings.setValue("transSuffix",TransSuffix);
    settings.setValue("transAddress",TransAddress);
    if(StockLogin) settings.setValue("transLogin",TransLogin);
    else settings.setValue("transLogin","");
    if(StockPassword) settings.setValue("transPassword",TransPassword);
    else settings.setValue("transPassword","");
    //settings.setValue("transCommand",TransCommand);
    settings.setValue("transCommand",QString::number((int)TransGetCommand));
    settings.setValue("transChangeDirectory",TransChangeDirectory);
    //
    settings.setValue("threadNumber",QString::number(_maxThreads));
    //
    settings.setValue("transLastTransferedNumber",QString::number((int)_transLastTransferedNumber));
    settings.setValue("transLastFilesNumber",QString::number((int)_transLastFilesNumber));
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
        _logText << "rbv retour fichier ini " << cbase << "n'existe pas !" << endl;
        return(false);
    }
    QSettings settings(_baseDir.path()+ _baseIniFile, QSettings::IniFormat);
    settings.beginGroup("version");
    _baseProgramVersion = settings.value("log").toInt();
    _baseUserVersion = settings.value("user").toInt();
    settings.endGroup();
    if(_baseProgramVersion < 0 || _baseUserVersion<0)
    {
        _logText << "rbv bpv<0 ou buv<0" << endl;
        return(false);
    }
    else return(true);
}

void TadaridaMainWindow::writeBaseVersion()
{
    QSettings settings(_baseDir.path()+ _baseIniFile, QSettings::IniFormat);
    _logText << "writebaseversion dans rep. " << _baseDir.path() << endl;
    settings.beginGroup("version");
    settings.setValue("log", QVariant::fromValue(_programVersion));
    settings.setValue("user", QVariant::fromValue(_userVersion));
    settings.endGroup();
}

void TadaridaMainWindow::on_btnParam_clicked()
{
    transStopAutomaticWait();
    /*
    QFile wavFile("c:/wav/wav10000/o.wav");
    if(wavFile.exists())
    {
        for(int i=100001;i<=110000;i++)
            wavFile.copy("c:/wav/wav10000/o"+QString::number(i)+".wav");
        return;
    }
    */
    param = new Param(this,17);
    param->afficher_ecran();
    QStringList lnbo;
    lnbo << "1" << "2" << "4" << "8";
    QStringList lte;
    lte << "10" << "1" ;
    // types : 1 entier  2 double 3 booléen 4 entier en combo avec liste en paramètre
    // param->creeParametre(QString("Facteur temp."),&_timeExpansion,4,0,0,0,0,&lte);
    param->creeParametre(QString("Seuil de detection"),&_detectionThreshold,1,10,30);
    param->creeParametre(QString("Seuil d'arret"),&_stopThreshold,1,5,25);
    param->creeParametre(QString("Frequence minimum"),&_minimumFrequency,1,0,20);
    param->creeParametre(QString("p_nbo"),&_overlapsNumber,4,0,0,0,0,&lnbo);
    param->creeParametre(QString("coef. x"),&Divrl,1,100,100000);

    param->creeParametre(QString("Traitement zones de silence"),&_useValflag ,3);

    param->creeParametre(QString("Seuil de silence "),&_jumpThreshold,1,10,50);
    param->creeParametre(QString("Bande verticale large"),&_widthBigControl,1,10,500);
    param->creeParametre(QString("Bande verticale etroite"),&_widthLittleControl,1,1,20);
    param->creeParametre(QString("Seuil haut"),&_highThresholdJB,1,9,20);
    param->creeParametre(QString("Seuil bas"),&_lowThresholdJB,1,-20,9);
    param->creeParametre(QString("Seuil bas 2"),&_lowThresholdC,1,-5,10);
    param->creeParametre(QString("Seuil haut 2"),&_highThresholdC,1,1,30);
    param->creeParametre(QString("Pourcentage q5"),&_qR,1,1,20);
    param->creeParametre(QString("Nb pixel min q5"),&_qN,1,2,10);

    param->creeParametre(QString("Version parametres"),&_paramVersion ,1,0,1);

    param->creeParametre(QString("Fichiers time.csv"),&_withTimeCsv,3);

    if(param->p_nbparam > param->n_param) param->p_nbparam = param->n_param;
    param->show();
}

void TadaridaMainWindow::on_btnBrowse_clicked()
{
    transStopAutomaticWait();
    QString soundsPath  = QFileDialog::getExistingDirectory( this,
                            tr("Choisir le dossier de fichiers wav"),
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
        if(_transferAuthorized)
        {
            _btnTransfer->setEnabled(true);
            //_btnResume->setEnabled(true);
        }
    }
    else
    {
        _btnOk->setEnabled(false);
        if(_transferAuthorized)
        {
            _btnTransfer->setEnabled(false);
            //_btnResume->setEnabled(false);
        }

    }
    _wavDirectory.setPath(txt);
}

void TadaridaMainWindow::on_btnOk_clicked()
{
    transStopAutomaticWait();
    if(!getDirectoryType(_wavDirectory.path()))
    {
        QMessageBox::warning(this, tr("Erreur"), tr("Ne pas lancer ce traitement sur un dossier de la base !"), QMessageBox::Ok);
        blockUnblock(true);
        return;
    }
    MustCancel = false;
    directoryTreat(_wavDirectory,_chkSubDirectories->isChecked());
}

void TadaridaMainWindow::on_btnTransfer_clicked()
{
    transStopAutomaticWait();
    _transNbReprisesAutoConsecutives = 0;
    openTransParameters();
    _transState = TRANSINITIALIZE;
    if(IDebug) _logText << "affecte état INITIALIZE" << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;
    _transClock->start(15);
}

bool TadaridaMainWindow::openTransParameters()
{
    TransParametersResponse = 0;
    if(_transParameters != 0) delete _transParameters;
    _transParameters     = new TransParameters(this);
    _transParameters->showWindow();
}

void TadaridaMainWindow::on_btnResume_clicked()
{
    transStopAutomaticWait();
    _transNbReprisesAutoConsecutives = 0;
    transReprise();
}

void TadaridaMainWindow::transReprise()
{
    initTransfer(true);
    _logText << "Reprise du transfert - " << QDateTime::currentDateTime().toString("hh:mm:ss") << endl;
    _transClock->start(15);
}


void TadaridaMainWindow::commandBegin(int nCommand)
{
    _transBegin =  QDateTime::currentDateTime().toMSecsSinceEpoch();
    _transBegin2 = _transBegin;
    int nuc = _ftp->currentCommand();
    if(IDebug) _logText << "Commande " << nCommand << " nuc= " << nuc << " lancée " << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;

}

void TadaridaMainWindow::messageNonModal(QString title,QString mess)
{
    if(_alreadyMessage) _pMessageBox->close();
   _pMessageBox = new QMessageBox( this );
   //_pMessageBox->setAttribute( Qt::WA_DeleteOnClose );
   _pMessageBox->setStandardButtons( QMessageBox::Ok );
   _pMessageBox->setWindowTitle(title );
   _pMessageBox->setText(mess );
   _pMessageBox->setIcon(QMessageBox::Information);
   _pMessageBox->setWindowModality(Qt::NonModal);
   //msgBox->open( this, SLOT(msgBoxClosed(QAbstractButton*)) );
   _pMessageBox->show();
   _alreadyMessage = true;
}


void TadaridaMainWindow::commandEnd(int nCommand,bool error)
{
	int memoTransState = _transState;
    int nuc = _ftp->currentCommand();
    if(IDebug) _logText << "Commande " << nCommand << " nuc= " << nuc << " terminée " ;
    if(error) 
	{
        if(IDebug) _logText << " avec erreur : " << _ftp->errorString() << " ";
        if(memoTransState == TRANSCONNECT)
        {
            _transState = TRANSERROR;
            if(IDebug) _logText << "affecte état ERROR" << endl;
            QString titre = "Echec de la connexion";
            QString info =  _ftp->errorString() + "  -  " + QDateTime::currentDateTime().toString("hh:mm:ss");
            _logText << titre << " : " << info  << endl;
            messageNonModal(titre,info);
        }
        if(memoTransState == TRANSLOGIN)
        {
            _transState = TRANSERROR;
            if(IDebug) _logText << "affecte état ERROR (2)" << endl;
            QString titre = "Echec de l'identification";
            QString info =  _ftp->errorString() + "  -  " + QDateTime::currentDateTime().toString("hh:mm:ss");
            _logText << titre << " : " << info  << endl;
            messageNonModal(titre,info);
        }
        if(memoTransState == TRANSPUT)
        {
            _transState = TRANSERROR;
            if(IDebug) _logText << "affecte état ERROR (3)" << endl;

            QString titre = "Transfert interrompu";
            QString info =  QString("Echec du transfert d'un fichier") + "  -  " + QDateTime::currentDateTime().toString("hh:mm:ss");
            _logText << titre << " : " << info  << endl;
            messageNonModal(titre,info);


            _transFileCursor--;
        }
    }
    else 
	{
        if(IDebug) _logText << " sans erreur " << endl;
        if(memoTransState == TRANSCONNECT)
        {
            _transState = TRANSCONNECTED;
            if(IDebug) _logText << "affecte état CONNECTED" << endl;

            _logText << "Connexion réussie " ;
        }
        if(memoTransState == TRANSDISCONNECT)
        {
            _transState = TRANSFINISH;
            if(IDebug) _logText << "affecte état FINISH" << endl;

            _logText << "Déconnexion réussie " ;
        }

        if(memoTransState == TRANSPUT)
        {
            _transNbReprisesAutoConsecutives = 0; // pour le cas où
            _logText << "Fin de transfert du fichier " << _transFile << endl;
            QString singpur = "";
            if(_transFileCursor > 1) singpur = "s";
            QString info = QString("Transféré ") + QString::number(_transFileCursor) + " fichier"+singpur
                    + " sur "  + QString::number(_transFilesNumber);
            //_logText << info << endl;
            infoShow2(info,true);
            _transState = TRANSREADY; // = attente de transfert du prochain fichier
            if(IDebug) _logText << "affecte état READY" << endl;

        }
        if(memoTransState == TRANSLOGIN)
        {
            _logText << "Login réussi " << endl;
            if(!TransCommand.isEmpty())
            {
                _transState = TRANSBEFORECOMMAND;
                if(IDebug) _logText << "affecte état BEFORECOMMAND" << endl;

            }
            else
            {
                if(!TransChangeDirectory.isEmpty())
                {
                    _transState = TRANSBEFORECD;
                    if(IDebug) _logText << "affecte état BEFORECD" << endl;
                }
                else
                {
                    _transState = TRANSREADY;
                    if(IDebug) _logText << "affecte état READY (2)" << endl;
                }

            }
        }
    }
    if(memoTransState==TRANSCOMMAND)
    {
        if(!TransChangeDirectory.isEmpty())
        {
            _transState = TRANSBEFORECD;
            if(IDebug) _logText << "affecte état BEFORECD (2)" << endl;

        }
        else
        {
            _transState = TRANSREADY;
            if(IDebug) _logText << "affecte état READY (3)" << endl;
        }
    }
    if(memoTransState==TRANSCHANGEDIRECTORY)
    {
        _transState = TRANSREADY;
        if(IDebug) _logText << "affecte état READY (4)" << endl;
    }
    if(IDebug) _logText << "  " << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;
}

void TadaridaMainWindow::infoSend(qint64 ns,qint64 nt)
{
    _transSent = (int) ns;
}

void TadaridaMainWindow::transStopAutomaticWait()
{
    if(!_transferAuthorized) return;
    if(_transState == TRANSATTENTEREPRISE)
    {
        _transState = TRANSNOTRANS;
        _transClock->stop();
    }
}

void TadaridaMainWindow::manageTransfer()
{
    _transTime =  QDateTime::currentDateTime().toMSecsSinceEpoch() - _transBegin;
    //
    int memoTransState = _transState;
    //
    if(memoTransState == TRANSATTENTEREPRISE)
    {
        _transCountSecondes ++;
        if(_transCountSecondes>=60)
        {
            _transCountMinutes ++;
            _transCountSecondes = 0;
            if(_transCountMinutes >=_transMinutesSeuil)
            {
                _transNbReprisesAutoConsecutives++;
                QString info = QString("Reprise automatique  (") + QString::number(_transNbReprisesAutoConsecutives)+") ";
                infoShow(info);
                QString titre = "Transfert relancé";
                info += "  -  " + QDateTime::currentDateTime().toString("hh:mm:ss");
                _logText << titre << " : " << info  << endl;
                messageNonModal(titre,info);
                _transClock->stop();
                transReprise();
                return;
            }
        }
        //_logText << "_transMinutesSeuil = " << _transMinutesSeuil << "  _transCountMinutes = " << _transCountMinutes << endl;
        QString info = QString("Reprise automatique  dans ")
                + QString::number(_transMinutesSeuil -_transCountMinutes)+ "mn "
                + QString::number(_transCountSecondes)+ "sec ";
                ;
        infoShow(info);
        _logText << info << " "<< QDateTime::currentDateTime().toString("hh:mm:ss") << endl;
    }
    //
    if(memoTransState == TRANSDISCONNECT)
    {
        if(_transTime > TRANSTIMEOUT2)
        {
            _transState = TRANSFINISH;
if(IDebug) _logText << "affecte état FINISH (2 )" << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;

            QString info;
            _logText << "Timeout sur déconnexion - force arrêt objet ftp" << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;
        }
        else return;
    }
    if(memoTransState == TRANSCONNECT || memoTransState == TRANSLOGIN)
    {
        if(_transTime > TRANSTIMEOUT)
        {
            if(IDebug) _logText << "affecte état ERROR (4) " << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;
            _transState = TRANSERROR;

            QString titre = "Echec de la tentative de transfert";
            QString info;
            if(memoTransState == TRANSCONNECT) info = QString("Echec de la tentative de connexion (timeout) ");
            else info = QString("Echec de l'identification (timeout) ");
            info += "  -  " + QDateTime::currentDateTime().toString("hh:mm:ss");
            _logText << titre << " : " << info  << endl;
            messageNonModal(titre,info);
        }
        else return;
    }

    if(memoTransState == TRANSCOMMAND)
    {
        if(_transTime > TRANSTIMEOUT)
        {
            if(!TransChangeDirectory.isEmpty())
            {
                _transState = TRANSBEFORECD;
                if(IDebug) _logText << "affecte état BEFORECD (3)" << endl;

            }
            else
            {
                _transState = TRANSREADY;
                if(IDebug) _logText << "affecte état READY (4)" << endl;
            }
        }
    }

    if(memoTransState == TRANSCHANGEDIRECTORY)
    {
        if(_transTime > TRANSTIMEOUT)
        {
            _transState = TRANSREADY;
            if(IDebug) _logText << "affecte état READY (5)" << endl;
        }
    }

    if(memoTransState == TRANSPUT)
    {
        if(_transSent != _transSentShown)
        {
            if(_transSent <= _transSize)
            {
                float av = ((float)_transSent)/((float)_transSize);
                updateProgBarValue(av);;
            }
            _transSentShown = _transSent;
            _transBegin2 = QDateTime::currentDateTime().toMSecsSinceEpoch() ;
        }
        else
        {
            _transTime2 =  QDateTime::currentDateTime().toMSecsSinceEpoch() - _transBegin2;
            if(_transTime2 > TRANSTIMEOUT)
            {
                _transState = TRANSERROR;
                if(IDebug) _logText << "affecte état ERROR (5) " << QDateTime::currentDateTime().toString("hh:mm:ss") << endl;

                QString titre = "Transfert interrompu";
                QString info = QString("Transfert interrompu (timeout) !") + "  -  " + QDateTime::currentDateTime().toString("hh:mm:ss");
                _logText << titre << " : " << info  << endl;
                messageNonModal(titre,info);

            }
        }
    }

    if(memoTransState == TRANSERROR || memoTransState == TRANSEND)
	{
        if(IDebug) _logText << "ftp->state =" << _ftp->state() << endl;
        if(_ftp->state()==QFtp::Connected || _ftp->state()==QFtp::LoggedIn)
        {
            _logText << "Déconnexion lancée " << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;
            _transState = TRANSDISCONNECT;
if(IDebug) _logText << "affecte état DISCONNECT (2) " << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;

            _ftp->close();
            return;
        }
        else
        {
            if(IDebug) _logText << "N'était pas connecté " << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;
            _transState = TRANSFINISH;
            if(IDebug) _logText << "affecte état FINISH (3)" << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;

        }

	}
    //
    if(_transState == TRANSFINISH)
    {
        closeTransfer();
        return;
    }
    //
    if(_transState == TRANSFINISHBEFORE)
    {
        closeBeforeTransfer();
        return;
    }
    //
    if(memoTransState == TRANSINITIALIZE)
    {
        if(TransParametersResponse == 1)
        {
            _transState = TRANSBEGIN;
            if(IDebug) _logText << "On a validé les paramètres " << QDateTime::currentDateTime().toString("hh:mm:ss") << endl;
            if(IDebug) _logText << "affecte état BEGIN (1) " << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;
            initTransfer(false);
        }
        if(TransParametersResponse == 2)
        {
            _transState = TRANSFINISHBEFORE;
            if(IDebug) _logText << "affecte BEFOREFINISH" << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;

            _logText << "Faute saisie de paramètres validés, on abandonne " << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;
        }
        return;
    }
    //

    if(memoTransState == TRANSCONNECTED)
    {
        _transState = TRANSLOGIN;
if(IDebug) _logText << "affecte état LOGIN " << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;

        _logText << "Execution du login " << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;
        //_ftp->login( "pluvier.dore","qnaurd7o" );
        _ftp->login( TransLogin,TransPassword);
        return;
    }

    if(memoTransState == TRANSBEFORECOMMAND)
    {
        _transState = TRANSCOMMAND;
        if(IDebug) _logText << "affecte état COMMAND " << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;
        int posesp = TransCommand.indexOf((char)32);
        bool deja=false;
        QString infodeb;
        if(posesp>0)
        {
            QString debut = TransCommand.left(posesp).toLower();
            QString suite = TransCommand.right(TransCommand.length()-posesp-1);
            if(suite.length()>0)
            {
                if(debut=="mkdir")
                {
                    deja=true;
                    _ftp->mkdir(suite);
                    infodeb = QString("exécution de mkdir ") + suite;
                }
                if(debut=="rmdir")
                {
                    deja=true;
                    _ftp->rmdir(suite);
                    infodeb = QString("exécution de rmdir ") + suite;
                }
                if(debut=="remove")
                {
                    deja=true;
                    _ftp->remove(suite);
                    infodeb = QString("exécution de remove ") + suite;
                }
            }
        }
        if(deja==false)
        {
            _ftp->rawCommand(TransCommand);
            infodeb = QString("exécution de ") + TransCommand;
        }
        _logText << infodeb << QDateTime::currentDateTime().toString("hh:mm:ss") << endl;
        infoShow(infodeb);
        return;
    }

    if(memoTransState == TRANSBEFORECD)
    {
        _transState = TRANSCHANGEDIRECTORY;
        if(IDebug) _logText << "affecte état CHANGEDIRECTORY " << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;

        _ftp->cd(TransChangeDirectory);
        _logText << "cd " << TransChangeDirectory << " " <<  QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;
        return;
    }

    if(memoTransState == TRANSSUITE)
    {
        _transState = TRANSCONNECT;
        if(IDebug) _logText << "affecte état CONNECT " << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;

        _logText << "On lance la connexion " << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;
        _ftp->connectToHost(TransAddress);
    }

    if(memoTransState == TRANSREADY)
    {
        if(_transFileCursor < _transFilesNumber)
        {
            if(MustCancel)
            {
                _transCancel = true;
                _transState = TRANSEND;
                if(IDebug) _logText << "affecte état END " << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;

                QString s = QString("Transfert interrompu : transféré ")+QString::number(_transFileCursor)+" fichier(s) sur "
                        +QString::number(_transFilesNumber);
                _logText << s << " " << QDateTime::currentDateTime().toString("hh:mm:ss") << endl;
            }
            else
            {
                _transFile = _transFilesList[_transFileCursor];
                QString pathFile = _wavDirectory.path() + '/' + _transFile;
                _transFileCursor++;
                if(QFile::exists(pathFile))
                {
                    QFile *f = new QFile(pathFile);
                    QString info = QString("Transfert de ") + _transFile ;
                    infoShow(info);
                    _logText << info << " "<< QDateTime::currentDateTime().toString("hh:mm:ss") << endl;
                    _transState = TRANSPUT;
                    if(IDebug) _logText << "affecte état PUT " << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;

                    _transSize = f->size();
                    _transSent = 0;
                    _transSentShown = 0;
                    updateProgBarValue(0.0f);
                    _ftp->put(f,_transFile);
                }
                else _logText << "Fichier non trouvé : " << pathFile << " " << QDateTime::currentDateTime().toString("hh:mm:ss") << endl;
            }
        }
        else
        {
            _logText << "Fin du transfert ! " << QDateTime::currentDateTime().toString("hh:mm:ss") << endl;
            QString s = QString("Transfere ")+QString::number(_transFilesNumber)+" fichiers";
            QMessageBox::warning(this, "Transfert terminé !",s,QMessageBox::Ok);
            _transState = TRANSEND;
            if(IDebug) _logText << "affecte état END (2) " << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;

        }

        return;
    }
}

bool TadaridaMainWindow::initTransfer(bool resume)
{
    _transBegin =  QDateTime::currentDateTime().toMSecsSinceEpoch();
    if(IDebug) _logText << "initTransfer() " << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;
    if(resume)
    {
        QDir lastDir(_transDirPath);
        if(lastDir.exists())
        {
            int cltf=_transLastTransferedNumber;
            // todo : simplifier le cas de la reprise dans la même session :
            // pas besoin de relire le fichier
            if(!readTransferedFiles()) return(false);
            if(_transLastTransferedNumber != cltf)
            {
                QString info = QString("Incohérence détectée dans les informations enregistrées : transLastTransferedNumber avant recalcul = ")
                        +QString::number(cltf) + " # ... après recalcul dans readTransferedFiles : "
                                         + QString::number(_transLastTransferedNumber);
                _logText << info << endl;
                QMessageBox::warning(this, "Opération abandonnée !",info,QMessageBox::Ok);
                return(false);
            }
            _transFilesList = _transLastTransferedFiles;
            _transFileCursor = _transLastTransferedNumber;
            _wavDirectory = lastDir;
            QStringList completeList = lastDir.entryList(QStringList(TransSuffix), QDir::Files);
            for(int i=0;i<completeList.size();i++)
            {
                QString f = completeList[i];
                if(!_transLastTransferedFiles.contains(f)) _transFilesList << f;
            }
        }
        else
        {
            // todo : message...
            QString info = QString("Répertoire ") + _transDirPath + " non trouvé !";
            return(false);
        }
    }
    else
    {
        _transFilesList = _wavDirectory.entryList(QStringList(TransSuffix), QDir::Files);
        //_transFilesList = _wavDirectory.entryList(QStringList("*.wav"), QDir::Files);
        _transFileCursor = 0;
    }
    _transFilesNumber = _transFilesList.size();
    if(!(_transFilesNumber  > _transFileCursor))
    {
        QString info = QString("Aucun fichier à transférer !");
        _logText << info << endl;
        QMessageBox::warning(this, "Opération abandonnée !",info,QMessageBox::Ok);
        return(false);
    }
    //
    _logText << (_transFilesNumber- _transFileCursor) << " fichiers à transferer" << endl;
    _ftp = new QFtp( this );
    connect(_ftp, SIGNAL(commandStarted(int)), SLOT(commandBegin(int)));
    connect(_ftp, SIGNAL(commandFinished(int,bool)), SLOT(commandEnd(int,bool)));
    connect(_ftp, SIGNAL(dataTransferProgress(qint64,qint64)), SLOT(infoSend(qint64,qint64)));
    _logText << "Début de session ftp" << endl;
    _transState = TRANSSUITE;
    if(IDebug) _logText << "affecte état TRANSSUITE" << endl;
    _transCancel = false;
    MustCancel = false;
    blockUnblock(false);
    return(true);
}

void TadaridaMainWindow::closeTransfer()
{
    _transClock->stop();
    delete _ftp;
    _transState = TRANSNOTRANS;
    if(IDebug) _logText << "affecte état NOTRANS (2) " << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;

    //
    _logText << "Fin de session FTP " << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;
    updateProgBarValue(0.0f);
    _transLastFilesNumber = _transFilesNumber;
    _transLastTransferedNumber = _transFileCursor;
    _transDirPath = _wavDirectory.path();
    // if(_transFileCursor < _transFilesNumber)
    if(IDebug) _logText << "closetransfer avant enregTransferedfiles: _transFileCursor = " << _transFileCursor << "_translastTransferedNumber = "
             << _transLastTransferedNumber << endl;
    if(_transFileCursor > 0 && _transFileCursor < _transFilesNumber)
    {
        // enregistrement d'un transfert non terminé à reprendre
        _transLastTransfer = false;
        enregTransferedFiles();
        if(IDebug) _logText << "closetransfer après enregTransferedfiles: _transFileCursor = " << _transFileCursor << "_translastTransferedNumber = "
                 << _transLastTransferedNumber << endl;
        // remise en route timer pour éventuelle reprise automatique
        // TODO ZZZZZ remettre la condition sur non cancel - retiré pour test plus facile
        if(!_transCancel)
        {
            if(_transNbReprisesAutoConsecutives < TRANSMAXREPRISES)
            {
                _transState = TRANSATTENTEREPRISE;
                if(IDebug) _logText << "affecte état ATTENTEREPRISE" << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;
                _transCountMinutes = 0;
                _transCountSecondes = 0;
                int nc = _transNbReprisesAutoConsecutives;
                _transMinutesSeuil = TRANSNBMINUTESREPRISE * (1 + (nc>1) + 2 * (nc>2)  + 4 * (nc>3));
                _transClock->start(1000);
            }
            else
            {
                //_logText << "affecte état ATTENTEREPRISE" << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;
                QString titre = QString("Transfert abandonné !");

                QString info = QString("Nombre maximum de reprises automatiques tenté (!") +QString::number(TRANSMAXREPRISES)+
                        + ")  -  " + QDateTime::currentDateTime().toString("hh:mm:ss");
                _logText << titre << " : " << info  << endl;
                 QMessageBox::warning(this,titre,info,QMessageBox::Ok);
            }
        }
    }
    else _transLastTransfer = true;
    blockUnblock(true);
    _btnResume->setEnabled(!_transLastTransfer);
    return;
}

void TadaridaMainWindow::closeBeforeTransfer()
{
    if(IDebug) _logText << "passe dans closeBeforeTransfer" << endl;
    _transClock->stop();
    _transState = TRANSNOTRANS;
if(IDebug) _logText << "affecte état NOTRANS (3) " << QDateTime::currentDateTime().toString("hh:mm:ss") << endl;

}

void TadaridaMainWindow::enregTransferedFiles()
{
    _transLastTransferedFiles.clear();
    if(_transFileCursor<1)
    {
        _transLastTransfer = true;
        return;
    }
    for(int i = 0;i <_transFileCursor;i++) _transLastTransferedFiles << _transFilesList[i];
    _transLastTransferedNumber = _transLastTransferedFiles.size();
    //
    QString fileTransferedlogFilePath("transfert.log");
    QFile transfertFile;
    transfertFile.setFileName(fileTransferedlogFilePath);
    if(transfertFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream transfertStream;
        transfertStream.setDevice(&transfertFile);
        for(int i=0;i<_transLastTransferedNumber;i++) transfertStream << _transLastTransferedFiles[i] << endl;
        transfertFile.close();
    }
    else
    {
        // close();
    }
    //
}

//$$$%%% début
bool TadaridaMainWindow::readTransferedFiles()
{
    _transLastTransferedFiles.clear();
    //
    QString fileTransferedlogFilePath("transfert.log");
    QFile transfertFile;
    transfertFile.setFileName(fileTransferedlogFilePath);
    int nread=0;
    if(transfertFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream transfertStream;
        transfertStream.setDevice(&transfertFile);
        QString nomfi;
        bool trread = true;
		QString lnf;
        while(trread)
        {
            if(transfertStream.atEnd()) trread=false;
            else
            {
                lnf = (transfertStream.readLine());
                if(lnf.isNull()) trread = false;
                else
                {
                    if(lnf.isEmpty()) trread = false;
                    else
                    {
                        nread++;
                        _transLastTransferedFiles << lnf;
                    }
                }
            }
        }
        _transLastTransferedNumber = _transLastTransferedFiles.size();
        transfertFile.close();
    }
    else
    {
        // close();
        _logText << "Echec lecture transfett.log " << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;
        return(false);
    }
    return(true);
    //
}
//$$$%%% fin

void TadaridaMainWindow::treatCancel()
{
    MustCancel = true;
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
    if(_transferAuthorized)
    {
        _btnTransfer->setEnabled(acdesac);
        _btnResume->setEnabled(acdesac && !_transLastTransfer);
    }
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
    //if(acdesac) MustCancel = false;
}

void TadaridaMainWindow::infoShow(QString mess)
{
    _lblPhase1Message->setText(mess);
}

void TadaridaMainWindow::infoShow2(QString mess,bool withLog)
{
    _lblPhase1Message2->setText(mess);
    if(withLog)  _logText << mess << endl;
}

void TadaridaMainWindow::infoShow3(QString mess,bool withLog)
{
    _lblPhase1Message3->setText(mess);
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
        for(int k=0;k<NTERRORS;k++) _tabError[i][k] =_pDetec[i]->_detecTreatment->TabErrors[k];
    }
    //_logText << "__WWW__ info recue de thread "     << iThread << "nbt= " << nbt   << "    nbTreatedTotal = " << _nbTreatedTotal << endl;
    if(_nbTreatedTotal>0)
    {
        if(_nbTreatedTotal>1) singpur ="s"; else singpur = "";
        mess = QString::number(_nbTreatedTotal) + " fichier"+singpur+" traité"+singpur;
    }
    if(_nbErrorTotal>0)
    {
        mess2 = createMessageErrors(_nbErrorTotal,_tabError);
        /*
        if(_nbErrorTotal>1) singpur =QString("s"); else singpur = QString("");
        mess2 = QString::number(_nbErrorTotal) + " fichier"+singpur+" non traité"+singpur+" (";
        bool ffe = false;
        for(int i=0;i<NTERRORS;i++)
        {
            int nee=0;
            for(int j=0;j<_nbThreadsLaunched;j++) nee += _pDetec[j]->_detecTreatment->TabErrors[i];
            if(nee>0)
            {
                if(ffe==false)   ffe=true; else mess2+=" - ";
                if(i==0) mess2+=" fichier son non reconnu";
                if(i==1) mess2+=" multi-channel non traité";
                if(i==2) mess2+=" durée trop petite";
                if(i==3) mess2+=" durée trop grande";
                if(nee<_nbErrorTotal) mess2+=" : "+QString::number(nee);
            }
        }
        mess2+=")";
        */
        if(_nbTreatedTotal==0) mess = mess2; else mess += " - " + mess2;
    }
    // YYYYYY
    //_logText << "__WWW__ mess = " << mess << endl;
    if(_filesNumber>0)
    {
        //_logText << "__WWWW__ progbar : " << _nbTreatedTotal+_nbErrorTotal << " sur " << _filesNumber << endl;
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
        m = QString::number(netot) + " fichier"+singpur+" non traité"+singpur+" (";
        bool ffe = false;
        for(int i=0;i<NTERRORS;i++)
        {
            int nee=0;
            for(int j=0;j<_nbThreadsLaunched;j++) nee += tabe[j][i];
            if(nee>0)
            {
                if(ffe==false)   ffe=true; else m+=" - ";
                if(i==FNREC) m+=" fichier son non reconnu";
                if(i==MCNT) m+=" multi-channel non traité";
                if(i==DTP) m+=" durée trop petite";
                if(i==DTG) m+=" durée trop grande";
                if(i==TNT) m+=" fact. temp. non defini";
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
        _stockTabError[ithread][k]+=_pDetec[ithread]->_detecTreatment->TabErrors[k];
    }
    fusionErrors(ithread);
}

void TadaridaMainWindow::updateProgBarValue(float av)
{
    _prgProgression->setValue(av*10000);
}

/*
void TadaridaMainWindow::on_btnPause_toggled(bool checked)
{
    transStopAutomaticWait();
    if(checked)
    {
        _logText << "appui sur pause  -  "<< QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;
        //_detec->Pause();
    }
    else
    {
        _logText << "appui sur resume  -  "<< QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;
        //_detec->Resume();
    }
}
*/

void TadaridaMainWindow::on_btnOpenBase_clicked()
{
    transStopAutomaticWait();
    QString basePath  = selectBase();

    if(!basePath.isEmpty() && basePath != _baseDir.path())
    {
        _baseDir.setPath(basePath);
        _lblBase->setText(QString("Base : ")+basePath);
        if(testBase()==false)
        {
            if(!CanContinue)
            {
                _logText << "openbase retour brutal apres testbase negatif" << endl;
                return;
            }
            else
            {
                _logText << "openbase enregistrement de la nouvelle base sur testbase negatif" << endl;
                createBase();

            }
        }
        updateBaseVariables();
        writeConfigFile();
    }
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
    transStopAutomaticWait();
     QString wavFile  = QFileDialog::getOpenFileName( this, tr("Choisir un fichier wav"),
                                                   _wavDirectory.path(), tr("(*.wav)"));
    if(!wavFile.isEmpty()) openWavTag(wavFile);
}

void TadaridaMainWindow::on_btnOpenPreviousWav_clicked()
{
    transStopAutomaticWait();
    QString wavFile  = _previousWav;
    if(!wavFile.isEmpty()) openWavTag(wavFile);
    else QMessageBox::warning(this, "Attention", "Premier fichier atteint", QMessageBox::Ok);
}

void TadaridaMainWindow::on_btnOpenNextWav_clicked()
{
    transStopAutomaticWait();
    QString wavFile  = _nextWav;
    if(!wavFile.isEmpty()) openWavTag(wavFile);
    else QMessageBox::warning(this, "Attention", "Pas de fichier suivant", QMessageBox::Ok);
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
        QMessageBox::warning(this, "Fichier .da2 inexistant", da2File, QMessageBox::Ok);
        return(false);
    }

    QString imaFile = dirName + "/ima/"+wavShortName + ".jpg";
    if(!QFile::exists(imaFile))
    {
        QMessageBox::warning(this, "Attention", tr("Fichier image inexistant !"), QMessageBox::Ok);
        return(false);
    }
    QString txtFile = dirName + "/txt/"+wavShortName + "."+ResultSuffix;
    if(!QFile::exists(txtFile))
    {
        QMessageBox::warning(this, "Attention", tr("Fichier .ta inexistant !"), QMessageBox::Ok);
        return(false);
    }
    if(consistencyCheck(wavFileName,da2File,txtFile)==false)
    {
        QMessageBox::warning(this, "Attention", tr("Fichier .ta et .da2 non concordants : retraiter ce dossier !"), QMessageBox::Ok);
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
    transStopAutomaticWait();
    if(_isRechercheOpen==true) {delete _precherche; _isRechercheOpen=false;}
    _precherche  = new Recherche(this);
    _precherche->afficher_ecran();
    _precherche->show();
    _isRechercheOpen=true;
}

void TadaridaMainWindow::on_btnUpdateTags_clicked()
{
    transStopAutomaticWait();
    QString wavFile  = QFileDialog::getOpenFileName( this, tr("Choisir un fichier wav de la base"),
                                                   DayPath, tr("(*.wav)"));
    if(!wavFile.isEmpty()) updateTags(wavFile);
}

void TadaridaMainWindow::updateTags(QString wavFile)
{
    QString dirName = wavFile.left(wavFile.lastIndexOf(QString("/")));
    QString wavShortName  = wavFile.right(wavFile.length()-dirName.length()-1);
    wavShortName = wavShortName.left(wavShortName.length()-4);
    QString datFile = dirName + "/dat/"+wavShortName + ".da2";
    if(!QFile::exists(datFile))
    {
        QMessageBox::warning(this, "Fichier .da2 inexistant", datFile, QMessageBox::Ok);
        return;
    }
    QString imaFile = dirName + "/ima/"+wavShortName + ".jpg";
    if(!QFile::exists(imaFile))
    {
        QMessageBox::warning(this, "Attention", tr("Fichier image inexistant !"), QMessageBox::Ok);
        return;
    }
    QString etiFile = dirName + "/eti/"+wavShortName + ".eti";
    if(!QFile::exists(etiFile))
    {
        QMessageBox::warning(this, "Attention", tr("Fichier .eti inexistant !"), QMessageBox::Ok);
        return;
    }
    showPicture(dirName,wavShortName,false);
}

bool TadaridaMainWindow::getDirectoryType(QString dirName)
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
    bool dirType = getDirectoryType(wavDir);
    bool gettingMode = true;
    // case A : creating of a tag file from a simple wav file
    if(typeA==true)
    {
        if(dirType==false)
        {
            QMessageBox::warning(this, tr("Erreur"), tr("Ne pas choisir par ce bouton un fichier dans la base !"), QMessageBox::Ok);
            return;
        }
        if(_dayBaseUpToDate==false)
        {
            QMessageBox::warning(this, tr("Erreur"), tr("Choix impossible : retraiter au moins le dossier du de la base du jour !"), QMessageBox::Ok);
            return;

        }
    }
    // case B : updating a tag file of the base
    if(typeA==false)
    {
        if(dirType==true)
        {
            QMessageBox::critical(this, tr("Erreur"), tr("Ce n'est pas un fichier de la base !"), QMessageBox::Ok);
            return;
        }
        readDirectoryVersion(wavDir);
    }
    // --------------------------------------------------------------------------
    fenim = new Fenim(this,wavDir,fileName,_baseDay,typeA,false,0,"",_programVersion,_userVersion);
    if(fenim->afficher_image(gettingMode)) _isFenimWindowOpen=true;
    if(typeA) _wavDirectory.setPath(wavDir);
}

void TadaridaMainWindow::initializeGlobalParameters()
{
    //_timeExpansionLeft=10;
    // _timeExpansionRight=10;
    // bonnes valeurs des seuils = 26 et 20
    // autres pour test retraitement : 25 et 17
    _detectionThreshold = 26;
    _stopThreshold = 20;
    _minimumFrequency = 0;
    _overlapsNumber = 4;
    //_littleParams = false;
    Divrl = 3000;
    //
    _useValflag = true;
    _jumpThreshold = 20;
    _widthBigControl = 60;
    _widthLittleControl = 5;
    _highThresholdJB = 9;
    //_lowThreshold = -4;
    // modifié valeur de _lowThreshold le 31/7 pour débugguage sur fichiers avec zones de silence non détectées
    _lowThresholdJB = 7;
    //
    _highThresholdC = 10;
    _lowThresholdC = -1;
    //
    _qR = 5;
    _qN = 10;
    //_withNewParams = false;
    // mis à true le 27/5/2015 pour que les nx paramètres soient inclus dans un retraitement général
    _withTimeCsv = false;
}

bool TadaridaMainWindow::proposeGeneralReprocessing()
{
    // TODO : I must test if the base is not empty
    bool result = false;
    if(QMessageBox::question(this, "Base en retard !", "Lancer le retraitement de la base ?",
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
        QString mess("Base vide - aucun traitement !");
        QMessageBox::warning(this, tr("Remarque"), mess, QMessageBox::Ok);
        return(true);
    }
    else
    {
        _logText << "Retraitement de la base" << endl;
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
        else return(false);
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
    _logText << "___ directorytreat debut nrep = " << directoriesList.count() << endl;
    _logText << "___ directorytreat debut reptotreat = " << repToTreat.path() << endl;
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
            //_logText << "___ test du répertoire " << drep << endl;
            QDir drt(drep);
            QStringList srepList=drt.entryList(QStringList("*"), QDir::Dirs);
            foreach(QString dn,srepList)
            {
                if(dn!="." && dn!=".." && dn!="ima" && dn!="txt" && dn!="dat")
                {
                    if(ajrep=="") ajrep2 = dn;
                    else ajrep2 = ajrep +  '/' + dn;
                    directoriesList << ajrep2;
                    //_logText << "______ajout de " << ajrep2 << endl;
                    if(!getDirectoryType(dn)) {dirAuthorized = false; break;}
                }
            }
            //_logText << "___ après ajout liste : nrep = " << directoriesList.count() << endl;
            QStringList wavSoundsList = drt.entryList(QStringList("*.wav"), QDir::Files);
            if(wavSoundsList.isEmpty())
            {
                //_logText << "suppression du répertoire " << drep << endl;
                directoriesList.removeAt(ndt);
            }
            else ndt++;
            premier=false;
            if(directoriesList.count()>50) break;
        }
    }
    _logText << "___ directorytreat __2"  << endl;
    if(!dirAuthorized)
    {
        QMessageBox::warning(this, "Traitement impossible",
                 "Dossier de la base : traitement interdit",QMessageBox::Ok);
        blockUnblock(true);
        return(false);
    }
    _logText << "___ directorytreat __3"  << endl;
    blockUnblock(false);
    _logText << "___ directorytreat __4"  << endl;
    _ledTreatedDirectory->setVisible(false);
    _lblTreatedDirectory->setVisible(true);
    int dirNumber = directoriesList.count();
    if(dirNumber>0)
    {
        _logText << "___ avant lancement de l'horloge " << endl;
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
        QMessageBox::warning(this, "Traitement impossible",
                 "Aucun fichier wav !",QMessageBox::Ok);
        blockUnblock(true);
        return(false);
    }
    _logText << "___ directorytreat __fin"  << endl;
    return(true);
}

void TadaridaMainWindow::clearThings()
{
    blockUnblock(true);
    //_ledTreatedDirectory->setVisible(true);
    //_lblTreatedDirectory->setVisible(false);
    _prgProgression->setValue(0);
    _lblPhase1Message->setText("");
    _lblPhase1Message2->setText("");
    _lblPhase1Message3->setText("");
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
        //if(!_detec->isRunning())
        if(countThreadsRunning()==0)
        {
            //_logText << "YYY passe sur countThreadsRunning()==0" << endl;
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
                        QMessageBox::warning(this, tr("Attention !"),"Fichier inaccessible : retraitement interrompu !",
                                             QMessageBox::Ok);
                    }
                    else
                    {
                        _dayBaseUpToDate = true;
                        QString title;
                        QString mess = QString::number(_tagsNumberAfter)+" etiquettes recuperees sur "+
                                                     QString::number(_tagsNumberBefore)+"  dans le retraitement principal.";
                        if(_tagsNumberFinal > 0)
                            mess += "\nApres recuperation dans les versions precedantes, nombre total d'etiquettes : "
                                           +QString::number(_tagsNumberAfter+ _tagsNumberFinal)+".";
                        if(_isGeneralReprocessing)
                        {
                            title = "Fin du retraitement general";
                            writeBaseVersion();
                        }
                        else
                        {
                            title = "Fin du retraitement du dossier du jour";
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
                        QMessageBox::warning(this, tr("Attention"),
                                             "Fichier inaccessible : traitement interrompu", QMessageBox::Ok);
                        _logText << endl << "Traitement interrompu suite probleme d'acces a un fichier" << endl;
                    }
                    else
                    {
                        if(!MustCancel)
                        {
                            QString mess = "",singpur;
                            if((_stockNbTreatedTotal + _stockNbErrorTotal)>0)
                            {
                                if(_stockNbTreatedTotal>0)
                                {

                                    if(_stockNbTreatedTotal>1) singpur ="s"; else singpur = "";
                                    mess += QString::number(_stockNbTreatedTotal) + " fichier"+singpur+" traité"+singpur;
                                }
                                if(_stockNbErrorTotal >0)
                                {
                                    mess += "  -  " + createMessageErrors(_stockNbErrorTotal,_stockTabError)+"   (voir error.log)";
                                    //fusionErrors();
                                }
                                QMessageBox::warning(this,"Fin du traitement", mess, QMessageBox::Ok);
                                _logText << endl << mess << endl;
                            }
                            //_logText << "...mustcancel = false" << endl;
                        }
                        else
                        {
                            //_logText << "...mustcancel = true" << endl;
                        }
                    }

                }
                if(MustEnd) close();
            }
            else
            {
                if(!MustCancel)
                {
                    launch=true;
                }
                else
                {
                    _clock->stop();
                    clearThings();
                    if(MustEnd && !_directoriesRetreatMode) close();
                    blockUnblock(true);
                }
            }
        }
    }
    if(launch==true)
    {
        //QString nomrep="";
        //if(_isGeneralReprocessing) nomrep=_directoriesList.at(_directoryIndex);
        QString dirName = _directoriesList.at(_directoryIndex);
        _lblPhase1Message->setText(QString("Traitement du dossier ")+dirName);
        _ledTreatedDirectory->setVisible(false);
        _lblTreatedDirectory->setVisible(true);
        //_logText << "à traiter : " << dirName << endl;
        QString dirToShow = dirName;
        if(dirToShow=="") dirToShow = _directoryRoot.path();
        int pos = dirToShow.lastIndexOf("/");
        if(pos>0) dirToShow = dirToShow.right(dirToShow.length()-pos-1);
         _lblTreatedDirectory->setText(dirToShow);
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
            if(_pDetec[ithread]->_errorFileOpen)
            {
                _pDetec[ithread]->_errorFileOpen = false;
                _pDetec[ithread]->_errorFile.close();
                if(_pDetec[ithread]->_errorFile.open(QIODevice::ReadOnly | QIODevice::Text))
                {
                    while(true)
                    {
                        QString line = _pDetec[ithread]->_errorStream.readLine();
                        if(line.isNull() || line.isEmpty()) break;
                        _errorStream << line << endl;
                    }
                    _pDetec[ithread]->_errorFile.close();
                }
                _pDetec[ithread]->_errorFile.remove();
            }
        }
        else
        {
            if(_pDetec[ithread]->_errorFileOpen) _pDetec[ithread]->_errorFile.remove();
        }
    }
}

bool TadaridaMainWindow::proposeDayReprocessing()
{
    if(!_baseDay.exists())
    {
        _logText << "Le dossier du jour n'existe pas : proposition de retraitement du dossier du jour inutile " << endl;
        _dayBaseUpToDate = true;
        return(true);
    }
    else
    {
        readDirectoryVersion(_baseDay.path());
        if(_dirProgramVersion == _programVersion && _dirUserVersion == _userVersion)
        {
            _logText << "Le dossier du jour est a jour : proposition de retraitement du repertoire du jour inutile " << endl;
            _dayBaseUpToDate = true;
            return(true);
        }
    }
    if(QMessageBox::question(this, "Base en retard", "Lancer le retraitement du dossier du jour ?",
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
            _logText << "pas de retraitement necessaire pour le dossier " << directoryPath << endl;
            return(true);
        }
    }
    if(sdir.exists())
    {
        _logText << "Retraitement du dossier " << sdir.path() << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;
        _lblPhase1Message->setText(QString("Traitement du dossier ")+sdir.path()+" en cours");
        //if(_directoryIndex>0) sdirReinit();
        detecCall(sdir,_directoriesRetreatMode);
    }
    else
    {
        QMessageBox::warning(this, tr("Erreur"), QString("Dossier ")+directoryPath+" inexistant !", QMessageBox::Ok);
        return(true);
    }
    return(true);
}

bool TadaridaMainWindow::detecCall(QDir dirToTreat,bool ReprocessingCase)
{
    _logText << endl <<  "rep_a_traiter=" << dirToTreat.path() << endl;
    if(!dirToTreat.exists())
    {
        QMessageBox::critical(this, tr("Erreur"), tr("Le dossier des fichiers WAV n'existe pas !"), QMessageBox::Ok);
        blockUnblock(true);
        return(false);
    }
    _wavPath = dirToTreat.path();
    //_logText << "detecCall 1" << endl;
    QStringList wavSoundsList;
    wavSoundsList = dirToTreat.entryList(QStringList("*.wav"), QDir::Files);
    //_logText << "detecCall 2" << endl;
    if(wavSoundsList.isEmpty())
    {
        if(!ReprocessingCase)
        {
            QString mess = QString("Il n'y a aucun fichier WAV dans le dossier ")+dirToTreat.path()+" !";
            QMessageBox::critical(this, tr("Erreur"), mess, QMessageBox::Ok);
            blockUnblock(true);
            return(false);
        }
    }
    //_logText << "detecCall 3" << endl;
	// 5/2/2015 :
    //_logText << "detecCall 4" << endl;
    bool generateImagesDat = false;
    if(_tadaridaMode==ETIQUETAGE)
    {
        if(_chkCreateImage->isChecked() || ReprocessingCase) generateImagesDat=true;
    }
    //_logText << "detecCall 5" << endl;
    if(ReprocessingCase) previousVersionSave(wavSoundsList,dirToTreat.path());
	// 5-2-2015 : retiré dernier paramètre (_withTimer)
    _logText << "avant lancement des detecs" << endl;
    //_btnPause->setEnabled(true);
   //_detec->Treatment();
    //_detec->start();
    // ZZZZ Lancement des detec
    _nbThreadsLaunched = _maxThreads;
// répartition des fichiers et recalcul du nombre de threads
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
                                   ReprocessingCase,_programVersion,_userVersion,generateImagesDat,_withTimeCsv,_remObject[i]))
        {
            _pDetec[i]->start();
        }
    }
    createErrorFile();
    _logText << "detecCall fin" << endl;
    return(true);
}

bool TadaridaMainWindow::readDirectoryVersion(QString dirpath)
{
    QString baseIniFile = dirpath + _baseIniFile;
    _dirProgramVersion = 0;
    _dirUserVersion = 0;
    if(!QFile::exists(baseIniFile)) return(false);
    QSettings settings(baseIniFile, QSettings::IniFormat);
    settings.beginGroup("version");
    _dirProgramVersion = settings.value("log").toInt();
    _dirUserVersion = settings.value("user").toInt();
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
    settings.endGroup();
    return(true);
}

void TadaridaMainWindow::createWindow()
{
    setStyleSheet("background-image : url(tadarida.jpg);");
    _pmx = 4; _pmy=4;
    _margx = 7; _margy = 8;
    if(_tadaridaMode == SIMPLE)
    {
        _lt = 1100;
        _ltc = _lt/2;
    }
    else
    {
        _lt  = 860;
        _ltc = _lt;
    }
    _ht  = 600;
    _lcw = _lt-_pmx;
    _hcw = _ht - _pmy;
    _lg1= (_lcw-_pmx*4)/2;
    _hg1 = _hcw-_pmy*3;
    _lbou = 150; _hbou=30; _lbi = 30; _hbi = 30; _lbou2 = 90;
    // £££££
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

    /*
    _lblTemps = new MyQLabel(_grpPhase1);
    _lblTemps->setFont(font1);
    _lblTemps->setVisible(true);
    _lblTemps->setGeometry(_margx,_hab1*2,larl5,_hbou);
    */
    int larl6= (_lg1-_margx*11)/6;
    int larlb = (_lg1-_margx*3)/2;


    _leftGroup = new QGroupBox(_grpPhase1);
    _leftGroup->setGeometry(_margx,_hab1*2,larlb,(_hbou*3)/2);
    _leftGroup->setTitle(QString("Mono ou gauche"));

    _rightGroup = new QGroupBox(_grpPhase1);
    _rightGroup->setGeometry(larlb+_margx*2,_hab1*2,larlb,(_hbou*3)/2);
    _rightGroup->setTitle(QString("Droit"));

     _left10 = new QRadioButton(QString("x 10"),_leftGroup);
    _left1 = new QRadioButton(QString("x 1"),_leftGroup);
    _left0 = new QRadioButton(QString("ignorer"),_leftGroup);

    _right10 = new QRadioButton(QString("x10"),_rightGroup);
    _right1 = new QRadioButton(QString("x1"),_rightGroup);
    _right0 = new QRadioButton(QString("ignorer"),_rightGroup);

    _left10->setGeometry(_margx,_hbou/3,larl6,_hbou);
    _left1->setGeometry(_margx*2+larl6,_hbou/3,larl6,_hbou);
    _left0->setGeometry(_margx*3+larl6*2,_hbou/3,larl6,_hbou);

    _right10->setGeometry(_margx,_hbou/3,larl6,_hbou);
    _right1->setGeometry(_margx*2+larl6,_hbou/3,larl6,_hbou);
    _right0->setGeometry(_margx*3+larl6*2,_hbou/3,larl6,_hbou);

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
    /*
    _btnPause = new QPushButton(_grpPhase1);
    _btnPause->setGeometry(_lg1-_margx-_lbi,_hab1*4,_lbi,_hbi);
    _btnPause->setFont(font1);
    _btnPause->setEnabled(false);
    _btnPause->setIcon(QIcon("pause.png"));
    _btnPause->setIconSize(QSize(15, 15));
    _btnPause->setCheckable(true);
    */

    _chkSubDirectories = new QCheckBox(_grpPhase1);
    _chkSubDirectories->setGeometry(_margx*2+_lg1/2,_hab1*6,_lg1/2,_hbou);
    _chkSubDirectories->setEnabled(true);
    _chkSubDirectories->setChecked(false);

    _btnOk = new QPushButton(_grpPhase1);
    int mar6 = (_lg1 - _lbou2*4)/5;
    _btnOk->setGeometry(mar6,_hab1*7,_lbou2,_hbou);
    _btnOk->setFont(font1);
    _btnOk->setEnabled(false);

    int decal = (int)(_transferAuthorized == false);
    _btnCancel = new QPushButton(_grpPhase1);
    _btnCancel->setGeometry(mar6*(2+decal) + _lbou2*(1+decal),_hab1*7,_lbou2,_hbou);
    _btnCancel->setFont(font1);
    _btnCancel->setEnabled(false);

    if(_transferAuthorized)
    {
        _btnTransfer = new QPushButton(_grpPhase1);
        _btnTransfer->setGeometry(mar6*3+_lbou2*2,_hab1*7,_lbou2,_hbou);
        _btnTransfer->setFont(font1);
        _btnTransfer->setEnabled(false);
        _btnResume = new QPushButton(_grpPhase1);
        _btnResume->setGeometry(mar6*4+_lbou2*3,_hab1*7,_lbou2,_hbou);
        _btnResume->setFont(font1);
        _btnResume->setEnabled(false);
        // mettre en enabled = true si reprise possible
    }

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
    //
    //
    _lblPhase1Title = new MyQLabel(_grpPhase1);
    _lblPhase1Title->setText("Traitement des fichiers de sons");
    _lblPhase1Title->setFont(fontG);

    _lblPhase1Title->setGeometry(_lg1/4,(_hab1*7)/10,(_lg1*2)/3,_hbou);
    _labelImage = new MyQLabel(_grpPhase1);
    //_labelImage->setGeometry(_margx*3,_margy*3,_hbou*3,_hbou*2);
    _labelImage->setGeometry(_margx,_margy,_hbou*3,_hbou*3);
    _labelImage->setPixmap(QPixmap("PictoVigieChiro.jpg"));
    _labelImage->show();


    if(_tadaridaMode==ETIQUETAGE)
    {
        //_lblPhase1Title->setGeometry(_lg1/6,(_hab1*7)/10,_lg1-_margx*2,_hbou);

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
        _lblPhase2Title->setText("Etiquetage");
        _lblBase = new MyQLabel(_grpPhase2);
        _lblBase->setGeometry(mxw2,_hab1*2,larw2,_hbou);
        _lblBase->setFont(font1);
        _lblBase->setText("Base : ");
        _btnOpenBase = new QPushButton(_grpPhase2);
        _btnOpenBase->setGeometry(mxw2+larw2/3,(_hab1*12)/5,(larw2*2)/3,_hbou);
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
    else
    {
        /*
        _comboTemps = new QComboBox(_grpPhase1);
        _comboTemps->move(_margx*2+larl5,_hab1*2);
        _comboTemps->resize(larl5,_hbou);
        _comboTemps->setFont(font1);
        QStringList lte;
        lte << "10" << "1";
        _comboTemps->insertItems(0,lte);
        */
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
    _lblThreads->setText(QString("Parallelisme : ")+QString::number(_maxThreads));
    updatesTexts();
}

void TadaridaMainWindow::updatesTexts()
{
    setWindowTitle("Tadarida");
    _chkSubDirectories->setText("Inclure les sous-dossiers");
    _lblWavDirectory->setText("Dossier des fichiers WAV");
    _btnBrowse->setText("Parcourir");
    //_btnPause->setText(QString());
    _btnOk->setText("Traiter");
    _btnCancel->setText("Annuler");
    if(_transferAuthorized)
    {
        _btnTransfer->setText("Transferer");
        _btnResume->setText("Reprise");
    }
    if(_tadaridaMode==ETIQUETAGE)
    {
        _btnParameters->setText(" Modifier les variables");
        _chkCreateImage->setText("Fichiers dat et images");
        _btnOpenWav->setText("Choisir un fichier WAV");
        _btnOpenBase->setText("Modifier le dossier de la base");
        _btnUpdateTags->setText("Modifier l'Etiquetage d'un fichier");
        _btnOpenPreviousWav->setText("Precedant");
        _btnOpenNextWav->setText("Suivant");
        _btnFind->setText("Recherche");
    }
}

bool TadaridaMainWindow::consistencyCheck(QString wavFileName,QString da2FileName,QString txtFileName)
{
    _logText << "consistenceyCheck " << endl;
    // lecture du nombre de cris dans le fichier da2
    _logText << " fichier da2 :  " << da2FileName << endl;
    QFile da2File;
    da2File.setFileName(da2FileName);
    if(da2File.open(QIODevice::ReadOnly)==false)
    {
        _logText << "fichier da2 non ouvert !!!" << da2FileName << endl;
        QMessageBox::warning(this, "Fin", "Fichier da2 non ouvert !!!", QMessageBox::Ok);
        return(false);
    }
    QDataStream da2Stream;
    da2Stream.setDevice(&da2File);
    int numver=0,numveruser=0,nbcris;
    da2Stream >> numver;
    da2Stream >> numveruser;
    da2Stream >> nbcris;
    da2File.close();
    _logText << "consistenceyCheck -->  nbre cris dans da2= " << nbcris << endl;
    // lecture du fichier param2.txt
    // lesEtiq[i]->DataFields[ESPECE]=ligne.section('\t',1,1);
    QFile txtFile;
    txtFile.setFileName(txtFileName);
    if(txtFile.open(QIODevice::ReadOnly)==false)
    {
        _logText << "fichier txt non ouvert !!!" << da2FileName << endl;
        QMessageBox::warning(this, "Fin", "Fichier txt non ouvert !!!", QMessageBox::Ok);
        return(false);
    }
    QTextStream txtStream;
    txtStream.setDevice(&txtFile);
    txtStream.readLine();
    QString txtLine,fwName;
    bool onyest=false;
    int nc=0;
    int ij=0;
    while(!txtStream.atEnd())
    {
        txtLine =txtStream.readLine();
        fwName = txtLine.section('\t',0,0);
        if(fwName==wavFileName) nc++;
    }
    txtFile.close();
    _logText << "consistenceyCheck -->  nbre cris dans fichier .ta = " << nc << endl;
    _logText << "nbcris = " << nbcris << " et nc = " << nc << endl;
    if(nc != nbcris) return(false);
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

/*
void TadaridaMainWindow::storeTrace(QString i1,QString i2,QString i3,
QString i4,QString i5,QString i6,QString i7,QString i8,QString i9,QString i10)
{
    _traceText<<i1<<'\t'<<i2<<'\t'<<i3<<'\t'<<i4<<'\t'<<i5<<'\t'
              <<i6<<'\t'<<i7<<'\t'<<i8<<'\t'<<i9<<'\t'<<i10<<endl;
}
*/
void TadaridaMainWindow::treatDirProblem()
{
    _oneDirProblem = true;
    _logText << "reçu un message dirProblem" << endl;
}

/*
void TadaridaMainWindow::selectTemps(const QString& codsel)
{
    _timeExpansion = codsel.toInt();
    if(_timeExpansion != 1 && _timeExpansion !=10) _timeExpansion = 10;
}
*/

void TadaridaMainWindow::initThreads()
{
    _nbThreadsLaunched = 0;
    _nbDetecCreated = 0;
    for(int i=0;i<MAXTHREADS;i++) TabDetecCreated[i] = false;
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

void TadaridaMainWindow::initThreadsLaunched(int nbLaunched)
{
    _nbThreadsLaunched = nbLaunched; // peut-être inutile : à voir ensuite
    if(nbLaunched>_nbDetecCreated) _nbDetecCreated= nbLaunched;
    int fh;
    for(int i=0;i<_nbThreadsLaunched;i++)
    {

        if(TabDetecCreated[i]==false)
        {
            _fftRes[i] 		= ( fftwf_complex* ) fftwf_malloc( sizeof( fftwf_complex ) * FFT_HEIGHT_MAX );
            _complexInput[i]        = ( fftwf_complex* ) fftwf_malloc( sizeof( fftwf_complex ) * FFT_HEIGHT_MAX );
            // _logText << "adresse _complexInput[0]="<<  (qint64)_complexInput[i] << endl;
            for(int k=0;k<6;k++)
            {
                fh = pow(2,7+k);
                Plan[i][k] = fftwf_plan_dft_1d(fh, _complexInput[i], _fftRes[i], FFTW_FORWARD, FFTW_ESTIMATE );
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
        _pDetec[i]->_detecTreatment->SetGlobalParameters(_timeExpansionLeft,_timeExpansionRight,_detectionThreshold,_stopThreshold,
                     _minimumFrequency,_overlapsNumber,
                     _useValflag,_jumpThreshold,_widthBigControl,_widthLittleControl,
                     _highThresholdJB,_lowThresholdJB,_lowThresholdC,_highThresholdC,_qR,_qN,_paramVersion);

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
        _lblThreads->setText(QString("Parallélisme : ")+QString::number(_maxThreads));

    }
}

int TadaridaMainWindow::connectDetectSignals(int iThread)
{
    connect(_pDetec[iThread], SIGNAL(threadFinished(int)),this, SLOT(detecFinished(int)));
    //connect(_pDetec[iThread], SIGNAL(moveBar(float)),this, SLOT(updateProgBarValue(int,float)));
    connect(_pDetec[iThread], SIGNAL(information(QString)),this, SLOT(infoShow(QString)));
    connect(_pDetec[iThread], SIGNAL(information2(QString,bool)),this, SLOT(infoShow2(QString,bool)));
    connect(_pDetec[iThread], SIGNAL(information2b(QString,bool)),this, SLOT(infoShow3(QString,bool)));
    connect(_pDetec[iThread], SIGNAL(information3(int,int,int)),this, SLOT(matchingInfoTreat(int,int,int)));
    connect(_pDetec[iThread], SIGNAL(information4(int,int,int)),this, SLOT(detecInfoTreat(int,int,int)));
    connect(_pDetec[iThread], SIGNAL(dirProblem()),this, SLOT(treatDirProblem()));
}
