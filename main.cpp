#include <QApplication>
#include "itemcontaniner.h"
#include "mainwindow.h"
int main(int argc,char* argv[])
{
    QApplication app(argc,argv);
    QPixmap pixmap = QPixmap(":/pictures/Hommer.jpg");
    //ItemContainer Container(pixmap,State());
    //Container.show();
    MainWindow window(pixmap);
    window.show();
    return app.exec();
}
