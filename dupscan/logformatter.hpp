/*******************************************************************************************
**  (C) Copyright October 2013 by
**  @author: Ibrahim Timothy Onogu {WhiZTiM}
**  @email: <ionogu@acm.org>
**
**	Provided this copyright notice appears on all derived works;
**  Use, modification and distribution are subject to the Boost Software License,
**  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
**  http://www.boost.org/LICENSE_1_0.txt).
**
**  Project DupLichaSe...2013
**  See http://github.com/WhiZTiM/duplichase for most recent version including documentation.
********************************************************************************************/
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
private:
    QString lastMessage;
};

#endif // LOGFORMATTER_HPP
