/*******************************************************************************************
**  (C) Copyright December 2013 by
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
#include "filepropertystoremanagement.hpp"
#include <QStringList>
#include <iostream>
#include <QLocale>

FilePropertyFromDiskStore::FilePropertyFromDiskStore(QString FileName)
    :   file(FileName), lineCount(0)
{
    //
}

DLS::DuplicatesContainer FilePropertyFromDiskStore::getDuplicatesContainer()
{
    return duplicates;
}

bool FilePropertyFromDiskStore::parse()
{
    parserError = parseAndReturnSuccess();
    return parserError.isEmpty();
}

QString FilePropertyFromDiskStore::parseAndReturnSuccess()
{
    if(!file.exists())
    {
        return QString("File Does Not Exist!");
    }
    if(!file.open( QFile::Text | QFile::ReadOnly ))
    {
        return QString("Failed to Open File");
    }

    file.seek(0);
    stream.setDevice(&file);
    QString er = "Parsing Error at line: ";

    QString versionInfo = nextLine();
    if(versionInfo != "VERSION 1")
        return QString("This Version is NOT Compatible!");

    while (true) {
        if(nextLine() != "")
            return err(er);
        if(stream.atEnd())
            return "";

        QPair<bool, FileProperties> section = forradPropertyConstruction();
        if(!section.first)
        {
            return err(er);
        }

        DLS::ptrVEC_FileProperty group(new DLS::VEC_FileProperty( std::move( section.second ) ) );
        duplicates.insert( group );
    }
}

QPair<bool, FilePropertyFromDiskStore::FileProperties> FilePropertyFromDiskStore::forradPropertyConstruction()
{
    QPair<bool, FilePropertyFromDiskStore::FileProperties> rtn;
    rtn.first = false;

    QString line = nextLine();
    QStringList category = line.split(' ');
    if(category.first() != "CATEGORY")
        return rtn;

    QLocale locale;
    bool succeeded = false;

    int categoryID = locale.toInt(category.last(), &succeeded);
    if(!succeeded)
        return rtn;

    int pathCount = locale.toInt(nextLine(), &succeeded);
    if(!succeeded)
        return rtn;

    unsigned long fileSize = locale.toUInt(nextLine(), &succeeded);
    if(!succeeded)
        return rtn;

    QString hashA = nextLine();
    if(hashA.size() != 32)
        return rtn;

    QString hashB = nextLine();
    if(hashB.size() != 32)
        return rtn;

    DLS::FileProperty property;
    property.setSize(fileSize);
    property.setHashA(hashA.toStdString());
    property.setHashB(hashB.toStdString());
    property.setTag( categoryID );

    for(int i=0; i < pathCount; i++)
    {
        QString path = nextLine();
        if(path.isEmpty())
            return rtn;
        property.setFilePath( path.toStdString() );
        rtn.second.push_back( property );
    }

    rtn.first = true;
    return rtn;
}

QString FilePropertyFromDiskStore::nextLine()
{
    if(!stream.atEnd())
        ++lineCount;
    return stream.readLine();
}

QString FilePropertyFromDiskStore::err(QString Error)
{
    return Error + QString::number(lineCount);
}

QString FilePropertyFromDiskStore::error() const
{
    return parserError;
}

/////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
///
///
//////////////////////////////////////////////

FilePropertyToDiskStore::FilePropertyToDiskStore(QString fileName)
    : file(fileName)
{
    // Nothing
}

bool FilePropertyToDiskStore::serializeToDisk(const DLS::DuplicatesContainer &duplicates)
{
    if(not file.open(QFile::Text | QFile::WriteOnly | QFile::Truncate) && (not file.isWritable()))
        return false;

    stream.setDevice(&file);
    stream << "VERSION 1" << endl;
    stream << endl;

    try
    {
        int CategoryID = 1;
        for(auto const& vecPtrs : duplicates)
        {
            QString category( "CATEGORY " + QString::number( CategoryID++ ) );
            stream << category << endl;

            bool first = true;
            for(auto const& fprop : *vecPtrs)
            {
                if(first)
                {
                    stream << vecPtrs->size() << endl;
                    stream << fprop.getSize() << endl;
                    stream << QString::fromStdString( fprop.getHashA() ) << endl;
                    stream << QString::fromStdString( fprop.getHashB() ) << endl;
                    first = false;
                }
                stream << QString::fromStdString( fprop.getFilePath() ) << endl;
            }
            stream << endl;
        }
    }
    catch(std::exception& e)
    {
        std::cerr << "Exception Occurred!" << std::endl;
        return false;
    }

    stream.flush();
    return true;
}

void FilePropertyToDiskStore::setFileName(QString fileName)
{
    file.setFileName(fileName);
}
