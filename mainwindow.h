#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtPlatformSupport/private/qunixeventdispatcher_qpa_p.h>
#include <QtCore/private/qeventdispatcher_unix_p.h>
#include <QtCore/private/qeventdispatcher_glib_p.h>
#include <boost/asio/io_service.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>
#include <QSocketNotifier>
#include <map>


struct Sock
{
    boost::asio::posix::stream_descriptor *sd;
    QSocketNotifier *n;
};

class MyEventDispatcher: public QUnixEventDispatcherQPA
{
    Q_OBJECT
    void read_ready(Sock s, const boost::system::error_code &error, int socket);
    bool should_interrupt;
public:
    typedef QUnixEventDispatcherQPA Base;
    MyEventDispatcher() {}
    MyEventDispatcher(QObject *parent): Base(parent) {}
    virtual ~MyEventDispatcher() {}

    boost::asio::io_service ioser;
    std::map<int, Sock> desc;

    void registerSocketNotifier(QSocketNotifier *notifier);
    void unregisterSocketNotifier(QSocketNotifier *notifier);
    bool processEvents(QEventLoop::ProcessEventsFlags flags);
    bool hasPendingEvents();
    void registerTimer(int timerId, int interval, Qt::TimerType timerType, QObject *object);
    void wakeUp();
    void interrupt();
    void flush();
    bool unregisterTimer(int timerId);
    bool unregisterTimers(QObject *object);

};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *);

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *);
};

#endif // MAINWINDOW_H
