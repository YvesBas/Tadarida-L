#include "param.h"


// Parametre class: this class manages each setting managed by the Param class
Parametre::Parametre(Param *parametreWindow,int index,void *pvar,int type,int imin,int imax,double dmin,double dmax):_pParam(parametreWindow),_paramIndex(index),_variablePointer(pvar),_parameterType(type),_intMin(imin),_intMax(imax),_doubleMin(dmin),_doubleMax(dmax)
{
    EditNumber=0; CheckBoxNumber=0; ComboNumber=0;
}

// Param class: this is the class of the window opened whene the user presses the button:
// "advanced settings"
Param::Param(QMainWindow *parent,int nParam) :
    QMainWindow(parent)
{
    PMainWindow = (TadaridaMainWindow *)parent;
    ParamOrderNumber=nParam;
    if(ParamOrderNumber>20) ParamOrderNumber=20;
    ParamsNumber=0;
    _editOrderNumber=0;
    _checkBoxOrderNumber=0;
    _comboOrderNumber=0;
}

Param::~Param()
{
}

// this method shows the Param window
// the input areas for the settings are managed by CreateParameter
void Param::ShowScreen()
{
    showMaximized();
    HalfWidth= 375;
    _windowHeight = 550;
    _heightInterval=_windowHeight/20;
    _mb=_heightInterval;
    _labelX=HalfWidth/20;
    _labelWidth=(HalfWidth*6)/20;
    _editX=_labelX*2+_labelWidth;
    _editWidth=(HalfWidth*8)/20;
    setWindowTitle("Tadarida  -  Update settings");
    _saveButton = new QPushButton(this);
    _saveButton->resize(_labelWidth,20);
    _saveButton->move(_editX,_windowHeight-_mb);
    _saveButton->setText("Save");
    _saveButton->show();
    connect(_saveButton,SIGNAL(clicked()),this,SLOT(storeParams()));
    activateWindow();
    raise();
}

// this method creates and shows the widgets managing one setting
// the method "on_btnParam_clicked()" of the main class calls this method
// for each "global" variable which may be updated
void Param::CreateParameter(QString title,void *pvar,int type,int intmin,int intmax,
                          double doublemin,double doublemax,QStringList *qsl)
{

    if(ParamsNumber>=ParamOrderNumber)return;
    _parametreArray[ParamsNumber]=new Parametre(this,ParamsNumber,pvar,type,intmin,intmax,
                                      doublemin,doublemax);


    int nppcol = (ParamOrderNumber+1)/2;
    int aj=0;
    int ndc=ParamsNumber;
    if(ParamsNumber>=nppcol)
    {
        aj=HalfWidth;
        ndc-=nppcol;
    }

    int hle=_mb+((_windowHeight - _heightInterval - _mb)*(ndc+1))/(nppcol+2);


    if(type==1 || type==2)
    {
        LabelArray[_editOrderNumber]=new QLabel(this);
        LabelArray[_editOrderNumber]->resize(_labelWidth,20);
        LabelArray[_editOrderNumber]->move(_labelX+aj,hle);
        LabelArray[_editOrderNumber]->setText(title);
        LabelArray[_editOrderNumber]->show();
        EditArray[_editOrderNumber]=new QLineEdit(this);
        EditArray[_editOrderNumber]->resize(_editWidth,20);
        EditArray[_editOrderNumber]->move(_editX+aj,hle);
        EditArray[_editOrderNumber]->show();
        if(type==1)
        {
            int ival=*(int *)pvar;
            EditArray[_editOrderNumber]->setText(QString::number(ival));
        }
        else
        {
            double dval=*(double *)pvar;
            EditArray[_editOrderNumber]->setText(QString::number(dval));
        }
        _parametreArray[ParamsNumber]->EditNumber=_editOrderNumber;
        _editOrderNumber++;
    }
    if(type==3)
    {
        bool bval=*(bool *)pvar;
        CheckBoxArray[_checkBoxOrderNumber]=new QCheckBox(this);
        CheckBoxArray[_checkBoxOrderNumber]->move(_labelX+aj,hle);
        CheckBoxArray[_checkBoxOrderNumber]->resize(_editX+_editWidth-_labelX,20);
        CheckBoxArray[_checkBoxOrderNumber]->setText(title);
        CheckBoxArray[_checkBoxOrderNumber]->setChecked(bval);
        CheckBoxArray[_checkBoxOrderNumber]->show();
        _parametreArray[ParamsNumber]->CheckBoxNumber=_checkBoxOrderNumber;
        _checkBoxOrderNumber++;
    }
    if(type==4)
    {
        LabelArray[_editOrderNumber]=new QLabel(this);
        LabelArray[_editOrderNumber]->resize(_labelWidth,20);
        LabelArray[_editOrderNumber]->move(_labelX+aj,hle);
        LabelArray[_editOrderNumber]->setText(title);
        LabelArray[_editOrderNumber]->show();
        _editOrderNumber++;
        ComboArray[_comboOrderNumber]=new QComboBox(this);
        ComboArray[_comboOrderNumber]->move(_editX,hle);
        ComboArray[_comboOrderNumber]->resize(_editWidth,20);
        ComboArray[_comboOrderNumber]->insertItems(0,*qsl);
        QString sval=QString::number(*(int *)pvar);
        ComboArray[_comboOrderNumber]->setCurrentIndex(ComboArray[_comboOrderNumber]->findText(sval));
        ComboArray[_comboOrderNumber]->show();
        _parametreArray[ParamsNumber]->ComboNumber=_comboOrderNumber;
        _comboOrderNumber++;
    }
    ParamsNumber++;
}

// this method checks validity of an entry for one setting
// if it is good, the "global" variable is directly updated
bool Parametre::InputControl()
{
    bool convid=true;
    QString mess_err="";
    if(_parameterType==1 || _parameterType==2)
    {

        if(_parameterType==1)
        {
            int n = _pParam->EditArray[EditNumber]->text().toInt(&convid);
            if(convid)
            {
                if(n < _intMin || n> _intMax)
                {
                    mess_err = _pParam->LabelArray[EditNumber]->text()+" : input out of range";
                    convid = false;
                }
            }
            else mess_err= "Incorrect entry";
            if(convid == false)
                _pParam->EditArray[EditNumber]->setText(QString::number(*(int *)_variablePointer));
            else *(int *)_variablePointer=n;
        }
        if(_parameterType==2)
        {
            double d = _pParam->EditArray[EditNumber]->text().toDouble(&convid);
            if(convid)
            {
                if(d < _doubleMin || d> _doubleMax)
                {
                    mess_err = "Input out of range";
                    convid = false;
                }
            }
            else mess_err = "Incorrect entry";
            if(convid == false)
            {
                _pParam->EditArray[EditNumber]->setText(QString::number(*(double *)_variablePointer));
            }
            else
            {
                *(double *)_variablePointer=d;
            }
        }
        if(convid==false)
        {
            QMessageBox::warning(_pParam,"Error",mess_err,QMessageBox::Ok);
            _pParam->EditArray[EditNumber]->setFocus();

        }
    }
    if(_parameterType==3)
    {
        *(bool *)_variablePointer=_pParam->CheckBoxArray[CheckBoxNumber]->isChecked();
    }
    if(_parameterType==4)
    {
        *(int *)_variablePointer=_pParam->ComboArray[ComboNumber]->currentText().toInt();
    }
    return(convid);
}

// this method saves the updated settings
void Param::storeParams()
{

    bool testInputs=true;
    for(int i=0;i<ParamOrderNumber;i++)
    {
        testInputs=_parametreArray[i]->InputControl();
        if(testInputs==false) break;
    }
    if(testInputs) close();
}
