#include "steptable.h"
#include <QString>
#include <QBrush>
/*
StepInfo Definition
*/
stepInfo::stepInfo(int a, int b, int c)
    :step(a),stateid(b),nextmove(c)
{

}

int stepInfo::getData(int opt)
{
    switch(opt)
    {
    case 0:return this->step;
    case 1:return this->stateid;
    case 2:return this->nextmove;
    default:return 0xffffffff;//means error!
    }
}


/*
StepModel Definition
*/
StepModel::StepModel(QList<stepInfo> &list, QObject *parent)
    :steplist(list),highlightline(-1),QAbstractTableModel(parent)
{

}

bool StepModel::setHighLightLine(int line)
{
    if(line<=-1)
        return false;
    highlightline = line;
    return true;
}

void StepModel::nextLine()
{
    int line = this->highlightline;
    line++;
    setHighLightLine(line);
}

int StepModel::getHighLightLine()
{
    return highlightline;
}

int StepModel::rowCount(const QModelIndex &parent) const
{
    return steplist.count();
}

int StepModel::columnCount(const QModelIndex &parent) const
{
    return 3;//stateid,step,nextmove
}

void StepModel::setStepList(QList<stepInfo> &li)
{
    steplist = li;
}

bool StepModel::insertLine(stepInfo& x,int line)
{
    int count = this->columnCount(QModelIndex());
    QModelIndex index;
    insertRows(line,1,QModelIndex());
    for(int i=0;i<count;i++){
        index = this->index(line,i);
        switch(i){
        case 0:
            setData(index,x.step,Qt::UserRole);
            break;
        case 1:
            setData(index,x.stateid,Qt::UserRole);
            break;
        case 2:
            setData(index,x.nextmove,Qt::UserRole);
            break;
        default:
            return false;
        }
    }
    return true;
}

bool StepModel::insertRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    beginInsertRows(QModelIndex(),row, row+count-1);

    for (int i=row;i<row+count;i++) {
        stepInfo info(0,0,0);
        steplist.insert(i,info);
    }
    endInsertRows();
    return true;
}

bool StepModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(index.isValid())
    {
     int row = index.row();
     switch(index.column()){
     case 0://step
         steplist[row].step = value.toInt();break;
     case 1://stepid
         steplist[row].stateid = value.toInt();break;
     case 2:
         steplist[row].nextmove = value.toInt();break;
     default:
         return false;
     }
     emit dataChanged(index,index);
     return true;
    }
    return false;
}


QVariant StepModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    int row = index.row();
    int col = index.column();
    stepInfo info = steplist[row];
    int num = info.getData(col);
    switch(role)
    {
    case Qt::DisplayRole:
        return QString::number(num,10);
    case Qt::TextAlignmentRole:
        return int(Qt::AlignHCenter | Qt::AlignVCenter);
    case Qt::BackgroundRole:
        if(highlightline == row)
            return QBrush(Qt::yellow);
        else
            return QBrush(QColor(104,86,205,128));
    }
    return QVariant();
}

QVariant StepModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QStringList list;
    list<<"No."<<"State"<<"Direction";
    if(role == Qt::DisplayRole&&orientation==Qt::Horizontal){
        return list[section];
    }
    return QVariant();
}
