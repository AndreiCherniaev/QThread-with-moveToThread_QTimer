#include <QThread>
#include <QDebug>
#include <QCoreApplication>
#include "Worker.h"

Worker::Worker(QObject *parent) :
    QObject(parent)
{
    qDebug() << "Worker::Worker";
    /*
    By the way, one extremely important thing to note here is that you should NEVER allocate heap objects (using new) in the
    constructor of the QObject class as this allocation is then performed on the main thread and not on the new QThread instance,
    meaning that the newly created object is then owned by the main thread and not the QThread instance. This will make your code
    fail to work. Instead, allocate such resources in the main function slot such as process() in this case as when that is called
    the object will be on the new thread instance and thus it will own the resource. https://wiki.qt.io/QThreads_general_usage
    */
}


void Worker::timer_clb(){
    static uint i;
    qDebug() << "timer_clb" << i++;
}


//The most important method in which the "useful" work of the object will be performed
void Worker::run()
{
    qDebug() << "Worker::run";
    tim= new QTimer(this);
    connect(tim, &QTimer::timeout, this, &Worker::timer_clb, static_cast<Qt::ConnectionType>(Qt::AutoConnection | Qt::UniqueConnection));
    tim->setInterval(400);
    while(1)
    {
        if(QThread::currentThread()->isInterruptionRequested()){
            goto exit_LABEL;
        }

        QCoreApplication::processEvents();//QThread::sleep(1);
    }
exit_LABEL:;
    emit finished();
    qDebug() << "emit finished()";
}


void Worker::GetTime(){
    static uint i;
    qDebug().noquote().nospace() << "thread;GetTime;" << QThread::currentThreadId() << ";" << i++;
    tim->start();
}


Worker::~Worker()
{
    qDebug() << "~Worker()";
}
