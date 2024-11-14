#pragma once

#include <QObject>
#include <QTimer>

class Worker : public QObject
{
    Q_OBJECT

    QTimer *tim;
public:
    explicit Worker(QObject *parent = 0);
    ~Worker();

signals:
    void finished(); //signal by which we will terminate the thread after the run() method has completed
    void messageChanged(QString message);
    void sendMessage(QStringView message);
private slots:
    void timer_clb();
public slots:
    void run(); //method with custom algorithms that can be implemented in this method inside while(1)
    void GetTime();
};
