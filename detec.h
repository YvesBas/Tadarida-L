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
#include <QtCore/qmath.h>
#include <QTextStream>
#include <QTimer>
#include <QProcess>
#include "sndfile.h"
#include "fftw3.h"
#include "detectreatment.h"


class Detec : public QObject
{
    Q_OBJECT
public:    
    explicit Detec(QMainWindow* parent = 0);
    ~Detec();
    bool InitializeDetec(const QStringList&, QString,bool,int,int,bool,RematchClass *); 
    void Pause();
    void Resume();
    void SetGlobalParameters(int,int,int,int,int,
                             bool,int,int,int,int,int,int,int);
    int Treatment();

    DetecTreatment               *_detecTreatment;
    bool                         _errorFileOpen;
    QTextStream                  _errorStream;
    bool                         IsRunning;
    QTextStream                  _logText;
    int                          _logVersion;
    bool                         MustCancel;
    bool                         MustCompress;
    bool                         ReprocessingMode;
    QString                      ResultSuffix;
    QString                      ResultCompressedSuffix;
    int                          _userVersion;

	
signals:
    void errorDetec(int);
    void information(QString);
    void information2(QString);
    void information3(int,int,int);
    void moveBar(float);
    void threadFinished();
    void infoTrace(QString,QString,QString,QString,QString,QString,QString,QString,QString,QString);
    void dirProblem();
private slots:
    bool treatOneFile();

private:

    // methods
    uint calculateRGB(double);
    void cleanVerSubdir();
    void cleanSubdir(QDir,QString,bool,QString);
    void createImage(QString);
    void createVersionsList();
    void endDetec();
    void initBvrvb(double,double,double);
    bool readDirectoryVersion();
    void saveCallsMatrix(QString);
    void saveDatFile(QString);
    void saveParameters(const QString&);
    void writeDirectoryVersion();
    bool checkAssociatedFiles(QString,QString);

    // attributes
    QDir                         _baseDayDir;
    float                        _callEnergyMax ;
    int                          _callEnergyMaxIndex;
    QVector< QVector<QPoint> >   _callsArray;
    QVector<QPoint>              _callMasterRidge;
    QVector< QVector<QPoint> >   _callMasterRidgeArray;
    QFile                        _callMatrixFile;
    QString                      _callMatrixName;
    QDataStream                  _callMatrixStream;
    QVector<QPoint>              _callNorthRidge;
    QVector< QVector<QPoint> >   _callNorthRidgeArray;
    QVector<QPoint>              _callSecondWestRidge;
    QVector< QVector<QPoint> >   _callSecondWestRidgeArray;
    QVector<QPoint>              _callSouthRidge;
    QVector< QVector<QPoint> >   _callSouthArray;
    QVector<QPoint>              _callWestRidge;
    QVector< QVector<QPoint> >   _callWestRidgeArray;
    int                          _callsNumber;
    QTimer                       *_clock;
    bool                         _collectPreviousVersionsTags;
    QString                      _datPath;
    int                          _dirLogVersion;
    int                          _dirUserVersion;
    QFile                        _errorFile;
    Fenim                        *_fenim;
    Fenim                        *_fenim2;
    int                          _fileIndex;
    bool                         _fileProblem;
    int                          _filesNumber;
    bool                         _imageData;
    QString                      _imagePath;
    QFile                        _logFile;
    int                          _numberEndTags;
    int                          _numberRecupTags;
    int                          _numberStartTags;
    QMainWindow                  *_parent;
    RematchClass                 *_remObject;
    bool                         _treating;
    QFile                        _txtFile;
    QString                      _txtPath;
    QFile                        _txtFile2;
    QString                      _txtFilePath2;
    uint                         _tvaleur[2000];
    QVector< QPoint >            _vectorCallPoints;
    QVector < int >         _vectorXMin;
    QVector < int >        _vectorXMax;
    QStringList               _versionDirList;
    int                              **_versionIndicators;
    bool                           _waiting;
    QString                     _wavFile;
    QStringList               _wavFileList;
    QString                     _wavPath;
};

#endif
