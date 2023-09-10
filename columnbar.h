#ifndef COLUMNBAR_H
#define COLUMNBAR_H

#include <QWidget>
#include <QVector>

namespace Ui {
class ColumnBar;
}

class ColumnBar : public QWidget
{
    Q_OBJECT

public:
    explicit ColumnBar(QWidget *parent = nullptr);
    ~ColumnBar();
    QVector<int> Columns;
    int BeginRow, EndRow;
    void SetMax();

public slots:
    void SaveBar();
    void AddBar(QList<int>);

private:
    Ui::ColumnBar *ui;
    bool Saved;
};

#endif // COLUMNBAR_H
