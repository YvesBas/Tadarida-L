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
#include <QWidget>
#include <QTextStream>
#include <QTimer>
//#include "application.h"
#include "detec.h"
#include "etiquette.h"

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

signals:
    void endOfMainWindow();
    
private slots:
    void detecFinished();
    void detecInfoTreat(int,int);
    void exitProgram();
    void infoShow(QString);
    void infoShow2(QString);
    void manageDetecCall();
    void matchingInfoTreat(int,int,int);
    void on_btnBrowse_clicked();
    void on_btnOk_clicked();
    void on_ledTreatedDirectory_textChanged(const QString &);
    void on_btnFind_clicked();
    void on_btnOpenBase_clicked();
    void on_btnOpenNextWav_clicked();
    void on_btnOpenPreviousWav_clicked();
    void on_btnOpenWav_clicked();
    void on_btnParam_clicked();
    void on_btnPause_toggled(bool checked);
    void on_btnUpdateTags_clicked();
    bool openWavTag(QString);
    //void storeTrace(QString,QString,QString,QString,QString,QString,QString,QString,QString,QString);
    void treatCancel();
    void updateProgBarValue(float);
    void treatDirProblem();
    void selectTemps(const QString&);
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
    QPushButton      *_btnOpenBase;
    QPushButton      *_btnOpenWav;
    QPushButton      *_btnOpenPreviousWav;
    QPushButton      *_btnOpenNextWav;
    QPushButton      *_btnParameters;
    QPushButton      *_btnPause;
    QPushButton      *_btnUpdateTags;
    bool             _canTag;
    QTimer           *_clock;
    QCheckBox        *_chkCreateImage;
    QCheckBox        *_chkSubDirectories;
    bool             _dayBaseUpToDate;
    Detec*           _detec;
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
    MyQLabel           *_lblPhase1Title;
    MyQLabel           *_lblPhase2Title;
    MyQLabel           *_lblTemps;
    MyQLabel           *_lblTreatedDirectory;
    MyQLabel           *_labelImage;
    QLineEdit      *_ledTreatedDirectory;
    QComboBox        *_comboTemps;
    int                    _lbou,_hbou,_lbi,_hbi; // sizes of buttons
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
    RematchClass     *_remObject;
    QString         _savedTextsFile;
    int                  _stopThreshold;
    QString         _strDay;
    int                  _tadaridaMode;
    int                  _tagsNumberAfter;
    int                  _tagsNumberBefore;
    int                  _tagsNumberFinal;
    int	             _timeExpansion;
    //QFile            _traceFile;
    //QTextStream _traceText;
    int                  _userVersion;
    QDir             _wavDirectory;
    // à ajouter
    bool _useValflag;
    int _jumpThreshold;
    int _widthBigControl;
    int _widthLittleControl;
    int _highThreshold;
    int _lowThreshold;
    int _qR;
    int _qN;
    int _paramVersion;
    bool _withTimeCsv;
    //
    bool _oneDirProblem;
    int _nbTreated;
    int _nbError;
    int _stockNbTreated;
    int _stockNbError;
    QString _treatDirMess;
};

#endif // MAINWINDOW_H
