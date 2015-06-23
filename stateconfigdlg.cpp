#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QValidator>
#include <QHBoxLayout>
#include "stateconfigdlg.h"

StateConfigDlg::StateConfigDlg(QWidget *parent)
    :QDialog(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    label = new QLabel(tr("State:"),this);

    QRegExpValidator *validator;
    validator = new QRegExpValidator(QRegExp("[1-8]{8,8}[1-9]"),this);
    lineEdit = new QLineEdit(this);
    lineEdit->setValidator(validator);

    goButton = new QPushButton(tr("Go"),this);
    connect(goButton,SIGNAL(clicked()),this,SLOT(goClicked()));
    layout->addWidget(label);
    layout->addWidget(lineEdit);
    layout->addWidget(goButton);

    setLayout(layout);
}

void StateConfigDlg::goClicked()
{
    QString str;
    str = lineEdit->text();
    emit sendState(str);
}
