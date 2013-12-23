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
#ifndef FILEPROPERTYPARSER_HPP
#define FILEPROPERTYPARSER_HPP
#include <QFile>
#include <QList>
#include <QTextStream>
#include "backend/include/fileproperty.hpp"
#include "backend/include/container_helpers/fp_holders.hpp"

class FilePropertyFromDiskStore
{
public:
    typedef DLS::VEC_FileProperty FileProperties;
    FilePropertyFromDiskStore(QString FileName);
    bool parse();
    QString error() const;
    DLS::DuplicatesContainer getDuplicatesContainer();

private:
    DLS::DuplicatesContainer duplicates;
    QTextStream stream;
    QString parserError;
    QFile file;
    int lineCount;

    QString parseAndReturnSuccess();
    QPair<bool, FileProperties> forradPropertyConstruction();
    QString err(QString Error);
    QString nextLine();
};

class FilePropertyToDiskStore
{
public:
    FilePropertyToDiskStore(QString fileName);
    bool serializeToDisk(const DLS::DuplicatesContainer &duplicates);
    void setFileName(QString fileName);

private:
    QFile file;
    QTextStream stream;
    bool success = false;
};

#endif // FILEPROPERTYPARSER_HPP
