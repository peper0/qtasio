#include "testqasioeventdispatcher.h"

#include "qasioeventdispatcher.h"

#include <QTcpServer>
#include <QTcpSocket>
#include <chrono>

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

    delete socket;
    delete server_socket;
    delete server;
    app->processEvents();
}

void TestQAsioEventDispatcher::qtimers()
{
    using namespace std::chrono;
    steady_clock::time_point tBegin, tEnd1, tEnd2;
    tBegin = steady_clock::now();
    tEnd1 = tEnd2 = tBegin;
    QTimer timer1;
    QObject::connect(&timer1, &QTimer::timeout, [&](){
        tEnd1 = steady_clock::now();
    });

    QTimer timer2;
    QObject::connect(&timer2, &QTimer::timeout, [&](){
        tEnd2 = steady_clock::now();
    });

    const int TOLERANCE = 50;

    timer1.setSingleShot(false);
    timer1.start(200);
    timer2.setSingleShot(true);
    timer2.start(300);

    app->processEvents(QEventLoop::WaitForMoreEvents);
    {
        auto timerDuration = tEnd1 - tBegin;
        qDebug() << "timer1Duration: " << duration_cast<milliseconds>(timerDuration).count();
        QVERIFY(timerDuration > milliseconds(200-TOLERANCE));
        QVERIFY(timerDuration < milliseconds(200+TOLERANCE));
    }
    app->processEvents(QEventLoop::WaitForMoreEvents);
    {
        auto timerDuration = tEnd2 - tBegin;
        qDebug() << "timer2Duration: " << duration_cast<milliseconds>(timerDuration).count();
        QVERIFY(timerDuration > milliseconds(300-TOLERANCE));
        QVERIFY(timerDuration < milliseconds(300+TOLERANCE));
    }
    timer1.start(200);
    app->processEvents(QEventLoop::WaitForMoreEvents);
    {
        auto timerDuration = tEnd1 - tBegin;
        qDebug() << "timer1Duration: " << duration_cast<milliseconds>(timerDuration).count();
        QVERIFY(timerDuration > milliseconds(500-TOLERANCE));
        QVERIFY(timerDuration < milliseconds(500+TOLERANCE));
    }
    app->processEvents(QEventLoop::WaitForMoreEvents);
    {
        auto timerDuration = tEnd1 - tBegin;
        qDebug() << "timer1Duration: " << duration_cast<milliseconds>(timerDuration).count();
        QVERIFY(timerDuration > milliseconds(700-TOLERANCE));
        QVERIFY(timerDuration < milliseconds(700+TOLERANCE));
    }
    {
        auto timerDuration = tEnd2 - tBegin;
        qDebug() << "timer2Duration: " << duration_cast<milliseconds>(timerDuration).count();
        QVERIFY(timerDuration > milliseconds(300-TOLERANCE));
        QVERIFY(timerDuration < milliseconds(300+TOLERANCE));
    }
}

void TestQAsioEventDispatcher::guiEvents()
{
    MainWindow w;
    qDebug() << "showing main window";
    w.show();
    //TODO: do widgets handle clicks, keypresses and so on properly? Does it receive "paint" signals?
    //app->processEvents(QEventLoop::WaitForMoreEvents);
#ifdef INTERACTIVE_TEST
    app->exec();
#endif
}


void TestQAsioEventDispatcher::cleanupTestCase()
{
    QTimer timer1;
    QObject::connect(&timer1, &QTimer::timeout, [&](){
        app->quit();
    });
    timer1.setSingleShot(true);
    timer1.start(1);
    qDebug("waiting for clean exit");
    app->exec();
    qDebug("destroying QApplication");
    delete app;
    app = 0;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui_MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *)
{
    std::clog<< __PRETTY_FUNCTION__  << std::endl;
}

void MainWindow::mousePressEvent(QMouseEvent *)
{
    std::clog<< __PRETTY_FUNCTION__  << std::endl;
}

void MainWindow::on_pushButton_clicked()
{
    std::clog<< __PRETTY_FUNCTION__  << std::endl;
}


QTEST_APPLESS_MAIN(TestQAsioEventDispatcher)
