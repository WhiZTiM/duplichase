#include "dactionslistmodel.hpp"
#include <QVector>
#include <QPair>
Q_DECLARE_METATYPE(DLS::FileProperty)
Q_DECLARE_METATYPE(DItem)

DActionsListModel::DActionsListModel(QObject *parent) :
    QAbstractListModel(parent)
{
    qRegisterMetaType<DLS::FileProperty>("DLS::FileProperty");
    qRegisterMetaType<DItem> ("DItem");
}

void DActionsListModel::setDuplicates(DLS::DuplicatesContainer container)
{
    duplicates = container;
    prepareModel();
}

void DActionsListModel::resetViewItems()
{
    beginResetModel();
    viewIndexes.clear();
    for(int i=0; i < items.size(); i++)
        viewIndexes.push_back( i );
    endResetModel();
}

void DActionsListModel::filterModelByExtension(const QStringList &extensionList)
{
    //
    for(int i=0; i < viewIndexes.size(); i++)
    {
        //
    }
}

/// THis function exposes the poor design and loose cohesion of this Software. Runs at O(2n + nlogn) worst case
void DActionsListModel::sortModel(Qt::SortOrder sortOrder)
{
    beginResetModel();

    //Run a linear scan to select the headers in viewItems. also taking note of their positions
    QVector< int > headers;
    for(int i=0; i < viewIndexes.size(); i++)
        if( items[ viewIndexes[i] ].isGroupHeader )
            headers.push_back( viewIndexes[i] );

    auto ascendingSorter = [&]( const int lhs, const int rhs )
    {
        return items[lhs].header.value() < items[rhs].header.value();
    };

    auto descendingSorter = [&]( const int lhs, const int rhs )
    {
        return items[lhs].header.value() > items[rhs].header.value();
    };

    // Sort them out
    if(sortOrder == Qt::AscendingOrder)
        qSort(headers.begin(), headers.end(), ascendingSorter);
    else
        qSort(headers.begin(), headers.end(), descendingSorter);

    //re-populate
    QList<int> sortedItems;
    for(int i = 0; i < headers.size(); i++)
    {
        sortedItems.push_back( headers[i] );
        const int start =  headers[i];
        const int limit = items[ headers[i] ].header.itemCount() + start;
        for(int k = start + 1; k <= limit; k++)
            sortedItems.push_back( k );
    }
    sortedItems.swap( viewIndexes );

    endResetModel();
}

int DActionsListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    //if(!parent.isValid())    //toplevel
    //    return duplicates.size();
    return viewIndexes.size();
}


//Private Functions
void DActionsListModel::prepareModel()
{
    //Assing unique numbers and headers
    beginResetModel();
    items.clear();
    viewIndexes.clear();
    int i = 1, t_counter = 0;
    for(auto& item : duplicates)
    {
        DItem itm;
        itm.deletionWeight = 0;
        itm.keepingWeight = 0;
        itm.isGroupHeader = true;
        itm.header.topString("Category <<( " + QString::number(i) + " )>>");
        items.push_back(itm);
        viewIndexes.push_back( t_counter++ );
        DItem& itmHeader_ref = items.back();    //!KEEP

        // Clear data for reuse in the for loop below
        itm.header.topString("");
        itm.isGroupHeader = false;

        int itemCount = 0;  //count items found in the loop
        bool Set_header_size = false;   //did the loop execute?
        for(auto& property : *item)
        {
            recommender.addPath( property.getFileName() );
            property.setTag(i);
            itm.property = property;
            items.push_back(itm);
            viewIndexes.push_back( t_counter++ );
            ++itemCount;
            Set_header_size = true;
        }
        itmHeader_ref.header.itemCount( itemCount );    //!KEEP

        //sets the group size as from the last element in the loop
        if(Set_header_size)
        {
            itmHeader_ref.header.value( items.back().property.getSize() );  //!KEEP
        }

        ++i;
    }
    endResetModel();
}

QVariant DActionsListModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(role);
    if(index.row() >= viewIndexes.size())
        return QVariant();
    return QVariant::fromValue<DItem>( items[ viewIndexes[index.row()] ] );
}
