#include "scatter.h"
#include "src/leastsquareAlg.h"
#include "visualization.h"

int Scatter :: GetDimension()
{
    QDialog* window = new QDialog(this);

    window -> resize(200, 77);

    QLabel* tip = new QLabel(window);
    tip -> setText("输入阶数：");
    tip -> setGeometry(0, 0, 100, 50);
    QLineEdit* edit = new QLineEdit(window);
    edit -> setGeometry(100, 0, 100, 50);

    QPushButton* OK = new QPushButton("确定", window);
    OK -> setGeometry(150, 50, 50, 25);

    // 检查输入合法性
    int dimension = 0;
    connect(OK, &QPushButton :: clicked, [&](){
        QString CheakerStr = "^[1-9][0-9]*$";
        QRegularExpression cheaker(CheakerStr);
        if(!cheaker.match(edit -> text()).hasMatch())
        {
            QMessageBox :: critical(this, "错误", "输入不合法");
        }
        if(edit -> text().toInt() >= 10)
        {
            QMessageBox :: critical(this, "错误", "阶数过大");
        }
        dimension = edit -> text().toInt();
        window -> close();
    });
    window -> exec();

    return dimension;
}

Scatter :: Scatter(const std :: vector<float>& InputDataX, const std :: vector<float>& InputDataY,
                   const QString& NameX, const QString& NameY, QWidget *parent, std :: vector<float>* const group):  QWidget(parent)
{
    QChart* ChartScatter = nullptr;
    setFixedSize(600, 460);

    if(group != nullptr)
    {
        QVector<QColor> GroupColor = {Qt :: black, Qt :: red, Qt :: blue, Qt :: green, Qt :: yellow, QColor(255, 192, 203), QColor(255, 165, 0)};
        int GroupAmount = 0;
        for(auto number : (*group))
        {
            GroupAmount = GroupAmount > (number + 1) ? GroupAmount : (number + 1);
        }
        // 每个vector<int>存储一组数据
        std :: vector<std :: vector<int>*> GroupData(GroupAmount);
        for(int i = 0; i < GroupAmount; i ++)
        {
            GroupData[i] = new std :: vector<int>;
        }
        int NumberAmount = InputDataX.size();
        for(int i = 0; i < NumberAmount; i ++)
        {
            GroupData[(*group)[i]] -> push_back(i);
        }

        QVector<QScatterSeries*> GroupSeries;
        for(int i = 0; i < GroupAmount; i ++)
        {
            auto pair = ScatterVis(InputDataX, InputDataY, NameX, NameY, GroupData[i], ChartScatter);
            ChartScatter = std :: get<0>(pair);
            GroupSeries.push_back(std :: get<1>(pair));
            GroupSeries[i] -> setColor(GroupColor[i]);
        }

        QChartView* ChartViewScatter = new QChartView(ChartScatter);
        ChartViewScatter -> setParent(this);
        ChartViewScatter -> setGeometry(0, 60, 600, 400);
        for(auto series : GroupSeries)
        {
            connect(series, &QScatterSeries::hovered, this, [=](const QPointF &point, bool state) {
                if (state) {
                    QToolTip::showText(QCursor::pos(), QString("X: %1, Y: %2").arg(point.x()).arg(point.y()), ChartViewScatter);
                } else {
                    QToolTip::hideText();
                }
            });
        }
    }

    else
    {
        auto pair = ScatterVis(InputDataX, InputDataY, NameX, NameY);
        ChartScatter = std :: get<0>(pair);
        auto SeriesScatter = std :: get<1>(pair);
        QChartView* ChartViewScatter = new QChartView(ChartScatter);

        ChartViewScatter -> setParent(this);
        ChartViewScatter -> setGeometry(0, 60, 600, 400);

        connect(SeriesScatter, &QScatterSeries::hovered, this, [=](const QPointF &point, bool state) {
            if (state) {
                QToolTip::showText(QCursor::pos(), QString("X: %1, Y: %2").arg(point.x()).arg(point.y()), ChartViewScatter);
            } else {
                QToolTip::hideText();
            }
        });
    }

    int PointsNumber = InputDataX.size(), RepeatPointsNum = 0;
    float MinValX = InputDataX[0], MaxValX = InputDataX[0], MinValY = InputDataY[0], MaxValY = InputDataY[0];
    for(auto number : InputDataX)
    {
        MinValX = MinValX < number ? MinValX : number;
        MaxValX = MaxValX > number ? MaxValX : number;
    }
    for(auto number : InputDataY)
    {
        MinValY = MinValY < number ? MinValY : number;
        MaxValY = MaxValY > number ? MaxValY : number;
    }
    for(int i = 0; i < PointsNumber; i ++)
    {
        float x = InputDataX[i], y = InputDataY[i];
        for(int j = 0; j < i; j ++)
        {
            if(abs(InputDataX[j] - x) < (MaxValX - MinValX) / 500 &&
               abs(InputDataY[j] - y) < (MaxValY - MinValY) / 500)
            {
                RepeatPointsNum ++;
                break;
            }
        }
    }
    QPushButton* MakeCurve = new QPushButton("多项式曲线", this);
    MakeCurve -> setGeometry(0, 0, 100, 30);

    QPushButton* GiveP = new QPushButton("计算p值", this);
    GiveP -> setGeometry(100, 0, 100, 30);
    QPushButton* GiveRSquare = new QPushButton("计算" + QString :: fromUtf8("r\xC2\xB2") + "值", this);
    GiveRSquare -> setGeometry(200, 0, 100, 30);
    QLabel* RepeatPointsShower = new QLabel("重合共有" + QString :: number(RepeatPointsNum) + "个", this);
    RepeatPointsShower -> setGeometry(0, 30, 150, 30);
    QLabel* PShower = new QLabel(this);
    QLabel* RSquareShower = new QLabel(this);
    PShower -> setGeometry(150, 30, 150, 30);
    RSquareShower -> setGeometry(300, 30, 150, 30);

    connect(MakeCurve, &QPushButton :: clicked, [=](){
        int dimension = GetDimension();
        PolynomialLineVis(InputDataX, InputDataY, dimension, ChartScatter);
    });

    connect(GiveP, &QPushButton :: clicked, [=](){
        int dimension = GetDimension();
        float P = std :: get<1>(fitLeastSquareAndPR(InputDataX, InputDataY, dimension));
        PShower -> setText(QString :: number(dimension) + "阶p值为" + QString :: number(P));
    });
    connect(GiveRSquare, &QPushButton :: clicked, [=](){
        int dimension = GetDimension();
        float RSquare = std :: get<2>(fitLeastSquareAndPR(InputDataX, InputDataY, dimension));
        RSquareShower -> setText(QString :: number(dimension) + "阶" + QString :: fromUtf8("r\xC2\xB2") + "值为" + QString :: number(RSquare));
    });

    this -> show();
}
