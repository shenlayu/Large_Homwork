// MainWindow层
// 分4层：MainWindow层，包装层，可视化层，算法层
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "visualization.h"
#include "src/rowfeatureAlg.h"
#include "src/kmeansAlg.h"
#include "histogram.h"
#include "scatter.h"
#include "covariance.h"
#include "correlation.h"
#include "pca.h"
#include "kmeans.h"
#include "columnbar.h"
#include <QDebug>
#include <QFileDialog>
#include <QRegularExpression>
#define Table ui->DataTable
#define Bar1 ui -> ColumnBar1
#define Bar2 ui -> ColumnBar2

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), usage(0)
{
    ui->setupUi(this);
    connect(ui -> actionInput, &QAction :: triggered, this, &MainWindow :: ImportFile);
    connect(ui -> actionHistogram, &QAction :: triggered, this, &MainWindow :: MakeHistogram);
    connect(ui -> CalculateAVG, &QPushButton :: clicked, this, &MainWindow :: AVG);
    connect(ui -> CalculateVAR, &QPushButton :: clicked, this, &MainWindow :: VAR);
    connect(ui -> actionScatterPlot, &QAction :: triggered, this, &MainWindow :: MakeScatter);
    connect(ui -> actionCovariance, &QAction :: triggered, this, &MainWindow :: MakeCovariance);
    connect(ui -> actionCorrelation, &QAction :: triggered, this, &MainWindow :: MakeCorrelation);
    connect(ui -> actionPCA, &QAction :: triggered, this, &MainWindow :: MakePCA);
    connect(ui -> actionKmeans, &QAction :: triggered, this, &MainWindow :: MakeKMeans);
    connect(ui -> SaveBar1, &QPushButton :: clicked, this, [this](){
         Bar1 -> SaveBar();
    });
    connect(ui -> SaveBar2, &QPushButton :: clicked, this, [this](){
         Bar2 -> SaveBar();
    });
    connect(ui -> AddBar1, &QPushButton :: clicked, this, &MainWindow :: AddBar1);
    connect(ui -> AddBar2, &QPushButton :: clicked, this, &MainWindow :: AddBar2);
    connect(ui -> UseBar1, &QPushButton :: clicked, this, &MainWindow :: UseBar1);
    connect(ui -> UseBar2, &QPushButton :: clicked, this, &MainWindow :: UseBar2);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow :: ImportFile()
{
    // TODO 未导入数据前进行提示
    QList<QStringList> list;
    QString filePath = ":/1.csv"; //QFileDialog::getOpenFileName(this, "选择文件", QDir::homePath(), "所有文件 (*.*)");

    QFile InputFile(filePath);

    if(!InputFile.exists())
    {
        //TODO 弹出报错窗口
        qDebug() << "File doesn't exist";
        return;
    }
    if(!InputFile.open(QIODevice :: ReadOnly))
    {
        //TODO 弹出报错窗口
        qDebug() << "File can't be opened";
        return;
    }

    QTextStream stream(&InputFile);
    while(!stream.atEnd())
    {
        QString LineText = stream.readLine();
        QStringList TextList = LineText.split(',', Qt :: SkipEmptyParts);
        list.append(TextList);
    }

    Table -> setVisible(false);
    ui -> horizontalLayout_6 -> removeWidget(Table);
    Table = TableVis(list);
    Table -> setParent(this);
    ui -> horizontalLayout_6 -> addWidget(Table, 0, 0);
    ui -> horizontalLayout_6 -> removeItem(ui -> verticalLayout_5);
    ui -> horizontalLayout_6 -> addItem(ui -> verticalLayout_5);
    Table -> setMinimumSize(600, 600);
    Table -> show();
}

std :: vector<float> MainWindow :: CheckDiscrete(int column, int BeginRow, int EndRow)
{
    QRegularExpression* checker = new QRegularExpression("^([0-9]+(.[0-9]*)?|[0-9]*)$");

    bool Discrete;
    QMap<QString, int> map;
    std :: vector<float> array;
    if(checker -> match(Table -> item(BeginRow, column) -> text()).hasMatch())
    {
        Discrete = false;
    }
    else
    {
        Discrete = true;
    }

    if(!Discrete)
    {
        for(int row = BeginRow; row <= EndRow; row ++)
        {
            array.push_back(Table -> item(row, column) -> text().toFloat());
        }
    }
    else
    {
        QStringList Occupied;
        int index = 0;
        for(int row = BeginRow; row <= EndRow; row ++)
        {
            bool flag = false;
            for(QString str: Occupied)
            {
                if(str == Table -> item(row, column) -> text())
                {
                    flag = true;
                    break;
                }
            }
            if(flag)
            {
                array.push_back(map[Table -> item(row, column) -> text()]);
            }
            else
            {
                array.push_back(index);
                map.insert(Table -> item(row, column) -> text(), index ++);
                Occupied.push_back(Table -> item(row, column) -> text());
            }
        }
    }
    return array;
}

QVector<int> MainWindow :: GetNumbers(const QStringList& Requests, const QVector<int>& Default)
{
    QDialog* window = new QDialog(this);
    int number = Requests.size();
    window -> resize(300, 27 + 50 * number);

    QVector<QLineEdit*> getter;
    for(int i = 0; i < number; i ++)
    {
        QLabel* tip = new QLabel(window);
        tip -> setText(Requests[i]);
        tip -> setGeometry(0, 50 * i, 200, 50);
        QLineEdit* edit = new QLineEdit(window);
        edit -> setGeometry(200, 50 * i, 100, 50);
        getter.push_back(edit);
    }
    QPushButton* OK = new QPushButton("确定", window);
    OK -> setGeometry(250, 50 * number, 50, 25);

    QVector<int> array;
    connect(OK, &QPushButton :: clicked, [=, &array](){
        for(int i = 0; i < number; i ++)
        {
            // 检查输入合法性
            if(getter[i] -> text() != NULL)
            {
                QString CheakerStr = "^[1-9][0-9]*$";
                QRegularExpression cheaker(CheakerStr);
                if(!cheaker.match(getter[i] -> text()).hasMatch())
                {
                    QMessageBox :: critical(window, "错误", "第" + QString :: number(i + 1) + "行输入不合法，请检查");
                    array.clear();
                    return;
                }
                array.push_back(getter[i] -> text().toFloat());
            }
            // 默认值
            else if(Default[i] != -1)
            {
                array.push_back(Default[i]);
            }
            else
            {
                // 弹出报错
                QMessageBox :: critical(window, "错误", "第" + QString :: number(i + 1) + "行无默认值且未输入，请检查");
                array.clear();
                return;
            }
        }
        window -> close();
    });
    window -> exec();

    return array;
}

void MainWindow :: AVG()
{
    QString CheakerStr = "^[1-9][0-9]*$";
    QRegularExpression cheaker(CheakerStr);
    if(!(cheaker.match(ui -> AVGEdit1 -> text()).hasMatch() &&
         cheaker.match(ui -> AVGEdit2 -> text()).hasMatch() &&
         cheaker.match(ui -> AVGEdit3 -> text()).hasMatch()))
    {
        QMessageBox :: critical(this, "错误", "输入不合法，请检查");
        return;
    }

    int column = ui -> AVGEdit1 -> text().toInt() - 1;
    int BeginRow = ui -> AVGEdit2 -> text().toInt() - 1;
    int EndRow = ui -> AVGEdit3 -> text().toInt() - 1;

    if(column > Table -> columnCount() || BeginRow > EndRow || BeginRow <= 0 || EndRow >= Table -> rowCount())
    {
        QMessageBox :: critical(this, "错误", "行列输入不在合法范围");
        return;
    }
    auto data = CheckDiscrete(column, BeginRow, EndRow);
    auto result = std :: get<0>(getAvgVar(data));
    ui -> AVGLabel5 -> setText(QString :: number(result));
}

void MainWindow :: VAR()
{
    QString CheakerStr = "^[1-9][0-9]*$";
    QRegularExpression cheaker(CheakerStr);
    if(!(cheaker.match(ui -> AVGEdit1 -> text()).hasMatch() &&
         cheaker.match(ui -> AVGEdit2 -> text()).hasMatch() &&
         cheaker.match(ui -> AVGEdit3 -> text()).hasMatch()))
    {
        QMessageBox :: critical(this, "错误", "输入不合法，请检查");
        return;
    }

    int column = ui -> VAREdit1 -> text().toInt() - 1;
    int BeginRow = ui -> VAREdit2 -> text().toInt() - 1;
    int EndRow = ui -> VAREdit3 -> text().toInt() - 1;

    if(column > Table -> columnCount() || BeginRow > EndRow || BeginRow <= 0 || EndRow >= Table -> rowCount())
    {
        QMessageBox :: critical(this, "错误", "行列输入不在合法范围");
        return;
    }
    auto data = CheckDiscrete(column, BeginRow, EndRow);
    auto result = std :: get<1>(getAvgVar(data));
    ui -> VARLabel5 -> setText(QString :: number(result));
}

void MainWindow :: MakeHistogram()
{
    auto PlaceData = GetNumbers({"输入列号:", "输入起始行号(默认为2):", "输入截止行号(默认为最后一列):"}, {-1, 2, Table -> rowCount()});
    if(PlaceData.size() == 0) // 用户直接关闭了窗口
    {
        return;
    }
    int column = PlaceData[0] - 1, BeginRow = PlaceData[1] - 1, EndRow = PlaceData[2] - 1;
    if(column > Table -> columnCount() || BeginRow > EndRow || BeginRow <= 0 || EndRow >= Table -> rowCount())
    {
        QMessageBox :: critical(this, "错误", "行列输入不在合法范围");
        return;
    }

    auto data = CheckDiscrete(column, BeginRow, EndRow);

    QDialog* HistogramShower = new QDialog(this);
    Histogram* HistogramObject = new Histogram(data, HistogramShower);
    HistogramShower -> show();
}

void MainWindow :: MakeScatter()
{
    auto PlaceData = GetNumbers({"输入横轴列号:", "输入纵轴列号:",
                                 "输入起始行号(默认为2):", "输入截止行号(默认为最后一列):"}, {-1, -1, 2, Table -> rowCount()});
    if(PlaceData.size() == 0)
    {
        return;
    }
    int ColumnX = PlaceData[0] - 1, ColumnY = PlaceData[1] - 1, BeginRow = PlaceData[2] - 1, EndRow = PlaceData[3] - 1;

    std :: vector<float> numbersX = CheckDiscrete(ColumnX, BeginRow, EndRow);
    std :: vector<float> numbersY = CheckDiscrete(ColumnY, BeginRow, EndRow);

    auto NameX = ui -> DataTable -> item(0, ColumnX) -> text();
    auto NameY = ui -> DataTable -> item(0, ColumnY) -> text();
    QDialog* ScatterShower = new QDialog(this);
    Scatter* ScatterObject = new Scatter(numbersX, numbersY, NameX, NameY, ScatterShower);
    ScatterShower -> show();
}

void  MainWindow :: MakeCovariance()
{
    QStringList* Heads = new QStringList;
    QVector<int> Columns;
    int ColumnNum = 0, BeginRow = 0, EndRow = 0;

    if(usage == 0)
    {
        auto PlaceData = GetNumbers({"输入列数:", "输入起始行号(默认为2):", "输入截止行号(默认为最后一列):"}, {-1, 2, Table -> rowCount()});
        if(PlaceData.size() == 0)
        {
            return;
        }
        ColumnNum = PlaceData[0];
        BeginRow = PlaceData[1] - 1;
        EndRow = PlaceData[2] - 1;

        for(int i = 0; i < ColumnNum; i ++)
        {
            auto Column = GetNumbers({"输入第" + QString :: number(i + 1) + "个列号:"}, {-1});
            if(Column.size() == 0)
            {
                return;
            }
            Columns.push_back(Column[0] - 1);
            Heads -> push_back(Table -> item(0, Columns[i]) -> text());
        }
    }
    else if(usage == 1)
    {
        Columns = Bar1 -> Columns;
        ColumnNum = Columns.size();
        if(ColumnNum == 0)
        {
            QMessageBox :: warning(this, "警告", "当前存储框中无元素");
            return;
        }
        for(int i = 0; i < ColumnNum; i ++)
        {
            Columns[i] --;
        }
        for(int i = 0; i < ColumnNum; i ++)
        {
            Heads -> push_back(Table -> item(0, Columns[i]) -> text());
        }
        BeginRow = Bar1 -> BeginRow == -1 ? 1 : Bar1 -> BeginRow;
        EndRow = Bar1 -> EndRow == -1 ? Table -> rowCount() - 1 : Bar1 -> EndRow;
    }
    else if(usage == 2)
    {
        Columns = Bar2 -> Columns;
        ColumnNum = Columns.size();
        if(ColumnNum == 0)
        {
            QMessageBox :: warning(this, "警告", "当前存储框中无元素");
            return;
        }
        for(int i = 0; i < ColumnNum; i ++)
        {
            Columns[i] --;
        }
        for(int i = 0; i < ColumnNum; i ++)
        {
            Heads -> push_back(Table -> item(0, Columns[i]) -> text());
        }
        BeginRow = Bar2 -> BeginRow == -1 ? 1 : Bar2 -> BeginRow;
        EndRow = Bar2 -> EndRow == -1 ? Table -> rowCount() - 1 : Bar2 -> EndRow;
    }

    std :: vector<std :: vector<float>> matrix;
    for(int i = 0; i < ColumnNum; i ++)
    {
        std :: vector<float> ColumnData;
        for(int row = BeginRow; row <= EndRow; row ++)
        {
            ColumnData.push_back(ui -> DataTable -> item(row, Columns[i]) -> text().toFloat());
        }
        matrix.push_back(ColumnData);
    }

    QDialog* CovarianceShower = new QDialog(this);
    Covariance* covarianceObject = new Covariance(matrix, Heads, CovarianceShower);
    CovarianceShower -> show();
}

void  MainWindow :: MakeCorrelation()
{
    QStringList* Heads = new QStringList;
    QVector<int> Columns;
    int ColumnNum = 0, BeginRow = 0, EndRow = 0;

    if(usage == 0)
    {
        auto PlaceData = GetNumbers({"输入列数:", "输入起始行号(默认为2):", "输入截止行号(默认为最后一列):"}, {-1, 2, Table -> rowCount()});
        if(PlaceData.size() == 0)
        {
            return;
        }
        ColumnNum = PlaceData[0];
        BeginRow = PlaceData[1] - 1;
        EndRow = PlaceData[2] - 1;

        for(int i = 0; i < ColumnNum; i ++)
        {
            auto Column = GetNumbers({"输入第" + QString :: number(i + 1) + "个列号:"}, {-1});
            if(Column.size() == 0)
            {
                return;
            }
            Columns.push_back(Column[0] - 1);
            Heads -> push_back(Table -> item(0, Columns[i]) -> text());
        }
    }
    else if(usage == 1)
    {
        Columns = Bar1 -> Columns;
        ColumnNum = Columns.size();
        if(ColumnNum == 0)
        {
            QMessageBox :: warning(this, "警告", "当前存储框中无元素");
            return;
        }
        for(int i = 0; i < ColumnNum; i ++)
        {
            Columns[i] --;
        }
        for(int i = 0; i < ColumnNum; i ++)
        {
            Heads -> push_back(Table -> item(0, Columns[i]) -> text());
        }
        BeginRow = Bar1 -> BeginRow == -1 ? 1 : Bar1 -> BeginRow;
        EndRow = Bar1 -> EndRow == -1 ? Table -> rowCount() - 1 : Bar1 -> EndRow;
    }
    else if(usage == 2)
    {
        Columns = Bar2 -> Columns;
        ColumnNum = Columns.size();
        if(ColumnNum == 0)
        {
            QMessageBox :: warning(this, "警告", "当前存储框中无元素");
            return;
        }
        for(int i = 0; i < ColumnNum; i ++)
        {
            Columns[i] --;
        }
        for(int i = 0; i < ColumnNum; i ++)
        {
            Heads -> push_back(Table -> item(0, Columns[i]) -> text());
        }
        BeginRow = Bar2 -> BeginRow == -1 ? 1 : Bar2 -> BeginRow;
        EndRow = Bar2 -> EndRow == -1 ? Table -> rowCount() - 1 : Bar2 -> EndRow;
    }

    std :: vector<std :: vector<float>> matrix;
    for(int i = 0; i < ColumnNum; i ++)
    {
        std :: vector<float> ColumnData;
        for(int row = BeginRow; row <= EndRow; row ++)
        {
            ColumnData.push_back(ui -> DataTable -> item(row, Columns[i]) -> text().toFloat());
        }
        matrix.push_back(ColumnData);
    }

    QDialog* CorrelationShower = new QDialog(this);
    Correlation* correlationObjext = new Correlation(matrix, Heads, CorrelationShower);
    CorrelationShower -> show();
}

void MainWindow :: MakePCA()
{
    int ColumnNum = 0, BeginRow = 0, EndRow = 0, dimension = 0;
    QVector<int> Columns;

    if(usage == 0)
    {
        auto PlaceData = GetNumbers({"输入列数:",
                                     "输入起始行号(默认为2):", "输入截止行号(默认为最后一列):", "输入目标维数:"}, {-1, 2, Table -> rowCount(), -1});
        if(PlaceData.size() == 0)
        {
            return;
        }
        ColumnNum = PlaceData[0], BeginRow = PlaceData[1] - 1, EndRow = PlaceData[2] - 1, dimension = PlaceData[3];

        for(int i = 0; i < ColumnNum; i ++)
        {
            auto Column = GetNumbers({"输入第" + QString :: number(i + 1) + "个列号："}, {-1});
            if(Column.size() == 0)
            {
                return;
            }
            Columns.push_back(Column[0] - 1);
        }
    }
    else if(usage == 1)
    {
        Columns = Bar1 -> Columns;
        ColumnNum = Columns.size();
        if(ColumnNum == 0)
        {
            QMessageBox :: warning(this, "警告", "当前存储框中无元素");
            return;
        }
        for(int i = 0; i < ColumnNum; i ++)
        {
            Columns[i] --;
        }
        auto PlaceData = GetNumbers({"输入目标维数:"}, {-1});
        dimension = PlaceData[0];
        BeginRow = Bar1 -> BeginRow == -1 ? 1 : Bar1 -> BeginRow;
        EndRow = Bar1 -> EndRow == -1 ? Table -> rowCount() - 1 : Bar1 -> EndRow;
    }
    else if(usage == 2)
    {
        Columns = Bar2 -> Columns;
        ColumnNum = Columns.size();
        if(ColumnNum == 0)
        {
            QMessageBox :: warning(this, "警告", "当前存储框中无元素");
            return;
        }
        for(int i = 0; i < ColumnNum; i ++)
        {
            Columns[i] --;
        }
        auto PlaceData = GetNumbers({"输入目标维数:"}, {-1});
        dimension = PlaceData[0];
        BeginRow = Bar2 -> BeginRow == -1 ? 1 : Bar2 -> BeginRow;
        EndRow = Bar2 -> EndRow == -1 ? Table -> rowCount() - 1 : Bar2 -> EndRow;
    }
    if(dimension >= 4)
    {
        QMessageBox :: critical(this, "错误", "无法生成" + QString :: number(dimension) + "维图像");
        return;
    }

    if(ColumnNum < dimension)
    {
        QMessageBox :: critical(this, "错误", "维度不得低于列数");
        return;
    }
    std :: vector<std :: vector<float>> matrix;
    std :: vector<float>* group = new std :: vector<float>;
    *group = CheckDiscrete(1, BeginRow, EndRow);
    for(int row = BeginRow; row <= EndRow; row ++)
    {
        std :: vector<float> ColumnData;
        for(int i = 0; i < ColumnNum; i ++)
        {
            ColumnData.push_back(ui -> DataTable -> item(row, Columns[i]) -> text().toFloat());
        }
        matrix.push_back(ColumnData);
    }
    QDialog* PCAShower = new QDialog(this);
    PCA* PCAObject = new PCA(matrix, dimension, PCAShower, group);
    PCAShower -> show();
}

void MainWindow :: MakeKMeans()
{
    int ColumnNum = 0, BeginRow = 0, EndRow = 0, k = 0;
    QVector<int> Columns;

    if(usage == 0)
    {
        auto PlaceData = GetNumbers({"输入列数:",
                                     "输入起始行号(默认为2):", "输入截止行号(默认为最后一列):", "输入聚合中心个数:"}, {-1, 2, Table -> rowCount(), -1});
        if(PlaceData.size() == 0)
        {
            return;
        }
        ColumnNum = PlaceData[0], BeginRow = PlaceData[1] - 1, EndRow = PlaceData[2] - 1, k = PlaceData[3];

        for(int i = 0; i < ColumnNum; i ++)
        {
            auto Column = GetNumbers({"输入第" + QString :: number(i + 1) + "个列号："}, {-1});
            if(Column.size() == 0)
            {
                return;
            }
            Columns.push_back(Column[0] - 1);
        }
    }
    else if(usage == 1)
    {
        Columns = Bar1 -> Columns;
        ColumnNum = Columns.size();
        if(ColumnNum == 0)
        {
            QMessageBox :: warning(this, "警告", "当前存储框中无元素");
            return;
        }
        for(int i = 0; i < ColumnNum; i ++)
        {
            Columns[i] --;
        }
        auto PlaceData = GetNumbers({"输入聚合中心个数:"}, {-1});
        k = PlaceData[0];
        BeginRow = Bar1 -> BeginRow == -1 ? 1 : Bar1 -> BeginRow;
        EndRow = Bar1 -> EndRow == -1 ? Table -> rowCount() - 1 : Bar1 -> EndRow;
    }
    else if(usage == 2)
    {
        Columns = Bar2 -> Columns;
        ColumnNum = Columns.size();
        if(ColumnNum == 0)
        {
            QMessageBox :: warning(this, "警告", "当前存储框中无元素");
            return;
        }
        for(int i = 0; i < ColumnNum; i ++)
        {
            Columns[i] --;
        }
        auto PlaceData = GetNumbers({"输入聚合中心个数:"}, {-1});
        k = PlaceData[0];
        BeginRow = Bar2 -> BeginRow == -1 ? 1 : Bar2 -> BeginRow;
        EndRow = Bar2 -> EndRow == -1 ? Table -> rowCount() - 1 : Bar2 -> EndRow;
    }

    if(ColumnNum < 2)
    {
        QMessageBox :: critical(this, "错误", "列数不得小于2");
        return;
    }
    if(k > 10)
    {
        QMessageBox :: warning(this, "警告", "不支持大于10个聚合中心");
        return;
    }
    std :: vector<std :: vector<float>> matrix;
    std :: vector<float>* group = new std :: vector<float>;
    *group = CheckDiscrete(1, BeginRow, EndRow);
    for(int row = BeginRow; row <= EndRow; row ++)
    {
        std :: vector<float> ColumnData;
        for(int i = 0; i < ColumnNum; i ++)
        {
            ColumnData.push_back(Table -> item(row, Columns[i]) -> text().toFloat());
        }
        matrix.push_back(ColumnData);
    }

    std :: vector<float>* VectorKMeans = new std :: vector<float>;
    auto VectorInt = std :: get<1>(clusterKMeans(matrix, k, 30));
    for(float number : VectorInt)
    {
        VectorKMeans -> push_back(number);
    }
    Table -> setColumnCount(Table -> columnCount() + 1);
    for(int column = 0; ;column ++)
    {
        if(Table -> item(0, column) == nullptr)
        {
            Table -> setItem(0, column, new QTableWidgetItem("KMeans"));
            for(int row = 1; row <= EndRow; row ++)
            {
                Table -> setItem(row, column, new QTableWidgetItem(QString :: number(VectorInt[row])));
            }
            break;
        }
    }
    QDialog* KMeansShower = new QDialog(this);
    KMeans* KMeansObjex = new KMeans(matrix, k, KMeansShower, VectorKMeans, group);
    KMeansShower -> show();
}

void MainWindow :: AddBar1()
{
    usage = 1;
    QList<QTableWidgetItem*> ItemList = Table -> selectedItems();
    if(ItemList.empty()){
        QMessageBox::warning(this,"警告","未选择列");
        return;
    }
    QList<int> NumberList;

    for(auto Item : ItemList)
    {
        NumberList.append(Item -> column() + 1);
    }
    ui -> ColumnBar1 -> AddBar(NumberList);
}
void MainWindow :: AddBar2()
{
    usage = 2;
    QList<QTableWidgetItem*> ItemList = Table -> selectedItems();
    if(ItemList.empty()){
        QMessageBox::warning(this,"警告","未选择列");
        return;
    }
    QList<int> NumberList;
    for(auto Item : ItemList)
    {
        NumberList.append(Item -> column() + 1);
    }
    ui -> ColumnBar2 -> AddBar(NumberList);
}

void MainWindow :: UseBar1()
{
    if(ui -> UseBar1 -> text() == "保存并设为使用")
    {
        Bar1 -> SaveBar();
        usage = 1;
        ui -> UseBar1 -> setText("取消使用");
        ui -> UseBar2 -> setText("保存并设为使用");
    }
    else if(ui -> UseBar1 -> text() == "取消使用")
    {
        usage = 0;
        ui -> UseBar1 -> setText("保存并设为使用");
    }
}
void MainWindow :: UseBar2()
{
    if(ui -> UseBar2 -> text() == "保存并设为使用")
    {
        Bar2 -> SaveBar();
        usage = 2;
        ui -> UseBar2 -> setText("取消使用");
        ui -> UseBar1 -> setText("保存并设为使用");
    }
    else if(ui -> UseBar2 -> text() == "取消使用")
    {
        usage = 0;
        ui -> UseBar2 -> setText("保存并设为使用");
    }
}
