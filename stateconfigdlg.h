#ifndef STATECONFIGDLG_H
#define STATECONFIGDLG_H

#include <QDialog>
class QLabel;
class QLineEdit;
class QPushButton;
class StateConfigDlg :public QDialog
{
Q_OBJECT
public:
    StateConfigDlg(QWidget *parent);
signals:
    void sendState(QString str);
protected slots:
    void goClicked();
private:
    QLabel *label;
    QLineEdit *lineEdit;
    QPushButton *goButton;
};

#endif // STATECONFIGDLG_H
