#include "mainwindow.h"
#include "qasioeventdispatcher.h"
#include <QApplication>
#include <QThread>
#include <glib.h>
#include <iostream>
#include <typeinfo>

int main(int argc, char *argv[])
{
    //MyEventDispatcher ed;
    //QThread::currentThread()->setEventDispatcher(new QEventDispatcherUNIX());
    //QAbstractEventDispatcher *ev = new QEventDispatcherGlib(g_main_context_default());
    //QAbstractEventDispatcher *ev = new QEventDispatcherUNIX();
    //QAbstractEventDispatcher *ev = new MyEventDispatcher();
#if 0
    QAbstractEventDispatcher *ev = new QAsioEventDispatcher();
    if(argc<=1)
    {
        QApplication::setEventDispatcher(ev);
    }
#endif
    boost::asio::io_service my_io_service;

    QApplication::setEventDispatcher(new QAsioEventDispatcher(my_io_service));

    std::clog << "declaring QApplication" << std::endl;
    //ev->startingUp();
    QApplication a(argc, argv);
    std::clog << "declarin main window" << std::endl;
    //QAbstractEventDispatcher* e = QThread::currentThread()->eventDispatcher();
    //std::clog << "ev: " << typeid(*e).name() << std::endl;
    //e->moveToThread(QThread::currentThread());
    //ev->setParent(&a);
    MainWindow w;
    std::clog << "showing main window" << std::endl;
    w.show();
    std::clog << "exec" << std::endl;

    return a.exec();
}
