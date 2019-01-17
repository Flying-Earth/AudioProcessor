#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    fileDialog(new QFileDialog(this))
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete fileDialog;
}

void MainWindow::on_btnPlay_clicked()
{
    if(ui->btnPlay->text()=="Play")
    {
        player.setMedia(NULL);
        QString tempPath = QCoreApplication::applicationDirPath();
        tempPath.append("/temp.wav");
        _af.save(tempPath.toStdString());
        player.setMedia(QUrl::fromLocalFile(tempPath));
        player.play();
        ui->btnPlay->setText("Pause");
    }
    else if(ui->btnPlay->text() == "Pause")
    {
        player.pause();
        ui->btnPlay->setText("Play");
    }
    else if(ui->btnPlay->text() == "Resume")
    {
        player.play();
        ui->btnPlay->setText("Pause");
    }
}

void MainWindow::on_btnStop_clicked()
{
    player.stop();
    ui->btnPlay->setText("Play");
}

void MainWindow::on_btnBrowse_clicked()
{
    if(fileDialog->exec())
    {
        QStringList files= fileDialog->selectedFiles();
        QDir dir = fileDialog->directory();
        QStringList filters;
        filters<<"*.wav"<<"*.mp3"<<"*.aif";

        fileList = dir.entryInfoList(filters);

        for(int i = 0;i < fileList.size();i++)
        {
            if(fileList.at(i).absoluteFilePath() == files[0])
            {
                currFileIdx = i;
                break;
            }
        }
        ui->lineEditFilePath->setText(fileList.at(currFileIdx).absoluteFilePath());
        ui->btnLoad->setEnabled(true);
    }
}

void MainWindow::on_btnLoad_clicked()
{
    QString log;
    QString tempPath = fileList.at(currFileIdx).absoluteFilePath();
    _af.load(tempPath.toStdString());
    log.append("|===============================|\n").append("load wave file: ")
            .append(tempPath).append("\n|===============================|\n")
            .append("---------------------------\n")
            .append("|Num Channels:").append(QString::number(_af.getNumChannels())).append("\n")
            .append("|Num Samples Per Channels:").append(QString::number(_af.getNumSamplesPerChannel())).append("\n")
            .append("|Sample Raet:").append(QString::number(_af.getSampleRate())).append("\n")
            .append("|Bit Depth:").append(QString::number(_af.getBitDepth())).append("\n")
            .append("|Length in Seconds:").append(QString::number(_af.getLengthInSeconds())).append("\n")
            .append("---------------------------\n");
    showLog(log);
    ui->labelFile->setText("WAV file loaded");
    ui->labelFile->setEnabled(true);
    ui->btnBrowse->setEnabled(true);
    ui->btnPlay->setEnabled(true);
    ui->btnSave->setEnabled(true);
    ui->btnStop->setEnabled(true);
    ui->labelFile->setEnabled(true);
    ui->labelJSON->setEnabled(true);
    ui->lineEditJSON->setEnabled(true);
    ui->menuBar->setEnabled(true);
    ui->textEdit->setEnabled(true);
}

void MainWindow::on_btnSave_clicked()
{
    if(fileDialog->exec())
    {
        QStringList files= fileDialog->selectedFiles();
        QDir dir = fileDialog->directory();
        QStringList filters;
        filters<<"*.wav"<<"*.mp3"<<"*.aif";

        fileList = dir.entryInfoList(filters);

        for(int i = 0;i < fileList.size();i++)
        {
            if(fileList.at(i).absoluteFilePath() == files[0])
            {
                currFileIdx = i;
                break;
            }
        }
        _af.save(fileList.at(currFileIdx).absoluteFilePath().toStdString());
        QString log;
        log.append("Save to ").append(fileList.at(currFileIdx).absoluteFilePath());
        showLog(log);
    }
}

void MainWindow::showLog(QString log)
{
    QDateTime currentDataTime = QDateTime::currentDateTime();
    QString currentData = currentDataTime.toString("yyyy-MM-dd hh:mm:ss ddd");
    QString info = ui->textEdit->toPlainText();
    info.append("[********").append(currentData).append("********]\n").append(log).append("\n");
    ui->textEdit->setText(info);
}

void MainWindow::on_actionreverse_triggered()
{
    showLog("Reverse");
    int numSamples = _af.getNumSamplesPerChannel();
    int numChannels = _af.getNumChannels();
    AudioFile<double> audio(_af);

    for(int j = 0; j < numChannels; j++)
    {
        for (int i = 0; i < numSamples; i++)
        {
            audio.samples[j][numSamples - i - 1] = _af.samples[j][i];
        }
    }

    for(int j = 0; j < numChannels; j++)
    {
        for (int i = 0; i < numSamples; i++)
        {
            _af.samples[j][i] = audio.samples[j][i];
        }
    }
}

void MainWindow::on_actionfaded_triggered()
{
    double end = _af.getLengthInSeconds();
    double start = end>2?end-2:end/2;

    QJsonValue jv = readParam("start");
    if(!jv.isUndefined() && jv.toDouble())
        start = jv.toDouble();
    jv = readParam("end");
    if(!jv.isUndefined() && jv.toDouble())
        end = jv.toDouble();

    int numChannels = _af.getNumChannels();
    int sampleRate = _af.getSampleRate();
    int len = (end - start)*sampleRate;

    if(end*sampleRate > _af.getNumSamplesPerChannel())
    {
        return;
    }


    for(int j = 0; j < numChannels; j++)
    {
        for (int i = start*sampleRate; i < end*sampleRate; i++)
        {
            _af.samples[j][i] *= (end*sampleRate - i)/len;
        }
    }

    QString log;
    log.append("Faded from ").append(QString::number(start)).append(" to ").append(QString::number(end)).append("\n");
    showLog(log);
}

void MainWindow::on_actionvolume_triggered()
{
    double valume = 0;
    QJsonValue jv = readParam("factor");
    if(!jv.isUndefined() && jv.toDouble())
        valume = jv.toDouble();

    double max = 0;
    double val = valume;

    if(valume == 1)
    {
        for(int j = 0; j < _af.getNumChannels(); j++)
        {
            for (int i = 0; i < _af.getNumSamplesPerChannel(); i++)
            {
                if(_af.samples[j][i] > max)
                    max = _af.samples[j][i];
            }
        }
        val = 1.0/max;
    }

    for(int j = 0; j < _af.getNumChannels(); j++)
    {
        for (int i = 0; i < _af.getNumSamplesPerChannel(); i++)
        {
            _af.samples[j][i] *= val;
        }
    }

    QString log;
    if(valume > 1) log.append("valume up: ");
    else if(valume < 1) log.append("valume down: ");
    else log.append("valume normalization: ");
    log.append(QString::number(valume)).append("\n");
    showLog(log);
}

void MainWindow::on_actionoriginal_triggered()
{
    QString tempPath = fileList.at(currFileIdx).absoluteFilePath();
    _af.load(tempPath.toStdString());
    QString log;
    log.append("original \n");
    showLog(log);
}

QJsonValue MainWindow::readParam(QString name)
{
    name = name.toLower();
    QString jsonString = ui->lineEditJSON->text();
    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonString.toUtf8());
    if( jsonDocument.isNull() ){
        qDebug()<< "===> please check the string "<< jsonString;
    }
    return jsonDocument.object().value(name);
}

void MainWindow::on_actionresampling_triggered()
{
    double resampling = 0;
    QJsonValue jv = readParam("factor");
    if(!jv.isUndefined() && jv.toDouble())
        resampling = jv.toDouble();

    AudioFile<double> audio;

    int numChannels = _af.getNumChannels();
    int numSamples = _af.getNumSamplesPerChannel();
    int newNumSamples = int(resampling*numSamples);

    audio.setNumSamplesPerChannel(newNumSamples);
    audio.setNumChannels(numChannels);

    for(int j = 0; j < numChannels; j++)
    {
        for (int i = 0; i < newNumSamples; i++)
        {
            if(i/resampling - int(i/resampling) == 0)
                audio.samples[j][i] = _af.samples[j][int(i/resampling)];
            else if(int(i/resampling) < numSamples - 1)
            {
                audio.samples[j][i] = _af.samples[j][int(i/resampling)]*(i/resampling - int(i/resampling))
                        + _af.samples[j][int(i/resampling) + 1]*(1 - i/resampling + int(i/resampling));
            }
        }
    }

    for(int i = 0;i <_af.getNumChannels();i++)
    {
        if(newNumSamples > numSamples)
            _af.samples[i].reserve(newNumSamples);
        _af.samples[i].resize(newNumSamples);
    }

    for(int j = 0; j < numChannels; j++)
    {
        for (int i = 0; i < newNumSamples; i++)
        {
            _af.samples[j][i] = audio.samples[j][i];
        }
    }

    QString log;
    log.append("resamping: ");
    log.append(QString::number(resampling)).append("\n");
    showLog(log);
}

void MainWindow::on_actionOLA_triggered()
{
    int winSZ = _af.getSampleRate()/10;
    int anaStep = winSZ/2;
    double factor = 0;
    int numChannels = _af.getNumChannels();
    int numSamples = _af.getNumSamplesPerChannel();

    QJsonValue jv = readParam("factor");
    if(!jv.isUndefined() && jv.toDouble())
        factor = jv.toDouble();

    int synStep = ceil(factor*anaStep);
    int N = ceil((numSamples - winSZ)/anaStep + 1);
    int newNumSamples = ceil((N-1)*synStep+winSZ);

    std::vector<double> hanning;
    hanning.reserve(winSZ);
    hanning.resize(winSZ);
    for(int i = 0;i < winSZ;i++)
        hanning[i] = 0.5 - 0.5*cos(2*M_PI*i/N);

    AudioFile<double> audio;

    audio.setNumSamplesPerChannel(newNumSamples);
    audio.setNumChannels(_af.getNumChannels());

    std::vector<std::vector<double> > ana;
    ana.reserve(N);
    ana.resize(N);

    std::vector<std::vector<double> > syn;
    syn.reserve(N);
    syn.resize(N);

    double temp = 0;

    for(int k=0; k<numChannels; k++)
    {
        for(int i=0;i < N;i++)
        {
            for(int j=0;j<winSZ;j++)
            {
                temp = 0;
                if(i*anaStep+j <= numSamples)
                    temp = hanning[j]*_af.samples[k][i*anaStep+j];
                ana[i].push_back(temp);
            }
        }

        for(int i=0;i<N;i++)
        {
            for(int j=0;j<winSZ;j++)
            {
                if(i*synStep+j <= audio.getNumSamplesPerChannel())
                    audio.samples[k][i*synStep+j] += ana[i][j];
            }
        }

    }

    for(int i = 0;i <_af.getNumChannels();i++)
    {
        _af.samples[i].reserve(newNumSamples);
        _af.samples[i].resize(newNumSamples);
    }

    for(int j = 0; j < _af.getNumChannels(); j++)
    {
        for (int i = 0; i < _af.getNumSamplesPerChannel(); i++)
        {
            _af.samples[j][i] = audio.samples[j][i];
        }
    }

    QString log;
    log.append("OLA rate: ");
    log.append(QString::number(factor)).append("\n");
    showLog(log);
}

void MainWindow::on_actionDTW_triggered()
{
    QString log;
    log.append("new wave file: ");

    AudioFile<double> audio;
    QString tempPath;

    if(fileDialog->exec())
    {
        QStringList files= fileDialog->selectedFiles();
        tempPath = files[0];
        audio.load(tempPath.toStdString());
    } else return;

    double step = _af.getSampleRate()/100;
    double rate = _af.getSampleRate()/1000;
    int w = _af.getNumSamplesPerChannel()/rate;
    int h = audio.getNumSamplesPerChannel()/rate;

    int n = (w-h)*rate/step + 1;

    std::vector<double> T;
    T.reserve(n);
    T.resize(n);

    std::vector<std::vector<double> > D;
    D.reserve(h);
    D.resize(h);

    for(int i=0; i<h; i++)
    {
        D[i].reserve(h);
        D[i].resize(h);
    }

    double minus = 0;

    for(int k=0; k<n; k++)
    {
        for(int i=0; i<h; i++)
        {
            for(int j=0; j<h; j++)
            {
                minus = _af.samples[0][i*rate+k*step] - audio.samples[0][j*rate];
                if(minus<0)
                    D[i][j] = sqrt(-minus);
                else
                    D[i][j] = sqrt(minus);
            }
        }

        T[k] = getDTW(D);
    }

    int start = 0;
    double value = T[0];

    for(int i=0; i<n; i++)
    {
        if(value > T[i])
        {
            value = T[i];
            start = i;
        }
    }

    double startTime = start*step/audio.getSampleRate();
    double endTime = startTime+h*rate/audio.getSampleRate();

    log.append(tempPath).append("\n");
    log.append("匹配起始位置为: ").append(QString::number(startTime, 8, 2)).append("s\n");
    log.append("匹配终止位置为: ").append(QString::number(endTime, 8, 2)).append("s\n");
    log.append("DTW匹配相似度: ").append(QString::number(1/(1+sqrt(value/n))*100, 8, 2)).append("%\n");
    showLog(log);
}

double MainWindow::getDTW(std::vector<std::vector<double> > D)
{
    int w = D.size();
    int h = D[0].size();

    std::vector<std::vector<double> > R;
    R.reserve(w);
    R.resize(w);

    for(int i=0; i<w; i++)
    {
        R[i].reserve(h);
        R[i].resize(h);
    }

    double min = 0;

    for(int i=0; i<w; i++)
    {
        for(int j=0; j<=h; j++)
        {
            if(i==0&&j==0)
            {
                R[i][j] = D[i][j];
            }
            else if(i==0)
            {
                R[i][j] = R[i][j-1] + D[i][j];
            }
            else if(j==0)
            {
                R[i][j] = R[i-1][j] + D[i][j];
            }
            else if(i>0&&j>0)
            {
                min = getMin(R[i][j-1], R[i-1][j-1], R[i-1][j]);
                R[i][j] = D[i][j] + min;
            }
        }
    }

    return R[w-1][h-1];
}

double MainWindow::getMin(double a, double b, double c)
{
    double min = a;
    if(a>b) min = b;
    if(c<min) min =c;
    return min;
}
