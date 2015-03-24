#ifndef PARAM_H
#define PARAM_H

#include <QMainWindow>
#include <QWidget>
#include <QCheckBox>
#include <QtGui/QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QSlider>
#include <QKeyEvent>
#include <QSlider>
#include <QToolTip>
#include <QMessageBox>

#include "TadaridaMainWindow.h"
//class SonoChiro_Gui;

class Param;

class Parametre
{
public:
    explicit Parametre(Param *fpa,int indice,void *pvar,int type,int imin,int imax,double dmin,double dmax);
    ~Parametre();
    bool controle();
    int par_ile;
    int par_icb;
    int par_ico;
private :
    Param *par_fpa;
    int par_indice;
    void *par_pvar;
    int par_type;
    int par_imin;
    int par_imax;
    double par_dmin;
    double par_dmax;
};

class Param : public QMainWindow
{
    Q_OBJECT
public:
    Param(QMainWindow *parent,int nbparam);
    ~Param();
    void afficher_ecran();
    TadaridaMainWindow *sg;
    void creeParametre(QString titre,void *pvar,int type,int imin=0,int imax=0,double dmin=0.0f,double dmax=0.0f,QStringList *qsl=new QStringList());
    QLabel *p_label[20];
    QLineEdit *p_edit[20];
    QCheckBox *p_checkbox[20];
    QComboBox *p_combo[20];
    int p_nbparam;
    int n_param;

signals:

public slots:
    void enregParams();

private:
    int p_mh;
    int p_mb;
    int p_lf;
    int p_hf;
    int p_demil;
    int p_xl;
    int p_ll;
    int p_xe;
    int p_le;
    int nle;
    int ncb;
    int nco;
    Parametre *pparametre[20];
    QPushButton *benreg;
};




#endif // PARAM_H
