#ifndef TESTQASIOEVENTDISPATCHER_H
#define TESTQASIOEVENTDISPATCHER_H

#include <QMainWindow>
#include <QObject>
#include <QPushButton>
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


class Ui_MainWindow
{
public:
    QPushButton *pushButton;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(400, 300);
        pushButton = new QPushButton(MainWindow);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(100, 80, 81, 23));
        retranslateUi(MainWindow);
        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        Q_UNUSED(MainWindow);
        pushButton->setText(QApplication::translate("MainWindow", "PushButton", 0));
    } // retranslateUi
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

private:
    Ui_MainWindow *ui;

protected:
    void mousePressEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *);
};


#endif // TESTQASIOEVENTDISPATCHER_H
