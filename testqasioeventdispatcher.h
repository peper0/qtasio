#ifndef TESTQASIOEVENTDISPATCHER_H
#define TESTQASIOEVENTDISPATCHER_H

#include <QObject>
#include <QtTest>

#include <boost/asio/io_service.hpp>

class TestQAsioEventDispatcher : public QObject
{
    Q_OBJECT
public:
    explicit TestQAsioEventDispatcher(QObject *parent = 0);

signals:

public slots:

private:
    int argc=0;
    char *argv={0};
    boost::asio::io_service io_service;
    QApplication *app;

private slots:
    void initTestCase();
    void asioHandlersAreExecuted();
    void qsocketReadWrite();
    void qtimers();
    void guiEvents();
    void cleanupTestCase();
};

#endif // TESTQASIOEVENTDISPATCHER_H
