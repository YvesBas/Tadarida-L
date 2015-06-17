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
    //tgui->_logText << "postmatch1 "  << _fileName << endl;
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
        //tgui->_logText << "postmatch2 "  << _fileName << endl;
        nbe2 = _fenim2->rematcheEtiquettes(_fenim1,initial,recupVersion,cpma);
        //tgui->_logText << "postmatch3 "  << _fileName << endl;
    }
    else
    {
        //tgui->_logText << "pas de rematchage car pas d'étiquette dans le fichier antérieur' "  << _fileName << endl;
    }
    delete _fenim1;
    //tgui->_logText << "postmatch4 "  << _fileName << endl;
    return(nbe2);
}

int RematchClass::EndMatch()
{
    TadaridaMainWindow *tgui = (TadaridaMainWindow *)_parent;
    //tgui->_logText << "endmatch1 " <<  endl;
    _fenim2->EnregEtiquettes();
    //tgui->_logText << "endmatch2 " <<  endl;
    delete _fenim2;
    //tgui->_logText << "endmatch3 " <<  endl;
    return(0);
}

TadaridaMainWindow::TadaridaMainWindow(QWidget *parent) : QMainWindow(parent)
{

    _logFile.setFileName(QString("tadarida.log"));
    _logFile.open(QIODevice::WriteOnly | QIODevice::Text);
    _logText.setDevice(&_logFile);
    /*
    _traceFile.setFileName(QString("trace.txt"));
    _traceFile.open(QIODevice::WriteOnly | QIODevice::Text);
    _traceText.setDevice(&_traceFile);
    _traceText.setRealNumberNotation(QTextStream::FixedNotation);
    _traceText.setRealNumberPrecision(2);
    _traceText << "Répertoire    "<< '\t'
               << "Fichier       " << '\t'
               << "Sens    " << '\t'
               << "Colonne (x) " << '\t'
               << "Colonne (ms) " << '\t'
               << "Début descendu" << '\t'
               << "Moy. avant (l)"<< '\t'
               << "Moy. avant (L)" << '\t'
               << "Moy. après (l)" << '\t'
               << "Moy. après (L)"
               << endl;
    */

    //
    _iniPath = QString("/config/config.ini");
    CanContinue = true;
    MustEnd = false;
    MustCancel = false;
    _wavDirectory   = QDir::current();
    _lastWav = QString("");
    _tadaridaMode = SIMPLE; // mode simple
    _userVersion = 0;
    _programVersion = 20;
    _baseDir   = QDir::current();
    _paramVersion = 1;
    readConfigFile();
    ResultSuffix = "ta";
    if(_tadaridaMode==ETIQUETAGE)
    {
        _previousWav = QString("");
        _nextWav = QString("");
        _imagesDirectory   = QDir::current();
        DayPath = "";
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
    _remObject = new RematchClass(this);
    _detec = new Detec(this);
    initializeGlobalParameters();
    createWindow();
    _isFenimWindowOpen = false;
    _isRechercheOpen = false;
    connect(_btnBrowse,SIGNAL(clicked()),this,SLOT(on_btnBrowse_clicked()));
    connect(_ledTreatedDirectory,SIGNAL(textChanged(const QString&)),this,SLOT(on_ledTreatedDirectory_textChanged(const QString&)));
    connect(_btnOk,SIGNAL(clicked()),this,SLOT(on_btnOk_clicked()));
    QObject::connect(_btnCancel, SIGNAL(clicked(bool)), this, SLOT(treatCancel()));
    connect(_btnPause,SIGNAL(toggled(bool)),this,SLOT(on_btnPause_toggled(bool)));
    connect(_detec, SIGNAL(threadFinished()),this, SLOT(detecFinished()));
    connect(_detec, SIGNAL(moveBar(float)),this, SLOT(updateProgBarValue(float)));
    connect(_detec, SIGNAL(information(QString)),this, SLOT(infoShow(QString)));
    connect(_detec, SIGNAL(information2(QString)),this, SLOT(infoShow2(QString)));
    connect(_detec, SIGNAL(information3(int,int,int)),this, SLOT(matchingInfoTreat(int,int,int)));
    connect(_detec, SIGNAL(information4(int,int)),this, SLOT(detecInfoTreat(int,int)));
    /*
    connect(_detec, SIGNAL(infoTrace(QString,QString,QString,QString,QString,QString,QString,QString,QString,QString)),
               this, SLOT(storeTrace(QString,QString,QString,QString,QString,QString,QString,QString,QString,QString)));
    */
    connect(_detec, SIGNAL(dirProblem()),this, SLOT(treatDirProblem()));
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
        connect(_comboTemps,SIGNAL(activated(const QString&)), this, SLOT(selectTemps(const QString&)));


    }
}

TadaridaMainWindow::~TadaridaMainWindow()
{
    delete _detec;
    delete _remObject;
    if(_isFenimWindowOpen)
    {
        delete fenim;
    }
    if(_isRechercheOpen) delete _precherche;
    _logText << "tdmc" << endl;
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
    // _logText << "avant test base ouverte" << endl;
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
    //emit endOfMainWindow();
    _logText << "closeevent" << endl;
    if(_detec->IsRunning)
    {
        //_logText << "closeevent - passe mustend à true" << endl;
        MustEnd = true;
        if(!_detec->ReprocessingMode)
        {
            MustCancel = true;
            _detec->MustCancel = true;
        }
        event->ignore();
    }
    else
    {
        _logText << "closeevent - accepte fermeture" << endl;
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
        if(DayPath != "") settings.setValue("jourTraite",DayPath);
    }
    settings.endGroup();
    settings.beginGroup("version");
    settings.setValue("log", QString::number(_programVersion));
    settings.setValue("user",QString::number(_userVersion));
    settings.setValue("mode",QString::number(_tadaridaMode));
    settings.setValue("paramVersion",QString::number(_paramVersion));
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
    //_logText << "rbv cbase=" << cbase << endl;
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
    param->creeParametre(QString("Facteur temp."),&_timeExpansion,4,0,0,0,0,&lte);
    param->creeParametre(QString("Seuil de detection"),&_detectionThreshold,1,10,30);
    param->creeParametre(QString("Seuil d'arret"),&_stopThreshold,1,5,25);
    param->creeParametre(QString("Frequence minimum"),&_minimumFrequency,1,0,20);
    param->creeParametre(QString("p_nbo"),&_overlapsNumber,4,0,0,0,0,&lnbo);
    param->creeParametre(QString("coef. x"),&Divrl,1,100,100000);

    param->creeParametre(QString("Traitement zones de silence"),&_useValflag ,3);

    param->creeParametre(QString("Seuil de silence "),&_jumpThreshold,1,10,50);
    param->creeParametre(QString("Bande verticale large"),&_widthBigControl,1,10,500);
    param->creeParametre(QString("Bande verticale etroite"),&_widthLittleControl,1,1,20);
    param->creeParametre(QString("Seuil haut"),&_highThreshold,1,0,20);
    param->creeParametre(QString("Seuil bas"),&_lowThreshold,1,-20,0);
    param->creeParametre(QString("Pourcentage q5"),&_qR,1,1,20);
    param->creeParametre(QString("Nb pixel min q5"),&_qN,1,2,10);

    param->creeParametre(QString("Version parametres"),&_paramVersion ,1,0,1);

    param->creeParametre(QString("Fichiers time.csv"),&_withTimeCsv,3);

    if(param->p_nbparam > param->n_param) param->p_nbparam = param->n_param;
    param->show();
}

void TadaridaMainWindow::on_btnBrowse_clicked()
{
    QString soundsPath  = QFileDialog::getExistingDirectory( this,
                            tr("Choisir le dossier de fichiers wav"),
                            _wavDirectory.path(),
                            QFileDialog::ShowDirsOnly);

    if(!soundsPath.isEmpty())
        _ledTreatedDirectory->setText(QDir::fromNativeSeparators(soundsPath));
}

void TadaridaMainWindow::on_ledTreatedDirectory_textChanged(const QString &txt)
{
    if(!txt.isEmpty()) _btnOk->setEnabled(true);
    else _btnOk->setEnabled(false);
    _wavDirectory.setPath(txt);
}

void TadaridaMainWindow::on_btnOk_clicked()
{
    if(!getDirectoryType(_wavDirectory.path()))
    {
        QMessageBox::warning(this, tr("Erreur"), tr("Ne pas lancer ce traitement sur un dossier de la base !"), QMessageBox::Ok);
        blockUnblock(true);
        return;
    }
    MustCancel = false;
    directoryTreat(_wavDirectory,_chkSubDirectories->isChecked());
}

void TadaridaMainWindow::treatCancel()
{
    MustCancel = true;
    _detec->MustCancel = true;
}

void TadaridaMainWindow::blockUnblock(bool acdesac)
{
    _ledTreatedDirectory->setEnabled(acdesac);
    _btnBrowse->setEnabled(acdesac);
    _btnOk->setEnabled(acdesac);
    _ledTreatedDirectory->setEnabled(acdesac);
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
    //if(acdesac) MustCancel = false;
}

void TadaridaMainWindow::infoShow(QString mess)
{
    _lblPhase1Message->setText(mess);
}

void TadaridaMainWindow::infoShow2(QString mess)
{
    _lblPhase1Message2->setText(mess);
    _logText << mess << endl;
}

void TadaridaMainWindow::matchingInfoTreat(int nbeav,int nbeap,int nbere)
{
    _tagsNumberBefore += nbeav;
    _tagsNumberAfter  += nbeap;
    _tagsNumberFinal  += nbere;
}

void TadaridaMainWindow::detecInfoTreat(int nbt,int nbe)
{
    QString mess,singpur,mess2;
    _nbTreated =  _stockNbTreated + nbt;
    _nbError = _stockNbError + nbe;
    if(_nbTreated>0)
    {
        if(_nbTreated>1) singpur ="s"; else singpur = "";
        mess = QString::number(_nbTreated) + " fichier"+singpur+" traité"+singpur;
    }
    if(_nbError>0)
    {
        if(_nbError>1) singpur =QString("s"); else singpur = QString("");
        mess2 = QString::number(_nbError) + " fichier"+singpur+" non traité"+singpur;
        if(_nbTreated==0) mess = mess2; else mess += " - " + mess2;
    }
    _lblPhase1Message2->setText(mess);
    _treatDirMess = mess;
}

void TadaridaMainWindow::detecFinished()
{
     _stockNbTreated += _nbTreated;
     _stockNbError += _nbError;
     _nbTreated = 0;
     _nbError = 0;
    _btnPause->setEnabled(false);
    _prgProgression->setFormat("");
    _prgProgression->setValue(10000);
    infoShow("");
    blockUnblock(true);
}

void TadaridaMainWindow::updateProgBarValue(float av)
{
    _prgProgression->setValue(av*10000);
}

void TadaridaMainWindow::on_btnPause_toggled(bool checked)
{
    if(checked)
    {
        _logText << "appui sur pause  -  "<< QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;
        _detec->Pause();
    }
    else
    {
        _logText << "appui sur resume  -  "<< QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;
        _detec->Resume();
    }
}

void TadaridaMainWindow::on_btnOpenBase_clicked()
{
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
    QString wavFile  = QFileDialog::getOpenFileName( this, tr("Choisir un fichier wav"),
                                                   _wavDirectory.path(), tr("(*.wav)"));
    if(!wavFile.isEmpty()) openWavTag(wavFile);
}

void TadaridaMainWindow::on_btnOpenPreviousWav_clicked()
{
    QString wavFile  = _previousWav;
    if(!wavFile.isEmpty()) openWavTag(wavFile);
    else QMessageBox::warning(this, "Attention", "Premier fichier atteint", QMessageBox::Ok);
}

void TadaridaMainWindow::on_btnOpenNextWav_clicked()
{
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
    if(_isRechercheOpen==true) {delete _precherche; _isRechercheOpen=false;}
    _precherche  = new Recherche(this);
    _precherche->afficher_ecran();
    _precherche->show();
    _isRechercheOpen=true;
}

void TadaridaMainWindow::on_btnUpdateTags_clicked()
{
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
    _timeExpansion=10;
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
    _jumpThreshold = 30;
    _widthBigControl = 60;
    _widthLittleControl = 5;
    _highThreshold = 10;
    _lowThreshold = -4;
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
            _clock->start(50);
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
        _nbTreated = 0;
        _nbError = 0;
        _stockNbTreated = 0;
        _stockNbError = 0;

        _clock->start(50);
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
        if(!_detec->IsRunning)
        {
            _directoryIndex++;
            if(_directoryIndex>=_directoriesList.count() || _oneDirProblem)
            {
                _clock->stop();
                blockUnblock(true);
                _ledTreatedDirectory->setVisible(true);
                _lblTreatedDirectory->setVisible(false);
                _logText << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;
                if(_directoriesRetreatMode)
                {
                    if(_oneDirProblem)
                    {
                        QMessageBox::warning(this, tr("Attention !"),
                                             "Fichier inaccessible : retraitement interrompu !",
                                             QMessageBox::Ok);
                    }
                    else
                    {
                        _dayBaseUpToDate = true;
                        RetreatText << _tagsNumberAfter << " etiquettes recuperees sur "
                                    << _tagsNumberBefore << " dans le retraitement principal." <<  endl;
                        if(_isGeneralReprocessing)
                        {
                            _logText << _tagsNumberAfter << " etiquettes recuperees sur "
                                     << _tagsNumberBefore << " dans le retraitement principal." << endl;
                            if(_tagsNumberFinal > 0)
                                RetreatText << "Apres recuperation dans les versions precedantes, nombre total d'etiquettes : "
                                  << _tagsNumberAfter + _tagsNumberFinal <<  endl;
                            writeBaseVersion();
                            _baseUpToDate=true;
                            QMessageBox::warning(this, tr("Remarque"), "Fin du retraitement de la base", QMessageBox::Ok);
                            _logText << endl << "Fin du retraitement de la base" << endl;
                        }
                        else
                        {
                            writeDirectoryVersion(_baseDay.path());
                            QMessageBox::warning(this, tr("Remarque"), "Fin du retraitement du dossier du jour", QMessageBox::Ok);
                            _logText << endl << "Fin du retraitement du dossier du jour" << endl;
                        }
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
                            QString mess = _treatDirMess;
                            if((_nbError+_stockNbError)>0) mess += " (voir error.log)";
                            if((_stockNbTreated+_nbTreated)>0 || (_nbError>0+_stockNbError)>0)
                            QMessageBox::warning(this,"Fin du traitement", mess, QMessageBox::Ok);
                            _logText << endl << mess << endl;
                            //_logText << "...mustcancel = false" << endl;
                        }
                        else
                        {
                            //_logText << "...mustcancel = true" << endl;
                        }
                    }

                }
                _prgProgression->setValue(0);
                infoShow("");
                _lblPhase1Message->setText("");
                _lblPhase1Message2->setText("");
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
         _logText << "après mofifs : dts = " << dirToShow << endl;
        _prgProgression->setValue(0);
        dirTreat(dirName);
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
        _clock->start(50);
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
        detecCall(sdir,_directoriesRetreatMode);
    }
    else
    {
        QMessageBox::warning(this, tr("Erreur"), QString("Dossier ")+directoryPath+" inexistant !", QMessageBox::Ok);
        return(true);
    }
    return(true);
}

bool TadaridaMainWindow::detecCall(QDir dirToReprocess,bool ReprocessingCase)
{
    _logText << "rep_a_traiter=" << dirToReprocess.path() << endl;
    if(!dirToReprocess.exists())
    {
        QMessageBox::critical(this, tr("Erreur"), tr("Le dossier des fichiers WAV n'existe pas !"), QMessageBox::Ok);
        blockUnblock(true);
        return(false);
    }
    _logText << "detecCall 1" << endl;
    QStringList wavSoundsList;
    wavSoundsList = dirToReprocess.entryList(QStringList("*.wav"), QDir::Files);
    _logText << "detecCall 2" << endl;
    if(wavSoundsList.isEmpty())
    {
        if(!ReprocessingCase)
        {
            QString mess = QString("Il n'y a aucun fichier WAV dans le dossier ")+dirToReprocess.path()+" !";
            QMessageBox::critical(this, tr("Erreur"), mess, QMessageBox::Ok);
            blockUnblock(true);
            return(false);
        }
    }
    _logText << "detecCall 3" << endl;
	// 5/2/2015 :
    _detec->_detecTreatment->SetGlobalParameters(_timeExpansion,_detectionThreshold,_stopThreshold,
                 _minimumFrequency,_overlapsNumber,
                 _useValflag,_jumpThreshold,_widthBigControl,_widthLittleControl,
                 _highThreshold,_lowThreshold,_qR,_qN,_paramVersion);
    _logText << "detecCall 4" << endl;
    bool generateImagesDat = false;
    if(_tadaridaMode==ETIQUETAGE)
    {
        if(_chkCreateImage->isChecked() || ReprocessingCase) generateImagesDat=true;
    }
    _logText << "detecCall 5" << endl;
    if(ReprocessingCase) previousVersionSave(wavSoundsList,dirToReprocess.path());
	// 5-2-2015 : retiré dernier paramètre (_withTimer)
    if(_detec->InitializeDetec(wavSoundsList, dirToReprocess.path(),
      ReprocessingCase,_programVersion,_userVersion,generateImagesDat,_withTimeCsv,_remObject))
    {
        _logText << "avant lancement de detec" << endl;
        _btnPause->setEnabled(true);
        _detec->Treatment();
    }
    _logText << "detecCall 6" << endl;
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
    _margx = 8; _margy = 8;
    if(_tadaridaMode == SIMPLE) _lt = 1100;
    else _lt  = 860;
    _ht  = 600;
    _lcw = _lt-_pmx; _hcw = _ht - _pmy;
    _lg1= (_lcw-_pmx*4)/2; _hg1 = _hcw-_pmy*3;
    _lbou = 150; _hbou=30; _lbi = 30; _hbi = 30;
    // £££££
    _hab1 = (_hg1*2)/17;
    //_hab1 = _hg1/9;
    if(_tadaridaMode==SIMPLE) _ltc = _lt/2; else _ltc = _lt;
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

    _lblWavDirectory = new MyQLabel(_grpPhase1);
    int larl5= (_lg1-_margx*4)/5;
    _lblWavDirectory->setGeometry(_margx,_hab1*3,larl5*2,_hbou);
    _lblWavDirectory->setFont(font2);
    _lblTreatedDirectory = new MyQLabel(_grpPhase1);
    _lblTreatedDirectory->setGeometry(_margx*2+larl5*2,_hab1*3,larl5*2,_hbou);
    _lblTreatedDirectory->setFont(font1);
    _ledTreatedDirectory = new QLineEdit(_grpPhase1);
    _ledTreatedDirectory->setGeometry(_margx*2+larl5*2,_hab1*3,larl5*2,_hbou);
    _ledTreatedDirectory->setFont(font1);
    _btnBrowse = new QPushButton(_grpPhase1);
    _btnBrowse->setGeometry(_margx*3+larl5*4,_hab1*3,larl5,_hbou);
    _btnBrowse->setFont(font1);
    int lar51 = _lg1-_margx*3-_lbi;
    _prgProgression = new QProgressBar(_grpPhase1);
    _prgProgression->setGeometry(_margx,_hab1*4,lar51,_hbou);
    _prgProgression->setMaximum(10000);
    _prgProgression->setValue(0);
    _prgProgression->setStyleSheet(QString::fromUtf8(""));
    _prgProgression->setTextVisible(false);
    _prgProgression->setInvertedAppearance(false);
    _prgProgression->setVisible(true);
    _btnPause = new QPushButton(_grpPhase1);
    _btnPause->setGeometry(_lg1-_margx-_lbi,_hab1*4,_lbi,_hbi);
    _btnPause->setFont(font1);
    _btnPause->setEnabled(false);
    _btnPause->setIcon(QIcon("pause.png"));
    _btnPause->setIconSize(QSize(15, 15));
    _btnPause->setCheckable(true);
    _chkSubDirectories = new QCheckBox(_grpPhase1);
    _chkSubDirectories->setGeometry(_margx*2+_lg1/2,_hab1*5,_lg1/2,_hbou);
    _chkSubDirectories->setEnabled(true);
    _chkSubDirectories->setChecked(false);
    _btnOk = new QPushButton(_grpPhase1);
    int mar6 = (_lg1 - _lbou*2)/3;
    _btnOk->setGeometry(mar6,_hab1*6,_lbou,_hbou);
    _btnOk->setFont(font1);
    _btnOk->setEnabled(false);
    _btnCancel = new QPushButton(_grpPhase1);
    _btnCancel->setGeometry(mar6*2+_lbou,_hab1*6,_lbou,_hbou);
    _btnCancel->setFont(font1);
    _btnCancel->setEnabled(false);
    _lblPhase1Message = new MyQLabel(_grpPhase1);
    _lblPhase1Message->setGeometry(_margx,_hab1*7,_lg1-_margx*2,_hbou);
    _lblPhase1Message->setFont(font2);
    _lblPhase1Message2 = new MyQLabel(_grpPhase1);
    _lblPhase1Message2->setGeometry(_margx,(_hab1*31)/4,_lg1-_margx*2,_hbou);
    _lblPhase1Message2->setFont(font2);
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
        _chkCreateImage->setGeometry(_margx,_hab1*5,_lg1/2,_hbou);
        _chkCreateImage->setEnabled(true);
        _chkCreateImage->setChecked(true);
        _btnParameters = new QPushButton(_grpPhase1);
        _btnParameters->setGeometry((_lg1-_lbou)/2,_hab1*2,_lbou,_hbou);
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
        _lblTemps = new MyQLabel(_grpPhase1);
        _lblTemps->setGeometry(_margx,_hab1*2,larl5*2,_hbou);
        _lblTemps->setFont(font1);
        _lblTemps->setVisible(true);
        _comboTemps = new QComboBox(_grpPhase1);
        _comboTemps->move(_margx*2+larl5*2,_hab1*2);
        _comboTemps->resize(larl5,_hbou);
        _comboTemps->setFont(font1);
        QStringList lte;
        lte << "10" << "1";
        _comboTemps->insertItems(0,lte);
    }
    updatesTexts();
}

void TadaridaMainWindow::updatesTexts()
{
    setWindowTitle("Tadarida");
    _chkSubDirectories->setText("Inclure les sous-dossiers");
    _lblWavDirectory->setText("Dossier des fichiers WAV");
    _btnBrowse->setText("Parcourir");
    _btnPause->setText(QString());
    _btnOk->setText("Traiter");
    _btnCancel->setText("Annuler");
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
    else
    {
        _lblTemps->setText("Facteur temps");
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

void TadaridaMainWindow::selectTemps(const QString& codsel)
{
    _timeExpansion = codsel.toInt();
    if(_timeExpansion != 1 && _timeExpansion !=10) _timeExpansion = 10;
}
