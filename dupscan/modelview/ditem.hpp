#ifndef DITEM_HPP
#define DITEM_HPP
#include "backend/include/fileproperty.hpp"
#include <QString>
#include <QHash>

class GroupHeader
{
public:
    GroupHeader() : _itemCount(0), _value(0) {}
    inline void topString(const QString& strinG ) { _topString = strinG; }
    inline void auxiliary(const QString& strinG ) { _auxiliary = strinG; }
    inline void value(unsigned long Val) { _value = Val; }
    inline void itemCount(int c) { _itemCount = c; }
    inline QString topString() const { return _topString; }
    inline QString auxiliary() const { return _auxiliary; }
    inline unsigned long value() const { return _value; }
    inline int itemCount() const { return _itemCount; }
    inline QString uniqueName() const { return _topString + _auxiliary + QString::number(_itemCount); }
private:
    QString _topString;
    QString _auxiliary;
    int _itemCount;
    unsigned long _value;
};

struct DItem
{
    bool isGroupHeader = false;
    bool isDeleteChecked = false;
    bool isKeepChecked = false;
    DLS::FileProperty property;
    GroupHeader header;
    long long deletionWeight;
    long long keepingWeight;
    int parentIndex = -1;
};

/*
struct DItemSelection
{
    bool toDelete = false;
    bool toKeep = false;
};

struct ExtraProperty
{
    //QString
};
*/

inline bool operator == (const DItem& lhs, const DItem& rhs)
{
    if(lhs.isGroupHeader && !rhs.isGroupHeader)
        return false;
    if(!lhs.isGroupHeader && rhs.isGroupHeader)
        return false;
    if(lhs.isGroupHeader)
        return lhs.header.uniqueName() == rhs.header.uniqueName();
    return lhs.property.getFileName() == rhs.property.getFileName();
}

inline uint qHash(const DItem& item)
{
    return qHash( item.header.uniqueName() + QString::fromStdString(item.property.getFileName()) );
}


#endif // DITEM_HPP
