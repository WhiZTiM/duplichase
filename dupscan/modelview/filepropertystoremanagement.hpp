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
