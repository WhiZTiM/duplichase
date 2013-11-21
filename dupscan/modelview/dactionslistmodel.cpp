#include "dactionslistmodel.hpp"
#include "dupscan/safe_deletion.hpp"
#include <QStringList>
#include <QVector>
#include <QPair>
#include <QFile>
#include <iostream>

Q_DECLARE_METATYPE(DLS::FileProperty)
Q_DECLARE_METATYPE(DItem)

//#define OLD_MODEL

DActionsListModel::DActionsListModel(QObject *parent) :
    QAbstractListModel(parent)
{
    qRegisterMetaType<DLS::FileProperty>("DLS::FileProperty");
    qRegisterMetaType<DItem> ("DItem");
}

void DActionsListModel::setDuplicates(DLS::DuplicatesContainer container)
{
    duplicates = container;
    freshPreparation = true;
    prepareModel();
}

void DActionsListModel::resetViewItems()
{
    beginResetModel();
    #ifdef OLD_MODEL
    viewIndexes.clear();
    for(int i=0; i < items.size(); i++)
    {
        viewIndexes.push_back( i );
    }
    #endif

    freshPreparation = false;
    prepareModel();
    endResetModel();
}

void DActionsListModel::filterModelByExtension(const QStringList &extensionList)
{
    beginResetModel();
    bool last_iteration_was_group_header = false;
    #ifdef OLD_MODEL
    QList<int> filteredList;
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
                last_iteration_was_group_header = false;
            }
        }
    }

    filteredList.swap( viewIndexes );
    #endif

    QList<iData> filteredViewIt;
    last_iteration_was_group_header = false;
    for(int i = 0; i < viewIt.size(); i++)
    {

        if(viewIt[i].item->isGroupHeader)
        {
            if(last_iteration_was_group_header)
                filteredViewIt.removeLast();
            filteredViewIt.push_back(viewIt[i]);
            last_iteration_was_group_header = true;
        }
        else
        {
            QString extension(QString::fromStdString( viewIt[i].item->property.getFileExtension() ));
            if( extensionList.contains(extension, Qt::CaseInsensitive) )
            {
                filteredViewIt.push_back( viewIt[i] );
                last_iteration_was_group_header = false;
            }
        }
    }

    endResetModel();
}

/// THis function exposes the poor design and loose cohesion of this Software. Runs at O(2n + nlogn) worst case
void DActionsListModel::sortModel(Qt::SortOrder sortOrder)
{
    beginResetModel();
    #ifdef OLD_MODEL
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
    #endif

    QList< QPair<iData, int> > Headers;
    for(int k = 0; k < viewIt.size(); k++)
        if( viewIt[k].item == viewIt[k].header )    //its a header... we can also use //if(k.item->isGroupHeader)
            Headers.push_back( qMakePair(viewIt[k], k) );

    auto ascSort = [](const QPair<iData, int>& lhs, const QPair<iData, int>& rhs)
    {
        return lhs.first.item->header.value() < rhs.first.item->header.value();
    };

    auto dscSort = [](const QPair<iData, int>& lhs, const QPair<iData, int>& rhs)
    {
        return lhs.first.item->header.value() > rhs.first.item->header.value();
    };

    if(sortOrder == Qt::AscendingOrder)
        qSort(Headers.begin(), Headers.end(), ascSort);
    else
        qSort(Headers.begin(), Headers.end(), dscSort);

    QList<iData> sortedViewIt;
    for (int k = 0; k < Headers.size(); k++)
    {
        sortedViewIt.push_back( Headers[k].first );
        int m = Headers[k].second;
        auto iHeader = Headers[k].first.header;
        auto mChild  = viewIt[++m];
        while(mChild.header == iHeader )
        {
            sortedViewIt.push_back( mChild );
            if(++m >= viewIt.size())
                break;
            mChild = viewIt[m];
        }
    }
    sortedViewIt.swap( viewIt );
    endResetModel();
}

int DActionsListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    #ifdef OLD_MODEL
    //if(!parent.isValid())    //toplevel
    //    return duplicates.size();
    return viewIndexes.size();
    #else
    return viewIt.size();
    #endif
}


//Private Functions
void DActionsListModel::prepareModel()
{
    //Assing unique numbers and headers
    beginResetModel();
    auto cmp_ptrVEC_FileProperty =
            [](const DLS::ptrVEC_FileProperty& lhs, const DLS::ptrVEC_FileProperty& rhs) -> bool
    {
        return lhs->back().getSize() > rhs->back().getSize();
    };

    if(freshPreparation)
    {
        vec_duplicates.clear();
        for(auto i : duplicates)
            vec_duplicates.push_back(i);
        std::sort(vec_duplicates.begin(), vec_duplicates.end(), cmp_ptrVEC_FileProperty);
    }

    #ifdef OLD_MODEL
    items.clear();
    viewIndexes.clear();
    #endif

    //!NEWW
    dItems.clear();
    viewIt.clear();
    QLinkedList<DItem>::iterator pLast = dItems.begin();

    int i = 1, t_counter = 0, headerIndex = 0;
    for(auto& item : vec_duplicates)
    {
        DItem itm;
        itm.deletionWeight = 0;
        itm.keepingWeight = 0;
        itm.isGroupHeader = true;
        itm.header.topString("Category <<( " + QString::number(i) + " )>>");
        #ifdef OLD_MODEL
        items.push_back(itm);
        headerIndex = t_counter;
        viewIndexes.push_back( t_counter++ );
        DItem& itmHeader_ref = items.back();    //!KEEP
        #endif

        //!NEWW
        //pLast = dItems.insert(pLast, itm );
        dItems.push_back(itm);  pLast = dItems.end() - 1;
        iData kItem;
        kItem.header = pLast;
        kItem.item = pLast;
        viewIt.push_back( kItem );
        DItem& itmHeader_reference = dItems.back();                 //! REMOVE
        auto itmHeader_iter = pLast;

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
            #ifdef OLD_MODEL
            items.push_back(itm);
            viewIndexes.push_back( t_counter++ );
            #endif

            //!NEWW
            //pLast = dItems.insert(pLast, itm);
            dItems.push_back(itm);  pLast = dItems.end() - 1;
            kItem.item = pLast;
            viewIt.push_back( kItem );

            ++itemCount;
            Set_header_size = true;
        }
        #ifdef OLD_MODEL
        itmHeader_ref.header.itemCount( itemCount );
        #endif

        //!NEWW
        itmHeader_reference.header.itemCount( itemCount );
        std::cerr << "Header Count Original: " << itmHeader_reference.header.itemCount() << std::endl;
        std::cerr << "Header Count Iterator: " << itmHeader_iter->header.itemCount() << std::endl;
        std::cerr << "Header Count LastIter: " << (dItems.end() - 1)->header.itemCount() << std::endl;
        std::cerr << "Back() @: " << &(dItems.back()) << "  Address of pLast @:" << &(*pLast) << std::endl;

        //sets the group size as from the last element in the loop
        if(Set_header_size)
        {
            #ifdef OLD_MODEL
            itmHeader_ref.header.value( items.back().property.getSize() );  //!KEEP
            #endif

            //! NEWW
            itmHeader_reference.header.value( dItems.back().property.getSize() );   //!REMOVE!!
        }

        ++i;
    }
    endResetModel();
}

QVariant DActionsListModel::data(const QModelIndex &index, int role) const
{
    #ifdef OLD_MODEL
    Q_ASSERT_X( index.row() < viewIndexes.size(), "dataRequest", " Out of range index requested! " );
    DItem item = items[ viewIndexes[index.row()] ];
    #else

    //!NEWW
    //int ki = index.row(), kv = viewIt.size();
    Q_ASSERT_X( index.row() < viewIt.size(), "dataRequest", " Out of range index requested! " );
    DItem item = (*viewIt[ index.row() ].item);
    //!<--ENDNEW
    #endif

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
    #ifdef OLD_MODEL
    for(auto const& index : indexes)
    {
        Q_ASSERT_X( index.row() < viewIndexes.size(), "selectionForDeletion", " Out of range index requested! " );
        items[ viewIndexes[ index.row() ] ].isDeleteChecked = true;
        items[ viewIndexes[ index.row() ] ].isKeepChecked = false;
        emit dataChanged(index, index);
    }
    #endif
    for(auto const& index : indexes)
    {
        Q_ASSERT_X( index.row() < viewIt.size(), "selectionForDeletion", " Out of range index requested! " );
        viewIt[ index.row() ].item->isDeleteChecked = true;
        viewIt[ index.row() ].item->isKeepChecked = false;
        emit dataChanged(index, index);
    }
}

void DActionsListModel::selectForKeep(QModelIndexList indexes)
{
    #ifdef OLD_MODEL
    for(auto const& index : indexes)
    {
        Q_ASSERT_X( index.row() < viewIndexes.size(), "selectionForKeep", " Out of range index requested! " );
        items[ viewIndexes[ index.row() ] ].isKeepChecked = true;
        items[ viewIndexes[ index.row() ] ].isDeleteChecked = false;
        emit dataChanged(index, index);
    }
    #endif
    for(auto const& index : indexes)
    {
        Q_ASSERT_X( index.row() < viewIt.size(), "selectionForKeep", " Out of range index requested! " );
        viewIt[ index.row() ].item->isKeepChecked = true;
        viewIt[ index.row() ].item->isDeleteChecked = false;
        emit dataChanged(index, index);
    }
}

void DActionsListModel::deselectForDeletion(QModelIndexList indexes)
{
    #ifdef OLD_MODEL
    for(auto const& index : indexes)
    {
        Q_ASSERT_X( index.row() < viewIndexes.size(), "deselectionForDeletion", " Out of range index requested! " );
        items[ viewIndexes[ index.row() ] ].isDeleteChecked = false;
        emit dataChanged(index, index);
    }
    #endif
    for(auto const& index : indexes)
    {
        Q_ASSERT_X( index.row() < viewIt.size(), "deselectionForDeletion", " Out of range index requested! " );
        viewIt[ index.row() ].item->isDeleteChecked = false;
        emit dataChanged(index, index);
    }
}

void DActionsListModel::deselectForKeep(QModelIndexList indexes)
{
    #ifdef OLD_MODEL
    for(auto const& index : indexes)
    {
        Q_ASSERT_X( index.row() < viewIndexes.size(), "deselectionForKeep", " Out of range index requested! " );
        items[ viewIndexes[ index.row() ] ].isKeepChecked = false;
        emit dataChanged(index, index);
    }
    #endif
    for(auto const& index : indexes)
    {
        Q_ASSERT_X( index.row() < viewIt.size(), "deselectionForKeep", " Out of range index requested! " );
        viewIt[ index.row() ].item->isKeepChecked = false;
        emit dataChanged(index, index);
    }
}

void DActionsListModel::deleteFilesNow(QModelIndexList indexes)
{
    QModelIndex parent;
    qSort(indexes.begin(), indexes.end(),
          [](const QModelIndex& lhs, const QModelIndex& rhs) { return lhs.row() < rhs.row(); });


    #ifdef OLD_MODEL
    int r = 0;
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

        Q_ASSERT_X(k < vIndex, "Deletion: Parent check", "This is a BADDD BUG!!!");
        bool removeLastSeenHeader = (items.at(k).header.itemCount() < 1) ? true : false;

        recommender.setWeights( items.at(vIndex).property.getFilePath() , std::string() );
        QString filePath(QString::fromStdString( items.at(vIndex).property.getFilePath() ));
        items.removeAt( vIndex );
        viewIndexes.removeAt(index);

        QString deletionReply = DeletionAgent::toTrash( filePath );
        if(!deletionReply.isEmpty())
        {
            QString ddkk("Problem With \"" + filePath + "\" " + deletionReply);
            emit logMessage( formartForLog( ddkk ) );
        }
        else
        {
            QString ddkk("The File \"" + filePath + "\" was SUCSESSFULLY deleted");
            emit logMessage( formartForLog( ddkk ) );
        }

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
    #endif

    //!
    int t_count = 0;
    for(auto const& i : indexes)
    {
        const int index = i.row() - (t_count++);
        if(viewIt[index].item == viewIt[index].header)  //if we are dealing with headers
            continue;
        beginRemoveRows( parent, index, index );
        dItems.erase( viewIt[index].item );
        int pkpp = viewIt[index].header->header.itemCount();
        viewIt[index].header->header.itemCount( viewIt.at(index).header->header.itemCount() - 1 );
        bool removeHeader = false;
        int kpp = viewIt[index].header->header.itemCount();
        if(viewIt[index].header->header.itemCount() < 1)
        {
            std::cerr << "As Header:   " << viewIt[index].header->header.topString().toStdString() << std::endl;
            std::cerr << "As Property: " << viewIt[index].header->property.getFilePath() << std::endl;
            dItems.erase( viewIt[index].header );
            removeHeader = true;
        }
        viewIt.removeAt( index );
        endRemoveRows();

        if(removeHeader)
        {
            const int headerIndex = index - 1;
            Q_ASSERT_X(viewIt[headerIndex].item == viewIt[headerIndex].header, "ItemRemmoval",
                       " This is a SERIOUS LOGIC ERROR!");

            beginRemoveRows(parent, headerIndex, headerIndex);
            viewIt.removeAt( headerIndex );
            endRemoveRows();
        }
    }
    //!
}

void DActionsListModel::commitMarkings()
{
    beginResetModel();
    #ifdef OLD_MODEL
    for(auto const& i : items)
    {
        if(i.isDeleteChecked)
            recommender.setWeights(i.property.getFilePath(), std::string());
        else if(i.isKeepChecked)
            recommender.setWeights(std::string(), i.property.getFilePath());
    }
    #endif

    for(auto const& i : dItems)
    {
        if(i.isDeleteChecked)
            recommender.setWeights(i.property.getFilePath(), std::string());
        else if(i.isKeepChecked)
            recommender.setWeights(std::string(), i.property.getFilePath());
    }
    endResetModel();
}

void DActionsListModel::unmarkAllKeeps()
{
    beginResetModel();
    #ifdef OLD_MODEL
    for(auto& k : items)
        k.isKeepChecked = false;
    #endif
    for(auto& k : dItems)
        k.isKeepChecked = false;
    endResetModel();
}

void DActionsListModel::unmarkAllDeletes()
{
    beginResetModel();
    #ifdef OLD_MODEL
    for(auto& k : items)
        k.isDeleteChecked = false;
    #endif

    for(auto& k : dItems)
        k.isDeleteChecked = false;
    endResetModel();
}

void DActionsListModel::unmarkAll()
{
    beginResetModel();
    #ifdef OLD_MODEL
    for(auto& k : items)
    {
        k.isKeepChecked = false;
        k.isDeleteChecked = false;
    }
    #endif
    for(auto& k : dItems)
    {
        k.isKeepChecked = false;
        k.isDeleteChecked = false;
    }
    endResetModel();
}

QString DActionsListModel::formartForLog(const QString &str)
{
    return "<font color='#4389A4'>><b>DupscanGUIModel: -</b></font>" + str;
}
