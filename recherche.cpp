#include "Recherche.h"
#include <QModelIndexList>

Recherche::Recherche(QMainWindow *parent) :
    QMainWindow(parent)
{
    tgui = (TadaridaMainWindow *)parent;
    _findSaveText = "";
    _dirSaveText1 = "";
    _dirSaveText2 = "";
    _fieldSaveNumber = 0;
    _nbFields = tgui->FieldsList.count();
    _withControl = new bool[_nbFields];
    _controlTableList << "especes" << "types" << "" << "zone" << "" << ""
                      << "materiel" << "" << "" << "auteur" << "etiqueteur";
    for(int i=0;i<_nbFields;i++)
    {
        _withControl[i]=false;
        if(i<_controlTableList.size())
            if(_controlTableList.at(i).length()>0) _withControl[i]=true;
    }
}

Recherche::~Recherche()
{
}

void Recherche::afficher_ecran()
{
    showMaximized();
	/*
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
    setWindowTitle("Tadarida - Recherche");
	*/
    _labelSearch = new QLabel(this);
    _labelSearch->setGeometry(30,50,120,20);
    _labelSearch->setText("Texte recherché");
    _labelSearch->setVisible(true);
    _editSearch = new QLineEdit(this);
    _editSearch->setGeometry(150,50,200,20);
    _editSearch->setVisible(true);

    _labelDir = new QLabel(this);
    _labelDir->setGeometry(30,90,100,20);
    _labelDir->setText("Répertoire début");
    _labelDir->setVisible(true);
    _editDir1 = new QLineEdit(this);
    _editDir1->setGeometry(150,90,200,20);
    _editDir1->setText(tgui->DayPath);
    _editDir1->setVisible(true);
    _btnBrowse = new QPushButton(this);
    _btnBrowse->setGeometry(380,90,100,20);
    _btnBrowse->setText("Parcourir");
    _btnBrowse->setVisible(true);

    _labelDir2 = new QLabel(this);
    _labelDir2->setGeometry(530,90,100,20);
    _labelDir2->setText("Répertoire fin");
    _labelDir2->setVisible(true);
    _editDir2 = new QLineEdit(this);
    _editDir2->setGeometry(650,90,200,20);
    _editDir2->setText(tgui->DayPath);
    _editDir2->setVisible(true);
    _btnBrowse2 = new QPushButton(this);
    _btnBrowse2->setGeometry(880,90,100,20);
    _btnBrowse2->setText("Parcourir");
    _btnBrowse2->setVisible(true);

    _cbField = new QComboBox(this);
    _cbField->setGeometry(150,130,200,20);
    _cbField->insertItems(0,tgui->FieldsList);
    _cbField->setVisible(true);

    _btnSearch = new QPushButton(this);
    _btnSearch->setGeometry(30,190,100,20);
    _btnSearch->setText("Rechercher");
    _btnSearch->show();



    _labelReplace = new QLabel(this);
    _labelReplace->setGeometry(30,250,110,20);
    _labelReplace->setText("Texte de remplacement");
    _labelReplace->setVisible(true);
    _editReplace = new QLineEdit(this);
    _editReplace->setGeometry(150,250,150,20);
    _editReplace->setText("");
    _editReplace->setVisible(true);
    _btnReplace = new QPushButton(this);
    _btnReplace->setGeometry(370,250,100,20);
    _btnReplace->setText("Remplacer");
    _btnReplace->setVisible(true);

    _columnTitles << "Fichier" << "Occurences";

    _filesTable = new QTableWidget(10,2,this);
    _filesTable->move(50,320);
    _filesTable->resize(750,400);
    _filesTable->setHorizontalHeaderLabels(_columnTitles);
    _filesTable->setColumnWidth(0,600);
    _filesTable->setColumnWidth(1,100);
    _filesTable->setVisible(true);

    _lblSelectedNumber = new QLabel(this);
    _lblSelectedNumber->setGeometry(170,190,300,20);
    _lblSelectedNumber->setText("");
    _lblSelectedNumber->setVisible(true);

    _btnOpen = new QPushButton(this);
    _btnOpen->setGeometry(680,250,80,20);
    _btnOpen->setText("Ouvrir");
    _btnOpen->setVisible(true);

    connect(_btnSearch,SIGNAL(clicked()),this,SLOT(filesFind()));
    connect(_btnBrowse,SIGNAL(clicked()),this,SLOT(on_btnBrowse_clicked()));
    connect(_btnReplace,SIGNAL(clicked()),this,SLOT(on_btnReplace_clicked()));
    connect(_btnBrowse2,SIGNAL(clicked()),this,SLOT(on_btnBrowse2_clicked()));
    connect(_btnOpen,SIGNAL(clicked()),this,SLOT(on_btnOpen_clicked()));


    activateWindow();
    raise();
}


void Recherche::on_btnBrowse_clicked()
{
    treatBrowse(1);
}

void Recherche::on_btnBrowse2_clicked()
{
    treatBrowse(2);
}

void Recherche::treatBrowse(int nb)

{
    QLineEdit *editDir;
    if(nb==2) editDir = _editDir2; else editDir = _editDir1;
    QString baseDirName  = QFileDialog::getExistingDirectory( this,
                                                              tr("Sélectionner le répertoire de la base à traiter"),
                                                              editDir->text(),
                                                              QFileDialog::ShowDirsOnly);

    if(!baseDirName.isEmpty())
    {
        QString debut = baseDirName.left(baseDirName.lastIndexOf(QString("\\")));
        QString fin  = baseDirName.right(baseDirName.length()-debut.length()-1);

        if(!tgui->getDirectoryType(fin))
        {
            editDir->setText(QDir::fromNativeSeparators(baseDirName));
        }
        else
        {
            QMessageBox::warning(this, "pas type voulu",
                                 fin,QMessageBox::Ok);

        }
    }
}

void Recherche::filesFind()
{
    findTreat(true);
}

// --------------------------------------------------------------------
bool Recherche:: findTreat(bool findMode)
{
    _filesTable->clear();
    _lblSelectedNumber->setText("");
    _filesTable->setHorizontalHeaderLabels(_columnTitles);
    // sélection des fichiers eti du répertoire
    QString searchedText = this->_editSearch->text();
    if(searchedText.isEmpty())
    {
        QMessageBox::warning(this, "Recherche impossible",
                             "Texte à rechercher non saisi !",QMessageBox::Ok);
        return(false);
    }
    int nfield = _cbField->currentIndex();
    if(nfield < 0 || nfield >= _nbFields)
    {
        QMessageBox::warning(this, "Recherche impossible",
                             "Sélectionner le champ à rechercher !",QMessageBox::Ok);
        return(false);
    }
    int nc = nfield +1;
    bool isControlled = _withControl[nfield];
    QString dirPath1(_editDir1->text());
    if(dirPath1.isEmpty())
    {
        QMessageBox::warning(this, "Recherche impossible",
                             "Répertoire non défini !",QMessageBox::Ok);
        return(false);
    }
    QString dirPath2(_editDir2->text());
    if(dirPath2.isEmpty()) dirPath2 = dirPath1;
    //
    QString deb1 = dirPath1.left(dirPath1.lastIndexOf(QString("/")));
    if(dirPath1!=dirPath2)
    {
        QString deb2 = dirPath2.left(dirPath2.lastIndexOf(QString("/")));
        if(deb2 != deb1)
        {
            QMessageBox::warning(this, "Recherche impossible",
                                 "Répertoires pas dans la même base !",QMessageBox::Ok);
            return(false);
        }
        if(dirPath2.compare(dirPath1)<0)
        {
            QString dp=dirPath2;
            dirPath2 = dirPath1;
            dirPath1 = dp;
        }
    }
    //
    QString dp1 =dirPath1.right(8);
    QString dp2 =dirPath2.right(8);
    QStringList directoriesList;
    directoriesList << dp1;
    if(dirPath1!=dirPath2)
    {
        QDir parentDir(deb1);
        if(!parentDir.exists())
        {
            QMessageBox::warning(this, "Recherche impossible",
                                 "Répertoire inaccessible !",QMessageBox::Ok);
            return(false);
        }
        QStringList directoriesList1 = parentDir.entryList(QStringList("*"), QDir::Dirs);
        directoriesList1.sort();
        foreach(QString dirName,directoriesList1)
        {
            if(dirName.length()==8)
                if(!tgui->getDirectoryType(dirName))
                {
                    if(dirName.compare(dp1)>0 && dirName.compare(dp2)<=0)
                        directoriesList << dirName;
                }
        }
    }
    //
    _selFileList.clear();

    int nl=0;
    int nt=0;
    foreach(QString dayDirName,directoriesList)
    {
        QString tagDirName = deb1 + "/" + dayDirName + "/eti";
        QDir searchDir(tagDirName);
        if(!searchDir.exists())continue;
        QStringList tagList = searchDir.entryList(QStringList("*.eti"), QDir::Files);
        bool fileToSelect;
        foreach(QString tagfile, tagList)
        {
            QString tagFileName = tagDirName + "/" + tagfile;
            QFile tagFile(tagFileName);
            QTextStream tagStream;
            fileToSelect = false;
            if(tagFile.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                nt++;
                tagStream.setDevice(&tagFile);
                tagStream.readLine();
                QString tagLine;
                QString readText;
                while(!tagStream.atEnd())
                {
                    tagLine =tagStream.readLine();
                    if(tagLine.isNull() or tagLine.isEmpty()) break;
                    readText=tagLine.section('\t',nc,nc);
                    if(!readText.isEmpty())
                    {
                        if(isControlled)
                        {
                            if(readText==searchedText) fileToSelect = true;
                        }
                        else
                        {
                            if(readText.contains(searchedText)) fileToSelect = true;
                        }
                        if(fileToSelect) break;
                    }
                }
                tagFile.close();
            }
            if(fileToSelect)
            {
                _selFileList << tagFileName;
                if(nl>9) _filesTable->setRowCount(nl+1);
                _filesTable->setCellWidget(nl,0,new QLabel(tagFileName,this));
                nl++;
            }
        }
    }
    if(findMode)
    {
        _lblSelectedNumber->setText(QString("Fichiers sélectionnés : ")+QString::number(nl));
        QString res = QString::number(nl) + " sur "
                +QString::number(nt);
        QMessageBox::warning(this, "Nombre de fichiers eti sélectionnés",res,QMessageBox::Ok);
    }
    if(nl<1)
    {
        if(!findMode)
            QMessageBox::warning(this,"Aucun remplacement !","Aucune occurence trouvée !",QMessageBox::Ok);

        return(false);
    }
    _findSaveText = searchedText;
    _dirSaveText1 = dirPath1;
    _dirSaveText2 = dirPath2;
    _fieldSaveNumber = nc;
    return(true);
}
// --------------------------------------------------------------------
void Recherche::on_btnReplace_clicked()
{
    QString searchedText = this->_editSearch->text();
    if(searchedText.isEmpty())
    {
        QMessageBox::warning(this, "Remplacement impossible",
                             "Texte à remplacer non saisi !",QMessageBox::Ok);
        return;
    }
    //
    int nfield = _cbField->currentIndex();
    if(nfield < 0 || nfield >= _nbFields)
    {
        QMessageBox::warning(this, "Remplacement impossible",
                             "Sélectionner le champ à rechercher !",QMessageBox::Ok);
        return;
    }
    int nc = nfield + 1;
    bool isControlled = _withControl[nfield];
    //
    QString replaceText = this->_editReplace->text();
    if(replaceText.isEmpty())
    {
        QMessageBox::warning(this, "Remplacement impossible",
                             "Texte de remplacement non saisi !",QMessageBox::Ok);
        return;
    }
    // controle
    if(isControlled)
    {
        if(controle(replaceText,nfield)==false)
        {
            QMessageBox::warning(this, "Remplacement impossible",
                                 "Texte de remplacement absent de la table de contrôle !",QMessageBox::Ok);
            return;
        }
    }
    //
    QString dirPath1(_editDir1->text()+"/eti");
    if(dirPath1.isEmpty())
    {
        QMessageBox::warning(this, "Recherche impossible",
                             "Répertoire non défini !",QMessageBox::Ok);
        return;
    }
    QString dirPath2(_editDir2->text()+"/eti");
    if(dirPath2.isEmpty()) dirPath2 = dirPath1;
    // --------------------------------
    //QString               _findSaveText;
    //QString               _dirSaveText;
    //QString               _fieldSaveText;
    if(searchedText != _findSaveText || dirPath1 != _dirSaveText1
            || dirPath2 != _dirSaveText2
            || nc != _fieldSaveNumber)
    {
        if(findTreat(false)==false) return;
    }
    // --------------------------------
    if(_selFileList.isEmpty())
    {
        QMessageBox::warning(this, "Remplacement impossible",
                             "Aucun fichier sélectionné !",QMessageBox::Ok);
        return;
    }
    //
    int nattr = _nbFields+1;
    int nbr = 0;
    int nl = 0;
    int nocc = 0;
    foreach(QString tagFileName,this->_selFileList)
    {
        QFile tagFile(tagFileName);
        QTextStream tagStream;
        QTextStream tagStream2;
        nocc = 0;
        if(tagFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            tagStream.setDevice(&tagFile);
            QString tagLine=tagStream.readLine();
            QString readText;
            bool toUpdate;
            //
            QString tagFileName2 = tagFileName.left(tagFileName.length()-4)+".pro";
            QFile tagFile2;
            tagFile2.setFileName(tagFileName2);
            if(tagFile2.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
            {
                tagStream2.setDevice(&tagFile2);
                tagStream2 << tagLine << endl;
                while(!tagStream.atEnd())
                {
                    tagLine =tagStream.readLine();
                    if(tagLine.isNull() or tagLine.isEmpty()) break;
                    readText=tagLine.section('\t',nc,nc);
                    toUpdate=false;
                    if(!readText.isEmpty())
                    {
                        if(isControlled)
                        {
                            if(readText==searchedText) toUpdate = true;
                        }
                        else
                        {
                            if(readText.contains(searchedText)) toUpdate = true;
                        }
                        if(toUpdate)
                        {
                            nbr++;
                            nocc++;
                        }

                    }
                    if(!toUpdate)
                    {
                        tagStream2 << tagLine << endl;
                    }
                    else
                    {
                        QString tagLine2="";
                        for(int j=0;j<nattr;j++)
                        {
                            if(j==nc)
                            {
                                if(isControlled) tagLine2 += replaceText ;
                                else tagLine2 += readText.replace(searchedText,replaceText);
                            }
                            else tagLine2 += tagLine.section('\t',j,j);
                            if(j<nattr-1) tagLine2 += "\t";
                        }
                        tagStream2 << tagLine2 << endl;
                    }
                }
                tagFile2.close();
            }
            tagFile.close();
            tagFile.remove();
            tagFile2.rename(tagFileName);
            _filesTable->setCellWidget(nl,1,new QLabel(QString::number(nocc),this));
            nl++;
        }
    }
    if(nbr>0)
    {
        QString sp =QString("s").left((int)(nbr>1));
        QString affi = QString::number(nbr)+" remplacement"+sp+" effectué"+sp;
        QMessageBox::warning(this, "Remplacement terminé !",affi,QMessageBox::Ok);
    }
}

bool Recherche::controle(QString rtext,int nf)
{
    if(!_withControl[nf]) return(true);
    QFile fichier;
    fichier.setFileName(_controlTableList.at(nf)+".txt");
    QTextStream textefi;
    bool finded = false;
    if(fichier.open(QIODevice::ReadOnly)==true)
    {
        textefi.setDevice(&fichier);
        while(!textefi.atEnd())
        {
            if(textefi.readLine()==rtext)
            {
                finded = true;
                break;
            }
        }
    }
    return(finded);
}
// --------------------------------------------------------------------
void Recherche::on_btnOpen_clicked()
{
    // QModelIndexList il = _filesTable->    selectedIndexes();

    QModelIndexList il = _filesTable->selectionModel()->selectedIndexes();

    if(il.size()>0)
    {
        int n=il.at(0).row();
        // QString affi = QString::number(n);
        QString tagf = _selFileList.at(n);
        QString wavf = tagf.replace("/eti/","/");
        wavf=wavf.replace(".eti",".wav");
        tgui->updateTags(wavf);
    }
}
// --------------------------------------------------------------------

