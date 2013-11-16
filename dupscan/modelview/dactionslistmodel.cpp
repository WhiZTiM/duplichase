#include "dactionslistmodel.hpp"
#include <QStringList>
#include <QVector>
#include <QPair>
#include <QFile>
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
    {
        viewIndexes.push_back( i );
    }
    endResetModel();
}

void DActionsListModel::filterModelByExtension(const QStringList &extensionList)
{
    QList<int> filteredList;
    bool last_iteration_was_group_header = false;
    for(int i=0; i < viewIndexes.size(); i++)
    {
        if(items[ viewIndexes[i] ].isGroupHeader)
        {
            if(last_iteration_was_group_header)
                filteredList.removeLast();
            filteredList.push_back( viewIndexes[i] );
            last_iteration_was_group_header = true;
        }
        else
        {
            QString extension(QString::fromStdString( items[viewIndexes[i]].property.getFileExtension() ));
            if( extensionList.contains( extension, Qt::CaseInsensitive) )
            {
                filteredList.append( viewIndexes[i] );
            }
            last_iteration_was_group_header = false;
        }
    }

    filteredList.swap( viewIndexes );
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
        {
            items[ k ].parentIndex = headers[i];
            sortedItems.push_back( k );
        }
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

    std::vector<DLS::ptrVEC_FileProperty> vec_duplicates;
    auto cmp_ptrVEC_FileProperty =
            [](const DLS::ptrVEC_FileProperty& lhs, const DLS::ptrVEC_FileProperty& rhs) -> bool
    {
        return lhs->back().getSize() > rhs->back().getSize();
    };

    for(auto i : duplicates)
        vec_duplicates.push_back(i);
    std::sort(vec_duplicates.begin(), vec_duplicates.end(), cmp_ptrVEC_FileProperty);

    items.clear();
    viewIndexes.clear();
    int i = 1, t_counter = 0, headerIndex = 0;
    for(auto& item : vec_duplicates)
    {
        DItem itm;
        itm.deletionWeight = 0;
        itm.keepingWeight = 0;
        itm.isGroupHeader = true;
        itm.header.topString("Category <<( " + QString::number(i) + " )>>");
        items.push_back(itm);

        headerIndex = t_counter;
        viewIndexes.push_back( t_counter++ );
        DItem& itmHeader_ref = items.back();    //!KEEP

        // Clear data for reuse in the for loop below
        itm.header.topString("");
        itm.isGroupHeader = false;

        int itemCount = 0;  //count items found in the loop
        bool Set_header_size = false;   //did the loop execute?
        for(auto& property : *item)
        {
            recommender.addPath( property.getFilePath() );
            property.setTag(i);
            itm.property = property;
            itm.parentIndex = headerIndex;
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
    //if(index.row() >= viewIndexes.size())
    //    return QVariant();        --- - - > REPLACED WITH AN ASSERTION
    Q_ASSERT_X( index.row() < viewIndexes.size(), "dataRequest", " Out of range index requested! " );
    DItem item = items[ viewIndexes[index.row()] ];

    if(role == Qt::ToolTipRole)
    {
        if(item.isGroupHeader)
            return QVariant(QString());
        return QVariant( extraPropertyHandle.extraProperty( item.property.getFilePath() ) );
    }

    item.keepingWeight = recommender.getKeepingWeight( item.property.getFilePath() );
    item.deletionWeight = recommender.getDeletionWeight( item.property.getFilePath() );
    return QVariant::fromValue<DItem>( item );
}

void DActionsListModel::selectForDeletion(QModelIndexList indexes)
{
    for(auto const& index : indexes)
    {
        Q_ASSERT_X( index.row() < viewIndexes.size(), "selectionForDeletion", " Out of range index requested! " );
        items[ viewIndexes[ index.row() ] ].isDeleteChecked = true;
        items[ viewIndexes[ index.row() ] ].isKeepChecked = false;
        emit dataChanged(index, index);
    }
}

void DActionsListModel::selectForKeep(QModelIndexList indexes)
{
    for(auto const& index : indexes)
    {
        Q_ASSERT_X( index.row() < viewIndexes.size(), "selectionForKeep", " Out of range index requested! " );
        items[ viewIndexes[ index.row() ] ].isKeepChecked = true;
        items[ viewIndexes[ index.row() ] ].isDeleteChecked = false;
        emit dataChanged(index, index);
    }
}

void DActionsListModel::deselectForDeletion(QModelIndexList indexes)
{
    for(auto const& index : indexes)
    {
        Q_ASSERT_X( index.row() < viewIndexes.size(), "deselectionForDeletion", " Out of range index requested! " );
        items[ viewIndexes[ index.row() ] ].isDeleteChecked = false;
        emit dataChanged(index, index);
    }
}

void DActionsListModel::deselectForKeep(QModelIndexList indexes)
{
    for(auto const& index : indexes)
    {
        Q_ASSERT_X( index.row() < viewIndexes.size(), "deselectionForKeep", " Out of range index requested! " );
        items[ viewIndexes[ index.row() ] ].isKeepChecked = false;
        emit dataChanged(index, index);
    }
}

void DActionsListModel::deleteFilesNow(QModelIndexList indexes)
{
    QModelIndex parent;
    int r = 0;

    qSort(indexes.begin(), indexes.end(),
          [](const QModelIndex& lhs, const QModelIndex& rhs) { return lhs.row() < rhs.row(); });

    for(auto const& i : indexes)
    {
        int index = i.row() - r;
        const int vIndex = viewIndexes[ index ];
        Q_ASSERT_X( index < viewIndexes.size(), "Deletion", " Out of range index requested! " );
        if(items[ vIndex ].isGroupHeader)
            continue;
        ++r;

        beginRemoveRows(parent, index, index);
        const int k = items.at( vIndex ).parentIndex;
        items[k].header.itemCount( items.at(k).header.itemCount() - 1 );

        Q_ASSERT_X(k < index, "Deletion: Parent check", "This is a BUG!!!");
        Q_ASSERT_X(k < vIndex, "Deletion: Parent check", "This is a BADDD BUG!!!");
        bool removeLastSeenHeader = (items.at(k).header.itemCount() < 1) ? true : false;

        recommender.setWeights( items.at(vIndex).property.getFilePath() , std::string() );
        items.removeAt( vIndex );
        viewIndexes.removeAt(index);

        for(int lp = 0; lp < viewIndexes.size(); lp++)
        {
            if(viewIndexes[lp] > vIndex)
                viewIndexes[lp] = viewIndexes[lp] - 1;
        }
        for (int lc = 0; lc < items.size(); ++lc)
            if(items.at(lc).parentIndex > vIndex)
                --(items[lc].parentIndex);

        endRemoveRows();

        if(removeLastSeenHeader)
        {
            beginRemoveRows(parent, k, k);
            const int kIndex = viewIndexes[ k ];
            items.removeAt( kIndex );
            viewIndexes.removeAt(k);
            for(int lp = 0; lp < viewIndexes.size(); lp++)
            {
                if(viewIndexes[lp] > kIndex)
                    viewIndexes[lp] = viewIndexes[lp] - 1;
            }

            for (int lc = 0; lc < items.size(); ++lc)
                if(items.at(lc).parentIndex > k)
                    --(items[lc].parentIndex);

            ++r;
            endRemoveRows();
        }
    }
}

void DActionsListModel::commitMarkings()
{
    for(auto const& i : items)
    {
        if(i.isDeleteChecked)
            recommender.setWeights(i.property.getFilePath(), std::string());
        else if(i.isKeepChecked)
            recommender.setWeights(std::string(), i.property.getFilePath());
    }
}
