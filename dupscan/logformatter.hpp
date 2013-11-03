#ifndef LOGFORMATTER_HPP
#define LOGFORMATTER_HPP
#include <QObject>
#include <QString>

enum class LOGType
{
    DLSCoreWarning,
    DLSCoreError,
    DLSCoreInfo,
    Info,
    Error,
    Warning,
    None
};

class LogFormatter : public QObject
{
    Q_OBJECT
public:
    LogFormatter();
signals:
    void logMessage(QString);
public slots:
    void formatAndWriteLogMessage(LOGType logType, QString header, QString body);
};

#endif // LOGFORMATTER_HPP
