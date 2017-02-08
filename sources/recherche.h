#ifndef RECHERCHE_H
#define RECHERCHE_H

#include <QMainWindow>
#include <QWidget>
#include <QCheckBox>
#include <QComboBox>
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
    void showScreen();

private:
    bool findTreat(bool);
    bool controle(QString,int);
    void treatBrowse(int);

    // ajouté le 28/3/2015
    void initCsvTable();
    void completeCsvTable(QString,QString,QString);
    void endCsvTable(QString,QString);

    TadaridaMainWindow    *tgui;
    QPushButton           *_btnOpen;
    QPushButton           *_btnReplace;
    QComboBox             *_cbField;
    QComboBox             *_cbFieldB;
    QCheckBox             *_cpComp;
    QLineEdit             *_editDir1;
    QStringList           _columnTitles;
    QStringList           _controlTableList;
    bool                  _csvTableOpen;
    bool                  _csvTreat;
    DetecTreatment        *_detecTreatment;
    QString               _dirSaveText1;
    QString               _dirSaveText2;
    QLineEdit             *_editDir2;
    QLineEdit             *_editReplace;
    QLineEdit             *_editSearch;
    QLineEdit             *_editSearch2;
    QLineEdit             *_editSearchB;
    QLineEdit             *_editSearchB2;
    QPushButton           *_btnBrowse;
    QPushButton           *_btnBrowse2;
    int                   _fieldSaveNumber;
    QComboBox             *_fieldSearch;
    QTextStream           _fileStream;
    QPushButton           *_btnSearch;
    QStringList           _filesList;
    QString               _findSaveText;
    QTableWidget          *_filesTable;
    QLabel                *_labelDir;
    QLabel                *_labelDir2;
    QLabel                *_labelSearch;
    QLabel                *_labelReplace;
    QLabel                *_labelSearch2;
    QLabel                *_labelSearchB;
    QLabel                *_lblSelectedNumber;
    QLabel                *_labelSearchB2;
    int                   _nbFields;
    int                   _nCompLines;
    QStringList           _selFileList;
    QFile                 _txtFile;
    bool                  *_withControl;

private slots:
    void                  filesFind();
    void                  on_btnBrowse_clicked();
    void                  on_btnBrowse2_clicked();
    void                  on_btnReplace_clicked();
    void                  on_btnOpen_clicked();

};




#endif // Recherche_H
