#include "loggerworker.h"

#include <QApplication>

LoggerWorker::LoggerWorker(QObject *parent) : QObject(parent)
{
    charDB.clear();
    currWindowName.clear();

    fileHandle = new QFile(this);
    fileHandle->setFileName("./log.txt");
    if(fileHandle->open(QIODevice::WriteOnly |  QIODevice::Text))
    {
        qDebug() << "LOG location " << qApp->applicationDirPath();
    }

    // Move to a different thread
    cThread = new QThread(parent);
    this->moveToThread(cThread);
    cThread->start();

}

LoggerWorker::~LoggerWorker()
{
    // Close filehandle
    if(fileHandle == nullptr)
    {

    }
    else if(fileHandle->isOpen())
    {
        fileHandle->close();
    }
}

void LoggerWorker::storeCharData(bool isPressed, QString charName, QString wName)
{
    CharData_t cData;
    cData.pressed = isPressed;
    cData.name = charName;
    // MutexLock of our DB
    QMutexLocker locker(&mutex);
    // Check if we already have data regarding the current pressed char
    int idx = charDB.indexOf(cData, 0);
    if(idx >= 0)
    {
        CharData_t ptr = charDB.at(idx);
        ptr.times++;
        charDB.replace(idx, ptr);
        if(cData.pressed)
        {
            qDebug() << " Char " << ptr.name << " PRESSED event for " << ptr.times;
        }else{
            qDebug() << " Char " << ptr.name << " pressed event for " << ptr.times << " RELEASED";
            // Store
            QTextStream out(fileHandle);
            filterWindowName(wName);
            if(wName != currWindowName)
            {
                out << " ### " << wName << " ###\n";
                currWindowName = wName;
            }
            out << "\t" << ptr.name << "-" << ptr.times <<"\n";
            //Remove item
            charDB.removeAt(idx);
        }
    }else{
        qDebug() << " Char " << charName << " PRESSED!";
        charDB.append(cData);
    }
}

void LoggerWorker::filterWindowName(QString& name)
{
    Q_UNUSED(name);
    /*
    QStringList sList;
    sList = name.split("-");
    if(sList.size() > 1)
    {
        name = sList.at(1);
        for(int i=1; i<sList.size(); i++)
        {
            name += "-"+sList.at(i);
        }
    }
    */
}
