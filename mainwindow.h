#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include "state.h"
class ItemContainer;
class StateConfigDlg;
class QSplitter;
class QTableWidget;

class MainWindow:public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QPixmap pict,QWidget *parent=0);
    ~MainWindow();
public slots:
    void solvePuzzle();
    void solveNextMove();
protected slots:
    void newBackground();
    void restartGame();
    void showStateConfigDlg();
    void setNewState(QString str);
protected:
    void createActions();
    void createMenu();
    void createToolBar();
    void fillTable();
public:
private:
    QPixmap currentPicture;
    ItemContainer *central;
    StateConfigDlg *stateDlg;
    QTableWidget *table;
    QSplitter *splitter;

    QMenu *fileMenu;
    QMenu *toolsMenu;
    QToolBar *baseToolBar;

    QAction *openPict;
    QAction *newGame;
    QAction *exitGame;
    QAction *nextStep;
    QAction *autoComplete;
    QAction *configState;

    QDir lastDir;/*keep the path which is open last time*/
    State *destination;
    stack<State> result;

};

#endif // MAINWINDOW_H
