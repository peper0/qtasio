#include "qasioeventdispatcher.h"
#include <boost/asio/io_service.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/bind.hpp>

#include <qpa/qwindowsysteminterface.h>
//#include "qplatformdefs.h"

#include "qcoreapplication.h"
#include "qpair.h"
#include "qsocketnotifier.h"
#include "qthread.h"
#include "qelapsedtimer.h"

#define LOG(expr) #expr": " << (expr) << "; "


/* TODO:
 * - timers
 * - write, error types of sockets
 * - delete all allocated objects on destroy
 * - remove socket from the list on cleanup
 */

// One object per fd; three QSocketNotifier's may be connected to it;
struct QAsioSockNotifier
{
    QSocketNotifier *notif[3]={0, 0, 0}; //notifiers for {Read, Write, Exception}
    int revision[3]={0, 0, 0}; //revision[x] is incremented each time notif[i] is changed (therefore a new completion handler loop is started)
    int fd;
    boost::asio::posix::stream_descriptor *sd;
    int pending_operations = 0;
};

struct QAsioTimer
{
};

class Q_CORE_EXPORT QAsioEventDispatcherPrivate : public QAbstractEventDispatcherPrivate
{
    Q_DECLARE_PUBLIC(QAsioEventDispatcher)

public:
    QAsioEventDispatcherPrivate(boost::asio::io_service &io_service);
    ~QAsioEventDispatcherPrivate();

    //int doSelect(QEventLoop::ProcessEventsFlags flags, timespec *timeout);
    //virtual int initThreadWakeUp() FINAL_EXCEPT_BLACKBERRY;
    //virtual int processThreadWakeUp(int nsel) FINAL_EXCEPT_BLACKBERRY;

    //bool mainThread;

    boost::asio::io_service &io_service;
    QList<QAsioSockNotifier *> socketNotifiers;
    QList<QAsioTimer *> timers;

    QAtomicInt interrupt;
    QAsioSockNotifier *socketNotifierForFd(int fd, bool createIfNotFound);
    void removeSocketNotifier(QAsioSockNotifier *sn);
};


QAsioEventDispatcherPrivate::QAsioEventDispatcherPrivate(boost::asio::io_service &io_service_)
    : io_service(io_service_)
{
}

QAsioEventDispatcherPrivate::~QAsioEventDispatcherPrivate()
{
    // cleanup timers
    qDeleteAll(socketNotifiers);
    qDeleteAll(timers);
}

QAsioEventDispatcher::QAsioEventDispatcher(boost::asio::io_service &io_service, QObject *parent)
    : QAbstractEventDispatcher(*new QAsioEventDispatcherPrivate(io_service), parent)
{ }

QAsioEventDispatcher::QAsioEventDispatcher(QAsioEventDispatcherPrivate &dd, QObject *parent)
    : QAbstractEventDispatcher(dd, parent)
{ }

QAsioEventDispatcher::~QAsioEventDispatcher()
{
}

void QAsioEventDispatcher::registerTimer(int timerId, int interval, Qt::TimerType timerType, QObject *obj)
{
#ifndef QT_NO_DEBUG
    if (timerId < 1 || interval < 0 || !obj) {
        qWarning("QAsioEventDispatcher::registerTimer: invalid arguments");
        return;
    } else if (obj->thread() != thread() || thread() != QThread::currentThread()) {
        qWarning("QObject::startTimer: timers cannot be started from another thread");
        return;
    }
#endif

    Q_D(QAsioEventDispatcher);
    //d->timerList.registerTimer(timerId, interval, timerType, obj);
}

/*!
    \internal
*/
bool QAsioEventDispatcher::unregisterTimer(int timerId)
{
#ifndef QT_NO_DEBUG
    if (timerId < 1) {
        qWarning("QAsioEventDispatcher::unregisterTimer: invalid argument");
        return false;
    } else if (thread() != QThread::currentThread()) {
        qWarning("QObject::killTimer: timers cannot be stopped from another thread");
        return false;
    }
#endif

    Q_D(QAsioEventDispatcher);
//    return d->timerList.unregisterTimer(timerId);
    return true;
}

/*!
    \internal
*/
bool QAsioEventDispatcher::unregisterTimers(QObject *object)
{
#ifndef QT_NO_DEBUG
    if (!object) {
        qWarning("QAsioEventDispatcher::unregisterTimers: invalid argument");
        return false;
    } else if (object->thread() != thread() || thread() != QThread::currentThread()) {
        qWarning("QObject::killTimers: timers cannot be stopped from another thread");
        return false;
    }
#endif

    Q_D(QAsioEventDispatcher);
    //return d->timerList.unregisterTimers(object);
    return true;
}

QList<QAsioEventDispatcher::TimerInfo>
QAsioEventDispatcher::registeredTimers(QObject *object) const
{
    if (!object) {
        qWarning("QAsioEventDispatcher:registeredTimers: invalid argument");
        return QList<TimerInfo>();
    }

    Q_D(const QAsioEventDispatcher);
    //return d->timerList.registeredTimers(object);
    return {};
}

QAsioSockNotifier *QAsioEventDispatcherPrivate::socketNotifierForFd(int fd, bool createIfNotFound)
{
    for(int i = 0; i < socketNotifiers.count(); ++i) {
        QAsioSockNotifier *n = socketNotifiers.at(i);
        if(n->fd == fd) return n;
    }
    if(createIfNotFound)
    {
        QAsioSockNotifier *sn = new QAsioSockNotifier;
        sn->fd = fd;
        sn->sd = new boost::asio::posix::stream_descriptor(io_service, fd);
        socketNotifiers.push_back(sn);
        return sn;
    }
    return 0;
}

void QAsioEventDispatcherPrivate::removeSocketNotifier(QAsioSockNotifier *sn)
{
    //removeAll, but there should be only one instance in the list

    socketNotifiers.removeAll(sn);
}

static void fdMaybeCleanup(QAsioSockNotifier *sn)
{
    //cleanup must be done after last operation on this fn is finished
    if(sn->pending_operations == 0)
    {
        Q_ASSERT(sn->notif[0] == 0
            && sn->notif[1] == 0
            && sn->notif[2] == 0);
        delete sn->sd;
        delete sn;
    }

}

static void fdReadStart(QAsioSockNotifier *sn);

static void fdReadReady(QAsioSockNotifier *sn, const boost::system::error_code& error, int revision)
{
    sn->pending_operations--;
    //std::clog<< __PRETTY_FUNCTION__ << LOG(error.message()) << LOG(socket) << std::endl;
    bool sn_changed = sn->revision[(int)QSocketNotifier::Read] != revision;

    if(!sn_changed && !error) {
        QEvent event(QEvent::SockAct);
        QCoreApplication::sendEvent(sn->notif[QSocketNotifier::Read], &event);
        fdReadStart(sn);
    }
    fdMaybeCleanup(sn);
}

static void fdReadStart(QAsioSockNotifier *sn)
{
    int type = (int)QSocketNotifier::Read;
    sn->pending_operations++;
    sn->sd->async_read_some(boost::asio::null_buffers(), boost::bind(fdReadReady, sn, _1, sn->revision[type]));
}

void QAsioEventDispatcher::registerSocketNotifier(QSocketNotifier *notifier)
{
    std::clog<< __PRETTY_FUNCTION__ << LOG(notifier->type()) << LOG(notifier->socket()) << std::endl;
    Q_ASSERT(notifier);
    int sockfd = notifier->socket();
    int type = notifier->type();
#ifndef QT_NO_DEBUG
    if (sockfd < 0
        || unsigned(sockfd) >= FD_SETSIZE) {
        qWarning("QSocketNotifier: Internal error");
        return;
    } else if (notifier->thread() != thread()
               || thread() != QThread::currentThread()) {
        qWarning("QSocketNotifier: socket notifiers cannot be enabled from another thread");
        return;
    }
#endif

    Q_D(QAsioEventDispatcher);

    QAsioSockNotifier *sn = d->socketNotifierForFd(sockfd, true);

    Q_ASSERT(sn);
    Q_ASSERT(type<3 && type>=0);
    Q_ASSERT(sn->notif[type] == 0);

    sn->notif[type] = notifier;
    sn->revision[type]++;
    switch(type)
    {
        case QSocketNotifier::Read:
            fdReadStart(sn);
            break;
        //TODO: write, error...
    }
}

void QAsioEventDispatcher::unregisterSocketNotifier(QSocketNotifier *notifier)
{
    std::clog<< __PRETTY_FUNCTION__ << LOG(notifier->type()) << LOG(notifier->socket()) << std::endl;
    Q_ASSERT(notifier);
    int sockfd = notifier->socket();
    int type = notifier->type();
#ifndef QT_NO_DEBUG
    if (sockfd < 0
        || unsigned(sockfd) >= FD_SETSIZE) {
        qWarning("QSocketNotifier: Internal error");
        return;
    } else if (notifier->thread() != thread()
               || thread() != QThread::currentThread()) {
        qWarning("QSocketNotifier: socket notifiers cannot be disabled from another thread");
        return;
    }
#endif

    Q_D(QAsioEventDispatcher);
    QAsioSockNotifier *sn = d->socketNotifierForFd(sockfd, false);
    Q_ASSERT(sn);
    if(sn)
    {
        Q_ASSERT(sn->notif[type] == notifier);
        sn->notif[type] = 0;
        sn->revision[type]++;
        if(sn->notif[0] == 0
                && sn->notif[1] == 0
                && sn->notif[2] == 0
                ) {
            //if no more socket notifiers on given fd are pending, we don't need it anymore
            //but we cannot delete "sn" yet, since some handlers that use it may be pending (see fdMaybeCleanup())
            sn->sd->cancel();
            //sn->sd->close();
            //d->removeSocketNotifier(sn);
            //FIXME: what if we create a new sn on the same fd soon (while handlers are still pending)?
        }
        //if not all notifiers are unregistered, we don't cancel pending handler, since we cannot do it selectively (i.e. cancel only read or only write)
        //however, it's not a problem, since it will notice that sn->revision doesn't match and will "cancel" itself during the next call
    }
}

bool QAsioEventDispatcher::processEvents(QEventLoop::ProcessEventsFlags flags)
{
    Q_D(QAsioEventDispatcher);
    d->interrupt.store(0);

    // we are awake, broadcast it
    //emit awake();
    //QCoreApplicationPrivate::sendPostedEvents(0, 0, d->threadData); //unix dispatcher use this
    QCoreApplication::sendPostedEvents(); //glib dispatcher call this after every call of "awake".

    const bool canWait = (/*d->threadData->canWaitLocked()
                          &&*/ !d->interrupt.load() //it may have been set during the call of QCoreApplication::sendPostedEvents()
                          && (flags & QEventLoop::WaitForMoreEvents));

    if (d->interrupt.load())
    {
        return false; //unix event dispatcher returns false if nothing more than "postedEvents" were dispatched
    }

    //FIXME: we should exit from "procesEvents" when some timer elapsed?
    // return the maximum time we can wait for an event.
    if ((flags & QEventLoop::X11ExcludeTimers)) {
        //FIXME: ignore timers?
    }

    int nevents = 0; /* FIXME: +1 for stop? */

    if (canWait) {
        //run at least one handler - may bloc
        emit aboutToBlock();
        d->io_service.reset();
        nevents += d->io_service.run_one();
        emit awake();
    }
    d->io_service.reset();
    //run all ready handlers
    nevents += d->io_service.poll();

    if ((flags & QEventLoop::ExcludeSocketNotifiers)) {
        //FIXME: ignore stream descriptoss
    }

    if ((flags & QEventLoop::X11ExcludeTimers)) {
        //FIXME: ignore timers?
    }

    // return true if we handled events, false otherwise
    return QWindowSystemInterface::sendWindowSystemEvents(flags) || (nevents > 0);
}

bool QAsioEventDispatcher::hasPendingEvents()
{
    //This is copy-pasted from QUnixEventDispatcherQPA. I don't really understand how it works.
    extern uint qGlobalPostedEventsCount(); // from qapplication.cpp
    return qGlobalPostedEventsCount() || QWindowSystemInterface::windowSystemEventsQueued();
}

int QAsioEventDispatcher::remainingTime(int timerId)
{
#ifndef QT_NO_DEBUG
    if (timerId < 1) {
        qWarning("QAsioEventDispatcher::remainingTime: invalid argument");
        return -1;
    }
#endif
    //FIXME
    Q_D(QAsioEventDispatcher);
    //return d->timerList.timerRemainingTime(timerId);
    return 999;
}

void QAsioEventDispatcher::wakeUp()
{
    Q_D(QAsioEventDispatcher);
    //I don't know what a logic stays behind it, but qeventdispatcher_glib works in such a way, that after every "wakeUp", the following function is called
    //io_service.post(QCoreApplication::sendPostedEvents);
    d->io_service.stop(); //exit from run_one
}

void QAsioEventDispatcher::interrupt()
{
    Q_D(QAsioEventDispatcher);
    d->interrupt.store(1);
    wakeUp();
}

void QAsioEventDispatcher::flush()
{ }

QT_END_NAMESPACE
