#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <cmath>
#include <QBitArray>
#include <QByteArray>
#include <QTextCodec>

#define c1 0.352
#define c2 7.815
#define c3 2.167
#define c4 14.067
#define c5 7.261
#define c6 24.996

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_genSeq_clicked();

    void on_lengthOfShow_valueChanged(int arg1);

    void on_serialTest_clicked();

    double calcConfines(int Nk);

    void on_loadReg_clicked();

    void on_genReg_clicked();

    void on_savReg_clicked();

    void on_correlTest_clicked();

    void on_loadText_clicked();

    void on_codeText_clicked();

    void on_decodeText_clicked();

    void on_serialComboBox_activated(const QString &arg1);

    void on_correlComboBox_activated(const QString &arg1);

    void on_keyRadioButtonForSerial_clicked();

    void on_textRadioButtonForSerial_clicked();

    void on_keyRadioButtonForCorrel_clicked();

    void on_textRadioButtonForCorrel_clicked();

private:
    Ui::MainWindow *ui;

    QBitArray polynom, keySeq;
    int L, a, b, N;
    QBitArray mod;

    double theoryFreq, practFreq, pyrsonKrit=0;
    int k;

    double Sx = 0, Sy = 0, Rk = 0, xy=0, x2=0, y2=0;

    int Nk=0;

    int *K;

    bool t=false;

    double confines;

    QMessageBox msgBox;

    QString fileName;

    QString strKeySeq;
    QString partKeySeq;
    QString strReg;

    QByteArray textByteArray;
    QBitArray textBitArray;
    QByteArray codeByteArray;
    QBitArray codeBitArray;
    QDataStream s;
    int textLength;
    QByteArray decodeByteArray;

    QBitArray buffer;
    int d1=1;
    int d2=1;

    QFile fileReg;
    QFile fileText;
};

#endif // MAINWINDOW_H
