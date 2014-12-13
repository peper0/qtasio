#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <qsocketnotifier.h>
#include <boost/bind.hpp>
#include <qpa/qwindowsysteminterface.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

#define LOG(expr) #expr": " << (expr) << "; "

void MyEventDispatcher::read_ready(Sock s, const boost::system::error_code& error, int socket)
{
    std::clog<< __PRETTY_FUNCTION__ << LOG(error.message()) << LOG(socket) << std::endl;
    if(!error)
    {
        QEvent event(QEvent::SockAct);
        //p->non_blocking(true);
        QCoreApplication::sendEvent(s.n, &event);
        s.sd->async_read_some(boost::asio::null_buffers(), boost::bind(&MyEventDispatcher::read_ready, this, s, _1, socket));
    }
}

void MyEventDispatcher::registerSocketNotifier(QSocketNotifier *notifier)
{
    std::clog<< __PRETTY_FUNCTION__ << LOG(notifier->type()) << LOG(notifier->socket()) << std::endl;
    //Base::registerSocketNotifier(notifier);
    Sock s;
    s.sd = new boost::asio::posix::stream_descriptor(ioser, notifier->socket());
    s.n = notifier;
    desc[notifier->socket()] = s;
    read_ready(s, boost::system::error_code(), notifier->socket());
}

void MyEventDispatcher::unregisterSocketNotifier(QSocketNotifier *notifier)
{
    std::clog<< __PRETTY_FUNCTION__ << LOG(notifier->type()) << LOG(notifier->socket()) << std::endl;
    //desc[notifier->socket()]->cancel();
    Sock s = desc[notifier->socket()];
    s.sd->cancel();
    s.sd->release();
    //delete p;
}

void MainWindow::on_pushButton_clicked()
{
    std::clog<< __PRETTY_FUNCTION__ <<std::endl;
}


bool MyEventDispatcher::processEvents(QEventLoop::ProcessEventsFlags flags)
{
    std::clog<< __PRETTY_FUNCTION__ <<LOG(flags) <<std::endl;

    should_interrupt = false;
    // we are awake, broadcast it
    emit awake();
    QCoreApplication::sendPostedEvents(0, 0);

    //int nevents = 0;
    const bool canWait = !should_interrupt && (flags & QEventLoop::WaitForMoreEvents);

    if (canWait)
        emit aboutToBlock();

    if (canWait) {
        //bool res = Base::processEvents(flags);
        ioser.reset();
        ioser.run_one();
    }

    QWindowSystemInterface::sendWindowSystemEvents(flags);
    bool res = true;
    std::clog<< __PRETTY_FUNCTION__ << LOG(res) << std::endl;
    return res;
}

bool MyEventDispatcher::hasPendingEvents()
{
    std::clog<< __PRETTY_FUNCTION__ <<std::endl;
    bool res = Base::hasPendingEvents();
    std::clog<< __PRETTY_FUNCTION__ << LOG(res) << std::endl;
    return res;
}

void MyEventDispatcher::registerTimer(int timerId, int interval, Qt::TimerType timerType, QObject *object)
{
    std::clog<< __PRETTY_FUNCTION__ << LOG(timerId) << LOG(interval) <<  LOG(timerType) << std::endl;
    return Base::registerTimer(timerId, interval, timerType, object);
}

void MyEventDispatcher::wakeUp()
{
    std::clog<< __PRETTY_FUNCTION__ <<std::endl;
    ioser.stop();
//    return Base::wakeUp();
}

void MyEventDispatcher::interrupt()
{
    should_interrupt = true;
    wakeUp();
    std::clog<< __PRETTY_FUNCTION__ <<std::endl;
//    return Base::interrupt();
}

void MyEventDispatcher::flush()
{
    std::clog<< __PRETTY_FUNCTION__ <<std::endl;
    return Base::flush();
}


void MainWindow::mousePressEvent(QMouseEvent *)
{
    std::clog<< __PRETTY_FUNCTION__ <<std::endl;
}


bool MyEventDispatcher::unregisterTimer(int timerId)
{
    std::clog<< __PRETTY_FUNCTION__ << LOG(timerId) << std::endl;
    return Base::unregisterTimer(timerId);
}

bool MyEventDispatcher::unregisterTimers(QObject *object)
{
    std::clog<< __PRETTY_FUNCTION__ << LOG(object) << std::endl;
    return Base::unregisterTimers(object);
}


void MainWindow::paintEvent(QPaintEvent *)
{
    std::clog<< __PRETTY_FUNCTION__  << std::endl;
}

