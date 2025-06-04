#ifndef TESTQASIOEVENTDISPATCHER_H
#define TESTQASIOEVENTDISPATCHER_H

#include <QMainWindow>
#include <QObject>
#include <QPushButton>
#include <QtTest>

#include <boost/asio/io_service.hpp>
#include "qtasio/qasioeventdispatcher.h"

class TestQAsioEventDispatcher : public QObject
{
    Q_OBJECT
public:
    explicit TestQAsioEventDispatcher(QObject *parent = nullptr);

signals:

public slots:

private:
    int argc=0;
    char *argv={nullptr};
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
        pushButton->setText(QApplication::translate("MainWindow", "PushButton", nullptr));
    } // retranslateUi
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

private:
    Ui_MainWindow *ui;

protected:
    void mousePressEvent(QMouseEvent *) override;
    void paintEvent(QPaintEvent *) override;
};


#endif // TESTQASIOEVENTDISPATCHER_H