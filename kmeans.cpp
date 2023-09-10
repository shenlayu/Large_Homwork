#include "kmeans.h"
#include "src/kmeansAlg.h"
#include "pca.h"
#include "visualization.h"
#include "scatter.h"

KMeans::KMeans(const std :: vector<std :: vector<float>>& InputData, int k, QWidget *parent,
               std :: vector<float>* const group, std :: vector<float>* const ConstGroup) : QWidget(parent)
{
    auto pair = clusterKMeans(InputData, k, 30);
    auto GroupKmeans = std :: get<1>(pair);
    setFixedSize(1200, 490);

    PCA* PCAObject2 = new PCA(InputData, 2, this, group);
    PCAObject2 -> setGeometry(0, 30, 600, 460);
    PCA* PCAObject2Const = new PCA(InputData, 2, this, ConstGroup);
    PCAObject2Const -> setGeometry(0, 30, 600, 460);
    PCAObject2Const -> setVisible(false);

    PCA* PCAObject3 = nullptr;
    PCA* PCAObject3Const = nullptr;

    int dimension = InputData[0].size();
    if(dimension >= 3)
    {
        resize(1200, 490);
        PCAObject3 = new PCA(InputData, 3, this, group);
        PCAObject3 -> setGeometry(600, 30, 600, 460);
        PCAObject3Const = new PCA(InputData, 3, this, ConstGroup);
        PCAObject3Const -> setGeometry(600, 30, 600, 460);
        PCAObject3Const -> setVisible(false);
    }


    QPushButton* changer = new QPushButton("切换为按性质分类", this);
    changer -> setGeometry(0, 0, 200, 30);

    connect(changer, &QPushButton :: clicked, this, [=](){
        if(changer -> text() == "切换为按性质分类")
        {
            changer -> setText("切换为按聚类分类");
            PCAObject2 -> setVisible(false);
            PCAObject2Const -> setVisible(true);
            if(dimension >= 3)
            {
                PCAObject3 -> setVisible(false);
                PCAObject3Const -> setVisible(true);
            }

        }
        else if(changer -> text() == "切换为按聚类分类")
        {
            changer -> setText("切换为按性质分类");
            PCAObject2 -> setVisible(true);
            PCAObject2Const -> setVisible(false);
            if(dimension >= 3)
            {
                PCAObject3 -> setVisible(true);
                PCAObject3Const -> setVisible(false);
            }
        }
    });
    show();
}
