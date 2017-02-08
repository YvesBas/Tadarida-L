#include "recherche.h"
#include <QModelIndexList>


// the Recherche class creates a window which allows the expert user to
// search for .wav and .eti files
// we can search from two fields, with a "or" possible
// these fields may be selected with a combobox
// two "browse" buttons make it possible to select two day directories
// of the reference database: the search will be done over the entire interval
// between the two dates
// the list of selected files allows you to directly open one of them

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
    _detecTreatment = new DetecTreatment();
}

Recherche::~Recherche()
{
	delete _detecTreatment;
}

// this method creates and shows the graphics objects of the window
void Recherche::showScreen()
{
    showMaximized();
    _labelSearch = new QLabel(this);
    _labelSearch->setGeometry(30,30,120,20);
    _labelSearch->setText("Searched text");
    _labelSearch->setVisible(true);
    _editSearch = new QLineEdit(this);
    _editSearch->setGeometry(150,30,200,20);
    _editSearch->setVisible(true);
    _labelSearch2 = new QLabel(this);
    _labelSearch2->setGeometry(400,30,50,20);
    _labelSearch2->setText("or");
    _labelSearch2->setVisible(true);
    _editSearch2 = new QLineEdit(this);
    _editSearch2->setGeometry(500,30,200,20);
    _editSearch2->setVisible(true);
    _cbField = new QComboBox(this);
    _cbField->setGeometry(150,55,200,20);
    _cbField->insertItems(0,tgui->FieldsList);
    _cbField->setVisible(true);
    _labelSearchB = new QLabel(this);
    _labelSearchB->setGeometry(30,85,120,20);
    _labelSearchB->setText("Searched text");
    _labelSearchB->setVisible(true);
    _editSearchB = new QLineEdit(this);
    _editSearchB->setGeometry(150,85,200,20);
    _editSearchB->setVisible(true);
    _labelSearchB2 = new QLabel(this);
    _labelSearchB2->setGeometry(400,85,50,20);
    _labelSearchB2->setText("or");
    _labelSearchB2->setVisible(true);
    _editSearchB2 = new QLineEdit(this);
    _editSearchB2->setGeometry(500,85,200,20);
    _editSearchB2->setVisible(true);
    _cbFieldB = new QComboBox(this);
    _cbFieldB->setGeometry(150,110,200,20);
    _cbFieldB->insertItems(0,tgui->FieldsList);
    _cbFieldB->setVisible(true);
    _labelDir = new QLabel(this);
    _labelDir->setGeometry(30,140,100,20);
    _labelDir->setText("Folder from");
    _labelDir->setVisible(true);
    _editDir1 = new QLineEdit(this);
    _editDir1->setGeometry(150,140,200,20);
    _editDir1->setText(tgui->SearchDir1);
    _editDir1->setVisible(true);
    _btnBrowse = new QPushButton(this);
    _btnBrowse->setGeometry(380,140,100,20);
    _btnBrowse->setText("Browse");
    _btnBrowse->setVisible(true);
    _labelDir2 = new QLabel(this);
    _labelDir2->setGeometry(530,140,100,20);
    _labelDir2->setText("Folder to");
    _labelDir2->setVisible(true);
    _editDir2 = new QLineEdit(this);
    _editDir2->setGeometry(650,140,200,20);
    _editDir2->setText(tgui->SearchDir2);
    _editDir2->setVisible(true);
    _btnBrowse2 = new QPushButton(this);
    _btnBrowse2->setGeometry(880,140,100,20);
    _btnBrowse2->setText("Browse");
    _btnBrowse2->setVisible(true);
    _btnSearch = new QPushButton(this);
    _btnSearch->setGeometry(30,190,100,20);
    _btnSearch->setText("Search");
    _btnSearch->show();
    _cpComp = new QCheckBox(QString("Comparison"),this);
    _cpComp->setGeometry(520,190,120,20);
    _cpComp->setChecked(false);
    _cpComp->setVisible(true);
    _cpComp->setEnabled(true);
    _labelReplace = new QLabel(this);
    _labelReplace->setGeometry(30,250,110,20);
    _labelReplace->setText("Replacement text");
    _labelReplace->setVisible(true);
    _editReplace = new QLineEdit(this);
    _editReplace->setGeometry(150,250,150,20);
    _editReplace->setText("");
    _editReplace->setVisible(true);
    _btnReplace = new QPushButton(this);
    _btnReplace->setGeometry(370,250,100,20);
    _btnReplace->setText("Replace");
    _btnReplace->setVisible(true);
    _columnTitles << "File" << "Occurences";
    _filesTable = new QTableWidget(10,2,this);
    _filesTable->move(50,300);
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
    _btnOpen->setText("Open");
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

// this method controls the validity of each directory selected
void Recherche::treatBrowse(int nb)
{
    QLineEdit *editDir;
    if(nb==2) editDir = _editDir2; else editDir = _editDir1;
    QString baseDirName  = QFileDialog::getExistingDirectory( this,
     "Select the database folder(s) to treat",editDir->text(),QFileDialog::ShowDirsOnly);
    if(!baseDirName.isEmpty())
    {
        QString debut = baseDirName.left(baseDirName.lastIndexOf(QString("\\")));
        QString fin  = baseDirName.right(baseDirName.length()-debut.length()-1);

        if(!tgui->GetDirectoryType(fin))
        {
            editDir->setText(QDir::fromNativeSeparators(baseDirName));
        }
        else
        {
            QMessageBox::warning(this, "Not a database folder",
                                 fin,QMessageBox::Ok);
        }
    }
}

// this method launches search
void Recherche::filesFind()
{
    findTreat(true);
}

// this method launches the search when it is called by filesFind()
// it is also called by on_btnReplace_clicked(), to replace selected texts
// with the text of the widget: _editReplace
bool Recherche:: findTreat(bool findMode)
{
    _filesTable->clear();
    _lblSelectedNumber->setText("");
    _filesTable->setHorizontalHeaderLabels(_columnTitles);
    // sélection des fichiers eti du répertoire
    QString searchedText = this->_editSearch->text();
    if(searchedText.isEmpty())
    {
        QMessageBox::warning(this, "Error",
                             "Empty searched text !",QMessageBox::Ok);
        return(false);
    }

    int nfield = _cbField->currentIndex();
    if(nfield < 0 || nfield >= _nbFields)
    {
        QMessageBox::warning(this, "Error",
                             "Select the field to search !",QMessageBox::Ok);
        return(false);
    }
    int nc = nfield +1;
    bool isControlled = _withControl[nfield];
    int nfieldB = _cbFieldB->currentIndex();
	int ncB = 0;
	bool isControlledB = false;
	
    if(nfieldB >= 0 && nfieldB < _nbFields) 
	{
		ncB = nfieldB +1;
		isControlledB = _withControl[nfieldB];
	}

    QString dirPath1(_editDir1->text());
    if(dirPath1.isEmpty())
    {
        QMessageBox::warning(this, "Error",
                             "Required folder !",QMessageBox::Ok);
        return(false);
    }
    QString dirPath2(_editDir2->text());
    if(dirPath2.isEmpty()) dirPath2 = dirPath1;
    QString deb1 = dirPath1.left(dirPath1.lastIndexOf(QString("/")));
    if(dirPath1!=dirPath2)
    {
        QString deb2 = dirPath2.left(dirPath2.lastIndexOf(QString("/")));
        if(deb2 != deb1)
        {
            QMessageBox::warning(this, "Error",
                                 "Folders from different databases !",QMessageBox::Ok);
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
            QMessageBox::warning(this, "Error",
                                 "Folder does not exist !",QMessageBox::Ok);
            return(false);
        }
        QStringList directoriesList1 = parentDir.entryList(QStringList("*"), QDir::Dirs);
        directoriesList1.sort();
        foreach(QString dirName,directoriesList1)
        {
            if(dirName.length()==8)
                if(!tgui->GetDirectoryType(dirName))
                {
                    if(dirName.compare(dp1)>0 && dirName.compare(dp2)<=0)
                        directoriesList << dirName;
                }
        }
    }
    _selFileList.clear();
	_csvTreat = false;	
    bool fpl=false;
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
	bool sB = false;
	bool sB2 = false;
	bool sSF = false;
	QString searchedTextB,searchedTextB2;
	if(ncB>0)
	{
		searchedTextB = _editSearchB->text();
		searchedTextB2 = _editSearchB2->text();
		if(!searchedTextB.isEmpty()) 
		{
			sB = true;
		}
		if(!searchedTextB2.isEmpty()) 
		{
			sB2 = true;
		}
        if(ncB == nc && sB==true) sSF = true;
	}
    int nl=0;
    int nt=0;
	QString parFileName,parDirName;
	QDir parDir;
    QFile *parFile;
    QTextStream parStream;
    foreach(QString dayDirName,directoriesList)
    {
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
        bool fileToSelect,find1,find2,fileTSA,fileTSB;
        foreach(QString tagfile, tagList)
        {
            QString tagFileName = tagDirName + "/" + tagfile;
            QFile tagFile(tagFileName);
            QTextStream tagStream;
            fileToSelect = false;
			fileTSA=false; fileTSB=false;
            if(tagFile.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                nt++;
                tagStream.setDevice(&tagFile);
                tagStream.readLine();
                QString tagLine;
                QString readText,readTextB;
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
					if(sB) readTextB = tagLine.section('\t',ncB,ncB);
                    find1=false; find2=false;
                    if(!readText.isEmpty())
                    {
						bool condA = false;
                        if(isControlled)
                        {
                            if(readText==searchedText) {condA = true; find1=true;}
                            else
                            {
                                if(s2nn && readText==searchedText2) {condA = true; find2=true;}
                            }
                        }
                        else
                        {
                            if(readText.contains(searchedText)) {condA = true; find1=true;}
                            else
                            {
                                if(s2nn && readText.contains(searchedText2)) {condA = true; find2=true;}
                            }
							
                        }
						if(!sSF)
                        {
							if(condA) fileToSelect = true;
						}
						else
						{
							if(condA) 
							{
								fileTSA = true;
								if(fileTSA && fileTSB) fileToSelect = true;
							}
						}
						
						if(sB && ((!sSF && fileToSelect) || (sSF && !fileTSB)))
						{
							bool condB = false;
							if(isControlledB)
							{
								if(readTextB==searchedTextB) condB = true; 
								else
								{
                                    if(sB2) if(readTextB == searchedTextB2) condB = true;
								}
							}
							else
							{
                                if(readTextB.contains(searchedTextB)) condB = true;
								else
								{
                                    if(sB2) if(readTextB.contains(searchedTextB2)) condB = true;
								}
							}
							if(!sSF)
                            {
								if(condB==false) fileToSelect = false;
							}
							else
							{
								if(condB==true) 
								{
									fileTSB = true;
									if(fileTSA && fileTSB) fileToSelect = true;
								}
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
                if(nl>9) _filesTable->setRowCount(nl+1);
                _filesTable->setCellWidget(nl,0,new QLabel(tagFileName,this));

                nl++;
            }
            if(_csvTreat)
            {
                if(fpl)  parFile->close();
                delete parFile;
            }
        }
    }
    if(findMode)
    {
        _lblSelectedNumber->setText(QString("Selected files : ")+QString::number(nl));
        QString res = QString::number(nl) + " out of "
                +QString::number(nt);
        QMessageBox::warning(this, "Number of selected labelled files",res,QMessageBox::Ok);
    }
    if(nl<1)
    {
        if(!findMode)
            QMessageBox::warning(this,"No replacement !","No occurence !",QMessageBox::Ok);
        return(false);
    }
    _findSaveText = searchedText;
    _dirSaveText1 = dirPath1;
    _dirSaveText2 = dirPath2;
    tgui->SearchDir1 = dirPath1;
    tgui->SearchDir2 = dirPath2;
    _fieldSaveNumber = nc;
	
    if(_csvTreat)
    {
        endCsvTable(searchedText,searchedText2);
    }
    return(true);
}

// this method replaces selected texts with the text of the widget: _editReplace
void Recherche::on_btnReplace_clicked()
{
    QString searchedText = this->_editSearch->text();
    if(searchedText.isEmpty())
    {
        QMessageBox::warning(this, "Error",
                             "Required searched text !",QMessageBox::Ok);
        return;
    }
    int nfield = _cbField->currentIndex();
    if(nfield < 0 || nfield >= _nbFields)
    {
        QMessageBox::warning(this, "Error",
                             "Select file to search !",QMessageBox::Ok);
        return;
    }
    int nc = nfield + 1;
    bool isControlled = _withControl[nfield];
    QString replaceText = this->_editReplace->text();
    if(replaceText.isEmpty())
    {
        QMessageBox::warning(this, "Error",
                             "Required replacement text !",QMessageBox::Ok);
        return;
    }
    // control
    if(isControlled)
    {
        if(controle(replaceText,nfield)==false)
        {
            QMessageBox::warning(this, "Error",
                                 "Replacement text not in the control table !",QMessageBox::Ok);
            return;
        }
    }
    QString dirPath1(_editDir1->text()+"/eti");
    if(dirPath1.isEmpty())
    {
        QMessageBox::warning(this, "Error",
                             "Undefined folder !",QMessageBox::Ok);
        return;
    }
    QString dirPath2(_editDir2->text()+"/eti");
    if(dirPath2.isEmpty()) dirPath2 = dirPath1;
    if(searchedText != _findSaveText || dirPath1 != _dirSaveText1
            || dirPath2 != _dirSaveText2
            || nc != _fieldSaveNumber)
    {
        if(findTreat(false)==false) return;
    }
    if(_selFileList.isEmpty())
    {
        QMessageBox::warning(this, "Error",
                             "No selected file !",QMessageBox::Ok);
        return;
    }
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
        QString affi = QString::number(nbr)+" replacement"+sp;
        QMessageBox::warning(this, "End of treatment !",affi,QMessageBox::Ok);
    }
}

// this method controls inputs for fields which are controlled by reference tables
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

// this method opens a .wav file in labelling mode
// for it, it launches UpdateTags method of the main class
void Recherche::on_btnOpen_clicked()
{
    QModelIndexList il = _filesTable->selectionModel()->selectedIndexes();
    if(il.size()>0)
    {
        int n=il.at(0).row();
        QString tagf = _selFileList.at(n);
        QString wavf = tagf.replace("/eti/","/");
        wavf=wavf.replace(".eti",".wav");
        tgui->UpdateTags(wavf);
    }
}

// these three methods are used when a special checkbox is checked: _cpComp
// they are used to compare data of ".ta" files associated to ".eti" files selected
// for two species, and make a ".csv" file with these data
// it can be used by expert user to analyse settings for these species
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
    for(int j=0;j<_detecTreatment->VectPar.size();j++) _fileStream << '\t' << _detecTreatment->VectPar[j].ColumnTitle;
    _fileStream << endl;
    _nCompLines = 0;
}

void Recherche::completeCsvTable(QString tsel,QString pdir,QString parline)
{
    _fileStream << tsel << '\t' << pdir << '\t' << parline << endl;
    _nCompLines++;
}

void Recherche::endCsvTable(QString esp1,QString esp2)
{
    _txtFile.close();
    if(_nCompLines < 1) return;
    if(_txtFile.open(QIODevice::ReadWrite | QIODevice::Text)==false) return;
    QString resultLine = "";
    _fileStream.readLine();
    int npar = _detecTreatment->VectPar.size();
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
    if(ns[0]>0)
    {
        _fileStream << '\t' << '\t'  << '\t' << '\t' << '\t' << '\t';
        for(int j=0;j<npar;j++)
        {
            _detecTreatment->SortFloatIndArray(tabPL[j],_nCompLines,sortPL[j]);
            int ncp = 0, ncg = 0;
            for(int k=0;k<ns[0];k++) if(ws[sortPL[j][k]]==0) ncp++;
            if(ncp*2<ns[0])
            {
                for(int k=_nCompLines-ns[0];k<_nCompLines;k++) if(ws[sortPL[j][k]]==0) ncg++;
            }
            int mpg = qMax(ncp,ncg);
            float indic = ((float)mpg)/((float)ns[0]);
            float indic2 = ((float)(ns[1]-ns[0]+mpg))/((float)ns[1]);
            _fileStream <<  '\t' << QString::number((indic+indic2)/2.0f);
        }
    }
    _fileStream << endl;

    delete[] ws;
    for(int i=0;i<npar;i++)  {delete[] tabPL[i]; delete[] sortPL[i];}
    delete[] tabPL;
    delete[] sortPL;
    _txtFile.close();
}

