/*******************************************************************************************
**  (C) Copyright November 2013 by
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
#ifndef DITEMCONTAINER_HPP
#define DITEMCONTAINER_HPP
#include <array>
#include <QHash>
#include <QString>
#include <QStringList>
#include "ditem.hpp"

class DItemContainer
{
public:
    DItemContainer();

private:
    std::array<DItem, 9> kt;
};

class ExtensionCategory
{
public:
    QStringList extensionList;
    QString type;
};

class ExtensionFilterTypes
{
    static QHash<QString, ExtensionCategory> filterTypes;

    //friend class StaticDefaultsConstructor;
    struct StaticDefaultConstructor
    {
        StaticDefaultConstructor();
    };
    static StaticDefaultConstructor defaultConstructedFilterTypes;

public:
    static void addExtensionCategory(ExtensionCategory extension);
    static bool removeExtensionCategory(ExtensionCategory extension);
    static bool removeExtensionCategory(QString extensionType);
    static QStringList getExtensionList(QString type);
    static QStringList getExtensionTypes();
    static int getExtensionCategoryCount();
};


#endif // DITEMCONTAINER_HPP
