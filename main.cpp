//#include <iostream>

//using namespace std;

//int main()
//{
//    cout << "Hello World!" << endl;
//    return 0;
//}


#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}




