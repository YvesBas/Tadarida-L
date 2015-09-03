#include "transparameters.h"
#include "TadaridaMainWindow.h"

TransParameters::TransParameters(QWidget *parent) : QMainWindow(parent)
{
    _tmw = (TadaridaMainWindow *)parent;
    //setWindowModality(Qt::ApplicationModal);
    if(_tmw->TransGetCommand) lcom = 1; else lcom = 0;

}

void TransParameters::closeEvent(QCloseEvent *event)
 {
    if(_tmw->TransParametersResponse == 0) _tmw->TransParametersResponse = 2;
}

void TransParameters::showWindow()
{
    setFixedSize(600,350+lcom*50);
    setWindowTitle("Transfert par FTP ; paramètres");
    _labelSuffix = new QLabel(this);
    _labelSuffix->setGeometry(50,50,100,20);
    _labelSuffix->setText("Suffixe");
    _labelSuffix->setVisible(true);
    _comboSuffix = new QComboBox(this);
    _comboSuffix->setGeometry(200,50,300,20);

    QStringList liste_codes;
    QFile fichier;
    fichier.setFileName("extensions.txt");
    QTextStream textefi;
    if(fichier.open(QIODevice::ReadOnly)==true)
    {
        textefi.setDevice(&fichier);
        QString lte;
        bool trcod =  true;
        while(trcod)
        {
            if(textefi.atEnd()) trcod=false;
            else
            {
                lte = (textefi.readLine());
                if(lte.isNull()) trcod = false;
                else
                {
                    if(lte.isEmpty()) trcod = false;
                    else liste_codes.append(lte);
                }
            }
        }
        fichier.close();
    }
    if(liste_codes.size()<1) liste_codes << "*.wav" << "*.ta" << "*.tac";
    _comboSuffix->insertItems(0,liste_codes);
    _comboSuffix->setVisible(true);
    //
    int pos = _comboSuffix->findText(_tmw->TransSuffix);
    if(pos>=0) _comboSuffix->setCurrentIndex(pos);
//
    _labelAddress = new QLabel(this);
    _labelAddress->setGeometry(50,100,100,20);
    _labelAddress->setText("Adresse");
    _labelAddress->setVisible(true);
    _editAddress = new QLineEdit(this);
    _editAddress->setGeometry(200,100,300,20);
    _editAddress->setVisible(true);
//
    _labelLogin = new QLabel(this);
    _labelLogin->setGeometry(50,150,100,20);
    _labelLogin->setText("Login");
    _labelLogin->setVisible(true);
    _editLogin = new QLineEdit(this);
    _editLogin->setGeometry(200,150,200,20);
    _editLogin->setVisible(true);
    _cbLogin = new QCheckBox(QString("Mémoriser"),this);
    _cbLogin->setGeometry(450,150,100,20);
    _cbLogin->setChecked(false);
    _cbLogin->setVisible(true);
    _cbLogin->setEnabled(true);
//
    _labelPassword = new QLabel(this);
    _labelPassword->setGeometry(50,200,100,20);
    _labelPassword->setText("Mot de passe");
    _labelPassword->setVisible(true);
    _editPassword = new QLineEdit(this);
    _editPassword->setGeometry(200,200,300,20);
    _editPassword->setVisible(true);
    _editPassword->setEchoMode(QLineEdit::Password);
    _cbPassword = new QCheckBox(QString("Mémoriser"),this);
    _cbPassword->setGeometry(450,200,100,20);
    _cbPassword->setChecked(false);
    _cbPassword->setVisible(true);
    _cbPassword->setEnabled(true);
    //
    if(lcom==1)
    {
    _labelCommand = new QLabel(this);
    _labelCommand->setGeometry(50,250,100,20);
    _labelCommand->setText("commande ftp");
    _labelCommand->setVisible(true);
    _editCommand = new QLineEdit(this);
    _editCommand->setGeometry(200,250,300,20);
    _editCommand->setVisible(true);
    }
    //
    _labelChangeDirectory = new QLabel(this);
    _labelChangeDirectory->setGeometry(50,250+lcom*50,100,20);
    _labelChangeDirectory->setText("change directory");
    _labelChangeDirectory->setVisible(true);
    _editChangeDirectory = new QLineEdit(this);
    _editChangeDirectory->setGeometry(200,250+lcom*50,300,20);
    _editChangeDirectory->setVisible(true);
    //
    _editAddress->setText(_tmw->TransAddress);
    //
    _cbLogin->setChecked(_tmw->StockLogin);
    _editLogin->setText(_tmw->TransLogin);
    //
    _cbPassword->setChecked(_tmw->StockPassword);
    _editPassword->setText(_tmw->TransPassword);
    //
    if(lcom==1) _editCommand->setText(_tmw->TransCommand);
    _editChangeDirectory->setText(_tmw->TransChangeDirectory);
    //
    btnOk = new QPushButton(this);
    btnOk->resize(150,20);
    btnOk->move(50,300+lcom*50);
    btnOk->setText("Lancer le transfert");
    btnCancel = new QPushButton(this);
    btnCancel->resize(150,20);
    btnCancel->move(300,300+lcom*50);
    btnCancel->setText("Abandonner");
    connect(btnOk,SIGNAL(clicked()),this,SLOT(launchTransfert()));
    connect(btnCancel,SIGNAL(clicked()),this,SLOT(cancel()));
    activateWindow();
    raise();
    show();
}

void TransParameters::launchTransfert()
{
    if(_editAddress->text().isEmpty() || _editLogin->text().isEmpty() || _editPassword->text().isEmpty())
    {
        QMessageBox::warning(this, "Transfert impossible !","Saisie obligatoire manquante !",QMessageBox::Ok);
        _tmw->_logText <<  "Transfert impossible : saisie obligatoire manquante" << endl;
    }
    else
    {
        _tmw->TransParametersResponse = 1;
        _tmw->TransSuffix = _comboSuffix->currentText();
        _tmw->TransAddress = _editAddress->text();
        _tmw->TransLogin = _editLogin->text();
        _tmw->TransPassword = _editPassword->text();
        _tmw->StockLogin = _cbLogin->isChecked();
        _tmw->StockPassword = _cbPassword->isChecked();
        if(lcom==1) _tmw->TransCommand = _editCommand->text();
        _tmw->TransChangeDirectory = _editChangeDirectory->text();
        close();
    }
}

void TransParameters::cancel()
{
    _tmw->TransParametersResponse = 2;
    close();
}

