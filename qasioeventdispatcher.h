#ifndef QASIOEVENTDISPATCHER_H
#define QASIOEVENTDISPATCHER_H

#include "QtCore/qabstracteventdispatcher.h"
#include "private/qabstracteventdispatcher_p.h"
#include <QAbstractEventDispatcher>

/*

#include "QtCore/qabstracteventdispatcher.h"
#include "QtCore/qlist.h"
#include "private/qabstracteventdispatcher_p.h"
#include "private/qcore_unix_p.h"
#include "private/qpodlist_p.h"
#include "QtCore/qvarlengtharray.h"

#if !defined(Q_OS_VXWORKS)
#  include <sys/time.h>
#  if (!defined(Q_OS_HPUX) || defined(__ia64)) && !defined(Q_OS_NACL)
#    include <sys/select.h>
#  endif
#endif

*/

class QAsioEventDispatcherPrivate;

namespace boost {
namespace asio {
class io_service;
}
}

class Q_CORE_EXPORT QAsioEventDispatcher : public QAbstractEventDispatcher
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QAsioEventDispatcher)

public:
    explicit QAsioEventDispatcher(boost::asio::io_service &io_service, QObject *parent = 0);
    ~QAsioEventDispatcher();

    //TODO: in progress
    bool processEvents(QEventLoop::ProcessEventsFlags flags);
    //TODO: done, not tested
    bool hasPendingEvents();
    //TODO: done, not tested
    void registerSocketNotifier(QSocketNotifier *notifier) Q_DECL_FINAL;
    //TODO: done, not tested
    void unregisterSocketNotifier(QSocketNotifier *notifier) Q_DECL_FINAL;

    //TODO:
    void registerTimer(int timerId, int interval, Qt::TimerType timerType, QObject *object) Q_DECL_FINAL;
    //TODO:
    bool unregisterTimer(int timerId) Q_DECL_FINAL;
    //TODO:
    bool unregisterTimers(QObject *object) Q_DECL_FINAL;
    //TODO:
    QList<TimerInfo> registeredTimers(QObject *object) const Q_DECL_FINAL;

    //TODO:
    int remainingTime(int timerId) Q_DECL_FINAL;

    //TODO: in progress
    void wakeUp() Q_DECL_FINAL;
    //TODO: in progress
    void interrupt() Q_DECL_FINAL;
    //TODO:
    void flush();

protected:
    QAsioEventDispatcher(QAsioEventDispatcherPrivate &dd, QObject *parent = 0);

};

#endif // QASIOEVENTDISPATCHER_H
