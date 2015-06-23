#include <QAction>
#include <QFileDialog>
#include <QMenuBar>
#include <QToolBar>
#include <time.h>
#include <QSplitter>
#include <QTableWidget>
#include <QFile>
#include <QMessageBox>
#include <QTextCodec>
#include "stateconfigdlg.h"
#include "mainwindow.h"
#include "itemcontaniner.h"


MainWindow::MainWindow(QPixmap pict, QWidget *parent)
    :currentPicture(pict),central(new ItemContainer(currentPicture,State(),this)),stateDlg(NULL),QMainWindow(parent)
{
    srand((unsigned)time(NULL));
    destination = new State(3,false);//standard state which means its id is "123456789"
    setWindowIcon(QIcon(":icons/windowicon.jpg"));

    /*setup splitter*/
    QStringList strlist;
    strlist<<"step"<<"id"<<"direction";
    table = new QTableWidget(this);
    table->setColumnCount(3);
    table->setHorizontalHeaderLabels(strlist);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(central);
    splitter->addWidget(table);
    splitter->setStretchFactor(1,1);

    setCentralWidget(splitter);
    createActions();
	createMenu();
    createToolBar();

    /*read map from local file*/
//    if(QFile::exists("result.dat")){
//        QFile file("result.dat");
//        QDataStream in(&file);
//        in.setVersion(QDataStream::Qt_4_3);
//        if(!file.open(QIODevice::ReadOnly)){
//            //error and calculate map
//            std::cerr<<"Cannot Open file for reading:"
//                    <<qPrintable(file.errorString())<<std::endl;
//            getAllBestPath(*destination);
//        }else{
//            //read from local
//            in>>allPathMap;
//        }
//    }else{
//        //calculate the map
//        getAllBestPath(*destination);
//        QFile file("result.dat");
//        QDataStream out(&file);
//        out.setVersion(QDataStream::Qt_4_3);
//        if(!file.open(QIODevice::WriteOnly)){
//            //error
//            std::cerr<<"Cannot Open file for writing:"
//                    <<qPrintable(file.errorString())<<std::endl;
//        }else{
//            //write to local;
//            out<<allPathMap;
//        }
//    }
    getAllBestPath(*destination);
}

MainWindow::~MainWindow()
{
    delete destination;
}

void MainWindow::createMenu()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openPict);
    fileMenu->addAction(newGame);

    toolsMenu = menuBar()->addMenu(tr("Tools"));
    toolsMenu->addAction(configState);
}

void MainWindow::createToolBar()
{
    baseToolBar = addToolBar(tr("Operation"));
    baseToolBar->addAction(nextStep);
    baseToolBar->addAction(autoComplete);
}

void MainWindow::createActions()
{
    openPict = new QAction(tr("&Open..."),this);
    openPict->setShortcut(QKeySequence::Open);
    openPict->setStatusTip(tr("use user-defined picture as game's background."));
    connect(openPict,SIGNAL(triggered()),this,SLOT(newBackground()));

    newGame = new QAction(tr("&New Game"),this);
    newGame->setShortcut(QKeySequence::New);
    connect(newGame,SIGNAL(triggered()),this,SLOT(restartGame()));

    nextStep = new QAction(tr("Next"),this);
    nextStep->setIcon(QIcon(":/icons/next.JPG"));
    nextStep->setToolTip(tr("The next best move"));
    connect(nextStep,SIGNAL(triggered()),this,SLOT(solveNextMove()));

    autoComplete = new QAction(tr("Auto"),this);
    autoComplete->setIcon(QIcon(":/icons/auto.JPG"));
    autoComplete->setToolTip(tr("complete puzzle with computer"));
    connect(autoComplete,SIGNAL(triggered()),this,SLOT(solvePuzzle()));

    configState = new QAction(tr("Set State"),this);
    configState->setIcon(QIcon(":/icons/set.jpg"));
    connect(configState,SIGNAL(triggered()),this,SLOT(showStateConfigDlg()));
}

void MainWindow::showStateConfigDlg()
{
    if(!stateDlg){
        stateDlg = new StateConfigDlg(this);
        connect(stateDlg,SIGNAL(sendState(QString)),this,SLOT(setNewState(QString)));
    }
    stateDlg->show();
    stateDlg->raise();
    stateDlg->activateWindow();
}

void MainWindow::setNewState(QString str)
{
    if(!State::isAcceptable(3,str.toInt())){
        QMessageBox::warning(this,tr("State Illegal"),
                             tr("Please recheck your input state,digit is not enough or there is repeat item!"));
        return;
    }
    delete central;
    central = new ItemContainer(currentPicture,State(3,str.toInt()),this);
    splitter->insertWidget(0,central);
    setCentralWidget(splitter);
}

void MainWindow::solveNextMove()
{
	while(!result.empty())
		result.pop();
    if(!central->isSolvable()){
        QMessageBox::warning(this,tr("Something You Should Know"),
                             tr("Sorry,You know This State can never be solve!"));
        return;
    }
	getBestWayToStack(central->getStateId(),result);
	State& it = result.top();
	central->move(oppositeWay(it.moveHistory));
    result.pop();
    fillTable();
}

void MainWindow::solvePuzzle()
{
    //make sure the result stack is clear
    while(!result.empty())
        result.pop();
    if(!central->isSolvable()){
        QMessageBox::warning(this,tr("Something You Should Know"),
                             tr("Sorry,You know This State can never be solve!"));
        return;
    }
    getBestWayToStack(central->getStateId(),result);
    fillTable();
    while(!result.empty())
    {
        State& it = result.top();
        central->move(oppositeWay(it.moveHistory));
        result.pop();
    }
}

void MainWindow::fillTable()
{
    if(result.empty())
        return;
    stack<State> tempResult = result;
    QTableWidgetItem *item;
    State top;
    int row = 0;
    while(!tempResult.empty()){
        top = tempResult.top();
        table->setRowCount(row+1);
        for(int col=0;col<3;col++){
            if(col == 0){
                item = new QTableWidgetItem(tr("%1").arg(top.step));//is memory leaks here??
                item->setTextAlignment(Qt::AlignHCenter);
                table->setItem(row,col,item);
            }
            else if(col==1){
                item = new QTableWidgetItem(tr("%1").arg(top.id));
                item->setTextAlignment(Qt::AlignHCenter);
                table->setItem(row,col,item);
            }
            else
            {
                QByteArray ch;
                QTextCodec *codec = QTextCodec::codecForName("GB2312");
                switch(oppositeWay(top.moveHistory)){
                case State::U:ch = "¡ü";break;
                case State::D:ch = "¡ý";break;
                case State::R:ch = "¡ú";break;
                case State::L:ch = "¡û";break;
                case State::Z:ch = " ";break;
                default:break;
                }
                QString uni_ch = codec->toUnicode(ch);
                item = new QTableWidgetItem(tr("%1").arg(uni_ch));
                item->setTextAlignment(Qt::AlignHCenter);
                table->setItem(row,col,item);
            }
        }
        tempResult.pop();
		row++;
    }
}

void MainWindow::newBackground()
{
    QString filePath = QFileDialog::getOpenFileName(this,
                 tr("Open Image"), lastDir.absolutePath(), "Image Files (*.png *.jpg *.bmp)");
    if(filePath.isEmpty())
        return ;
    QPixmap newpict(filePath);
    currentPicture = newpict;
    central->changeBackground(newpict);
    lastDir.setPath(filePath);
}

void MainWindow::restartGame()
{
    delete central;
    central = new ItemContainer(currentPicture,State(),this);
    splitter->insertWidget(0,central);
    setCentralWidget(splitter);
}
