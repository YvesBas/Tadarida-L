#ifndef LOUPE_H
#define LOUPE_H

#include <QMainWindow>
#include <QMessageBox>
#include "fenim.h"
class Fenim;

#define CRESTSNUMBER 5

class Loupe : public QMainWindow
{
    Q_OBJECT
public:
    explicit Loupe(Fenim *pf,QMainWindow *parent = 0,int x=0,int y=0);
    ~Loupe();
    void                               ShowBubble(QString sbulle);
    void                               ShowCalls();
    void                               ShowGrid(bool);
    void                               ShowLoupe();
    void                               ShowMasterPoints();
    void                               ShowOneCall(int ncri,bool crisel,bool affichercri);
    void                               ShowOneMasterPoint(int ncri,bool crisel,bool afficherpm);
    void                               ShowOneOtherCrestl(int ncri,bool affichercri);
    void                               ShowOneOtherPoint(int ncri,bool affichercri);
    void                               ShowOtherCrests();
    void                               ShowOtherPoints();
    void                               Zoom(int x,int y);
    void                               ZoomeF(float,float);

    QCheckBox                          *BcCalls;
    QCheckBox                          *BcMasterPoints;
    QCheckBox                          *BcSuppl;
    QImage                             *Fenima;
    QLabel                             *LabelX;
    QLabel                             *LabelY;
    QLabel                             *LabelR;
    QLabel                             *LabelEnergy;
    int                                LastCenterX,LastCenterY;
    MyQGraphicsScene                   *LoupeScene;
    QGraphicsView                      *LoupeView;
    Fenim                              *PFenim;

signals:
    
public slots:
    void                               ActivateCalls();
    void                               ActivateGrid(int);
    void                               ActivateMasterPoints();
    void                               ActivateOtherCrests();
    void                               UnZoom();
    void                               UnZoomH();
    void                               Zoom();
    void                               ZoomH();

protected:
    void                               resizeEvent(QResizeEvent *);

private:
    float                              getRatio();
    void                               showButtons();
    void                               showRatio();

    QCheckBox                          *_bcGrid;
    MyQPushButton                      *_bZoom;
    MyQPushButton                      *_bZoomH;
    MyQPushButton                      *_bUnZoom;
    MyQPushButton                      *_bUnZoomH;
    int                                _callColour[MAXCRI];
    QGroupBox                          *_gboxButtons;
    QGraphicsEllipseItem               *_gepm[MAXCRI];
    QGraphicsPathItem                  **_gplt[CRESTSNUMBER];
    QGraphicsEllipseItem               *_gepsu[CRESTSNUMBER][NSUPPL][MAXCRI];
    QGraphicsLineItem                  *_gliv[500];
    QGraphicsLineItem                  *_glih[500];
    bool                               _ilc[MAXCRI];
    bool                               _ilcs[MAXCRI];
    bool                               _ilps[MAXCRI];
    bool                               _ipmc[MAXCRI];
    float                              _lWl,_lWh;
    float                              _liaj;
    int                                _lzx,_lzy,_llf,_lhf,_hbo,_lbo;
    int                                _nliv,_nlih;
    QGraphicsPixmapItem                *_pix;
    bool                               _vaf;
};

#endif // LOUPE_H
