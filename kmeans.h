#pragma once
#ifndef KMEANS_H
#define KMEANS_H

#include <QWidget>

class KMeans : public QWidget
{
    Q_OBJECT
public:
    explicit KMeans(const std :: vector<std :: vector<float>>& InpttData,
                    int k, QWidget *parent = nullptr, std :: vector<float>* const group = nullptr, std :: vector<float>* const ConstGroup = nullptr);

signals:

};

#endif // KMEANS_H
