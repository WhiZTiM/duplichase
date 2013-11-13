#ifndef EXTRAFILEPROPERTYTHREAD_HPP
#define EXTRAFILEPROPERTYTHREAD_HPP

#include <atomic>
#include <QMutex>
#include <QRunnable>
#include <QThreadPool>
#include <string>

inline uint qHash(const std::string &key, uint seed)
{
    Q_UNUSED(seed);
    return std::hash<std::string>()(key);
}

#include <QCache>


class ExtraPropertyHandler : public QObject
{
    Q_OBJECT
public:
    explicit ExtraPropertyHandler(int cacheSize = 3000, QObject* parent = nullptr);
    void setMaxDelayTime(uint value);

    // military terminology for good news... SPARK PLUG RECIEVED!!!
    void recieveSparkPlug(std::string key, QString value) const;
    QString extraProperty(std::string filePath) const;

private:
    void spawnRequest(std::string& filePath) const;
    std::atomic<uint> delayTimeOut;
    mutable QCache<std::string, QString> cache;
    mutable QThreadPool threadPool;
    mutable QMutex mutex;
};

class ExtraPropertyRequestWorker : public QRunnable
{
public:
    ExtraPropertyRequestWorker(const std::string& fileName, const ExtraPropertyHandler* handle);
    void run();
private:
    static void (ExtraPropertyHandler::*Callable)(std::string, QString) const;
    QString fileName;
    std::string stdName;
    const ExtraPropertyHandler* ephandle;
};

#endif // EXTRAFILEPROPERTYTHREAD_HPP
