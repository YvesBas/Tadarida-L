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
    _fenim1->m_logStream << "Prematch 2" << endl;
    nbc1=0; nbe1=0;
    if(_fenim1->chargeCrisEtiquettes()==false) return(0);
    _fenim1->m_logStream << "Prematch 3" << "nb eti=" <<_fenim1->m_nbeti << endl;
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
    _traceFile.setFileName(QString("trace.txt"));
    _traceFile.open(QIODevice::WriteOnly | QIODevice::Text);
    _traceText.setDevice(&_traceFile);
    _traceText.setRealNumberNotation(QTextStream::FixedNotation);
    _traceText.setRealNumberPrecision(2);
    _traceText << "R�pertoire    "<< '\t'
               << "Fichier       " << '\t'
               << "Sens    " << '\t'
               << "Colonne (x) " << '\t'
               << "Colonne (ms) " << '\t'
               << "D�but descendu" << '\t'
               << "Moy. avant (l)"<< '\t'
               << "Moy. avant (L)" << '\t'
               << "Moy. apr�s (l)" << '\t'
               << "Moy. apr�s (L)"
               << endl;

    //
    _iniPath = QString("/config/config.ini");
    CanContinue = true;
    MustEnd = false;
    MustCancel = false;
    _wavDirectory   = QDir::current();
    _lastWav = QString("");
    _tadaridaMode = SIMPLE; // mode simple
    _userVersion = 0;
    _programVersion = 17;
    _baseDir   = QDir::current();
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
    connect(_detec, SIGNAL(infoTrace(QString,QString,QString,QString,QString,QString,QString,QString,QString,QString)),
               this, SLOT(storeTrace(QString,QString,QString,QString,QString,QString,QString,QString,QString,QString)));
    connect(_detec, SIGNAL(dirProblem()),this, SLOT(treatDirProblem()));
    _clock = new QTimer(this);
    connect(_clock,SIGNAL(timeout()),this,SLOT(manageDetecCall()));
    setWindowTitle("Tadarida");
    window()->setWindowTitle("Tadarida");
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
                QMessageBox::warning(this, "Base non cr��e",
                                     "S�lectionner le r�pertoire de la base !",QMessageBox::Ok);
                QString basepath = "";
                basepath=selectBase();
                if(basepath.isEmpty())
                {
                    QMessageBox::critical(this, "Arr�t du programme",
                    "Le mode �tiquetage ne peut fonctionner si on ne s�lectionne pas une base !", QMessageBox::Ok);
                    exitProgram();
                    return;
                }
                _baseDir.setPath(basepath);
                writeConfigFile();
                if(testBase()==false)
                {
                    if(!CanContinue)
                    {
                        _logText << "retour brutal apr�s 2�me testbase n�gatif" << endl;
                        return;
                    }
                    else
                    {
                        _logText << "enregistrement de la nouvelle base sur testbase n�gatif" << endl;
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
}

TadaridaMainWindow::~TadaridaMainWindow()
{
    _logText << "tdmc 1" << endl;
    _traceFile.close();
    _logText << "tdmc 2" << endl;
    delete _detec;
    // ajout� le 4-3-2015
    delete _remObject;
    _logText << "tdmc 3" << endl;
    if(_isFenimWindowOpen)
    {
        _logText << "tdmc 3,5" << endl;
        delete fenim;
    }
    _logText << "tdmc 4" << endl;
    if(_isRechercheOpen) delete _precherche;
    _logText << "tdmc 5" << endl;
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
                _logText << "versions incoh�rentes" << endl;
                QMessageBox::critical(this, "Arr�t du programme",
                "La base a une version plus avanc�e que le logiciel !", QMessageBox::Ok);
                exitProgram();
                return(false);
            }
            else
            {
                updateBaseVariables();
                _logText << "version d�pass�e" << endl;
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
        _logText << "base non cr��e = version.ini manquant" << endl;
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
        _logText << "closeevent - passe mustend � true" << endl;
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
        QMessageBox::warning(this, tr("Changement de version"), tr("Version logiciel diff�rente dans config.ini : va �tre mise � jour "), QMessageBox::Ok);
        _logText << "verlog=" << verlog << endl;
        _logText << "programVersion=" << _programVersion << endl;
    }
    _userVersion = settings.value("user").toInt();
    int mode = settings.value("mode").toInt();
    if(mode==ETIQUETAGE) _tadaridaMode = ETIQUETAGE;
    else _tadaridaMode = SIMPLE;
    settings.endGroup();
}

void TadaridaMainWindow::writeConfigFile()
{
    QFile cf(QDir::currentPath() + _iniPath);
    if(cf.open(QIODevice::WriteOnly)) cf.close();
    else QMessageBox::warning(this, tr("Attention"), tr("Fichier config.ini inaccessible en �criture : fermer ce fichier s'il est ouvert !"), QMessageBox::Ok);
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
    // types : 1 entier  2 double 3 bool�en 4 entier en combo avec liste en param�tre
    param->creeParametre(QString("Facteur temp."),&_timeExpansion,4,0,0,0,0,&lte);
    param->creeParametre(QString("Seuil de d�tection"),&_detectionThreshold,1,10,30);
    param->creeParametre(QString("Seuil d'arr�t de d�tection"),&_stopThreshold,1,5,25);
    param->creeParametre(QString("Fr�quence minimum"),&_minimumFrequency,1,0,20);
    param->creeParametre(QString("p_nbo"),&_overlapsNumber,4,0,0,0,0,&lnbo);
    param->creeParametre(QString("coef. x"),&Divrl,1,100,100000);
    //param->creeParametre(QString("cr�er petits fichiers .csv"),&_littleParams ,3);

    param->creeParametre(QString("traitement zones de silence"),&_useValflag ,3);


    param->creeParametre(QString("Seuil de silence "),&_jumpThreshold,1,10,50);
    param->creeParametre(QString("Bande verticale large"),&_widthBigControl,1,10,500);
    param->creeParametre(QString("Bande verticale �troite"),&_widthLittleControl,1,1,20);
    param->creeParametre(QString("Seuil haut"),&_highThreshold,1,0,20);
    param->creeParametre(QString("Seuil bas"),&_lowThreshold,1,-20,0);
    param->creeParametre(QString("Pourcentage q5"),&_qR,1,1,20);
    param->creeParametre(QString("Nb pixel min q5"),&_qN,1,2,10);

    if(param->p_nbparam > param->n_param) param->p_nbparam = param->n_param;
    param->show();
}

void TadaridaMainWindow::on_btnBrowse_clicked()
{
    QString soundsPath  = QFileDialog::getExistingDirectory( this,
                            tr("S�lectionner le r�pertoire de fichiers wav � traiter"),
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
        QMessageBox::warning(this, tr("Erreur"), tr("Ne pas lancer ce traitement sur un r�pertoire de la base !"), QMessageBox::Ok);
        blockUnblock(true);
        return;
    }
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

void TadaridaMainWindow::detecFinished()
{
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
                _logText << "openbase retour brutal apr�s testbase n�gatif" << endl;
                return;
            }
            else
            {
                _logText << "openbase enregistrement de la nouvelle base sur testbase n�gatif" << endl;
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
                            tr("S�lectionner le r�pertoire de la base"),
                            _baseDir.path(),
                            QFileDialog::ShowDirsOnly);
    return(basePath);
}

void TadaridaMainWindow::on_btnOpenWav_clicked()
{
    QString wavFile  = QFileDialog::getOpenFileName( this, tr("S�lectionner un fichier wav trait�"),
                                                   _wavDirectory.path(), tr("(*.wav)"));
    if(!wavFile.isEmpty()) openWavTag(wavFile);
}

void TadaridaMainWindow::on_btnOpenPreviousWav_clicked()
{
    QString wavFile  = _previousWav;
    if(!wavFile.isEmpty()) openWavTag(wavFile);
    else QMessageBox::warning(this, "Attention", "Pas de fichier pr�c�dant", QMessageBox::Ok);
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
        QMessageBox::warning(this, "Fichier .da2 non trouv�", da2File, QMessageBox::Ok);
        return(false);
    }

    QString imaFile = dirName + "/ima/"+wavShortName + ".jpg";
    if(!QFile::exists(imaFile))
    {
        QMessageBox::warning(this, "Attention", tr("Fichier image non trouv� !"), QMessageBox::Ok);
        return(false);
    }
    QString txtFile = dirName + "/txt/"+wavShortName + "."+ResultSuffix;
    if(!QFile::exists(txtFile))
    {
        QMessageBox::warning(this, "Attention", tr("Fichier r�sultat non trouv� !"), QMessageBox::Ok);
        return(false);
    }
    if(consistencyCheck(wavFileName,da2File,txtFile)==false)
    {
        QMessageBox::warning(this, "Attention", tr("Incoh�rence entre fichier .ta et fichier .da2: retraiter ce r�pertoire !"), QMessageBox::Ok);
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
    QString wavFile  = QFileDialog::getOpenFileName( this, tr("S�lectionner un fichier wav �tiquet�"),
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
        QMessageBox::warning(this, "Fichier .da2 non trouv�", datFile, QMessageBox::Ok);
        return;
    }
    QString imaFile = dirName + "/ima/"+wavShortName + ".jpg";
    if(!QFile::exists(imaFile))
    {
        QMessageBox::warning(this, "Attention", tr("Fichier non trait� !"), QMessageBox::Ok);
        return;
    }
    QString etiFile = dirName + "/eti/"+wavShortName + ".eti";
    if(!QFile::exists(etiFile))
    {
        QMessageBox::warning(this, "Attention", tr("Fichier non �tiquet� !"), QMessageBox::Ok);
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
            char c=(char)dirToExamine[i].toAscii();
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
            QMessageBox::warning(this, tr("Erreur"), tr("Ne pas s�lectionner par ce choix un fichier dans la base !"), QMessageBox::Ok);
            return;
        }
        if(_dayBaseUpToDate==false)
        {
            QMessageBox::warning(this, tr("Erreur"), tr("Choix impossible : il faut au moins avoir retrait� le r�pertoire du jour !"), QMessageBox::Ok);
            return;

        }
    }
    // case B : updating a tag file of the base
    if(typeA==false)
    {
        if(dirType==true)
        {
            QMessageBox::critical(this, tr("Erreur"), tr("Ce fichier n'est pas un fichier �tiquet� de la base !"), QMessageBox::Ok);
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
}

bool TadaridaMainWindow::proposeGeneralReprocessing()
{
    // TODO : I must test if the base is not empty
    bool result = false;
    if(QMessageBox::question(this, "La base n'est pas � jour", "Lancer le retraitement g�n�ral ?",
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
        QString mess("Il n'y a aucun r�pertoire de jour � traiter !");
        QMessageBox::warning(this, tr("Remarque"), mess, QMessageBox::Ok);
        return(true);
    }
    else
    {
        _logText << "Retraitement g�n�ral" << endl;
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
    _logText << "___ directorytreat d�but nrep = " << directoriesList.count() << endl;
    _logText << "___ directorytreat d�but reptotreat = " << repToTreat.path() << endl;
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
            //_logText << "___ test du r�pertoire " << drep << endl;
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
            //_logText << "___ apr�s ajout liste : nrep = " << directoriesList.count() << endl;
            QStringList wavSoundsList = drt.entryList(QStringList("*.wav"), QDir::Files);
            if(wavSoundsList.isEmpty())
            {
                //_logText << "suppression du r�pertoire " << drep << endl;
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
                 "Base ou r�pertoire de base : traitement interdit",QMessageBox::Ok);
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
        _clock->start(50);
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
                                             "Retraitement interrompu suite � probl�me d'acc�s � un fichier",
                                             QMessageBox::Ok);
                    }
                    else
                    {
                        _dayBaseUpToDate = true;
                        RetreatText << _tagsNumberAfter << " �tiquettes r�cup�r�es sur "
                                    << _tagsNumberBefore << " dans le retraitement principal." <<  endl;
                        if(_isGeneralReprocessing)
                        {
                            _logText << _tagsNumberAfter << " �tiquettes r�cup�r�es sur "
                                     << _tagsNumberBefore << " dans le retraitement principal." << endl;
                            if(_tagsNumberFinal > 0)
                                RetreatText << "Apr�s r�cup�ration dans les versions pr�c�dantes, nombre total d'�tiquettes : "
                                  << _tagsNumberAfter + _tagsNumberFinal <<  endl;
                            writeBaseVersion();
                            _baseUpToDate=true;
                            QMessageBox::warning(this, tr("Remarque"), "Fin du retraitement g�n�ral", QMessageBox::Ok);
                            _logText << endl << "Fin du retraitement g�n�ral" << endl;
                        }
                        else
                        {
                            writeDirectoryVersion(_baseDay.path());
                            QMessageBox::warning(this, tr("Remarque"), "Fin du retraitement du r�pertoire du jour", QMessageBox::Ok);
                            _logText << endl << "Fin du retraitement du r�pertoire du jour" << endl;
                        }
                        afterRetreating();
                    }
                }
                else
                {
                    if(_oneDirProblem)
                    {
                        QMessageBox::warning(this, tr("Attention"),
                                             "Traitement interrompu suite � probl�me  d'acc�s � un fichier", QMessageBox::Ok);
                        _logText << endl << "Traitement interrompu suite � probl�me  d'acc�s � un fichier" << endl;
                    }
                    else
                    {
                        if(!MustCancel)
                        {
                            QMessageBox::warning(this, tr("Remarque"), "Fin du traitement du r�pertoire", QMessageBox::Ok);
                            _logText << endl << "Fin de traitement du r�pertoire" << endl;
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
        _lblPhase1Message->setText(QString("Traitement du r�pertoire ")+dirName);
        _ledTreatedDirectory->setVisible(false);
        _lblTreatedDirectory->setVisible(true);
        _logText << "� traiter : " << dirName << endl;
        QString dirToShow = dirName;
        if(dirToShow=="") dirToShow = _directoryRoot.path();
        int pos = dirToShow.lastIndexOf("/");
        if(pos>0) dirToShow = dirToShow.right(dirToShow.length()-pos-1);
         _lblTreatedDirectory->setText(dirToShow);
         _logText << "apr�s mofifs : dts = " << dirToShow << endl;
        _prgProgression->setValue(0);
        dirTreat(dirName);
    }
}

bool TadaridaMainWindow::proposeDayReprocessing()
{
    if(!_baseDay.exists())
    {
        _logText << "Le r�pertoire du jour n'existe pas : proposition de retraitement du r�pertoire du jour inutile " << endl;
        _dayBaseUpToDate = true;
        return(true);
    }
    else
    {
        readDirectoryVersion(_baseDay.path());
        if(_dirProgramVersion == _programVersion && _dirUserVersion == _userVersion)
        {
            _logText << "Le r�pertoire du jour est � jour : proposition de retraitement du r�pertoire du jour inutile " << endl;
            _dayBaseUpToDate = true;
            return(true);
        }
    }
    if(QMessageBox::question(this, "La base n'est pas � jour", "Lancer le retraitement du r�pertoire du jour ?",
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
            _logText << "pas de retraitement n�cessaire pour le r�pertoire " << directoryPath << endl;
            return(true);
        }
    }
    if(sdir.exists())
    {
        _logText << "Retraitement du r�pertoire " << sdir.path() << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;
        _lblPhase1Message->setText(QString("Traitement du r�pertoire ")+sdir.path()+" en cours");
        detecCall(sdir,_directoriesRetreatMode);
    }
    else
    {
        QMessageBox::warning(this, tr("Erreur"), QString("R�pertoire ")+directoryPath+" inexistant !", QMessageBox::Ok);
        return(true);
    }
    return(true);
}

bool TadaridaMainWindow::detecCall(QDir dirToReprocess,bool ReprocessingCase)
{
    _logText << "rep_a_traiter=" << dirToReprocess.path() << endl;
    if(!dirToReprocess.exists())
    {
        QMessageBox::critical(this, tr("Erreur"), tr("Le r�pertoire des fichiers WAV n'existe pas !"), QMessageBox::Ok);
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
            QString mess = QString("Il n'y a aucun fichier WAV dans le r�pertoire ")+dirToReprocess.path()+" !";
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
                 _highThreshold,_lowThreshold,_qR,_qN);
    _logText << "detecCall 4" << endl;
    bool generateImagesDat = false;
    if(_tadaridaMode==ETIQUETAGE)
    {
        if(_chkCreateImage->isChecked() || ReprocessingCase) generateImagesDat=true;
    }
    _logText << "detecCall 5" << endl;
    if(ReprocessingCase) previousVersionSave(wavSoundsList,dirToReprocess.path());
	// 5-2-2015 : retir� dernier param�tre (_withTimer)
    if(_detec->InitializeDetec(wavSoundsList, dirToReprocess.path(),
      ReprocessingCase,_programVersion,_userVersion,generateImagesDat,_remObject))
    {
        _logText << "avant lancement de detec" << endl;
        _btnPause->setEnabled(true);
        if(_detec->Treatment()<0)
        {
            QMessageBox::critical(this, tr("Erreur"), tr("Cr�ation du fichier de param�tre impossible !"), QMessageBox::Ok);
            blockUnblock(true);
            return(false);
        }
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
    _pmx = 4; _pmy=4;
    _margx = 8; _margy = 8;
    _lt  = 860; _ht  = 600;
    _lcw = _lt-_pmx; _hcw = _ht - _pmy;
    _lg1= (_lcw-_pmx*4)/2; _hg1 = _hcw-_pmy*3;
    _lbou = 150; _hbou=30; _lbi = 30; _hbi = 30;
    // �����
    _hab1 = (_hg1*2)/19;
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
    _lblWavDirectory = new QLabel(_grpPhase1);
    int larl5= (_lg1-_margx*4)/5;
    _lblWavDirectory->setGeometry(_margx,_hab1*3,larl5*2,_hbou);
    _lblWavDirectory->setFont(font2);
    _lblTreatedDirectory = new QLabel(_grpPhase1);
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
    _lblPhase1Message = new QLabel(_grpPhase1);
    _lblPhase1Message->setGeometry(_margx,_hab1*7,_lg1-_margx*2,_hbou);
    _lblPhase1Message->setFont(font1);
    _lblPhase1Message2 = new QLabel(_grpPhase1);
    _lblPhase1Message2->setGeometry(_margx,(_hab1*31)/4,_lg1-_margx*2,_hbou);
    _lblPhase1Message2->setFont(font1);
    _lblPhase1Title = new QLabel(_grpPhase1);
    _lblPhase1Title->setGeometry(_lg1/8,(_hab1*3)/4,_lg1-_margx*2,_hbou);
    _lblPhase1Title->setText("Traitement des fichiers de sons");
    _lblPhase1Title->setFont(fontG);
    if(_tadaridaMode==ETIQUETAGE)
    {
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
        int larw2 = (_lg1*3)/4;
        int mxw2 = _lg1/8;
        _lblPhase2Title = new QLabel(_grpPhase2);
        _lblPhase2Title->setGeometry(mxw2,(_hab1*3)/4,_lg1-_margx*2,_hbou);
        _lblPhase2Title->setFont(fontG);
        _lblPhase2Title->setText("Mise � jour de la base d'�tiquettes");
        _lblBase = new QLabel(_grpPhase2);
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
    updatesTexts();
}

void TadaridaMainWindow::updatesTexts()
{
    setWindowTitle("Tadarida");
    _chkSubDirectories->setText("Traiter les sous-r�pertoires");
    _lblWavDirectory->setText("R�pertoire des fichiers WAV");
    _btnBrowse->setText("Parcourir");
    _btnPause->setText(QString());
    _btnOk->setText("Ok");
    _btnCancel->setText("Annuler");
    if(_tadaridaMode==ETIQUETAGE)
    {
        _btnParameters->setText(" Modifier les param�tres ");
        _chkCreateImage->setText("Cr�er fichiers dat et images");
        _btnOpenWav->setText("S�lectionner un fichier WAV trait�");
        _btnOpenBase->setText("Modifier le r�pertoire de la base");
        _btnUpdateTags->setText("Modifier les �tiquettes d'un fichier WAV");
        _btnOpenPreviousWav->setText("Pr�c�dent");
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

void TadaridaMainWindow::storeTrace(QString i1,QString i2,QString i3,
QString i4,QString i5,QString i6,QString i7,QString i8,QString i9,QString i10)
{
    _traceText<<i1<<'\t'<<i2<<'\t'<<i3<<'\t'<<i4<<'\t'<<i5<<'\t'
              <<i6<<'\t'<<i7<<'\t'<<i8<<'\t'<<i9<<'\t'<<i10<<endl;
}

void TadaridaMainWindow::treatDirProblem()
{
    _oneDirProblem = true;
    _logText << "re�u un message dirProblem" << endl;
}
