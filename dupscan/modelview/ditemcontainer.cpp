#include "ditemcontainer.hpp"

DItemContainer::DItemContainer()
{
}

QHash<QString, ExtensionCategory> ExtensionFilterTypes::filterTypes = QHash<QString, ExtensionCategory>();

void ExtensionFilterTypes::addExtensionCategory(ExtensionCategory extension)
{
    if(extension.type.isEmpty())
        return;
    filterTypes.insert( extension.type, extension );
}

bool ExtensionFilterTypes::removeExtensionCategory(ExtensionCategory extension)
{
    if(extension.type.isEmpty())
        return false;
    return filterTypes.remove( extension.type );
}

bool ExtensionFilterTypes::removeExtensionCategory(QString extensionType)
{
    return filterTypes.remove( extensionType );
}

QStringList ExtensionFilterTypes::getExtensionList(QString type)
{
    return filterTypes.value( type ).extensionList;
}

QStringList ExtensionFilterTypes::getExtensionTypes()
{
    return QStringList( filterTypes.keys() );
}

int ExtensionFilterTypes::getExtensionCategoryCount()
{
    return filterTypes.count();
}
