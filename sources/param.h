#ifndef PARAM_H
#define PARAM_H

#include <QMainWindow>
#include <QWidget>
#include <QCheckBox>
#include <QComboBox>
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
    bool                   InputControl();

    int                    CheckBoxNumber;
    int                    ComboNumber;
    int                    EditNumber;

private :
    double                 _doubleMin;
    double                 _doubleMax;
    int                    _intMin;
    int                    _intMax;
    Param                  *_pParam;
    int                    _paramIndex;
    int                    _parameterType;
    void                   *_variablePointer;
};

class Param : public QMainWindow
{
    Q_OBJECT
public:
    Param(QMainWindow *parent,int nbparam);
    ~Param();
    void                    CreateParameter(QString title,void *pvar,int type,int imin=0,int imax=0,double dmin=0.0f,double dmax=0.0f,QStringList *qsl=new QStringList());
    void                    ShowScreen();

    QCheckBox               *CheckBoxArray[20];
    QComboBox               *ComboArray[20];
    QLineEdit               *EditArray[20];
    int                     HalfWidth;
    QLabel                  *LabelArray[20];
    int                     ParamsNumber;
    int                     ParamOrderNumber;
    TadaridaMainWindow      *PMainWindow;

public slots:
    void storeParams();

private:
    int                     _checkBoxOrderNumber;
    int                     _comboOrderNumber;
    int                     _editOrderNumber;
    int                     _editWidth;
    int                     _editX;
    int                     _heightInterval;
    int                     _labelWidth;
    int                     _labelX;
    int                     _mb;
    Parametre               *_parametreArray[20];
    QPushButton             *_saveButton;
    int                     _windowHeight;
};




#endif // PARAM_H
