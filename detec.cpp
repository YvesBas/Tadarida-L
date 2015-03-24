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

bool Detec::InitializeDetec(const QStringList& wavList, QString soundsPath,bool reprocessingCase,int vl,int vu,bool imadat,RematchClass *ro)
{
    _logVersion = vl;
    _userVersion = vu;
    ReprocessingMode = reprocessingCase;
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
    _detecTreatment->SetDirParameters(_wavPath,_txtPath,_imageData,_imagePath,_datPath);
    _detecTreatment->InitializeDetecTreatment();
    IsRunning = true;
    return(true);
}

/*
void Detec::initWavVariables()
{
    for(int j=0;j<FFT_HEIGHT_HALF_MAX;j++) memset(_pointFlagsArray[j],0,SONOGRAM_WIDTH_MAX);
    _maxCallWidth = 0;
    _maxCallHeight = 0;
}
*/

void Detec::endDetec()
{
    if(ReprocessingMode) emit information3(_numberStartTags,_numberEndTags,_numberRecupTags);
    if(_fileProblem) emit dirProblem();
    else writeDirectoryVersion();
    _errorFile.close();
    if(!ReprocessingMode) emit threadFinished();
    _detecTreatment->EndDetecTreatment();
    IsRunning = false;
    MustCancel = false;
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

int Detec::Treatment()
{
    readDirectoryVersion();
    int nbTreatedFiles = 0;
    _filesNumber = _wavFileList.size();
    _fileIndex=0;
    _numberStartTags=0;
    _numberEndTags=0;
    _numberRecupTags=0;
    // 4-3-2015
    if(ReprocessingMode && _collectPreviousVersionsTags) createVersionsList();
    _treating = false;
    _clock = new QTimer(this);
    connect(_clock,SIGNAL(timeout()),this,SLOT(treatOneFile()));
    _clock->start(50);
    return(nbTreatedFiles);
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
    _logText << "liste des sv versions - nbre = " << _versionDirList.size() << endl;
    for(int j=0;j<_versionDirList.size();j++)
        _logText << j << ") " << _versionDirList.at(j) << endl;
    if(_versionDirList.size()>1)
    {
        bool ontrie = true;
        while(ontrie)
        {
            ontrie = false;
            for(int j=0;j<_versionDirList.size()-1;j++)
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
    _logText << "tri des sv versions - nbre = " << _versionDirList.size() << endl;
    for(int j=0;j<_versionDirList.size();j++)
        _logText << j << ") " << _versionDirList.at(j) << endl;
}

bool Detec::treatOneFile()
{
    //_logText << "entre dans tof" << endl;
    if(_treating || _waiting) return(true);
    if(_fileIndex >= _filesNumber || (MustCancel  && !ReprocessingMode) || _fileProblem)
    {
        _clock->stop();
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
            _detecTreatment->createImage(wavFile);
            // _logText << "Entre createimage et savedatfile - "<< QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;
            _detecTreatment->saveDatFile(wavFile);
            // _logText << "Fin enregistrement image et fichier dat - "<< QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;
        }
	}
    // -------------------------------------------------------
    if(ReprocessingMode)
    {
        //_logText << "Retraitement - fenim2" << endl;
        int nbTags = _remObject->PostMatch(true,"");
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
            _logText << info0 << endl;
            _logText << info1 << endl;
            if(_fileIndex == _filesNumber-1) _logText << "Rematchage du répertoire "<< info2 << endl;
            // boucle de récupération sur versions antérieures
            int nbrecup = 0;
            if(_collectPreviousVersionsTags)
            {
                for(int j=_versionDirList.size()-1;j>=0;j--)
                {
                    QString verPath = _wavPath + "/" + _versionDirList.at(j);
                    // _logText << "Boucle récup - verPath = " << verPath << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << endl;
                    _remObject->PreMatch(wavFile,verPath);
                    // _logText << "Boucle récup - 2" << endl;
                    _remObject->PostMatch(false,verPath);
                    // _logText << "Boucle récup - 3" << endl;
                    if(_remObject->nbe2 > 0)
                    {
                        _numberRecupTags += _remObject->nbe2;
                        nbTags += _remObject->nbe2;
                        info1=wavFile+QString(" - après récupération sur version ")
                          +verPath+" : "+QString::number(nbTags)+QString(" étiquettes rematchées.");
                        _logText << info1 << endl;
                        info2=_wavPath + QString(" : ") + QString::number(_numberEndTags+_numberRecupTags)
                                + QString(" étiquettes rematchées.");
                        emit information2(info2);
                        // _logText << "Boucle récup - 4 - cas trouvé fin" << endl;
                    }
                }
                if(_fileIndex == _filesNumber-1 && _numberRecupTags>0)
                  _logText << "Après récupération sur versions antérieures, rematchage du répertoire "<< info2 << endl;
                // _logText << "Boucle récup - 5" << endl;
            }
            //
            // _logText << "fin de traitement du fichier 1" << endl;
            _remObject->EndMatch();
            // _logText << "fin de traitement du fichier 2" << endl;
        }
    }
    _logText << "fin de traitement du fichier 3" << endl;
    emit moveBar((float)_fileIndex/(float)_filesNumber);
    _treating = false;
    return(true);
}


bool Detec::readDirectoryVersion()
{
    //_logText << "rvr début" << endl;
    QString cbase = _wavPath + _baseIniFile;
    _dirLogVersion = 0;
    _dirUserVersion = 0;
    if(!QFile::exists(cbase)) return(false);
    QSettings settings(cbase, QSettings::IniFormat);
    settings.beginGroup("version");
    _dirLogVersion = settings.value("log").toInt();
    _dirUserVersion = settings.value("user").toInt();
    settings.endGroup();
    //_logText << "rvr fin" << endl;
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
                // _logText << testName << " existe et s'ouvre bien" << endl;
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
