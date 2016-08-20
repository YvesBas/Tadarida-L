#ifndef DETEC_H
#define DETEC_H

class Fenim;
class RematchClass;

#include <float.h>
#include <iostream>
#include <QDate>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QMainWindow>
#include <QThread>
#include <QtCore/qmath.h>
#include <QTextStream>
#include <QProcess>
#include "sndfile.h"
#include "fftw3.h"
#include "detectreatment.h"
#include "TadaridaMainWindow.h"

class TadaridaMainWindow;

class Detec : public QThread
{
    Q_OBJECT
public:    
    explicit Detec(QMainWindow* parent = 0,int iThread=0);
    ~Detec();
    void                         run();
    bool                         InitializeDetec(const QStringList&, QString,bool,int,int,bool,bool,RematchClass *,int);
    void                         SetGlobalParameters(int,int,int,int,int,bool,int,int,int,int,int,int,int);

    QFile                        ErrorFile;
    bool                         ErrorFileOpen;
    QTextStream                  ErrorStream;
    bool                         IDebug;
    bool                         ImageData;
    int                          IThread;
    QTextStream                  LogStream;
    bool                         MustCancel;
    bool                         MustCompress;
    float                        NumtE;
    int                          NumVer;
    DetecTreatment               *PDetecTreatment;
    TadaridaMainWindow           *PMainWindow;
    bool                         ReprocessingMode;
    int                          TE;
    bool                         TimeFileOpen;
    QTextStream                  TimeStream;

signals:
    void                         dirProblem();
    void                         errorDetec(int);
    void                         information(QString);
    void                         information2(QString,bool);
    void                         information2b(QString,bool);
    void                         information3(int,int,int);
    void                         information4(int,int,int);
    void                         moveBar(float);
    void                         threadFinished(int);

private slots:
    bool                         treatOneFile();

private:
    uint                          calculateRGB(double);
    bool                         checkAssociatedFiles(QString,QString);
    void                         cleanVerSubdir();
    void                         cleanSubdir(QDir,QString,bool,QString);
    void                         createImage(QString);
    void                         createVersionsList();
    void                         endDetec();
    void                         initBvrvb(double,double,double);
    void                         initBvrvb(double,double);
    bool                         readDirectoryVersion();
    void                         saveCallsMatrix(QString);
    void                         saveDatFile(QString);
    void                         saveParameters(const QString&);
    void                         writeDirectoryVersion();

    QDir                         _baseDayDir;
    double                       _bRGB[5][4];
    float                        _callEnergyMax ;
    int                          _callEnergyMaxIndex;
    QFile                        _callMatrixFile;
    QString                      _callMatrixName;
    QDataStream                  _callMatrixStream;
    QTimer                       *_clock;
    bool                         _collectPreviousVersionsTags;
    QString                      _datPath;
    int                          _dirLogVersion;
    int                          _dirUserVersion;
    int                          _dirModeFreq;
    QString                      _errorFilePath;
    int                          _imaWidth;
    int                          _fileIndex;
    bool                         _fileProblem;
    int                          _filesNumber;
    QString                      _imagePath;
    QFile                        _logFile;
    int                          _logVersion;
    int                          _modeFreq;
    int                          _nbErrorFiles;
    int                          _nbTreatedFiles;
    int                          _numberEndTags;
    int                          _numberRecupTags;
    int                          _numberStartTags;
    QMainWindow                  *_parent;
    float                        _numTe;
    RematchClass                 *_remObject;
    QString                      _resultCompressedSuffix;
    QString                      _resultSuffix;
    QFile                        _timeFile;
    bool                         _treating;
    QFile                        _txtFile;
    QString                      _txtPath;
    QFile                        _txtFile2;
    QString                      _txtFilePath2;
    uint                         _tvaleur[2000];
    int                          _userVersion;
    QVector< QPoint >            _vectorCallPoints;
    QVector < int >              _vectorXMin;
    QVector < int >              _vectorXMax;
    QStringList                  _versionDirList;
    int                          **_versionIndicators;
    QString                      _wavFile;
    QStringList                  _wavFileList;
    QString                      _wavPath;
    bool                         _withTimeCsv;
    bool                         _xHalf;
};

#endif
