#ifndef USEFUL_QT_FUNCTIONS_HPP
#define USEFUL_QT_FUNCTIONS_HPP

#include "dupscan/useful_functions.hpp"
#include <QStringList>
#include <QVector>
#include <QtAlgorithms>

QList<QPair<QString, QString> > overlappingList(const QStringList& newList, const QStringList& oldList);

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

#endif // USEFUL_QT_FUNCTIONS_HPP
