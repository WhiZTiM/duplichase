/*******************************************************************************************
**  (C) Copyright September 2013 - October 2013 by
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
#include "useful_qt_functions.hpp"
#include "useful_functions.hpp"
#include <QVector>
#include <QtAlgorithms>
/// Free Function Implementation
QString returnTimeInString(ulong msec)
{
    static const int g = 2;
    static const char f = 'f';
    if(msec < 100)
    {
        return (QString::number(msec) + " milliseconds");
    }
    else if(msec < 1000)
    {
        return (QString::number( 1000.0 / msec, f, g ) + " seconds");
    }
    else if(msec < 60000)
    {
        return (QString::number( msec / 1000.0, f, g ) + " seconds");
    }
    std::div_t divVar = std::div(msec, 60000);
    return (QString::number(divVar.quot) + " minutes, " +
            QString::number(divVar.rem / 1000.0, f, g) + " seconds");
}


QList<QPair<QString, QString> > overlappingList(const QStringList &newList, const QStringList &oldList)
{
    QList<QPair<QString, QString>> rtn;
    for(auto& newIt : newList)
    {
        for(auto& oldIt : oldList)
        {
            if(DLS::isTrackableTo(newIt.toStdString(), oldIt.toStdString()))
                rtn.append(qMakePair(newIt, oldIt));
        }
    }

    return rtn;
}

