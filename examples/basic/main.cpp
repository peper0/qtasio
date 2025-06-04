#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTimer>
#include <QLabel>
#include <QDebug>

#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/system/error_code.hpp>

#include "qtasio/qasioeventdispatcher.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(boost::asio::io_service& io_service, QWidget* parent = nullptr)
        : QMainWindow(parent)
        , io_service_(io_service)
        , timer_(io_service)
    {
        setWindowTitle("QtAsio Basic Example");
        resize(400, 300);
        
        QWidget* centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        
        QVBoxLayout* layout = new QVBoxLayout(centralWidget);
        
        countLabel_ = new QLabel("Counter: 0", this);
        layout->addWidget(countLabel_);
        
        QPushButton* qtTimerButton = new QPushButton("Start Qt Timer", this);
        connect(qtTimerButton, &QPushButton::clicked, this, &MainWindow::startQtTimer);
        layout->addWidget(qtTimerButton);
        
        QPushButton* asioTimerButton = new QPushButton("Start Asio Timer", this);
        connect(asioTimerButton, &QPushButton::clicked, this, &MainWindow::startAsioTimer);
        layout->addWidget(asioTimerButton);
        
        QPushButton* bothButton = new QPushButton("Start Both Timers", this);
        connect(bothButton, &QPushButton::clicked, this, [this]() {
            startQtTimer();
            startAsioTimer();
        });
        layout->addWidget(bothButton);
        
        QPushButton* quitButton = new QPushButton("Quit", this);
        connect(quitButton, &QPushButton::clicked, qApp, &QApplication::quit);
        layout->addWidget(quitButton);
        
        layout->addStretch();
    }
    
private slots:
    void startQtTimer() {
        if (qtTimer_) {
            qtTimer_->stop();
            delete qtTimer_;
        }
        
        qtTimer_ = new QTimer(this);
        qtTimer_->setInterval(1000);
        connect(qtTimer_, &QTimer::timeout, this, &MainWindow::updateQtCounter);
        qtCounter_ = 0;
        qtTimer_->start();
        qDebug() << "Qt timer started";
    }
    
    void updateQtCounter() {
        qtCounter_++;
        updateCounterLabel();
        qDebug() << "Qt counter:" << qtCounter_;
    }
    
private:
    void startAsioTimer() {
        asioCounter_ = 0;
        scheduleAsioTimer();
        qDebug() << "Asio timer started";
    }
    
    void scheduleAsioTimer() {
        timer_.expires_after(std::chrono::seconds(1));
        timer_.async_wait([this](const boost::system::error_code& error) {
            if (!error) {
                asioCounter_++;
                updateCounterLabel();
                qDebug() << "Asio counter:" << asioCounter_;
                scheduleAsioTimer(); // Reschedule the timer
            }
        });
    }
    
    void updateCounterLabel() {
        countLabel_->setText(QString("Qt Counter: %1 | Asio Counter: %2")
                           .arg(qtCounter_)
                           .arg(asioCounter_));
    }
    
    boost::asio::io_service& io_service_;
    boost::asio::steady_timer timer_;
    QTimer* qtTimer_ = nullptr;
    QLabel* countLabel_;
    int qtCounter_ = 0;
    int asioCounter_ = 0;
};

int main(int argc, char *argv[]) {
    // Create the Boost.Asio io_service
    boost::asio::io_service io_service;
    
    // Install our custom event dispatcher that integrates with Boost.Asio
    QApplication::setEventDispatcher(new QAsioEventDispatcher(io_service));
    
    // Create the Qt application
    QApplication app(argc, argv);
    
    // Create and show the main window
    MainWindow mainWindow(io_service);
    mainWindow.show();
    
    // Run a simple asio task when we start
    io_service.post([]() {
        qDebug() << "Hello from Asio! This message is posted from the io_service.";
    });
    
    // Start the Qt event loop, which will also process Asio events
    return app.exec();
}

#include "main.moc"