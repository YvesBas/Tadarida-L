#include "detec.h"
#include "fenim.h"

using namespace std;

const QString _baseIniFile = "/version.ini";

Detec::Detec(QMainWindow* parent):QObject(parent)
{
    _parent=parent;
    _waiting = false;
    IsRunning = false;
    TadaridaMainWindow *ptmw = (TadaridaMainWindow *)parent;
    ResultSuffix = ptmw->ResultSuffix;
    ResultCompressedSuffix = QString("tac");
    MustCancel = false;
    MustCompress = true;
    _fileProblem = false;
    // 4-3-2015 :
    _collectPreviousVersionsTags = true;
    _detecTreatment = new DetecTreatment(this);
}

Detec::~Detec()
{
}

bool Detec::InitializeDetec(const QStringList& wavList, QString soundsPath,bool reprocessingCase,int vl,int vu,bool imadat,bool withTimeCsv,RematchClass *ro)
{
    _logVersion = vl;
    _userVersion = vu;
    _withTimeCsv = withTimeCsv;
    ReprocessingMode = reprocessingCase;
    if(ReprocessingMode) MustCompress = false;
    _imageData = imadat;
    _remObject = ro;
    QDate today(QDate::currentDate());
    _wavFileList = wavList;
    _wavPath = soundsPath;
    _txtPath = soundsPath+"/txt";
    QDir reptxt(_txtPath);
    if(!reptxt.exists()) reptxt.mkdir(_txtPath);
    if(_imageData)
    {
        _datPath = soundsPath+"/dat";
        QDir repdat(_datPath);
        if(!repdat.exists()) repdat.mkdir(_datPath);
        _imagePath = soundsPath+"/ima";
        QDir repima(_imagePath);
        if(!repima.exists()) repima.mkdir(_imagePath);
    }
    QString errorFilePath(_txtPath+"/error.log");
    _errorFile.setFileName(errorFilePath);
    if(_errorFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
    _errorStream.setDevice(&_errorFile);
    _errorFileOpen = true;
    }
    else _errorFileOpen = false;
    QString logFilePath(_txtPath+"/detec.log");
    _logFile.setFileName(logFilePath);
    _logFile.open(QIODevice::WriteOnly | QIODevice::Text);
    _logText.setDevice(&_logFile);
	//
    _timeFileOpen = false;
    if(_withTimeCsv)
    {
        QString timePath = _txtPath+"/time.csv";
        _timeFile.setFileName(timePath);
        if(_timeFile.open(QIODevice::WriteOnly | QIODevice::Text)==true)
        {
            _timeFileOpen = true;
            _timeStream.setDevice(&_timeFile);
            _timeStream.setRealNumberNotation(QTextStream::FixedNotation);
            _timeStream.setRealNumberPrecision(2);
            _timeStream << "filename" << '\t' << "computefft" << '\t' << "noisetreat" << '\t' << "shapesdetects" << '\t' << "parameters" << '\t'
                        << "save - end" << '\t' << "total time(ms)" << endl;
        }
    }
    //
    _detecTreatment->SetDirParameters(_wavPath,_txtPath,_imageData,_imagePath,_datPath);
    _detecTreatment->InitializeDetecTreatment();
    IsRunning = true;
    return(true);
}

void Detec::endDetec()
{
    if(ReprocessingMode) emit information3(_numberStartTags,_numberEndTags,_numberRecupTags);
    if(_fileProblem) emit dirProblem();
    else writeDirectoryVersion();
    _errorFile.close();
    if(_timeFileOpen) _timeFile.close();
    if(!ReprocessingMode) emit threadFinished();
    _detecTreatment->EndDetecTreatment();
    IsRunning = false;
    MustCancel = false;
    if(ReprocessingMode)
    {


        int vdl = _versionDirList.size();
        if(vdl>0) for(int k=0;k<vdl;k++) delete[] _versionIndicators[k];
        delete[] _versionIndicators;
    }
    _logFile.close();
}

void Detec::Pause()
{
    _waiting = true;
}

void Detec::Resume()
{
    _waiting = false;
}

void Detec::Treatment()
{
    readDirectoryVersion();
    _nbTreatedFiles = 0;
    _nbErrorFiles = 0;
    _filesNumber = _wavFileList.size();
    _fileIndex=0;
    _numberStartTags=0;
    _numberEndTags=0;
    _numberRecupTags=0;
    if(ReprocessingMode && _collectPreviousVersionsTags) createVersionsList();
    _treating = false;
    _clock = new QTimer(this);
    connect(_clock,SIGNAL(timeout()),this,SLOT(treatOneFile()));
    _clock->start(50);
}

void Detec::createVersionsList()
{
    _versionDirList.clear();
    QDir drep(_wavPath);
    QStringList sverList=drep.entryList(QStringList("ver*"), QDir::Dirs);
    int vl,vu,vl2,vu2;
    QStringList lv,lv2;
    QString sv1,sv2;
    bool conv1,conv2;
    foreach(QString sv,sverList)
    {
        lv = sv.right(sv.length()-3).split("_");
        if(lv.size()==2)
        {
            vl=lv.at(0).toInt(&conv1);
            vu=lv.at(1).toInt(&conv2);
            if(conv1 && conv2) _versionDirList.append(sv);
        }
    }
    //_logText << "liste des sv versions - nbre = " << _versionDirList.size() << endl;
    int vdl =_versionDirList.size();
    if(vdl > 1)
    {
        bool ontrie = true;
        while(ontrie)
        {
            ontrie = false;
            for(int j=0;j<vdl-1;j++)
            {
                sv1 = _versionDirList.at(j);
                sv2 = _versionDirList.at(j+1);
                lv =  sv1.right(sv1.length()-3).split("_");
                lv2 = sv2.right(sv2.length()-3).split("_");
                vl=lv.at(0).toInt(); vu=lv.at(1).toInt();
                vl2=lv2.at(0).toInt(); vu2=lv2.at(1).toInt();
                if(vl>vl2 || (vl==vl2 && vu>vu2))
                {
                    _versionDirList.replace(j,sv2);
                    _versionDirList.replace(j+1,sv1);
                    ontrie=true;
                }
            }
        }

    }
    //_logText << "tri des sv versions - nbre = " << vdl << endl;
    // for(int j=0;j<_versionDirList.size();j++) _logText << j << ") " << _versionDirList.at(j) << endl;
    if(vdl>0)
    {
        _versionIndicators = new int*[vdl];
        for(int k=0;k<vdl;k++)
        {
            _versionIndicators[k] = new int[3];
            for(int l=0;l<3;l++) _versionIndicators[k][l] = 0;
        }
    }

}


void Detec::cleanVerSubdir()
{
    //_logText << "Nettoyage des sous-répertoires du répertoire " << _wavPath << endl;
    QDir wDir(_wavPath);
    if(!wDir.exists()) return;
    for(int j=0;j<_versionDirList.size();j++)
    {
        bool isCleaned = false;
        QString svPath = _wavPath + "/" + _versionDirList.at(j);
        if(j<_versionDirList.size()-1)
        {
            if(_versionIndicators[j][0]==_versionIndicators[j+1][0] && _versionIndicators[j][1]==_versionIndicators[j+1][1]
                    && _versionIndicators[j][2]==_versionIndicators[j+1][2])
            {
                cleanSubdir(wDir,svPath,true,QString("*.*"));
                isCleaned = true;
            } // fin suppression d'un répertoire
        } // j< size-1
        if(!isCleaned)
        {
            cleanSubdir(wDir,svPath+"/txt",true,QString("*.*"));
            cleanSubdir(wDir,svPath+"/ima",true,QString("*.*"));
        }
    } // next j
    cleanSubdir(wDir,_wavPath+"/dat",false,QString("*.dat"));
    cleanSubdir(wDir,_wavPath+"/txt",false,QString("*.txt"));
    cleanSubdir(wDir,_wavPath+"/txt",false,QString("*.csv"));
    cleanSubdir(wDir,_wavPath+"/txt",false,QString("*.tac"));
}

void Detec::cleanSubdir(QDir cdir,QString ndirPath,bool cleanAll,QString filter)
{
    QDir nDir(ndirPath);
    if(!nDir.exists()) return;
    QStringList listFiles = nDir.entryList(QStringList(filter), QDir::Files);
    foreach(QString f,listFiles) nDir.remove(f);
    if(cleanAll)
    {
        QStringList sdirList = nDir.entryList(QStringList("*"), QDir::Dirs);
        foreach(QString r,sdirList)
        {
            if(r.length()>2)
            {
                cleanSubdir(nDir,ndirPath+"/"+r,true,"*.*"); // attention : récursivité
            }
        }
        cdir.rmdir(ndirPath);
    }
}

bool Detec::treatOneFile()
{
    //_logText << "entre dans tof" << endl;
    if(_treating || _waiting) return(true);
    if(_fileIndex >= _filesNumber || (MustCancel  && !ReprocessingMode) || _fileProblem)
    {
        _clock->stop();
        if(ReprocessingMode && _collectPreviousVersionsTags && _fileIndex >= _filesNumber) cleanVerSubdir();
        endDetec();
        return(true);
    }
    _treating = true;
    QString wavFile = _wavFileList.at(_fileIndex);
    _wavFile = wavFile;
    _fileIndex++;
    _logText << endl;
    _logText << endl << "Début de traitement : "<< QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;
    _logText << "wavfile=" << wavFile << endl;
    QString nomfic = wavFile;
    if(ReprocessingMode)
    {
        _remObject->initialize();
        _numberStartTags+=_remObject->PreMatch(wavFile,_wavPath);
        _numVer = _remObject->_fenim1->_numVer;
        _tE = _remObject->_fenim1->_tE;
        _numtE = _remObject->_fenim1->_numtE;
    }
    //
    emit information(nomfic);
    //***_logText << "Avant initialiseVariablesParFichier "<< QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;
    //initWavVariables();
	// ZZZ : vérifier que le contenu de initWavVariables est bien dans une méthode de DetecTreatment

    //***_logText << "Avant suite initialisations diverses "<< QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;
    QString pathFile = _wavPath + '/' + wavFile;
    if(checkAssociatedFiles(_wavPath,_wavFile)== false)
    {
        if(_errorFileOpen) _errorStream << _wavFile << ": problème sur fichier associé" << endl;
        _fileProblem = true;
        _treating = false; return(false);
    }
    if(_detecTreatment->CallTreatmentsForOneFile(wavFile,pathFile))
	{
        if(_imageData)
        {
            // _logText << "Avant enregistrement image et fichier dat - "<< QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;
            createImage(wavFile);
            // _logText << "Entre createimage et savedatfile - "<< QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;
            _detecTreatment->saveDatFile(wavFile);
            // _logText << "Fin enregistrement image et fichier dat - "<< QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;
        }
        _nbTreatedFiles++;
	}
    else _nbErrorFiles++;
    // -------------------------------------------------------
    if(ReprocessingMode)
    {
        //_logText << "Retraitement - fenim2" << endl;
        int pcpma;
        int nbTags = _remObject->PostMatch(true,"",&pcpma);
        if(_remObject->Ok)
        {
            _numberEndTags+=nbTags;
            QString info0=wavFile+QString(" : ")
                    + QString::number(_remObject->nbc2)+QString(" cris sur ")
                    +QString::number(_remObject->nbc1)+".";
            QString info1=wavFile+QString(" : ")
                    + QString::number(nbTags)+QString(" étiquettes rematchées sur ")
                    +QString::number(_remObject->nbe1)+".";
            QString info2=_wavPath+QString(" : ")
                    + QString::number(_numberEndTags)+QString(" étiquettes rematchées sur ")
                    +QString::number(_numberStartTags)+".";
            emit information2(info2);
            //_logText << "après rematchage fenim2 a " << nbTags << "etiquettes"<< endl;
            //_logText << info0 << endl;
            //_logText << info1 << endl;
            if(_fileIndex == _filesNumber-1) _logText << "Rematchage du répertoire "<< info2 << endl;
            // boucle de récupération sur versions antérieures
            int nbrecup = 0;
            if(_collectPreviousVersionsTags)
            {
                for(int j=_versionDirList.size()-1;j>=0;j--)
                {
                    QString verPath = _wavPath + "/" + _versionDirList.at(j);
                    _remObject->PreMatch(wavFile,verPath);
                    _versionIndicators[j][0] += _remObject->nbc1;
                    _versionIndicators[j][1] += _remObject->nbe1;
                    _remObject->PostMatch(false,verPath,_versionIndicators[j]+2);
                    if(_remObject->nbe2 > 0)
                    {
                        _numberRecupTags += _remObject->nbe2;
                        nbTags += _remObject->nbe2;
                        info1=wavFile+QString(" - après récupération sur version ")
                          +verPath+" : "+QString::number(nbTags)+QString(" étiquettes rematchées.");
                        //_logText << info1 << endl;
                        info2=_wavPath + QString(" : ") + QString::number(_numberEndTags+_numberRecupTags)
                                + QString(" étiquettes rematchées.");
                        emit information2(info2);
                    }
                }
                //
            }
            //
            _remObject->EndMatch();
        }
    }
    else emit information4(_nbTreatedFiles,_nbErrorFiles);
    _logText << "fin de traitement du fichier" << endl;
    emit moveBar((float)_fileIndex/(float)_filesNumber);
    _treating = false;
    return(true);
}


bool Detec::readDirectoryVersion()
{
    QString cbase = _wavPath + _baseIniFile;
    _dirLogVersion = 0;
    _dirUserVersion = 0;
    if(!QFile::exists(cbase)) return(false);
    QSettings settings(cbase, QSettings::IniFormat);
    settings.beginGroup("version");
    _dirLogVersion = settings.value("log").toInt();
    _dirUserVersion = settings.value("user").toInt();
    settings.endGroup();
    return(true);
}

void Detec::writeDirectoryVersion()
{
    QSettings settings(_wavPath + _baseIniFile, QSettings::IniFormat);
    settings.beginGroup("version");
    settings.setValue("log", QVariant::fromValue(_logVersion));
    settings.setValue("user", QVariant::fromValue(_userVersion));
    settings.endGroup();
}

bool Detec::checkAssociatedFiles(QString pathName,QString wavName)
{
    bool resu = true;
    QString shortName;
    QString testName;
    if(wavName.length()>4) shortName = wavName.left(wavName.length()-4);
    else return(false);
    for(int i=0;i<4;i++)
    {
        if(i==0) testName = pathName + "/txt/" + shortName + "." + ResultSuffix;
        if(i==1) testName = pathName + "/eti/" + shortName + ".eti";
        if(i==2) testName = pathName + "/da2/" + shortName + ".da2";
        if(i==3) testName = pathName + "/ima/" + shortName + ".jpg";
        QFile testFile;
        testFile.setFileName(testName);
        if(testFile.exists())
        {
            if(testFile.open(QIODevice::WriteOnly))
            {
                testFile.close();
            }
            else
            {
                _logText << testName << " : problème d'ouverture" << endl;
                resu = false;
                break;
            }
        }
        else
        {
            // _logText << testName << " n'existe pas" << endl;
        }
    }
    //
    return(resu);
}

void Detec::createImage(QString wavFile)
{
    if(_detecTreatment->_sonogramWidth > 32767) {_xmoitie = true; _imaWidth=(_detecTreatment->_sonogramWidth+1)/2;}
    else  {_xmoitie = false; _imaWidth=_detecTreatment->_sonogramWidth;}
    QImage ima = QImage(_imaWidth, _detecTreatment->_fftHeightHalf,QImage::Format_RGB32);
    initBvrvb(_detecTreatment->_energyMin,_detecTreatment->_energyMax);
    int imax=((int)(_detecTreatment->_energyMax-_detecTreatment->_energyMin)*5)+1;
    uint crgb;
    for(int i=0;i<imax;i++)
    {
        _tvaleur[i]=calculateRGB((double)i/5.0f);
    }
    qint16 *ydl;
    int exceptions = 0;
    int lyi = qMin(_detecTreatment->_fftHeightHalf,_detecTreatment->_limY);
    for(int y = 0; y < lyi ; y++)
    {
        ydl=_detecTreatment->_sonogramArray[y];
        int digitPos = 0;
        char *pBoolChar = _detecTreatment->_pointFlagsArray[y];;
        char boolChar = *pBoolChar;
        for (int x = 0 ; x < _imaWidth ; x++)
        {
            int valeur=(int)(((float)(*ydl)/20.0f)  -  _detecTreatment->_energyMin*5.0f);
            if(valeur>=0 && valeur<imax)crgb=_tvaleur[valeur];
            else {crgb=0; exceptions++;}
            if((boolChar & (1 << digitPos))!=0) crgb |= 224 << 16;
            ima.setPixel(x, _detecTreatment->_fftHeightHalf-y-1,crgb);
            for(int k=0;k<1+_xmoitie;k++)
            {
                ydl++;
                digitPos++;
                if(digitPos==8) {pBoolChar++; boolChar = *pBoolChar; digitPos=0;}
            }
        }
    }
    QString imageName = _imagePath + '/' + wavFile.replace(QString(".wav"), QString(".jpg"), Qt::CaseInsensitive);
    ima.save(imageName,0,100); // save image
}

// void DetecTreatment::initBvrvb(double bornemin,double median,double bornemax)
void Detec::initBvrvb(double bornemin,double bornemax)
{
    /*
    _bRGB[0][0]=bornemin;
    _bRGB[1][0]=median;
    _bRGB[2][0]=median+(double)_stopThreshold;
    _bRGB[3][0]=median+(double)_detectionThreshold;
    _bRGB[4][0]=bornemax+1;
    */
    _bRGB[0][0]=bornemin;
    // _bRGB[1][0]=0;
    // _bRGB[2][0]=(double)_stopThreshold;
    // _bRGB[3][0]=(double)_detectionThreshold;
    _bRGB[1][0]=bornemin/2;
    _bRGB[2][0]=(double)_detecTreatment->_energyStopThreshold;
    _bRGB[3][0]=(double)_detecTreatment->_energyShapeThreshold;
    _bRGB[4][0]=bornemax+1;
    for(int i=0;i<5;i++) _bRGB[i][0]-=bornemin;
    _bRGB[0][1]=0;   _bRGB[0][2]=0;   _bRGB[0][3]=32;
    _bRGB[1][1]=0;   _bRGB[1][2]=32;  _bRGB[1][3]=64;
    _bRGB[2][1]=0;   _bRGB[2][2]=64; _bRGB[2][3]=96;
    _bRGB[3][1]=0;   _bRGB[3][2]=96; _bRGB[3][3]=128;
    _bRGB[4][1]=0;   _bRGB[4][2]=128; _bRGB[4][3]=160;
}

uint Detec::calculateRGB(double value)
{
    double rgb[3];
    for(int j=0;j<3;j++) rgb[j] =0.0f;
    for(int i=0;i<4;i++)
    {
        if(value>=_bRGB[i][0] && value<_bRGB[i+1][0])
        {
            for(int j=0;j<3;j++)
                rgb[j]=_bRGB[i][j+1]
                        +((_bRGB[i+1][j+1]-_bRGB[i][j+1])*(value-_bRGB[i][0]))/(_bRGB[i+1][0]-_bRGB[i][0]);
            break;
        }
    }
    uint urgb = qRgb(qRound(rgb[0]),qRound(rgb[1]),qRound(rgb[2]));
    return(urgb);
}
