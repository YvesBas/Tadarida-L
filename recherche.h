#ifndef RECHERCHE_H
#define RECHERCHE_H

#include <QMainWindow>
#include <QWidget>
#include <QCheckBox>
#include <QtGui/QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QCheckBox>
#include <QKeyEvent>
#include <QToolTip>
#include <QMessageBox>
#include <QString>
#include <QStringList>
#include <QTableView>
#include <QTableWidget>
#include "TadaridaMainWindow.h"
#include <QFile>

#include "detectreatment.h"

class Recherche : public QMainWindow
{
    Q_OBJECT
public:
    Recherche(QMainWindow *parent);
    ~Recherche();
    void afficher_ecran();

private:
    bool findTreat(bool);
    bool controle(QString,int);
    void treatBrowse(int);

    // ajouté le 28/3/2015
    void initCsvTable();
    void completeCsvTable(QString,QString,QString);
    void endCsvTable(QString,QString);

    TadaridaMainWindow    *tgui;
    int                   _nbFields;
    QComboBox             *_cbField;
    QLabel                *_labelSearch;
    QLineEdit             *_editSearch;
    QLabel                *_labelDir;
    QLineEdit             *_editDir1;
    QPushButton           *_btnBrowse;
    QLabel                *_labelDir2;
    QLineEdit             *_editDir2;
    QPushButton           *_btnBrowse2;
    QComboBox             *_fieldSearch;
    QPushButton           *_btnSearch;
	QStringList           _filesList;
    QTableWidget           *_filesTable;
    QStringList           _columnTitles;
    QLabel                *_lblSelectedNumber;
    QLabel                *_labelReplace;
    QLineEdit             *_editReplace;
    QPushButton           *_btnReplace;
    QStringList           _selFileList;
    QString               _findSaveText;
    QString               _dirSaveText1;
    QString               _dirSaveText2;
    int                   _fieldSaveNumber;
    QStringList           _controlTableList;
    bool                  *_withControl;
    QPushButton           *_btnOpen;

    // ajouté le 27/3/2015
    QLabel                    *_labelSearch2;
    QLineEdit               *_editSearch2;
    QCheckBox           *_cpComp;
    QFile    				    _txtFile;
    QTextStream          _fileStream;
    bool                        _csvTableOpen;
    bool                         _csvTreat;
    int                            _nCompLines;
    DetecTreatment    *_detecTreatment;

private slots:
    void                  filesFind();
    void                  on_btnBrowse_clicked();
    void                  on_btnBrowse2_clicked();
    void                  on_btnReplace_clicked();
    void                  on_btnOpen_clicked();

};




#endif // Recherche_H
