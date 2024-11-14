#pragma once

#include <QThread>
#include "Worker.h"

class MainClass : public QObject
{
    Q_OBJECT

public:
    //The main point that in Unix we can bind callback to static method only
    //but I want bind callback on non-static method, so I am sing pointer
    //pay attention to Rial *Rial::rialSelf; - this is a part of such hack
    //https://stackoverflow.com/questions/54467652/how-to-set-sa-handlerint-pointer-to-function-which-is-member-of-a-class-i
    static MainClass* rialSelf;
    void handleSignal(const int num);
    static void setSignalHandlerObject(MainClass* newRealSelf) {
        MainClass::rialSelf= newRealSelf;
    }
    static void callSignalHandler(int num){ //num is number of handler, in case of SIGINT (Ctrl+C) it is 2
        rialSelf->handleSignal(num);
    }
    
    explicit MainClass(QObject *parent = 0);
    ~MainClass();

public slots:
    void threadIsFinished();

private:
    QScopedPointer<Worker> worker;  //object, in the methods of which we can write custom algorithms, accessing all the richness of Qt as needed
    QScopedPointer<QThread> thread;

signals:
    void GetTimeAsk();
};
