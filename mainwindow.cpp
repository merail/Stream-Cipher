#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    srand(time(NULL));

    ui->serialComboBox->addItem("2");
    ui->serialComboBox->addItem("3");
    ui->serialComboBox->addItem("4");

    ui->correlComboBox->addItem("1");
    ui->correlComboBox->addItem("2");
    ui->correlComboBox->addItem("8");
    ui->correlComboBox->addItem("9");

    mod.resize(1);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_genSeq_clicked()
{
    L=int(ui->regLength->value());
    N=int(ui->seqLength->value());
    a=int(ui->aRank->value());
    b=int(ui->bRank->value());
    if(L==0 || N==0 || a==0 || b==0 || t==false)
    {
        msgBox.setText("Неправильно заданы параметры!");
        msgBox.exec();
    }
    else
    {
        ui->regLength->setEnabled(true);
        strKeySeq="";
        keySeq.resize(N);

        for (int j = 0; j < N; j++)
        {
            mod[0] = polynom[a] ^ polynom[b];
            keySeq[j] = polynom[L-1];
            strKeySeq+=keySeq.at(j)?'1':'0';
            for (int i = L-1; i > 0; i--)
            {
                polynom[i] = polynom[i - 1];
            }
            polynom[0] = mod[0];
        }

        Nk=N-ui->correlComboBox->currentText().toInt();
        confines=round(calcConfines(Nk)*10000)/10000;
        ui->correlVal->setText("Допустимые значения: "+QString::number(-confines)
                               + "÷"+QString::number(confines));
        msgBox.setText("Последовательность успешна создана");
        msgBox.exec();
    }
}

void MainWindow::on_lengthOfShow_valueChanged(int arg1)
{
    partKeySeq=strKeySeq.mid(0,arg1);
    ui->showSeq->setPlainText(partKeySeq);
}

double MainWindow::calcConfines(int Nk)
{
    return (1/(double(Nk) - 1) + (2 / (double(Nk) - 2))*sqrt((double(Nk)*(double(Nk) - 3)) / (double(Nk) + 1)));
}

void MainWindow::on_loadReg_clicked()
{
    fileName = QFileDialog::getOpenFileName(this,
        tr("Загрузить файл"), "/home/rail");
    fileReg.setFileName(fileName);
    if(fileReg.open(QIODevice::ReadOnly))
       {
           QTextStream stream(&fileReg);
           strReg=stream.readLine();
           L=strReg.size();
           polynom.resize(L);
           for(int i=0;i<L;i++)
               if(QString(strReg[i]).toInt()==0)
                    polynom.setBit(i,false);
                else
                    polynom.setBit(i,true);
           ui->regLength->setValue(L);
           ui->regLength->setDisabled(true);
           msgBox.setText("Данные успешно загружены");
           msgBox.exec();
           fileReg.close();
       }
    t=true;
}

void MainWindow::on_genReg_clicked()
{
    L=int(ui->regLength->value());
    if(L==0)
    {
        msgBox.setText("Неправильно заданы параметры!");
        msgBox.exec();
    }
    else
    {
        polynom.resize(L);
        for (int i = 0; i < L; i++)
        {
            int s=rand()%2;
            if(s==0)
            {polynom.setBit(i,false);
                        }
            else
                polynom.setBit(i,true);}
        {
            msgBox.setText("Регистр успешно создан");
            msgBox.exec();
        }
    }
    t=true;
}

void MainWindow::on_savReg_clicked()
{
    fileName = QFileDialog::getSaveFileName(this,
        tr("Сохранить в файл"), "/home/rail", tr("Текстовые документы (*)"));
    QFile file(fileName);
    if(file.open(QIODevice::WriteOnly))
       {

           QTextStream stream(&file);
           for(int i=0;i<L;i++)
               if(polynom.at(i)==false)
                    stream<<0;
               else
                   stream<<1;
           msgBox.setText("Данные успешно сохранены");
           msgBox.exec();
           file.close();
       }
}

void MainWindow::on_correlTest_clicked()
{
    Sx=0;
    Sy=0;
    xy=0;
    x2=0;
    y2=0;
    k=ui->correlComboBox->currentText().toInt();
    if(d2==1)
        buffer=keySeq;
    else
        buffer=textBitArray;
    for (Nk = 0; (Nk + k) < N; Nk++)
    {
        Sx = Sx + buffer.at(Nk);
        Sy = Sy + buffer.at(Nk + k);
        xy = xy + buffer.at(Nk) * buffer.at(Nk + k);
        x2 = x2 + buffer.at(Nk) * buffer.at(Nk);
        y2 = y2 + buffer.at(Nk+k) * buffer.at(Nk+k);
    }
    Rk = (double(Nk)*xy - Sx*Sy) / sqrt((double(Nk)*x2-Sx*Sx)*(double(Nk)*y2 - Sy*Sy));
    ui->correlLabel->setText( "Коэф. автокорреляции = "+ QString::number(Rk));
    if (Rk >= -confines &&
        Rk <= confines)
    {
        msgBox.setText("Тест пройден");
        msgBox.exec();
    }
    else
    {
        msgBox.setText("Тест провален");
        msgBox.exec();
    }
}

void MainWindow::on_serialTest_clicked()
{
    pyrsonKrit=0;
    k=ui->serialComboBox->currentText().toInt();
    K=new int[int(pow(k,2))];
    for (int i = 0; i < pow(2, k); i++)
        K[i] = 0;
    theoryFreq = double(N) /(k*pow(2, k));
    if(d1==1)
        buffer=keySeq;
    else
        buffer=textBitArray;
    if(k==2)
        for (int i = 2*L; i < N-1; i=i+2)
        {
            if (buffer.at(i) == false)
            {
                if (buffer.at(i+1) == false)
                    K[0]++;
                else
                    K[1]++;
            }
            if (buffer.at(i) == true)
            {
                if (buffer.at(i + 1) == false)
                    K[2]++;
                else
                    K[3]++;
            }
        }
    else if (k == 3)
        for (int i = 2*L; i < N - 2; i = i + 3)
        {
            if (buffer.at(i) == false)
            {
                if (buffer.at(i+1) == false)
                {
                    if (buffer.at(i+2) == false)
                        K[0]++;
                    else
                        K[1]++;
                }
                if (buffer.at(i + 1) == true)
                {
                    if (buffer.at(i + 2) == false)
                        K[2]++;
                    else
                        K[3]++;
                }
            }
        if (buffer.at(i) == true)
        {
            if (buffer.at(i+1) == false)
            {
                if (buffer.at(i+2) == false)
                    K[4]++;
                else
                    K[5]++;
            }
            if (buffer.at(i + 1) == true)
            {
                if (buffer.at(i + 2) == false)
                    K[6]++;
                else
                    K[7]++;
            }
        }
    }
    else
        for (int i = 2*L; i < N - 3; i = i + 4)
        {
            if (buffer.at(i) == false)
            {
                if (buffer.at(i+1) == false)
                {
                    if (buffer.at(i+2) == false)
                    {
                        if (buffer.at(i+3) == false)
                            K[0]++;
                        else
                            K[1]++;
                    }
                    if (buffer.at(i + 2) == true)
                    {
                        if (buffer.at(i + 3) == false)
                            K[2]++;
                        else
                            K[3]++;
                    }
                }
                if (buffer.at(i+1) == true)
                {
                    if (buffer.at(i+2) == false)
                    {
                        if (buffer.at(i+3) == false)
                            K[4]++;
                        else
                            K[5]++;
                    }
                    if (buffer.at(i + 2) == true)
                    {
                        if (buffer.at(i + 3) == false)
                            K[6]++;
                        else
                            K[7]++;
                    }
                }
            }
            if (buffer.at(i) == true)
            {
                if (buffer.at(i+1) == false)
                {
                    if (buffer.at(i+2) == false)
                        if (buffer.at(i+3) == false)
                            K[8]++;
                        else
                            K[9]++;
                    if (buffer.at(i + 2) == true)
                    {
                        if (buffer.at(i + 3) == false)
                            K[10]++;
                        else
                            K[11]++;
                    }
                }
                if (buffer.at(i+1) == true)
                {
                    if (buffer.at(i+2) == false)
                    {
                        if (buffer.at(i+3) == false)
                            K[12]++;
                        else
                            K[13]++;
                    }
                    if (buffer.at(i + 2) == true)
                    {
                        if (buffer.at(i + 3) == false)
                            K[14]++;
                        else
                            K[15]++;
                    }
                }
            }
        }
        for (int i = 0; i < pow(2, k); i++)
        {
            pyrsonKrit = pyrsonKrit + pow((K[i] - theoryFreq), 2) / theoryFreq;
        }
        ui->kritPyrson->setText("Критерий Пирсона = "+QString::number(pyrsonKrit));
        if((k==2 && pyrsonKrit>=c1 && pyrsonKrit<=c2)||(k==3 && pyrsonKrit>=c3 && pyrsonKrit<=c4)||
                (k==4 && pyrsonKrit>=c5 && pyrsonKrit<=c6))
        {
            msgBox.setText("Тест успешно пройден");
            msgBox.exec();
        }
        else
        {
            msgBox.setText("Тест провален");
            msgBox.exec();
        }
}

void MainWindow::on_serialComboBox_activated(const QString &arg1)
{
    if(arg1=="2")
        ui->pyrsonVal->setText("Допустимые значения: 0.352÷7.815");
    if(arg1=="3")
        ui->pyrsonVal->setText("Допустимые значения: 2.167÷14.067");
    if(arg1=="4")
        ui->pyrsonVal->setText("Допустимые значения: 7.261÷24.996");
}

void MainWindow::on_keyRadioButtonForSerial_clicked()
{
    d1=1;
}

void MainWindow::on_textRadioButtonForSerial_clicked()
{
    d1=2;
    textLength=textByteArray.count();
    textBitArray.resize(textLength);
    for(int i=0; i<textByteArray.count(); ++i)
    {
        if(textByteArray.at(i)=='1')
            textBitArray.setBit(i, true);
        else
            textBitArray.setBit(i, false);
    }
    N=textLength;
    L=0;
}

void MainWindow::on_correlComboBox_activated(const QString &arg1)
{
    if(Nk!=0)
    {

            confines=round(calcConfines(Nk-arg1.toInt())*10000)/10000;
            ui->correlVal->setText("Допустимые значения: "+
                               QString::number(-confines)+QString::number(confines));

    }
    else
        ui->correlVal->setText("Допустимые значения: 0");
}

void MainWindow::on_keyRadioButtonForCorrel_clicked()
{
    d2=1;
}

void MainWindow::on_textRadioButtonForCorrel_clicked()
{
    d2=2;
    textLength=textByteArray.count();
    textBitArray.resize(textLength);
    N=textLength;
    L=0;
    Nk=N-ui->correlComboBox->currentText().toInt();
    confines=round(calcConfines(Nk)*10000)/10000;
    ui->correlVal->setText("Допустимые значения: "+QString::number(-confines)
                           + "÷"+QString::number(confines));

    for(int i=0; i<textByteArray.count(); ++i)
    {
        if(textByteArray.at(i)=='1')
            textBitArray.setBit(i, true);
        else
            textBitArray.setBit(i, false);
    }
}

void MainWindow::on_loadText_clicked()
{
    fileName = QFileDialog::getOpenFileName(this,
        tr("Загрузить файл"), "/home/rail");
    fileText.setFileName(fileName);

    if(fileText.open(QIODevice::ReadOnly))
    {
        textByteArray.resize(fileText.size());
        textLength=fileText.size()*8;
        textBitArray.resize(textLength);
        codeBitArray.resize(textLength);
        textByteArray=fileText.readAll();
        ui->text->setPlainText(textByteArray);
        for(int i=0; i<textLength/8; ++i)
            for(int b=0; b<8; ++b)
                textBitArray.setBit(i*8+b, textByteArray.at(i)&(1<<b));
     }
    msgBox.setText("Файл для шифрования выбран");
    msgBox.exec();
}

void MainWindow::on_codeText_clicked()
{
    if(textByteArray.isEmpty())
        textByteArray=ui->text->document()->toPlainText().toUtf8();
    if ((textByteArray.isEmpty() && fileText.size()==0) || keySeq.isEmpty())
    {
        msgBox.setText("Недостаточно данных!");
        msgBox.exec();
    }

    for(int i=0; i<textLength; ++i)
        codeBitArray[i]=textBitArray[i]^keySeq[i];
    fileName = QFileDialog::getSaveFileName(this,
        tr("Сохранить шифротекст"), "/home/rail", tr("Текстовые документы (*)"));
    QFile file(fileName);
    if(file.open(QIODevice::WriteOnly))
       {
           QTextStream stream(&file);
           for(int i=0;i<textLength;i++)
           {
               if(codeBitArray.at(i)==true)
                    stream<<1;
               else
                    stream<<0;
           }
           msgBox.setText("Текст успешно зашифрован и сохранён в файл");
           msgBox.exec();
           file.close();
       }
}

void MainWindow::on_decodeText_clicked()
{
    if(textByteArray.isEmpty())
        textByteArray=ui->text->document()->toPlainText().toUtf8();
    if (textByteArray.isEmpty() || keySeq.isEmpty())
    {
        msgBox.setText("Недостаточно данных!");
        msgBox.exec();
    }
    textByteArray.resize(fileText.size());
    textLength=fileText.size();
    textBitArray.resize(textLength);
    textLength=textByteArray.count();
    textBitArray.resize(textLength);
    codeBitArray.resize(textLength);
    for(int i=0; i<textLength; ++i)
    {
        if(textByteArray.at(i)=='1')
            textBitArray.setBit(i, true);
        else
            textBitArray.setBit(i, false);
        codeBitArray[i]=textBitArray[i]^keySeq[i];
    }
    decodeByteArray.resize(textLength/8);
        decodeByteArray.fill(0);
    for(int b=0; b<textLength; ++b)
        decodeByteArray[b/8] = (decodeByteArray.at(b/8) | ((codeBitArray[b]?1:0)<<(b%8)));
    fileName = QFileDialog::getSaveFileName(this,
        tr("Сохранить шифротекст"), "/home/rail", tr("Текстовые документы (*)"));
    QFile file(fileName);
    if(file.open(QIODevice::WriteOnly))
       {
           QTextStream stream(&file);
           stream<<decodeByteArray;
           msgBox.setText("Текст успешно зашифрован и сохранён в файл");
           msgBox.exec();
           file.close();
       }
}
