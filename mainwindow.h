#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <math.h>
#include <AudioFile.h>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QFileDialog>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QtCore/qmath.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QFileDialog *fileDialog;
    QFileInfoList fileList;
    QMediaPlayer player;
    AudioFile<double> _af;

    int currFileIdx = 0;

    void showLog(QString log);
    QJsonValue readParam(QString name);
    double getMin(double a, double b, double c);
    double getDTW(std::vector<std::vector<double> > D);

public slots:
    void on_btnPlay_clicked();
    void on_btnStop_clicked();
    void on_btnBrowse_clicked();
    void on_btnLoad_clicked();
    void on_btnSave_clicked();
    void on_actionreverse_triggered();
    void on_actionfaded_triggered();
    void on_actionvolume_triggered();
    void on_actionoriginal_triggered();
    void on_actionresampling_triggered();
    void on_actionOLA_triggered();
    void on_actionDTW_triggered();
};

#endif // MAINWINDOW_H
