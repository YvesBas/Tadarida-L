#ifndef LOUPE_H
#define LOUPE_H

#include <QMainWindow>
#include <QMessageBox>
#include "fenim.h"
class Fenim;

#define NCRETES 5

class Loupe : public QMainWindow
{
    Q_OBJECT
public:
    explicit Loupe(Fenim *pf,QMainWindow *parent = 0,int x=0,int y=0);
    ~Loupe();
    void afficherLoupe();
    void affbulle(QString sbulle);
    // void rafraichit_image();
    void afficher_cris();
    void afficher_points_maitres();
    void afficher_cretes_suppl();
    void afficher_points_suppl();

    void zoomef(float,float);
    QImage *fenima;
    Fenim *pfm;
    QLabel *labelx;
    QLabel *labely;
    QLabel *labelr;
    QLabel *label_ener;
    MyQGraphicsScene   *lscene;
    QGraphicsView      *lview;
    void zoome(int x,int y);
    void afficher_grille(bool);
    float l_rl,l_rh;
    float l_iaj;
    int dercx,dercy;
    QCheckBox          *bcPMaitres;
    QCheckBox          *bcCris;
    QCheckBox          *bcSuppl;
    void afficher_un_cri(int ncri,bool crisel,bool affichercri);
    void afficher_un_point_maitre(int ncri,bool crisel,bool afficherpm);
    void afficher_une_crete_suppl(int ncri,bool affichercri);
    void afficher_un_point_suppl(int ncri,bool affichercri);


signals:
    
public slots:
    void zoome();
    void dezoome();
    void zoomeh();
    void dezoomeh();
    void actdesactGrille(int);
    void actdesactPMaitres();
    void actdesactCris();
    void actdesactSuppl();



protected:
    void resizeEvent(QResizeEvent *);

private:
    int l_zx,l_zy,l_lf,l_hf;
    int h_bo,l_bo;
    bool vaf;
    QGraphicsPixmapItem *pix;
    QGroupBox          *gbox_boutons;
    MyQPushButton      *bZoome;
    MyQPushButton      *bDezoome;
    MyQPushButton      *bZoomeH;
    MyQPushButton      *bDezoomeH;
    QGraphicsLineItem  *gliv[500];
    QGraphicsLineItem  *glih[500];
    int nliv,nlih;
    QCheckBox            *bcGrille;
    QGraphicsEllipseItem *gepm[MAXCRI];
    QGraphicsPathItem    **gplt[NCRETES];
    QGraphicsEllipseItem *gepsu[NCRETES][NSUPPL][MAXCRI];
    bool ipmc[MAXCRI];
    bool ilc[MAXCRI];
    int coucri[MAXCRI];
    //
    bool ilcs[MAXCRI];
    bool ilps[MAXCRI];
    float getratio();
    void afficheratio();
    void afficher_boutons();
};

#endif // LOUPE_H
