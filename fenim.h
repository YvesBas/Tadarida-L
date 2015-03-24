// 17/11/2014 : 463 lignes avant grosses modifs
#ifndef FENIM_H
#define FENIM_H

#define mlg 500
#define MAXCRI 2000
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
class Loupe;
class Fenim;


class MyQGraphicsView : public QGraphicsView
{
public:
    MyQGraphicsView(QMainWindow *parent);
    QMainWindow *qmaitre;
protected:
    void resizeEvent ( QResizeEvent * resizeEvent );
};

class MyQGraphicsScene : public QGraphicsScene
{
public:
    MyQGraphicsScene(Fenim *pf,QMainWindow *parent,bool il);
    QMainWindow *qmaitre;
    Fenim *pfenim;
    Loupe *ploupe;
    bool iloupe;

protected:
    void mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent );
    void mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * mouseEvent );
    void mouseReleaseEvent ( QGraphicsSceneMouseEvent * mouseEvent );
    void keyPressEvent(QKeyEvent* e);
    void mouseMoveEvent ( QGraphicsSceneMouseEvent * mouseEvent );
    int derx;
    int dery;
    QGraphicsRectItem *gri;
    bool algri;
};



class MyQLineEdit : public QLineEdit
{
public:
    MyQLineEdit(QWidget *parent,Fenim *fen,QString cod);
    QWidget *qmaitre;
    Fenim *pfenim;
    QString codeedit;
protected:
    void keyPressEvent(QKeyEvent* e);
    void textEdited(const QString&);
};

class MyQComboBox : public QComboBox
{
public:
    MyQComboBox(QWidget *parent,Fenim *fen,QString code="");
    QWidget *qmaitre;
    QStringList *ql;
    QString codecombo;
    Fenim *pfenim;
protected:
    // void mousePressEvent(QMouseEvent *e);
    void keyPressEvent(QKeyEvent* e);
    //void focusInEvent(QFocusEvent *e);

private:
    // 도도도도도도도�
};

class EditCombo;

class EC_LineEdit : public MyQLineEdit
{
    Q_OBJECT
public:
    EC_LineEdit(QWidget *parent,Fenim *fen,QString cod);
    EditCombo *ecp;
    void setecp(EditCombo *);
private:
    Fenim *pfenim;
public slots:
    void modifie_texte(const QString&);
protected:
    void keyPressEvent(QKeyEvent* e);
};

class Simple_LineEdit : public MyQLineEdit
{
    Q_OBJECT
public:
    Simple_LineEdit(QWidget *parent,Fenim *fen,QString cod);
private:
    Fenim *pfenim;
public slots:
    void modifie_texte(const QString&);
};



class EC_ComboBoxEdit : public MyQComboBox
{
    Q_OBJECT
public:
    EC_ComboBoxEdit(QWidget *parent,Fenim *fen,QString cod);
    EditCombo *ecp;
    void setecp(EditCombo *);
public slots:
    void selectionne_code(const QString&);
protected:
    void keyPressEvent(QKeyEvent* e);
};


class EditCombo
{
public:
    EditCombo(QWidget *parent,Fenim *fen,QString cod,bool autaj=true);
    QMainWindow *fenpar;
    EC_LineEdit *ec_le;
    EC_ComboBoxEdit *ec_co;
    QString codefi;
    QStringList *liste_codes;
    bool autorise_ajout;
    void realim_liste(const QString& codsai);
    void selectionne_code();
    void selectionne_code(const QString& codsai);
    bool controle(QString &s,QString &mess_err,bool obl);
    bool confirme_ajout(QString &s);
    void set_autor(bool);
};

class MyQLabel : public QLabel
{
public:
    MyQLabel(QWidget *parent);
    QWidget *qmaitre;
};


class MyQPushButton : public QPushButton
{
public:
    MyQPushButton(QWidget *parent);
    QWidget *qmaitre;
};

enum FIELDTYPE {SLE,EC,SLI,CHB};

class FieldClass
{
public :
    FieldClass(QWidget *parent,Fenim *pf,QString title,int fieldType,bool obl,bool unic,QString fieldCode,bool autaj,int smin,int smax);
    ~FieldClass();
    Fenim *pfenim;
    QString _title;
    int _fieldType;
    bool Obl;
    bool Unic;
    int _Smin,_Smax;
    QString _fieldCode;
    MyQLabel *_titleLabel;
    MyQLineEdit *_le;
    Simple_LineEdit *_sle;
    EditCombo *_ec;
    QSlider *_sli;
    QCheckBox *_chb;
    void affect(QString text);
    void colour(QString text);
    QString getText();
};

class FenetreFenim : public QMainWindow
{
    Q_OBJECT
public :
    FenetreFenim(Fenim *pf,QMainWindow *parent);
    Fenim *pfenim;
    void etablit_connexions();

public slots:
    void enreg_etiq();
    void EnregEtiquettes();
    void fermeFenim();
    void selectionne_indice(int);
    void modif_indice(const QString&);
    void clique_confi(int);
    void zoomef(float);
    void zoome();
    void dezoome();
    void actdesactGrille(int);
    void actdesactPMaitres();
    void actdesactCris();
    void choisit_cri();
    void crisuiv();
    void criprec();
    void crifin();
    void crideb();
    void selectSpecTags(const QString& codsel);
    void specTagNext();
    void specTagPrevious();
    void specTagLast();
    void specTagFirst();


protected:
    void closeEvent(QCloseEvent *event);
};

class Fenim
{
    //Q_OBJECT
    Q_GADGET
public:
    explicit Fenim(QMainWindow *parent,QString repwav,QString nomf,QDir baseJour,bool casa,bool  casretr=false,int typeretraitement = 0,QString suffixe="",int vl=0,int vu=0);
    ~Fenim();
    QMainWindow *fparent;
    TadaridaMainWindow *tgui;
    FenetreFenim *wfenim;
    void cree_fenetre(bool modesaisie);
    void afficher_ecran();
    bool afficher_image(bool modesaisie);
    void afficher_cris();
    void afficher_points_maitres();

    void selectionneCri(int n,bool specSelect);
    void selectionneCri(int x,int y,bool isCTRL);
    void afficher_un_cri(int ncri,bool crisel,bool affichercri);
    void afficher_un_point_maitre(int ncri,bool crisel,bool afficherpm);
    void archive_crisel();
    bool affiche_selections(bool specsel=false);
    void selectionneRectCri(int x1,int y1,int x2,int y2,bool isCTRL);
    void selectAllCalls();
    QString calculebulle(int x,int y);
    void affbulle(QString sb);
    QRect getFenetre();
    MyQGraphicsView * getView();
    QImage *getImage();
    void creeloupe(int x,int y);
    float getms(int x);
    float getkhz(int y);
    float getx(float t);
    float gety(float f);
    float getratio();
    void afficheratio();
    void afficher_grille(bool);

    void enreg_etiq();
    void EnregEtiquettes();
    void fermeFenim();
    void selectionne_indice(int);
    void modif_indice(const QString&);
    void clique_confi(int);
    void zoomef(float);
    void zoome();
    void dezoome();
    void actdesactGrille(int);
    void actdesactPMaitres();
    void actdesactCris();
    void choisit_cri();
    void crisuiv();
    void criprec();
    void crifin();
    void crideb();
    void selectSpecTags(const QString& codsel);
    void specTagNext();
    void specTagPrevious();
    void specTagLast();
    void specTagFirst();
    FieldClass *pfc[NBFIELDS];
    QString FileFields[NBFIELDS];

    Loupe *floupe;
    bool affloupe;
    QLabel *labelx;
    QLabel *labely;
    QLabel *labelr;
    QFont m_poltitre,m_poltexte;
    float m_rl,m_rh;
    QTextStream m_logStream;
    int m_nbcris;
    bool *m_crisel;
    Etiquette **lesEtiq;
    QVector< QPoint >  m_pointsMaitres;
    QVector<QPoint>  m_pointsSuppl[NCRETES][NSUPPL];
    QString            imaNom;
    int m_iSizeFFTHalf;
    int m_xmoitie;
    float m_factorX;
    float m_factorY;
    QCheckBox          *bcCris;
    QVector< QVector< QPoint > > m_matrixCalls[NCRETES];
    MyQLineEdit        *editCri;
    bool casA;

    MyQPushButton      *bFlecheDroite;
    MyQPushButton      *bFlecheGauche;
    MyQPushButton      *bFlecheFin;
    MyQPushButton      *bFlecheDebut;
    MyQPushButton      *bsaveUneEtiq;
    MyQPushButton      *bEnregEtiq;
    MyQPushButton      *bClose;
    MyQPushButton      *bZoome;
    MyQPushButton      *bDezoome;
    QCheckBox          *bcGrille;
    QCheckBox          *bcPMaitres;
    QProgressBar       *_prgSessionEnd;
    QComboBox          *chbTagSpec;
    MyQPushButton      *bFlecheDroiteSpec;
    MyQPushButton      *bFlecheGaucheSpec;
    MyQPushButton      *bFlecheFinSpec;
    MyQPushButton      *bFlecheDebutSpec;
    int                specTagList[MAXCRI];
    int                specTagNumber;
    int                specTagSel;

    bool saisie_a_sauver;
    void termine_session();
    bool neplus;
    void videfenim();

    // pour retraitement :
    bool chargeCrisEtiquettes();
    int rematcheEtiquettes(Fenim * fenim1,bool initial,QString recupVersion);
    bool readVersionRep();
    void writeVersionRep();
    int m_verRepLog,m_verRepUser;
    int m_verLog,m_verUser;
    // pour version contour
    bool loadMatriceCris2(QString,bool);
    QVector< QVector< QPoint > > m_matrixContours;
    QVector< QVector< float > > m_matrixEnergie;
    QString m_sufeti;
    int m_modeRep;
    void conserveSaisies(Etiquette *peti);
    void traite_champs_uniques();
    int m_nbeti;
    void repeint_en_vert();
    QString couleur[5];

protected:
    void reinitialise_etiquettes();
    void rafraichit_image();
    void rafraichit_tout();
    void updateTagNSpec(int);
    void updateChbTagSpec();
    void clearSpecTagsSelection();
    void enableMoreArrows();

    QLabel             *labelImage;
    QLabel             *labelTitreG2;
    QGroupBox          *gbox_saisie;
    QGroupBox          *gbox_gen;
    QGroupBox          *gbox_boutons;
    QLabel             *labelCris;

    MyQLabel           *labelMess;
    MyQLabel           *labelRep;
    MyQLabel           *labelTagSpec;
    MyQLabel           *labelNbcri;
    MyQLabel           *labelNbeti;

    bool               fenouv;
    bool               crefen;
    QString            nomImage;
    QImage *fenima;
    QImage *fenimor;
    QString            repWav;
    QString            nomFic;
    QString            nomEti,repEti;
    QString            datFile;
    QString            m_etiNom;
    QPixmap            *pima;
    MyQGraphicsScene   *scene;
    MyQGraphicsView      *view;
    QString            m_crisPath;
    QFile              m_crisFile;
    QDataStream        m_crisStream;

    QVector< QPoint >  m_vectorCalls;
    QVector< float >  m_vectorEnergie;

    QFile       m_logFile;

    QGraphicsPixmapItem *pix;
    QGraphicsLineItem *gliv[500];
    QGraphicsLineItem *glih[500];
    QGraphicsSimpleTextItem *gte[250];

    QGraphicsEllipseItem     *gepm[MAXCRI];
    QGraphicsPathItem        *gplt[MAXCRI];
    int nlt;
    QString            m_lesCris;
    QString            m_derCris;
    bool ipmc[MAXCRI];
    bool ilc[MAXCRI];
    int coucri[MAXCRI];
    int nliv,nlih,nte;
    bool *m_excrisel;
    QFile m_etiFile;
    QTextStream m_etiStream;
    bool bouge_fichiers;
    bool m_ecraserFichier;
    QFile              m_typeFile;
    QTextStream        m_typeStream;
    QStringList m_lesp;
    bool espece_ajoutee;
    //bool autorise_ajout_espece;
    QStringList m_ltyp;
    bool type_ajoute;
    bool autorise_ajout_type;
    int m_l,m_h,m_mx,m_my,m_hbou;
    QRect m_rf;
    QDir baseJour;
    QDir baseJourDat;
    QDir baseJourIma;
    QDir baseJourEti;
    QDir baseJourTxt;
    int npremcrisel;
    int ndercrisel;
    void initialise_cris();
    bool chargeEtiquettes();
    bool confirme_ajout_espece(QString& esp);
    void alim_cobox_type();
    bool confirme_ajout_type(QString& typ);
    void affiche_nbeti();
    void cree_points_maitres();
    void initialise_traits();
    // pour retraitement :
    bool m_casRetraitement;
    int m_typeretraitement;
    Fenim *fenim1;
    // pour version contour
    QString da2File;
    QFile              m_cris2File;
    QDataStream        m_cris2Stream;
    QVector< QPoint >  m_vectorContours;
    void initialiseZones();
    QStringList _listTaggedSpecies;
};


#endif // FENIM_H
