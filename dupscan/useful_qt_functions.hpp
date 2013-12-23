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
#ifndef USEFUL_QT_FUNCTIONS_HPP
#define USEFUL_QT_FUNCTIONS_HPP

#include "dupscan/useful_functions.hpp"
#include <QStringList>

QString returnTimeInString(ulong msec);
QList<QPair<QString, QString> > overlappingList(const QStringList& newList, const QStringList& oldList);

#endif // USEFUL_QT_FUNCTIONS_HPP
