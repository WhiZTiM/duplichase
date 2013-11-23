#include "extrafileproperty.hpp"
#include <mutex>
#include <QFileInfo>
#include <QDateTime>
/*
ExtraFilePropertyThread::ExtraFilePropertyThread(QObject *parent) :
    QThread(parent)
{
}
*/

ExtraPropertyHandler::ExtraPropertyHandler(int cacheSize, QObject *parent)
    : QObject(parent), delayTimeOut(10), cache(cacheSize)
{
    int maxCount = QThread::idealThreadCount() < 0 ? 4 : (QThread::idealThreadCount() + 3);
    threadPool.setMaxThreadCount( maxCount );
    threadPool.setExpiryTimeout(90000);
}

void ExtraPropertyHandler::setMaxDelayTime(uint value)
{
    delayTimeOut.store( value, std::memory_order_relaxed );
}

QString ExtraPropertyHandler::extraProperty(std::string filePath) const
{
    if(!mutex.tryLock( delayTimeOut.load(std::memory_order_relaxed) ))  //seqential ordering not needed here
        return QString();
    std::lock_guard<QMutex> lock(mutex, std::adopt_lock);
    if(cache.contains(filePath))
        return QString(*cache.object(filePath));

    spawnRequest(filePath);
    return QString();
}

void ExtraPropertyHandler::spawnRequest(std::string& filePath) const
{
    ExtraPropertyRequestWorker* worker =
            new ExtraPropertyRequestWorker(filePath, this);
    threadPool.start(worker);
}

void ExtraPropertyHandler::recieveSparkPlug(std::string key, QString value) const
{
    QMutexLocker lock(&mutex);
    cache.insert(key, new QString(value));
}

//! <><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
//! 1 JOHN 5:4 "....And this is The Victory that overcometh the world! even our Faith!!"
//! <><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

void (ExtraPropertyHandler::*ExtraPropertyRequestWorker::Callable)(std::string, QString) const
 = &ExtraPropertyHandler::recieveSparkPlug;

ExtraPropertyRequestWorker::ExtraPropertyRequestWorker(const std::string &fileName, const ExtraPropertyHandler *handle)
    : fileName(QString::fromStdString(fileName)), stdName(fileName), ephandle(handle)
{
    //
}

void ExtraPropertyRequestWorker::run()
{
    QFileInfo fileInfo( fileName );
    QString output("<qt>" + fileName + "<br />");
    if(fileInfo.exists())
    {
        output.append("<b>ATTRIBUTES</b><br />");
        output.append("<i><b>Date Created:  </b>");
        output.append( fileInfo.created().toString("h:m:s ap d MMMM, yyyy") );
        output.append("<br /><b>Last Modified: </b>");
        output.append( fileInfo.lastModified().toString("h:m:s ap d MMMM, yyyy") );
        output.append("<br /><b>Last Read:     </b>");
        output.append( fileInfo.lastRead().toString("h:m:s ap d MMMM, yyyy") );
        output.append("<br /><b>File Type:     </b>");
        output.append( (fileInfo.isExecutable() ? "executable" : "data") );
        output.append( "<br /></i></qt>" );
    }
    else
        output.append("<b><i>Details NOT available</i></b></qt>");

    (ephandle->*Callable)(stdName, output);
}




