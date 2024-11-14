#include "MainClass.h"
#include <QCoreApplication>

MainClass *MainClass::rialSelf;

MainClass::MainClass(QObject *parent) :
    QObject(parent)
  , worker(new Worker()) //Note: "The object cannot be moved if it has a parent."
  , thread(new QThread(this)) //parent is presented (not required)
{
    qDebug() << "MainWindow::MainWindow";
    MainClass::setSignalHandlerObject(this);

    worker->moveToThread(thread.get()); //pass the worker object (must not have a parent!) to the thread
    //Worker's run() method starts by thread's signal started()
    connect(thread.get(), &QThread::started, worker, &Worker::run);
    //When worker emit signal "finished" then thread quit
    connect(worker, &Worker::finished, thread.get(), &QThread::quit);
    connect(worker, &Worker::finished, worker, &Worker::deleteLater);
    //connect(thread, &QThread::finished, thread, &QThread::deleteLater); //no need because of QScopedPointer

    //The main thread analyzes what is happening with the others in threadIsFinished()
    connect(thread.get(), &QThread::finished, this, &MainClass::threadIsFinished);

    qDebug() << "thread->start()";
    thread->start();

    qDebug() << "parents" << worker->parent() << thread->parent();

    connect(this, &MainClass::GetTimeAsk, worker, &Worker::GetTime, Qt::QueuedConnection);
    QTimer *askTim(new QTimer(this));
    connect(askTim, &QTimer::timeout, this, [&]{
        qDebug().noquote().nospace() << "thread;askTim;" << QThread::currentThreadId();
        emit GetTimeAsk();
    });
    askTim->start(400);
}


MainClass::~MainClass()
{
    qDebug() << "~MainWindow()";
}


//handler of Ctrl+C case
void MainClass::handleSignal(const int num){
    qDebug()<<"Welcome to Signal handled: " << num;
    //Stopping threads by completing the run method in the worker object
    thread->requestInterruption(); //worker->setRunning(false);
    qDebug()<<"requestInterruption";
}


//is called when thread finish
void MainClass::threadIsFinished()
{
    //If you have multimp threads you can analizy which finish
    if(thread.get()->isFinished()){
        qDebug() << "threadIsFinished";
    }
    QCoreApplication::quit();
}
