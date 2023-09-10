#include "columnbar.h"
#include "ui_columnbar.h"
#include <QRegularExpression>
#include <QMessageBox>
#include <QDebug>

ColumnBar::ColumnBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ColumnBar), Saved(false), BeginRow(-1), EndRow(-1)
{
    ui->setupUi(this);
}

ColumnBar::~ColumnBar()
{
    delete ui;
}

void ColumnBar :: AddBar(QList<int> Numbers)
{
    Saved = false;
    SaveBar();
    if(!Saved)
    {
        return;
    }
    for(auto number : Numbers)
    {
        Columns.push_back(number);
    }
    std::sort(Columns.begin(),Columns.end());
    auto it= std::unique(Columns.begin(),Columns.end());
    Columns.erase(it,Columns.end());

    QString str;
    QVector<int> VectorNumbers;
    for(auto number : Columns)
    {
        str += QString :: number(number);
        str += ",";
    }
    str.chop(1);
    ui -> BarEdit -> setText(str);
}

void ColumnBar :: SaveBar()
{
    QRegularExpression* cheaker = new QRegularExpression("^([1-9][0-9]*(,[1-9][0-9]*)*)?$");

    QString text = ui -> BarEdit -> toPlainText();
    if(!cheaker -> match(text).hasMatch())
    {
        QMessageBox :: critical(this, "错误", "输入不合法,请检查");
        return;
    }
    Columns.clear();
    int size = text.size();
    QString TmpStr;
    for(int i = 0; i < size; i ++)
    {
        if(text[i] != ',')
        {
            TmpStr.append(text[i]);
        }
        else
        {
            Columns.push_back(TmpStr.toInt());
            TmpStr.clear();
        }
    }
    if(TmpStr != NULL)
    {
        Columns.push_back(TmpStr.toInt());
    }

    std::sort(Columns.begin(),Columns.end());
    auto it= std::unique(Columns.begin(),Columns.end());
    Columns.erase(it,Columns.end());

    QString str;
    QVector<int> VectorNumbers;
    for(auto number : Columns)
    {
        str += QString :: number(number);
        str += ",";
    }
    str.chop(1);
    ui -> BarEdit -> setText(str);

    if(ui -> BarBeginRowEdit -> text() != NULL)
    {
        QString text = ui -> BarBeginRowEdit -> text();
        QString CheakerStr = "^[1-9][0-9]*$";
        QRegularExpression cheaker(CheakerStr);
        if(!cheaker.match(text).hasMatch())
        {
            QMessageBox :: critical(this, "错误", "起始行输入不合法，请检查");
            return;
        }
        else
        {
            BeginRow = text.toInt();
        }
    }
    else
    {
        BeginRow = -1;
    }
    if(ui -> BarEndRowEdit -> text() != NULL)
    {
        QString text = ui -> BarEndRowEdit -> text();
        QString CheakerStr = "^[1-9][0-9]*$";
        QRegularExpression cheaker(CheakerStr);
        if(!cheaker.match(text).hasMatch())
        {
            QMessageBox :: critical(this, "错误", "截止行输入不合法，请检查");
            return;
        }
        else
        {
            EndRow = text.toInt();
        }
    }
    else
    {
        EndRow = -1;
    }
    Saved = true;
}
