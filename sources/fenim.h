#ifndef FENIM_H
#define FENIM_H

#define mlg 500
#define NSUPPL 3
#define NCRETES 5

#include <QMainWindow>
#include <QGraphicsScene.h>
#include <QGraphicsView.h>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include <QWidget>
#include <QTextEdit>
#include <QKeyEvent>
#include <QSlider>
#include <QToolTip>
#include "TadaridaMainWindow.h"
#include <QTextEdit>
#include <QDateTime>
#include <QtCore/qmath.h>
#include <QSettings>

#include "etiquette.h"

#if QT_VERSION >= 0x050000
  #define SCALE(a,b) setTransform(QTransform::fromScale(a,b),true)
#else
  #define SCALE(a,b) scale(a,b)
#endif

class Loupe;
class Fenim;

class MyQGraphicsView : public QGraphicsView
{
public:
    MyQGraphicsView(QMainWindow *parent);
    QMainWindow *qmaitre;
};

class MyQGraphicsScene : public QGraphicsScene
{
public:
    MyQGraphicsScene(Fenim *pf,QMainWindow *parent,bool il);

    bool                            Iloupe;
    Fenim                           *PFenim;
    Loupe                           *Ploupe;
    QMainWindow                     *Qmaitre;

protected:
    void                            keyPressEvent(QKeyEvent* e);
    void                            mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * mouseEvent );
    void                            mouseMoveEvent ( QGraphicsSceneMouseEvent * mouseEvent );
    void                            mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent );

    void                            mouseReleaseEvent ( QGraphicsSceneMouseEvent * mouseEvent );
    bool                            algri;
    int                             derx;
    int                             dery;
    QGraphicsRectItem               *gri;
};



class MyQLineEdit : public QLineEdit
{
public:
    MyQLineEdit(QWidget *parent,Fenim *fen,QString cod);
    QString                         Codeedit;
    Fenim                           *Pfenim;
    QWidget                         *Qmaitre;

protected:
    void                            keyPressEvent(QKeyEvent* e);
    void                            textEdited(const QString&);
};

class MyQComboBox : public QComboBox
{
public:
    MyQComboBox(QWidget *parent,Fenim *fen,QString code="");
    QString                         Codecombo;
    Fenim                           *PFenim;
    QWidget                         *Qmaitre;
    QStringList                     *Ql;

protected:
    void                            keyPressEvent(QKeyEvent* e);
};

class EditCombo;

class EC_LineEdit : public MyQLineEdit
{
    Q_OBJECT
public:
    EC_LineEdit(QWidget *parent,Fenim *fen,QString cod);
    void                            SetEcp(EditCombo *);

    EditCombo                       *Ecp;

private:
    Fenim                           *_pFenim;

public slots:
    void                            UpdateText(const QString&);

protected:
    void                            keyPressEvent(QKeyEvent* e);
};

class Simple_LineEdit : public MyQLineEdit
{
    Q_OBJECT
public:
    Simple_LineEdit(QWidget *parent,Fenim *fen,QString cod);

private:
    Fenim                           *_pFenim;

public slots:
    void                            UpdateText(const QString&);
};



class EC_ComboBoxEdit : public MyQComboBox
{
    Q_OBJECT
public:
    EC_ComboBoxEdit(QWidget *parent,Fenim *fen,QString cod);
    void                            SetEcp(EditCombo *);

    EditCombo                       *Ecp;

public slots:
    void                            SelectCode(const QString&);

protected:
    void                            keyPressEvent(QKeyEvent* e);
};


class EditCombo
{
public:
    EditCombo(QWidget *parent,Fenim *fen,QString cod,QString englishCode,bool allowAdd=true);
    bool                            AllowAdd;
    void                            redoList(const QString&);
    void                            SelectCode();
    void                            SelectCode(const QString& codsai);
    bool                            Control(QString &,QString &,bool);
    bool                            ConfirmAdd(QString &s);

    QString                         CodeFi;
    EC_ComboBoxEdit                 *EcCo;
    EC_LineEdit                     *EcLe;
    QString                         EnglishCode;
    QMainWindow                     *PFenimWindow;
    QStringList                     *ListCodes;
};

class MyQLabel : public QLabel
{
public:
    MyQLabel(QWidget *parent);
    void                            SetText(QString);

    QWidget                         *QMaster;
};

class MyQPushButton : public QPushButton
{
public:
    MyQPushButton(QWidget *parent);
    QWidget                         *QMaster;
};

enum FIELDTYPE {SLE,EC,SLI,CHB};

class FieldClass
{
public :
    FieldClass(QWidget *,Fenim *,QString,int,bool,bool,QString,bool,int,int,QString);
    ~FieldClass();
    void                            Affect(QString text);
    void                            Colour(QString text);
    QString                         GetText();

    QCheckBox                       *ChB;
    EditCombo                       *ECo;
    QString                         EnglishLabel;
    QString                         FieldCode;
    int                             FieldType;
    MyQLineEdit                     *LE;
    bool                            Obl;
    Fenim                           *PFenim;
    Simple_LineEdit                 *SLEd;
    QSlider                         *SLid;
    int                             SMin,SMax;
    QString                         Title;
    MyQLabel                        *TitleLabel;
    bool                            Unic;
};

class FenimWindow : public QMainWindow
{
    Q_OBJECT
public :
    FenimWindow(Fenim *pf,QMainWindow *parent);
    void                            CreateConnections();
    void                            CreateFenimWindow(bool);
    void                            ResizeFenimWindow(bool);

    Fenim                           *PFenim;
    QCheckBox                       *BCCalls;
    QCheckBox                       *BCGrid;
    MyQPushButton                   *BClose;
    MyQPushButton                   *BEndArrow;
    MyQPushButton                   *BEndArrowSpec;
    MyQPushButton                   *BLeftArrow;
    MyQPushButton                   *BLeftArrowSpec;
    QCheckBox                       *BCMasterPoints;
    MyQPushButton                   *BRightArrow;
    MyQPushButton                   *BRightArrowSpec;
    MyQPushButton                   *BSaveLabels;
    MyQPushButton                   *BSaveOneLabel;
    MyQPushButton                   *BStartArrow;
    MyQPushButton                   *BStartArrowSpec;
    MyQPushButton                   *BUnZoom;
    MyQPushButton                   *BZoom;
    QComboBox                       *ChbTagSpec;
    MyQLineEdit                     *EditCall;
    QImage                          *Fenima;
    QGroupBox                       *GBoxInput;
    QGraphicsEllipseItem            *GEpm[MAXCRI];
    QGraphicsLineItem               *GLiv[500];
    QGraphicsLineItem               *GLih[500];
    QGraphicsPathItem               *GPlt[MAXCRI];
    QGraphicsSimpleTextItem         *GTe[250];
    float                           HeightRatio;
    MyQLabel                        *LabelFolder;
    MyQLabel                        *LabelNbLabs;
    QLabel                          *LabelR;
    MyQLabel                        *LabelTagSpec;
    QLabel                          *LabelTitleG2;
    QLabel                          *LabelX;
    QLabel                          *LabelY;
    QGraphicsPixmapItem             *PixMap;
    QFont                           PolTitle,PolText;
    QProgressBar                    *PrgSessionEnd;
    MyQGraphicsScene                *Scene;
    MyQGraphicsView                 *View;
    float                           WidthRatio;
    QRect                           WinRect;

public slots:
    void                            ActivateCrests();
    void                            ActivateGrid(int);
    void                            ActivateMasterPoints();
    void                            ClickConfi();
    void                            CloseFenim();
    void                            EndCall();
    void                            NextCall();
    void                            PreviousCall();
    void                            SaveLabels();
    void                            SelectEditedCalls();
    void                            SelectIndex(int);
    void                            SelectSpecTags(const QString& codsel);
    void                            SpecTagFirst();
    void                            SpecTagLast();
    void                            SpecTagNext();
    void                            SpecTagPrevious();
    void                            StartCall();
    void                            UnZoom();
    void                            UpdateIndex(const QString&);
    void                            ValidateLabel();
    void                            Zoom();

protected:
    void                            closeEvent(QCloseEvent *event);
    void                            resizeEvent ( QResizeEvent * resizeEvent );

private:
    bool                            _alreadyScaled;
    int                             _countResize;
    int                             _fWl,_fWh,_fWmx,_fWmy,_fWbh;
    QGroupBox                       *_gGBoxButtons;
    QGroupBox                       *gBoxGen;
    QLabel                          *_labelCalls;
    QLabel                          *_labelImage;
    MyQLabel                        *_labelMess;
    MyQLabel                        *_labelNbCalls;

};

class Fenim
{
    Q_GADGET
public:
    Fenim(QMainWindow *parent,QString repwav,QString nomf,QDir dayBase,bool casa,bool  casretr=false,int _reprocessingType = 0,QString suffixe="",int vl=0,int vu=0,int freqType=0);
    ~Fenim();
    QString                         CalculateBubble(int,int);
    void                            ClearFenim();
    void                            CreateLoupe(int,int);
    void                            CreatFenimWindow(bool);
    QString                         FileFields[NBFIELDS];
    QImage *                        GetImage();
    float                           GetkHz(int y);
    float                           Getms(int x);
    float                           GetRatio();
    MyQGraphicsView *               GetView();
    QRect                           GetWindowRect();
    float                           GetX(float t);
    float                           GetY(float f);
    bool                            LoadCallsLabels();
    bool                            loadCallsMatrices(QString);
    int                             MatchLabels(Fenim * ,bool,QString,int *);
    FieldClass                      *PFC[NBFIELDS];
    bool                            ReadFolderVersion();
    void                            SelectAllCalls();
    void                            SelectCallsRect(int,int,int,int,bool);
    void                            SelectCall(int n,bool specSelect);
    void                            SelectCall(int x,int y,bool isCTRL);
    void                            ShowBubble(QString sb);
    void                            ShowCalls();
    bool                            ShowFenim(bool inputMode);
    void                            ShowRatio();
    void                            ShowGrid(bool);
    void                            ShowMasterPoints();
    void                            ShowOneCall(int,bool,bool);
    void                            ShowOneMasterPoint(int,bool,bool);
    void                            ShowSelections(bool specsel=false);
    void                            StockSelectedCalls();
    void                            SessionFinish();
    void                            WriteFolderVersion();

    void                            ActivateCrests();
    void                            ActivateGrid(int);
    void                            ActivateMasterPoints();
    void                            ClickConfi();
    void                            CloseFenim();
    void                            EndCall();
    void                            NextCall();
    void                            PreviousCall();
    void                            SaveLabels();
    void                            SelectEditedCalls();
    void                            SelectIndex(int);
    void                            SelectSpecTags(const QString& codsel);
    void                            SpecTagFirst();
    void                            SpecTagLast();
    void                            SpecTagNext();
    void                            SpecTagPrevious();
    void                            StartCall();
    void                            UnZoom();
    void                            UpdateIndex(const QString&);
    void                            ValidateLabel();
    void                            Zoom();
    void                            ZoomF(float);

    bool                            ACase;
    QVector<QPoint>                 AddPointsVector[NCRETES][NSUPPL];
    QVector< QVector< QPoint > >    CallsMatrix[NCRETES];
    QVector< QVector< QPoint > >    ContoursMatrices;
    int                             CallsNumber;
    char                            * EnergyAverageCol;
    QVector< QVector< float > > EnergyMatrix;
    Etiquette                       **EtiquetteArray;
    float                           FactorX;
    float                           FactorY;
    char                            *FlagGoodCol;
    char                            *FlagGoodColInitial;
    int                             FolderLogVersion;
    int                             FolderUserVersion;
    int                             FrequencyType;
    QString                         ImageFullName;
    int                             ImageHeight;
    QString                         ImageName;
    QString                         InputColors[5];
    bool                            InputToSave;
    QString                         LabelsFileName;
    QString                         LabelsFolderName;
    int                             LabelsNumber;
    QTextStream                     LogStream;
    int                             LogVersion;
    QVector< QPoint >               MasterPointsVector;
    bool                            NoMore;
    float                           NumtE;
    int                             NumVer;
    QMainWindow                     *ParentWindow;
    FenimWindow                     *PFenimWindow;
    Loupe                           *PLoupe;
    TadaridaMainWindow              *PMainWindow;
    int                             PtE;
    bool                            *SelectedCalls;
    bool                            ShowLoupe;
    int                             SonogramWidth;
    int                             SpecTagList[MAXCRI];
    int                             SpecTagNumber;
    int                             SpecTagSel;
    int                             UserVersion;
    bool                            WithSilence;
    int                             XHalf;


private:
    void                            clearSpecTagsSelection();
    void                            enableMoreArrows();
    void                            greenPaint();
    void                            initCalls();
    void                            initInputs();
    void                            initLines();
    void                            initMasterPoints();
    void                            inputsSave(Etiquette *);
    bool                            loadLabels();
    void                            reinitLabels();
    void                            showNbLabels();
    void                            treatUniqueFields();
    void                            updateChbTagSpec();
    void                            updateTagNSpec(int);

    int                             _callColour[MAXCRI];
    QFile                           _callsFile;
    QString                         _callsFileName;
    QDataStream                     _callsStream;
    QVector< QPoint >               _callsVector;
    QString                         _callsString;
    QVector< QPoint >               _contoursVector;
    QFile                           _da2File;
    QString                         _da2FileName;
    QDataStream                     _da2Stream;
    QString                         _datFile;
    QDir                            _dayBaseFolder;
    QDir                            _dayDatFolder;
    QDir                            _dayEtiFolder;
    QDir                            _dayImaFolder;
    QDir                            _dayTxtFolder;
    QVector< float >                _energyVector;
    bool                            _filesUpdated;
    int                             _firstCallSelected;
    bool                            _ipmc[MAXCRI];
    bool                            _ilc[MAXCRI];
    QFile                           _labelsFile;
    QString                         _labelsFullFileName;
    QTextStream                     _labelsStream;
    int                             _lastCallSelected;
    QStringList                     _listTaggedSpecies;
    QFile                           _logFile;
    QString                         _mainFileName;
    int                             _nlih;
    int                             _nliv;
    int                             _nte;
    bool                            *_oldSelectedCalls;
    bool                            _overwriteFile;
    QPixmap                         *_pixmap;
    bool                            _reprocessingCase;
    int                             _reprocessingType;
    QString                         _wavFolder;
    bool                            _windowCreated;
    bool                            _windowOpen;
};


#endif // FENIM_H
