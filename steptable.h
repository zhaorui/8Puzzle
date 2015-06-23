#ifndef STEPTABLE_H
#define STEPTABLE_H
#include <QAbstractTableModel>

class stepInfo
{
public:
    stepInfo(int,int,int);
    int step;
    int stateid;
    int nextmove;
    int getData(int opt);
};


class StepModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    StepModel(QList<stepInfo>& list,QObject *parent=0);
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool insertRows(int row, int count, const QModelIndex &parent);
    void setStepList(QList<stepInfo>& li);
    bool insertLine(stepInfo&, int line);
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    bool setHighLightLine(int line);
    int getHighLightLine();
public slots:
    void nextLine();
private:
    QList<stepInfo> steplist;
    int highlightline;
};

#endif // STEPTABLE_H
