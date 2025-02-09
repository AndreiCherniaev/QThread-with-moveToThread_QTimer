## About
Qt example how make multi-thread app with QTimer.
## Compiling
```
git clone https://github.com/AndreiCherniaev/QThread-with-moveToThread_QTimer.git && cd QThread-with-moveToThread_QTimer
mkdir build/
cmake -S src/ -B build/ --fresh # qt-cmake -S src/ -B build/ -DCMAKE_BUILD_TYPE=Release --fresh
cmake --build build/ --parallel
```
## Run
Tested on Ubuntu 24.04.1 LTS, Qt 6.8.0 with Debug and Release profiles. If push "Stop running program." in Qt Creator there is no problem, so please run proj in terminal. Start proj and push Ctrl+C to exit...
```
$ build/QThread_QTimer

Worker::Worker
MainWindow::MainWindow
thread->start()
parents QObject(0x0) MainClass(0x7fffc35c40e0)
Worker::run
thread;askTim;0x7b6260960000
thread;GetTime;0x7b625ca006c0;0
^CWelcome to Signal handled:  2
requestInterruption
emit finished()
threadIsFinished
~MainWindow()
~Worker()
QObject::killTimer: Timers cannot be stopped from another thread
QObject::~QObject: Timers cannot be stopped from another thread
```
## Problem
1) Looks like problem reason is `emit GetTimeAsk()` but this signal is connected to slot using `Qt::QueuedConnection` [connection type](https://doc.qt.io/qt-6/threads-qobject.html#signals-and-slots-across-threads):
"Queued Connection The slot is invoked when control returns to the event loop of the receiver's thread. The slot is executed in the receiver's thread"
```
connect(this, &MainClass::GetTimeAsk, worker.get(), &Worker::GetTime, Qt::QueuedConnection);
```
2) And threads is really different: `emit GetTimeAsk()` located in thread 0x7b6260960000 and slot `GetTime()` is started from 0x7b625ca006c0 thread... Sounds like problem should not be but in log we have
```
QObject::killTimer: Timers cannot be stopped from another thread
QObject::~QObject: Timers cannot be stopped from another thread
```
3) Insted of `Qt::QueuedConnection` I have tried `Qt::AutoConnection`, but problem still the same.  
4) Another idea was that `tim= new QTimer(this)` is started in incorrect thread but I don't think so because in log I see first `thread->start()` and then `Worker::run`.  
5) Anothere idead is to use QScopedPointer: `QScopedPointer<QTimer> tim` instead of `QTimer *tim`, but problem is the same.  

## How to fix
Instead of `QScopedPointer<Worker> worker` use raw pointer `Worker *worker`, see [raw_pointer](https://github.com/AndreiCherniaev/QThread-with-moveToThread_QTimer/tree/raw_pointer) branch.

## See also
<p align="center">
  <img alt="image from article Правильная работа с потоками в Qt" src="https://habrastorage.org/webt/zs/py/t2/zspyt2yti1t8-mr6k708rer0rao.png" width="800">
  <br>
    <em>Qt hierarchy</em>
</p>

Read official [doc](https://doc.qt.io/qt-6/threads-qobject.html). See [article](https://www.toptal.com/qt/qt-multithreading-c-plus-plus).
About SigInt (Ctrl+C) quit see [example](https://github.com/AndreiCherniaev/QCoreApplication_quit_example).
