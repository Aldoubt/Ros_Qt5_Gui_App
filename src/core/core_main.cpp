#include "mainwindow.h"
#include <QApplication>
#include <csignal>
int main(int argc,char**argv){QApplication app(argc,argv);MainWindow window;window.show();return app.exec();}
