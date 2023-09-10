#include "pca.h"
#include "scatter.h"
#include "src/pcaAlg.h"
#include "visualization.h"
#include <QDebug>

PCA :: PCA(const std :: vector<std :: vector<float>>& InputData, int dimension, QWidget *parent, std :: vector<float>* const group) : QWidget(parent)
{
    auto MatrixPCA = pca(InputData, dimension);

    if(dimension == 2)
    {
        setFixedSize(600, 460);
        std :: vector<float> DataX, DataY;
        for(int i = 0; i < MatrixPCA.rows(); i ++)
        {
            DataX.push_back(MatrixPCA(i, 0));
            DataY.push_back(MatrixPCA(i, 1));
        }
        Scatter* ScatterObject = new Scatter(DataX, DataY, "X", "Y", this, group);
    }
    else if(dimension == 3)
    {
        setFixedSize(600, 460);
        QtDataVisualization::Q3DScatter *Chart3DScatter = nullptr;
        QVector<QColor> GroupColor = {Qt :: black, Qt :: red, Qt :: blue, Qt :: green, Qt :: yellow, QColor(255, 192, 203), QColor(255, 165, 0),
                                     QColor(128, 128, 128), QColor(0, 128, 128), QColor(128, 0, 128)};
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
        int NumberAmount = InputData.size();
        for(int i = 0; i < NumberAmount; i ++)
        {
            GroupData[(*group)[i]] -> push_back(i);
        }

        for(int i = 0; i < GroupAmount; i ++)
        {
            auto pair = Scatter3DVis(MatrixPCA, GroupData[i], Chart3DScatter);
            Chart3DScatter = std :: get<0>(pair);
            std :: get<1>(pair) -> setBaseColor(GroupColor[i]);
        }

        QWidget *container = QWidget::createWindowContainer(Chart3DScatter, this);
        container->setMinimumSize(600, 460);
        container->setWindowTitle("3D Scatter Chart Example");
        container->show();
        show();
    }
}
