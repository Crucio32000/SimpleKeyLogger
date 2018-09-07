#ifndef LOGGERWORKER_H
#define LOGGERWORKER_H

#include <QDebug>
#include <QObject>
#include <QThread>
#include <QQueue>
#include <QMutexLocker>
#include <QMutex>
#include <QFile>
#include <QTextStream>

typedef struct CharData_t{
    bool pressed;
    QString name;
    int times;
    CharData_t(){
        pressed = false;
        name.clear();
        times = 0;
    }

    bool operator==(const CharData_t& a)
    {
        if((a.name == this->name))
        {
            return true;
        }else{
            return false;
        }
    }
}CharData_t;

class LoggerWorker : public QObject
{
    Q_OBJECT
public:
    explicit LoggerWorker(QObject *parent = nullptr);
    ~LoggerWorker();

    void storeCharData(bool isPressed, QString charName, QString wName);

signals:

public slots:

private:
    QThread* cThread;
    QMutex mutex;
    QFile* fileHandle;
    QString currWindowName;
    QList<CharData_t> charDB;

    void filterWindowName(QString &name);
};

#endif // LOGGERWORKER_H
