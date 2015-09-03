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
#include <QtNetwork/QFtp>
//#include "application.h"
#include "detec.h"
#include "etiquette.h"
#include "transparameters.h"

#define TRANSTIMEOUT 30000
#define TRANSTIMEOUT2 30000
// #define TRANSNBMINUTESREPRISE 60
#define TRANSNBMINUTESREPRISE 1
#define TRANSMAXREPRISES 10
#define MAXTHREADS 8


enum TRANSPAR {TRANSNOTRANS,TRANSFINISHBEFORE,TRANSFINISH,TRANSDISCONNECT, TRANSCANCEL,
                                 TRANSERROR,TRANSEND,TRANSBEGIN,TRANSSUITE,TRANSCONNECT,TRANSCONNECTED,
                                 TRANSLOGIN,TRANSBEFORECOMMAND,TRANSREADY,TRANSBEFORECD,
                                 TRANSCHANGEDIRECTORY,TRANSCOMMAND,TRANSPUT,TRANSINITIALIZE,TRANSATTENTEREPRISE};


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
    void initialize();
    int PreMatch(QString,QString);
    int PostMatch(bool,QString,int *);
    int EndMatch();
    int nbe1;
    int nbc1;
    int nbe2;
    int nbc2;
    bool Ok;
    Fenim *_fenim1;
    Fenim *_fenim2;

private:
    QMainWindow *_parent;
    QString _fileName;
    QString _wavPath;
    QDir _baseDayDir;
};

class TadaridaMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit TadaridaMainWindow(QWidget *parent = 0);
    ~TadaridaMainWindow();
    void                closeEvent(QCloseEvent* event);
    bool                getDirectoryType(QString);
    void                 updateTags(QString);

    bool                CanContinue;
    QString          DayPath;
    int	               Divrl;
    QStringList    FieldsList;
    QString          LastFields[NBFIELDS];
    QTextStream _logText;
    QString          ResultSuffix;
    QFile              RetreatFile;
    QTextStream RetreatText;
    bool                TextsToSave;
    int                    TransParametersResponse;
    QString           TransSuffix;
    QString           TransAddress;
    QString           TransLogin;
    QString           TransPassword;
    QString           TransCommand;
    QString           TransChangeDirectory;
    bool                 TransGetCommand;
    bool                 StockLogin;
    bool                 StockPassword;
    bool                 IDebug;
    fftwf_complex * _complexInput[10];
    fftwf_complex * _fftRes[10];
    fftwf_plan	     Plan[10][6];
    QString            SearchDir1;
    QString            SearchDir2;

signals:
    void endOfMainWindow();
    
private slots:
    void detecFinished(int);
    void detecInfoTreat(int,int,int);
    void exitProgram();
    void infoShow(QString);
    void infoShow2(QString,bool);
    void infoShow3(QString,bool);
    void manageDetecCall();
    void matchingInfoTreat(int,int,int);
    void on_btnBrowse_clicked();
    void on_btnOk_clicked();
    void on_btnTransfer_clicked();
    void on_btnResume_clicked();
    void on_ledTreatedDirectory_textChanged(const QString &);
    void on_btnFind_clicked();
    void on_btnOpenBase_clicked();
    void on_btnOpenNextWav_clicked();
    void on_btnOpenPreviousWav_clicked();
    void on_btnOpenWav_clicked();
    void on_btnParam_clicked();
    //void on_btnPause_toggled(bool checked);
    void on_btnUpdateTags_clicked();
    bool openWavTag(QString);
    //void storeTrace(QString,QString,QString,QString,QString,QString,QString,QString,QString,QString);
    void treatCancel();
    void updateProgBarValue(float);
    void treatDirProblem();
    //void selectTemps(const QString&);
    void commandBegin(int);
    void commandEnd(int,bool);
    void manageTransfer();
    void infoSend(qint64,qint64);
    void modifyMaxThreads(int);
    void affectTimeExpansions();

private:
    // methods
    void afterRetreating();
    void beforeRetreating();
    void blockUnblock(bool);
    bool consistencyCheck(QString,QString,QString);
    bool createBase();
    void createWindow();
    bool detecCall(QDir,bool);
    bool directoryTreat(QDir,bool);
    bool dirTreat(QString);
    void endTadarida();
    bool generalReprocessing();
    void initializeGlobalParameters();
    bool proposeDayReprocessing();
    bool proposeGeneralReprocessing();
    bool readBaseVersion();
    void readConfigFile();
    bool readDirectoryVersion(QString);
    void readLastTexts();
    QString selectBase();
    void showPicture(QString,QString,bool);
    bool tablesExists();
    bool testBase();
    void writeBaseVersion();
    void writeConfigFile();
    bool writeDirectoryVersion(QString);
    void writeLastTexts();
    void updateBaseVariables();
    void updatesTexts();
    void previousVersionSave(QStringList,QString);
    bool initTransfer(bool);
    void closeTransfer();
    void closeBeforeTransfer();
    void enregTransferedFiles();
    bool readTransferedFiles();
    bool openTransParameters();
    void transReprise();
    void transStopAutomaticWait();
    void messageNonModal(QString,QString);

    // attributes
    QDir             _baseDay;
    QDir             _baseDayDir;
    QDir             _baseDir;
    QString          _baseIniFile;
    int              _baseProgramVersion;
    bool             _baseUpToDate;
    int              _temoinAvant;
    int              _baseUserVersion;
    int              _temoinApres;
    int              _temoinAutre;
    QPushButton      *_btnBrowse;
    QPushButton      *_btnCancel;
    QPushButton      *_btnFind;
    QPushButton      *_btnOk;
    QPushButton      *_btnTransfer;
    QPushButton      *_btnResume;
    QPushButton      *_btnOpenBase;
    QPushButton      *_btnOpenWav;
    QPushButton      *_btnOpenPreviousWav;
    QPushButton      *_btnOpenNextWav;
    QPushButton      *_btnParameters;
    //QPushButton      *_btnPause;
    QPushButton      *_btnUpdateTags;
    bool             _canTag;
    QTimer           *_clock;
    QCheckBox        *_chkCreateImage;
    QCheckBox        *_chkSubDirectories;
    bool             _dayBaseUpToDate;
    int              _detectionThreshold;
    int              _directoryIndex;
    QStringList      _directoriesList;
    int              _directoriesNumber;
    bool             _directoriesRetreatMode;
    QDir             _directoryRoot;
    int              _dirProgramVersion;
    int              _dirUserVersion;
    Fenim            *fenim;
    QGroupBox        *_grpPhase1;
    QGroupBox        *_grpPhase2;
    int              _hab1;
    QDir             _imagesDirectory;
    QString          _iniPath;
    bool             _isFenimWindowOpen;
    bool             _isGeneralReprocessing;
    bool             _isRechercheOpen;
    QString          _lastWav;
    MyQLabel           *_lblBase;
    MyQLabel           *_lblWavDirectory;
    MyQLabel           *_lblPhase1Message;
    MyQLabel           *_lblPhase1Message2;
    MyQLabel           *_lblPhase1Message3;
    MyQLabel           *_lblPhase1Title;
    MyQLabel           *_lblPhase2Title;
    MyQLabel           *_lblTreatedDirectory;
    MyQLabel           *_labelImage;
    QLineEdit      *_ledTreatedDirectory;
    //QComboBox        *_comboTemps;
    int                    _lbou,_hbou,_lbi,_hbi,_lbou2; // sizes of buttons
    QFile              _logFile;
    int                    _lcw,_hcw; // size of main widget
    int                    _lg1,_hg1; // size of phase 1 group box
    int                    _lt,_ht,_ltc; // size of main window
    QWidget        *_mainWidget;
    int                    _margx,_margy; // marges
    int                    _minimumFrequency;
    bool                MustCancel;
    bool                MustEnd;
    QString          _nextWav;
    int	               _overlapsNumber;
//  bool               _littleParams;
    Param           *param;
    int                   _pmx,_pmy; // little marges
    QString          _previousWav;
    QProgressBar     *_prgProgression;
    int                  _programVersion;
    Recherche   *_precherche;
    RematchClass     *_remObject[MAXTHREADS];
    QString         _savedTextsFile;
    int                  _stopThreshold;
    QString         _strDay;
    QString         _wavPath;
    int                  _tadaridaMode;
    int                  _tagsNumberAfter;
    int                  _tagsNumberBefore;
    int                  _tagsNumberFinal;
    int	             _timeExpansion;
    int	             _timeExpansionLeft;
    int	             _timeExpansionRight;
    //QFile            _traceFile;
    //QTextStream _traceText;
    int                  _userVersion;
    QDir             _wavDirectory;
    // à ajouter
    bool _useValflag;
    int _jumpThreshold;
    int _widthBigControl;
    int _widthLittleControl;
    int _highThresholdJB;
    int _lowThresholdJB;
    int _lowThresholdC;
    int _highThresholdC;
    int _qR;
    int _qN;
    int _paramVersion;
    bool _withTimeCsv;
    //
    bool _oneDirProblem;
    QString _treatDirMess;
    QFtp *_ftp;
	int _transState;
	QString _transFile;
    QStringList _transFilesList;
	int _transFilesNumber;
	int _transFileCursor;
    QTimer *_transClock;
    int _transSize;
    int _transSent;
    int _transSentShown;
    qint64 _transBegin;
    qint64 _transBegin2;
    qint64 _transTime;
    qint64 _transTime2;
    int _transferAuthorized;
    bool _transCancel;
    int _transCountMinutes;
    int _transCountSecondes;
    //
    QString _transDirPath;
    //
    bool _transLastTransfer;
    QStringList _transLastTransferedFiles;
    int _transLastTransferedNumber;
    int _transLastFilesNumber;
    int _transNbReprisesAutoConsecutives;
    int _transMinutesSeuil;
    QMessageBox *_pMessageBox;
    bool _alreadyMessage;
    //
    TransParameters *_transParameters;
    //
    // variables ajoutées pour multitheading...
    int _nbThreadsLaunched;
    int _maxThreads;
    int _nbDetecCreated;
    bool TabDetecCreated[MAXTHREADS];
    //
    Detec                  *_pDetec[MAXTHREADS];
    QStringList         _pWavFileList[MAXTHREADS];
    bool                     _threadRunning[MAXTHREADS];
    int                         _nbTreated[MAXTHREADS];
    int                         _stockNbTreated[MAXTHREADS];
    int                         _nbError[MAXTHREADS];
    int                         _stockNbError[MAXTHREADS];
    int                         _tabError[MAXTHREADS][NTERRORS];
    int                         _stockTabError[MAXTHREADS][NTERRORS];
    int                         _nbTreatedTotal;
    int                         _nbErrorTotal;
    int                         _stockNbTreatedTotal;
    int                         _stockNbErrorTotal;

	int                         _filesNumber;
    bool                         _errorFileOpen;
    QTextStream          _errorStream;
    QFile                        _errorFile;
    void initThreads();
    void initThreadsLaunched(int);
    int countThreadsRunning();
    int connectDetectSignals(int);
    void clearThings();
    void  fusionErrors(int);
    void createErrorFile();
    QString createMessageErrors(int,int[][NTERRORS]);
    bool _threadAuthorized;
    QSlider *_sliderThreads;
    QLabel *_lblThreads;
    QRadioButton *_left10;
    QRadioButton *_left1;
    QRadioButton *_left0;
    QRadioButton *_right10;
    QRadioButton *_right1;
    QRadioButton *_right0;
    QGroupBox *_leftGroup;
    QGroupBox *_rightGroup;
};

#endif // MAINWINDOW_H
