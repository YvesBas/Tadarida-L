#ifndef TRANSPARAMETERS_H
#define TRANSPARAMETERS_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QMessageBox>
#include <QCheckBox>

class TadaridaMainWindow;

class TransParameters : public QMainWindow
{
    Q_OBJECT
public:
    explicit TransParameters(QWidget *parent = 0);
    void closeEvent(QCloseEvent* event);
    void showWindow();

signals:

public slots:
    void launchTransfert();
    void cancel();

private:
    QLabel  *_labelAddress;
    QLineEdit *_editAddress;
    QLabel  *_labelSuffix;
    //QLineEdit *_editSuffix;
    QComboBox *_comboSuffix;
    QLabel  *_labelLogin;
    QLineEdit *_editLogin;
    QLabel  *_labelPassword;
    QLineEdit *_editPassword;
    QLabel  *_labelCommand;
    QLineEdit *_editCommand;
    QLabel  *_labelChangeDirectory;
    QLineEdit *_editChangeDirectory;
// QComboBox *p_combo[20];
    QCheckBox *_cbLogin;
    QCheckBox *_cbPassword;

    QPushButton *btnOk;
    QPushButton *btnCancel;
    TadaridaMainWindow *_tmw;
    bool lcom;

};

#endif // TRANSPARAMETERS_H
