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
public:
    static void addExtensionCategory(ExtensionCategory extension);
    static bool removeExtensionCategory(ExtensionCategory extension);
    static bool removeExtensionCategory(QString extensionType);
    static QStringList getExtensionList(QString type);
    static QStringList getExtensionTypes();
    static int getExtensionCategoryCount();
};


#endif // DITEMCONTAINER_HPP
