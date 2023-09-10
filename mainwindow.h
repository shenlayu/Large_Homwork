#pragma once
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void ImportFile();
    void AVG();
    void VAR();
    void MakeHistogram();
    void MakeScatter();
    void MakeCovariance();
    void MakeCorrelation();
    void MakePCA();
    void MakeKMeans();
    void AddBar1();
    void AddBar2();
    void UseBar1();
    void UseBar2();

private:
    Ui::MainWindow *ui;
    std :: vector<float> CheckDiscrete(int column, int BeginRow, int EndRow);
    QVector<int> GetNumbers(const QStringList& Requests, const QVector<int>& Default);
    int usage;

};
#endif // MAINWINDOW_H
