#include "testqasioeventdispatcher.h"

#include "qasioeventdispatcher.h"

#include <QTcpServer>
#include <QTcpSocket>

TestQAsioEventDispatcher::TestQAsioEventDispatcher(QObject *parent) :
    QObject(parent)
{
}

void TestQAsioEventDispatcher::initTestCase()
{
    qDebug("creating QApplication");
    QApplication::setEventDispatcher(new QAsioEventDispatcher(io_service));
    app = new QApplication(argc, &argv);
}

void TestQAsioEventDispatcher::asioHandlersAreExecuted()
{
    int asioHandlersExecuted = 0;
    io_service.post([&asioHandlersExecuted](){asioHandlersExecuted++;});
    io_service.post([&asioHandlersExecuted](){asioHandlersExecuted++;});
    app->processEvents();
    QVERIFY(asioHandlersExecuted == 2);
}

void TestQAsioEventDispatcher::qsocketReadWrite()
{
    int clientsConnected = 0;
    int clientBytesRead = 0;
    int serverBytesWritten = 0;
    auto server = new QTcpServer(this);
    QObject::connect(server, &QTcpServer::newConnection, [&clientsConnected]()
    {
        clientsConnected++;
        qDebug() << "connected";
    });

    auto socket = new QTcpSocket(this);
    QObject::connect(socket, &QTcpSocket::readyRead, [&]()
    {
        clientBytesRead += socket->readAll().size();
        qDebug() << "readyRead";
    });


    server->listen(QHostAddress("127.0.6.1"));
    int port = server->serverPort();

    qDebug() << "Connecting... (port " << port << ")";
    socket->connectToHost("127.0.6.1", port);

    app->processEvents();
    QCOMPARE(clientsConnected, 1);

    QTcpSocket *server_socket = server->nextPendingConnection();
    QVERIFY(server_socket);
   // if(!server_socket) return;
    QObject::connect(server_socket, &QTcpSocket::disconnected, [&clientsConnected]()
    {
        clientsConnected--;
    });
    QObject::connect(server_socket, &QTcpSocket::bytesWritten, [&](qint64 bytes)
    {
        serverBytesWritten += bytes;
    });

    server_socket->write("abcd\n");
    server_socket->flush();
    app->processEvents();
    QCOMPARE(clientBytesRead, 5);
    QCOMPARE(serverBytesWritten, 5);

    socket->disconnectFromHost();
    app->processEvents();
    QCOMPARE(clientsConnected, 0);


    app->processEvents();
}

void TestQAsioEventDispatcher::qtimers()
{

}

void TestQAsioEventDispatcher::guiEvents()
{

}


void TestQAsioEventDispatcher::cleanupTestCase()
{
    delete app;
    app = 0;
    qDebug("destroying QApplication");
}

QTEST_APPLESS_MAIN(TestQAsioEventDispatcher)

//#include "testqasioeventdispatcher.moc"
