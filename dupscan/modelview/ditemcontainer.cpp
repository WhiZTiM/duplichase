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
#include "ditemcontainer.hpp"

DItemContainer::DItemContainer()
{
}

QHash<QString, ExtensionCategory> ExtensionFilterTypes::filterTypes = QHash<QString, ExtensionCategory>();
ExtensionFilterTypes::StaticDefaultConstructor ExtensionFilterTypes::defaultConstructedFilterTypes;

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

ExtensionFilterTypes::StaticDefaultConstructor::StaticDefaultConstructor()
{
    ExtensionCategory c1;
    c1.type = "video";
    c1.extensionList << "3g2" << "3pg" << "asf" << "asx" << "avi" << "flv" <<
                        "m4v" << "mp4" << "mov" << "mkv" << "mpg" << "vob" <<
                        "xvid" << "wmv";
    ExtensionFilterTypes::addExtensionCategory(c1);

    c1.type = "audio";
    c1.extensionList << "aif" << "pls" << "if" << "m3u" << "m4a" << "mid" <<
                        "ogg" << "mpa" << "ra" << "wav" << "wma";
    ExtensionFilterTypes::addExtensionCategory(c1);

    c1.extensionList.clear();
    c1.type = "document";
    c1.extensionList << "doc" << "docx" << "log" << "msg" << "odt" << "pages"
                     << "rtf" << "txt" << "tex" << "wpd" << "wps" << "pdf" << "epub"
                     << "csv" << "dat" << "gbr" << "ged" << "ibooks" << "key"
                     << "keychain" << "pps" << "ppt" << "pptx" << "sdf" << "tax2013"
                     << "vcf" << "xls" << "xml" << "xlr" << "xlsx"
                     << "3dm" << "3ds" << "max" << "collada" << "blend" << "obj";
    ExtensionFilterTypes::addExtensionCategory(c1);

    c1.extensionList.clear();
    c1.type = "image";
    c1.extensionList << "img" << "imgs" << "bmp" << "dds" << "gif" << "jpg" << "png"
                     << "psd" << "pspimage" << "tga" << "thm" << "tif" << "tiff"
                     << "yuv" << "wmf" << "ai" << "svg" << "eps" << "ps" << "jpeg" << "xcf";
    ExtensionFilterTypes::addExtensionCategory(c1);

    c1.extensionList.clear();
    c1.type = "executable";
    c1.extensionList << "exe" << "bat" << "sh" << "py" << "com" << "jar" << "wsf";
    ExtensionFilterTypes::addExtensionCategory(c1);

    c1.extensionList.clear();
    c1.type = "custom";
    ExtensionFilterTypes::addExtensionCategory(c1);
}
