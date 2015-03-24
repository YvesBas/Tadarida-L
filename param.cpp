#include "param.h"


Parametre::Parametre(Param *fpa,int indice,void *pvar,int type,int imin,int imax,double dmin,double dmax):par_fpa(fpa),par_indice(indice),par_pvar(pvar),par_type(type),par_imin(imin),par_imax(imax),par_dmin(dmin),par_dmax(dmax)
{
    par_ile=0; par_icb=0; par_ico=0;
}

Param::Param(QMainWindow *parent,int nbparam) :
    QMainWindow(parent)
{
    sg = (TadaridaMainWindow *)parent;
    p_nbparam=nbparam;
    if(p_nbparam>20) p_nbparam=20;
    n_param=0;
    nle=0;
    ncb=0;
    nco=0;
}

Param::~Param()
{
}

void Param::afficher_ecran()
{
    showMaximized();
    p_lf = width();
    p_hf = height();
    // p_lf=800;
    // p_hf=600;
    p_mh=p_hf/20;
    p_mb=p_mh;
    p_demil= p_lf/2;
    p_xl=p_demil/20;
    p_ll=(p_demil*6)/20;
    p_xe=p_xl*2+p_ll;
    p_le=(p_demil*8)/20;
    setWindowTitle("Tadarida  -  Mise à jour des paramètres");
    benreg = new QPushButton(this);
    benreg->resize(p_ll,20);
    benreg->move(p_xe,p_hf-p_mb);
    benreg->setText("Enregistrer");
    benreg->show();
    connect(benreg,SIGNAL(clicked()),this,SLOT(enregParams()));
    activateWindow();
    raise();
}


void Param::creeParametre(QString titre,void *pvar,int type,int intmin,int intmax,
                          double doublemin,double doublemax,QStringList *qsl)
{

    if(n_param>=p_nbparam)return;
    // 1) création des objets edit et linedit
    pparametre[n_param]=new Parametre(this,n_param,pvar,type,intmin,intmax,
                                      doublemin,doublemax);


    int nppcol = (p_nbparam+1)/2;
    int aj=0;
    int ndc=n_param;
    if(n_param>=nppcol)
    {
        aj=p_demil;
        ndc-=nppcol;
    }

    int hle=p_mb+((p_hf - p_mh - p_mb)*(ndc+1))/(nppcol+2);


    if(type==1 || type==2)
    {
        p_label[nle]=new QLabel(this);
        p_label[nle]->resize(p_ll,20);
        p_label[nle]->move(p_xl+aj,hle);
        p_label[nle]->setText(titre);
        p_label[nle]->show();
        p_edit[nle]=new QLineEdit(this);
        p_edit[nle]->resize(p_le,20);
        p_edit[nle]->move(p_xe+aj,hle);
        p_edit[nle]->show();
        if(type==1)
        {
            int ival=*(int *)pvar;
            p_edit[nle]->setText(QString::number(ival));
        }
        else
        {
            double dval=*(double *)pvar;
            p_edit[nle]->setText(QString::number(dval));
        }
        pparametre[n_param]->par_ile=nle;
        nle++;
    }
    if(type==3)
    {
        bool bval=*(bool *)pvar;
        p_checkbox[ncb]=new QCheckBox(this);
        p_checkbox[ncb]->move(p_xl+aj,hle);
        p_checkbox[ncb]->resize(p_xe+p_le-p_xl,20);
        p_checkbox[ncb]->setText(titre);
        p_checkbox[ncb]->setChecked(bval);
        p_checkbox[ncb]->show();
        pparametre[n_param]->par_icb=ncb;
        ncb++;
    }
    if(type==4)
    {
        p_label[nle]=new QLabel(this);
        p_label[nle]->resize(p_ll,20);
        p_label[nle]->move(p_xl+aj,hle);
        p_label[nle]->setText(titre);
        p_label[nle]->show();
        nle++; // à changer ajout 4ème variable dédiée labels = plus propre !
        p_combo[nco]=new QComboBox(this);
        p_combo[nco]->move(p_xe,hle);
        p_combo[nco]->resize(p_le,20);
        p_combo[nco]->insertItems(0,*qsl);
        QString sval=QString::number(*(int *)pvar);
        p_combo[nco]->setCurrentIndex(p_combo[nco]->findText(sval));
        p_combo[nco]->show();
        //delete qsl;
        pparametre[n_param]->par_ico=nco;
        nco++;
    }
    n_param++;
}

bool Parametre::controle()
{
    bool convid=true;
    QString mess_err="";
    if(par_type==1 || par_type==2)
    {

        if(par_type==1)
        {
            int n = par_fpa->p_edit[par_ile]->text().toInt(&convid);
            if(convid)
            {
                if(n < par_imin || n> par_imax)
                {
                    mess_err="Saisie hors limites";
                    convid = false;
                }
            }
            else mess_err="Saisie incorrecte";
            if(convid == false)
                par_fpa->p_edit[par_ile]->setText(QString::number(*(int *)par_pvar));
            else *(int *)par_pvar=n;
        }
        if(par_type==2)
        {
            double d = par_fpa->p_edit[par_ile]->text().toDouble(&convid);
            if(convid)
            {
                if(d < par_dmin || d> par_dmax)
                {
                    mess_err="Saisie hors limites";
                    convid = false;
                }
            }
            else mess_err="Saisie incorrecte";
            if(convid == false)
            {
                par_fpa->p_edit[par_ile]->setText(QString::number(*(double *)par_pvar));
            }
            else
            {
                *(double *)par_pvar=d;
            }
        }
        if(convid==false)
        {
            QMessageBox::warning(par_fpa,"Erreur",mess_err,QMessageBox::Ok);
            par_fpa->p_edit[par_ile]->setFocus();

        }
    }
    if(par_type==3)
    {
        *(bool *)par_pvar=par_fpa->p_checkbox[par_icb]->isChecked();
    }
    if(par_type==4)
    {
        *(int *)par_pvar=par_fpa->p_combo[par_ico]->currentText().toInt();
    }
    return(convid);
}

// bEnregEtiq = new MyQPushButton(gbox_gen);
void Param::enregParams()
{

    bool testezones=true;
    for(int i=0;i<p_nbparam;i++)
    {
        testezones=pparametre[i]->controle();
        if(testezones==false) break;
    }
    if(testezones) close();
}
