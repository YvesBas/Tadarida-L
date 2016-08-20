#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
#include <QWidget>
#include <QTextStream>
#include <QTimer>
//#include "application.h"
#include "detec.h"
#include "etiquette.h"

#define MAXTHREADS 8

enum MODES {SIMPLE,ETIQUETAGE};

class Param;
class Recherche;
class fenim;
class MyQLabel;

class RematchClass
{
public:
    RematchClass(QMainWindow *parent = 0);
    ~RematchClass();
    void                   initialize();
    int                    EndMatch();
    int                    PostMatch(bool,QString,int *);
    int                    PreMatch(QString,QString);

    Fenim                  *Fenim1;
    Fenim                  *Fenim2;
    int                    Nbc1;
    int                    Nbc2;
    int                    Nbe1;
    int                    Nbe2;
    bool                   Ok;

private:
    QDir                   _baseDayDir;
    QString                _fileName;
    QMainWindow            *_parent;
    QString                _wavPath;
};

class TadaridaMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit TadaridaMainWindow(QWidget *parent = 0);
    ~TadaridaMainWindow();
    bool                   GetDirectoryType(QString);
    void                   UpdateTags(QString);

    bool                   CanContinue;
    fftwf_complex*         ComplexInput[10];
    QString                DayPath;
    int                    Divrl;
    QStringList            FieldsList;
    fftwf_complex*         FftRes[10];
    bool                   IDebug;
    QString                LastFields[NBFIELDS];
    fftwf_plan             Plan[10][6];
    QString                ResultSuffix;
    QFile                  RetreatFile;
    QTextStream            RetreatText;
    QString                SearchDir1;
    QString                SearchDir2;
    bool                   TextsToSave;

private slots:
    void                   affectTimeExpansions();
    void                   cocheFreq();
    void                   detecFinished(int);
    void                   detecInfoTreat(int,int,int);
    void                   exitProgram();
    void                   infoShow(QString);
    void                   infoShow2(QString,bool);
    void                   infoShow3(QString,bool);
    void                   manageDetecCall();
    void                   matchingInfoTreat(int,int,int);
    void                   modifyMaxThreads(int);
    void                   on_btnBrowse_clicked();
    void                   on_btnOk_clicked();
    void                   on_freqLow_toogled(bool);
    void                   on_ledTreatedDirectory_textChanged(const QString &);
    void                   on_btnFind_clicked();
    void                   on_btnOpenBase_clicked();
    void                   on_btnOpenNextWav_clicked();
    void                   on_btnOpenPreviousWav_clicked();
    void                   on_btnOpenWav_clicked();
    void                   on_btnParam_clicked();
    void                   on_btnUpdateTags_clicked();
    bool                   openWavTag(QString);
    void                   treatCancel();
    void                   treatDirProblem();
    void                   updateProgBarValue(float);

private:
    void                   afterRetreating();
    void                   beforeRetreating();
    void                   blockUnblock(bool);
    void                   clearThings();
    void                   closeEvent(QCloseEvent* event);
    void                   connectDetectSignals(int);
    bool                   consistencyCheck(QString,QString,QString);
    int                    countThreadsRunning();
    bool                   createBase();
    void                   createErrorFile();
    QString                createMessageErrors(int,int[][NTERRORS]);
    void                   createWindow();
    bool                   detecCall(QDir,bool);
    bool                   directoryTreat(QDir,bool);
    bool                   dirTreat(QString);
    void                   endTadarida();
    void                   fusionErrors(int);
    bool                   generalReprocessing();
    void                   initializeGlobalParameters();
    void                   initThreads();
    void                   initThreadsLaunched(int);
    bool                   manageBase(bool,bool);
    void                   previousVersionSave(QStringList,QString);
    bool                   proposeDayReprocessing();
    bool                   proposeGeneralReprocessing();
    bool                   readBaseVersion();
    void                   readConfigFile();
    bool                   readDirectoryVersion(QString);
    void                   readLastTexts();
    QString                selectBase();
    void                   showPicture(QString,QString,bool);
    bool                   tablesExists();
    void                   writeBaseVersion();
    void                   writeConfigFile();
    bool                   writeDirectoryVersion(QString);
    void                   writeLastTexts();
    void                   updateBaseVariables();
    void                   updatesTexts();

    QDir                   _baseDay;
    QDir                   _baseDayDir;
    QDir                   _baseDir;
    QString                _baseIniFile;
    int                    _baseProgramVersion;
    bool                   _baseUpToDate;
    int                    _baseUserVersion;
    QPushButton            *_btnBrowse;
    QPushButton            *_btnCancel;
    QPushButton            *_btnFind;
    QPushButton            *_btnOk;
    QPushButton            *_btnOpenBase;
    QPushButton            *_btnOpenWav;
    QPushButton            *_btnOpenPreviousWav;
    QPushButton            *_btnOpenNextWav;
    QPushButton            *_btnParameters;
    QPushButton            *_btnUpdateTags;
    bool                   _canTag;
    QTimer                 *_clock;
    QCheckBox              *_chkCreateImage;
    QCheckBox              *_chkSubDirectories;
    bool                   _dayBaseUpToDate;
    int                    _detectionThreshold;
    bool                   _desactiveCorrectNoise;
    Detec                  *_pDetec[MAXTHREADS];
    int                    _directoryIndex;
    QStringList            _directoriesList;
    int                    _directoriesNumber;
    bool                   _directoriesRetreatMode;
    QDir                   _directoryRoot;
    int                    _dirProgramVersion;
    int                    _dirUserVersion;
    int                    _dirModeFreqVersion;
    QFile                  _errorFile;
    bool                   _errorFileOpen;
    QTextStream            _errorStream;
    Fenim                  *fenim;
    int                    _filesNumber;
    QGroupBox              *_freqGroup;
    QRadioButton           *_freqHigh;
    QRadioButton           *_freqLow;
    QGroupBox              *_grpPhase1;
    QGroupBox              *_grpPhase2;
    int                    _hab1;
    int                    _highThresholdJB;
    int                    _highThresholdC;
    QDir                   _imagesDirectory;
    QString                _iniPath;
    bool                   _isFenimWindowOpen;
    bool                   _isGeneralReprocessing;
    bool                   _isRechercheOpen;
    int                    _jumpThreshold;
    QString                _lastWav;
    MyQLabel               *_lblBase;
    MyQLabel               *_lblWavDirectory;
    MyQLabel               *_lblPhase1Message;
    MyQLabel               *_lblPhase1Message2;
    MyQLabel               *_lblPhase1Message3;
    MyQLabel               *_lblPhase1Title;
    MyQLabel               *_lblPhase2Title;
    MyQLabel               *_lblTreatedDirectory;
    MyQLabel               *_labelImage;
    QLabel                 *_lblThreads;
    int                    _lbou,_hbou,_lbi,_hbi,_lbou2;
    int                    _lcw,_hcw;
    QLineEdit              *_ledTreatedDirectory;
    int                    _lg1,_hg1;
    QRadioButton           *_left0;
    QRadioButton           *_left1;
    QRadioButton           *_left10;
    QGroupBox              *_leftGroup;
    QFile                  _logFile;
    QTextStream            _logText;
    int                    _lowThresholdJB;
    int                    _lowThresholdC;
    int                    _lt,_ht,_ltc;
    QWidget                *_mainWidget;
    int                    _margx,_margy;
    int                    _maxThreads;
    int                    _minimumFrequency;
    int                    _modeFreq;
    int                    _modeFreqBase;
    bool                   _modifyFreqAuthorized;
    bool                   _mustCancel;
    bool                   _mustEnd;
    int                    _nbDetecCreated;
    int                    _nbError[MAXTHREADS];
    int                    _nbErrorTotal;
    int                    _nbThreadsLaunched;
    int                    _nbTreated[MAXTHREADS];
    int                    _nbTreatedTotal;
    QString                _nextWav;
    bool                   _oneDirProblem;
    int                    _overlapsNumber;
    Param                  *param;
    int                    _paramVersion;
    int                    _pmx,_pmy;
    QString                _previousWav;
    QProgressBar           *_prgProgression;
    int                    _programVersion;
    Recherche              *_precherche;
    QStringList            _pWavFileList[MAXTHREADS];
    int                    _qN;
    int                    _qR;
    RematchClass           *_remObject[MAXTHREADS];
    QRadioButton           *_right0;
    QRadioButton           *_right1;
    QRadioButton           *_right10;
    QGroupBox              *_rightGroup;
    QString                _savedTextsFile;
    QSlider                *_sliderThreads;
    int                    _stockNbError[MAXTHREADS];
    int                    _stockNbErrorTotal;
    int                    _stockNbTreated[MAXTHREADS];
    int                    _stockNbTreatedTotal;
    int                    _stockTabError[MAXTHREADS][NTERRORS];
    int                    _stopThreshold;
    QString                _strDay;
    bool                   _tabDetecCreated[MAXTHREADS];
    int                    _tabError[MAXTHREADS][NTERRORS];
    int                    _tadaridaMode;
    int                    _tagsNumberAfter;
    int                    _tagsNumberBefore;
    int                    _tagsNumberFinal;
    bool                   _threadAuthorized;
    bool                   _threadRunning[MAXTHREADS];
    int                    _timeExpansion;
    int	                   _timeExpansionLeft;
    int	                   _timeExpansionRight;
    QString                _treatDirMess;
    bool                   _useValflag;
    int                    _userVersion;
    QDir                   _wavDirectory;
    QString                _wavPath;
    int                    _widthBigControl;
    int                    _widthLittleControl;
    bool                   _withTimeCsv;
};

#endif // MAINWINDOW_H
