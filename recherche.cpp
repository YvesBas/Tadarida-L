#include "recherche.h"
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
    _detecTreatment = new DetecTreatment(this);
    // ne sert qu'à utiliser _detecTreatment->_vectPar
}

Recherche::~Recherche()
{
	delete _detecTreatment;
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

	// ajouté le 27/03/2015
    _labelSearch2 = new QLabel(this);
    _labelSearch2->setGeometry(400,50,50,20);
    _labelSearch2->setText("ou");
    _labelSearch2->setVisible(true);
    _editSearch2 = new QLineEdit(this);
    _editSearch2->setGeometry(500,50,200,20);
    _editSearch2->setVisible(true);

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

    _cpComp = new QCheckBox(QString("Comparatif"),this);
    _cpComp->setGeometry(520,190,120,20);
    _cpComp->setChecked(false);
    _cpComp->setVisible(true);
    _cpComp->setEnabled(true);

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
	
	// ajouté le 27/03/2015
	_csvTreat = false;	
    bool fpl;
    QString searchedText2 = this->_editSearch2->text();
	bool s2nn = false;
	if(!searchedText2.isEmpty()) 
	{
		s2nn = true;
        if(findMode && _cpComp->isChecked())
		{
			_csvTreat = true;
            initCsvTable();
		}
	}
	// fin ajout le 27/03/2015
	
    int nl=0;
    int nt=0;
	// 27/03/2015
	QString parFileName,parDirName;
	QDir parDir;
    QFile *parFile;
    QTextStream parStream;
    foreach(QString dayDirName,directoriesList)
    {
        // 27/03/2015
        QString tagDirName = deb1 + "/" + dayDirName + "/eti";
        QDir searchDir(tagDirName);
        if(!searchDir.exists())continue;
        if(_csvTreat)
        {
            parDirName = deb1 + "/" + dayDirName + "/txt";
            QDir parDir(parDirName);
            if(!parDir.exists())continue;
        }
        QStringList tagList = searchDir.entryList(QStringList("*.eti"), QDir::Files);
        bool fileToSelect,find1,find2;
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
                QString parLine;
                fpl = false;
                if(_csvTreat)
                {
                    parFileName = parDirName + "/" + tagfile.left(tagfile.length()-3) + "ta";
                    parFile = new QFile(parFileName);
                    if(parFile->open(QIODevice::ReadOnly | QIODevice::Text)) fpl=true;
                    if(fpl)  { parStream.setDevice(parFile); parStream.readLine(); }
                }
                while(!tagStream.atEnd())
                {
                    tagLine =tagStream.readLine();
                    if(tagLine.isNull() or tagLine.isEmpty()) break;
                    if(_csvTreat && fpl) parLine = parStream.readLine();
                    readText=tagLine.section('\t',nc,nc);
                    // ajouté le 27/3/2015
                    find1=false; find2=false;
                    if(!readText.isEmpty())
                    {
                        if(isControlled)
                        {
                            // modifié le 27/3/2015
                            if(readText==searchedText) {fileToSelect = true; find1=true;}
                            else
                            {
                                if(s2nn && readText==searchedText2) {fileToSelect = true; find2=true;}
                            }
                        }
                        else
                        {
                            // modifié le 27/3/2015
                            if(readText.contains(searchedText)) {fileToSelect = true; find1=true;}
                            else
                            {
                                if(s2nn && readText.contains(searchedText2)) {fileToSelect = true; find2=true;}
                            }
                        }
                        if(fileToSelect)
                        {
                            if(_csvTreat)
                            {
                                if(find1 || find2)
                                {
                                    if(find1) completeCsvTable(searchedText,dayDirName,parLine);
                                    else completeCsvTable(searchedText2,dayDirName,parLine);

                                }
                            }
                            else break;
                        }
                    }
                }
                tagFile.close();
            }
            if(fileToSelect)
            {
                _selFileList << tagFileName;

                // ajouté le 27/03/2015 :
                if(nl>9) _filesTable->setRowCount(nl+1);
                _filesTable->setCellWidget(nl,0,new QLabel(tagFileName,this));

                nl++;
            } // filetoselect = true

            if(_csvTreat)
            {
                if(fpl)  parFile->close();
                delete parFile;
            }
        } // tagfile open
    } // next tagFile
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
	
    if(_csvTreat)
    {
        endCsvTable(searchedText,searchedText2);
    }
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
void Recherche::initCsvTable()
{
    QString txtFilePath = "comparatif.csv";
    _txtFile.setFileName(txtFilePath);
    if(_txtFile.open(QIODevice::WriteOnly | QIODevice::Text)==false)
    {
		_csvTableOpen = false;
        return;
    }
	_csvTableOpen = true;
	//
    _fileStream.setDevice(&_txtFile);
    _fileStream.setRealNumberNotation(QTextStream::FixedNotation);
    _fileStream.setRealNumberPrecision(2);
    _fileStream << "Espece" << '\t' << "Directory" << '\t' << "Filename" << '\t' << "CallNum"
               << '\t' << "Version"<< '\t' << "FileDur"<< '\t' << "SampleRate";
    for(int j=0;j<_detecTreatment->_vectPar.size();j++) _fileStream << '\t' << _detecTreatment->_vectPar[j].ColumnTitle;
    _fileStream << endl;
    _nCompLines = 0;
}
// --------------------------------------------------------------------
void Recherche::completeCsvTable(QString tsel,QString pdir,QString parline)
{
    _fileStream << tsel << '\t' << pdir << '\t' << parline << endl;
    _nCompLines++;
}
// --------------------------------------------------------------------
void Recherche::endCsvTable(QString esp1,QString esp2)
{
    tgui->_logText << "début endCsvTable" << endl;
    _txtFile.close();
    if(_nCompLines < 1) return;
    if(_txtFile.open(QIODevice::ReadWrite | QIODevice::Text)==false) return;
    QString resultLine = "";
    _fileStream.readLine();
    // traitement pour chaque paramètres
    //   nt++;
    int npar = _detecTreatment->_vectPar.size();
    float **tabPL = new float*[npar];
    int **sortPL = new int*[npar];
    for(int j=0;j<npar;j++)
    {
        tabPL[j] = new float[_nCompLines];
        sortPL[j] = new int[_nCompLines];
    }
    int *ws = new int[_nCompLines];
    int ns[2];
    ns[0]=0; ns[1]=0;
    tgui->_logText  << "avant alimentation de tabPL..." << endl;

    for(int i=0;i<_nCompLines;i++)
    {
        QString parLine = _fileStream.readLine();
        //if(parLine.isNull() or parLine.isEmpty()) break;
        QString species = parLine.section('\t',0,0);
        if(species==esp2) ws[i] = 1;  else ws[i] = 0;
        ns[ws[i]]++;
        for(int j=0;j<npar;j++)
        {
            int pospar = 7+j;
            tabPL[j][i] = parLine.section('\t',pospar,pospar).toFloat();
            if((i & 1)==1) tabPL[j][i] += 0.01f;
        }
    }
    // tri esp1, esp2 pour nb cris < chez esp1
    if(ns[1]<ns[0])
    {
        QString cesp1 = esp1;
        esp1 = esp2;
        esp2 = cesp1;
        int cns0 = ns[0];
        ns[0] = ns[1];
        ns[1] = cns0;
        for(int  i=0;i<_nCompLines;i++) ws[i] = 1-ws[i];
    }
    //
    // ajouter un tri pour mélanger ne pas avoir à la suite toutes les esp1
    // ce qui pourrait donner un indicateur 1 sur un paramètre où toutesles valeurs sont égales

    tgui->_logText  << "ns0=" << ns[0] << endl;
    tgui->_logText  << "ns1=" << ns[1] << endl;
    tgui->_logText  << "esp1=" << esp1 << endl;
    tgui->_logText  << "esp2=" << esp2 << endl;

    tgui->_logText  << "avant boucle calculant les résultats" << endl;

    if(ns[0]>0)
    {
        _fileStream << '\t' << '\t'  << '\t' << '\t' << '\t' << '\t';
        for(int j=0;j<npar;j++)
        {
            tgui->_logText  << "j=" << j << endl;
            //QString columnTitle = _detecTreatment->_vectPar[j].ColumnTitle;
            _detecTreatment->sortFloatIndArray(tabPL[j],_nCompLines,sortPL[j]);
            tgui->_logText  << "après tri des valeurs pour paramètre " << j << endl;
            //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
            int ncp = 0, ncg = 0;
            for(int k=0;k<ns[0];k++) if(ws[sortPL[j][k]]==0) ncp++;
            tgui->_logText  << "calcul - ncp = " << ncp << endl;
            if(ncp*2<ns[0])
            {
                for(int k=_nCompLines-ns[0];k<_nCompLines;k++) if(ws[sortPL[j][k]]==0) ncg++;
                tgui->_logText  << "calcul - ncg = " << ncg << endl;
            }
            float indic = ((float)qMax(ncp,ncg))/((float)ns[0]);
            tgui->_logText  << "calcul - indic = " << QString::number(indic) << endl;
            _fileStream <<  '\t' << QString::number(indic);
            tgui->_logText  << "résultat enregistré dans filestream "  << endl;
            //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
            //tgui->_logText << _detecTreatment->_vectPar[j].ColumnTitle << " min=" << vmin << "max=" << vmax << endl;
        }
    }
    tgui->_logText  << "après boucle calculant les résultats" << endl;
    _fileStream << endl;
    // tgui->_logText <<  esp1 << " : " << QString::number(ns[0]) << " lignes" << endl;

    delete[] ws;
    for(int i=0;i<npar;i++)  {delete[] tabPL[i]; delete[] sortPL[i];}
    delete[] tabPL;
    delete[] sortPL;
    _txtFile.close();
    tgui->_logText  << "endCsvTable fin" << endl;
}
// --------------------------------------------------------------------

