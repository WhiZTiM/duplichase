#ifndef USEFUL_QT_FUNCTIONS_HPP
#define USEFUL_QT_FUNCTIONS_HPP

#include "dupscan/useful_functions.hpp"
#include <QStringList>

QString returnTimeInString(ulong msec);
QList<QPair<QString, QString> > overlappingList(const QStringList& newList, const QStringList& oldList);

#endif // USEFUL_QT_FUNCTIONS_HPP
