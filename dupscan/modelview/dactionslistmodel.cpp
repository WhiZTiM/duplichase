#include "dactionslistmodel.hpp"
#include "dupscan/safe_deletion.hpp"
#include <QtConcurrent/QtConcurrent>
#include <QStringList>
#include <QVector>
#include <QPair>
#include <QFile>
#include <iostream>
#include <array>
#include <QMutexLocker>

Q_DECLARE_METATYPE(DLS::FileProperty)
Q_DECLARE_METATYPE(DItem)

//#define OLD_MODEL

DActionsListModel::DActionsListModel(QObject *parent) :
    QAbstractListModel(parent), AnyProcessRunning(false)
{
    qRegisterMetaType<DLS::FileProperty>("DLS::FileProperty");
    qRegisterMetaType<DItem> ("DItem");

    connect(this, SIGNAL(bestIndex(int)), this, SLOT(bestIndexScrollTo(int)));
}

void DActionsListModel::setDuplicates(DLS::DuplicatesContainer container)
{
    duplicates = container;
    freshPreparation = true;
    prepareModel();
    emit modelWasJustSet();
}

void DActionsListModel::resetViewItems()
{
    #ifdef OLD_MODEL
    viewIndexes.clear();
    for(int i=0; i < items.size(); i++)
    {
        viewIndexes.push_back( i );
    }
    #endif

    //freshPreparation = false;
    //prepareModel();

    beginResetModel();

    viewIt.clear();
    QLinkedList<DItem>::iterator pIterHeader = dItems.begin();
    for(QLinkedList<DItem>::iterator iter = dItems.begin(); iter != dItems.end(); iter++)
    {
        if(iter->isGroupHeader)
            pIterHeader = iter;

        iData kItem;
        kItem.header = pIterHeader;
        kItem.item = iter;
        viewIt.push_back( kItem );
    }

    endResetModel();

}

void DActionsListModel::filterModelByExtension(QStringList extensionList)
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
    filteredViewIt.swap( viewIt );
    endResetModel();
}

void DActionsListModel::filterModelBySize(ulong min, ulong max)
{
    Q_ASSERT_X(false, "Model Routine", "Incomplete Implementation, rework here!");
    beginResetModel();
    QList<int> removalIndexes;
    for(int i = 0; i < viewIt.size(); i++)
    {
        if(viewIt[i].item == viewIt[i].header)      //This is a header
            continue;
        const unsigned long fileSize = viewIt[i].item->property.getSize();
        if(min > fileSize || fileSize > max)
            removalIndexes.append( i );
    }
    int t_counter = 0;

    qSort(removalIndexes);
    for(auto const& i : removalIndexes)
        viewIt.removeAt( (i - (t_counter++)) );

    endResetModel();
}

void DActionsListModel::filterModelByPath(QString parentPath)
{
    beginResetModel();
    QList<int> removalIndexes;
    for(int i = 0; i < viewIt.size(); i++)
    {
        if(viewIt[i].item == viewIt[i].header)      //This is a header
            continue;
        const QString filePath( QString::fromStdString( viewIt[i].item->property.getFilePath() ) );
        if(filePath.contains( parentPath, Qt::CaseInsensitive ))
            removalIndexes.append( i );
    }

    int t_counter = 0;
    qSort(removalIndexes);
    for(auto const& i : removalIndexes)
    {
        const int k = i - (t_counter++);
        viewIt[k].header->header.itemCount( viewIt.at(k).header->header.itemCount() - 1 );
        viewIt.removeAt( k );
    }

    endResetModel();
}

/// THis function exposes the poor design and loose cohesion of this Software. Runs at O(2n + nlogn) worst case
/// EDIT: FIXED! Now nuns at (Sorting cost + copying cost) --> (nlogn + n) --> nlogn;
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

    int i = 1, /*t_counter = 0,*/ headerIndex = 0;
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
        //auto itmHeader_iter = pLast;

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
        //std::cerr << "Header Count Original: " << itmHeader_reference.header.itemCount() << std::endl;
        //std::cerr << "Header Count Iterator: " << itmHeader_iter->header.itemCount() << std::endl;
        //std::cerr << "Header Count LastIter: " << (dItems.end() - 1)->header.itemCount() << std::endl;
        //std::cerr << "Back() @: " << &(dItems.back()) << "  Address of pLast @:" << &(*pLast) << std::endl;

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

void DActionsListModel::removeIndexes(QModelIndexList indexes)
{
    p_deleteFilesNow(indexes, false, false);
}

void DActionsListModel::removeIndexesSession(QModelIndexList indexes)
{
    p_deleteFilesNow(indexes, true, false);
}

void DActionsListModel::deleteFilesNow(QModelIndexList indexes)
{
    p_deleteFilesNow(indexes, true, true);
}

void DActionsListModel::p_deleteFilesNow(QModelIndexList indexes, bool RemoveFromModel, bool MoveToTrash)
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
        const int index = i.row() - t_count;
        if(viewIt[index].item == viewIt[index].header)  //if we are dealing with headers
            continue;
        ++t_count;

        beginRemoveRows( parent, index, index );
        const QString filePath(QString::fromStdString( viewIt[index].item->property.getFilePath() ));

        if(RemoveFromModel)
            dItems.erase( viewIt[index].item );

        //int pkpp = viewIt[index].header->header.itemCount();
        viewIt[index].header->header.itemCount( viewIt.at(index).header->header.itemCount() - 1 );
        bool removeHeader = false;
        //int kpp = viewIt[index].header->header.itemCount();
        if(viewIt[index].header->header.itemCount() < 1)
        {
            if(RemoveFromModel)
                dItems.erase( viewIt[index].header );
            removeHeader = true;
        }
        viewIt.removeAt( index );
        endRemoveRows();

        if( RemoveFromModel && MoveToTrash )
        {
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
        }

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

void DActionsListModel::autoSelectNextKeeps()
{
    if(AnyProcessRunning.load())
    {
        emit statusBarMessage(QString("Please Be Patient With the Current Operation"));
        return;
    }
    if(!w_selectionFuture.isRunning())
        w_selectionFuture = QtConcurrent::run(this, &DActionsListModel::w_autoSelectNextKeeps);
}

void DActionsListModel::autoSelectNextDeletes()
{
    if(AnyProcessRunning.load())
    {
        emit statusBarMessage(QString("Please Be Patient With the Current Operation"));
        return;
    }
    if(!w_selectionFuture.isRunning())
        w_selectionFuture = QtConcurrent::run(this, &DActionsListModel::w_autoSelectNextDeletes);
}

void DActionsListModel::autoSelectNextPossibilities()
{
    if(AnyProcessRunning.load())
    {
        emit statusBarMessage(QString("Please Be Patient With the Current Operation"));
        return;
    }
    if(!w_selectionFuture.isRunning())
        w_selectionFuture = QtConcurrent::run(this, &DActionsListModel::w_autoSelectNextPossibilities);
}

void DActionsListModel::w_autoSelectNextKeeps()
{
    w_autoSelectNext(true);
}

void DActionsListModel::w_autoSelectNextDeletes()
{
    w_autoSelectNext(false);
}

void DActionsListModel::w_autoSelectNextPossibilities()
{
    //---> Some cranky stuff here...    ||| NOT YET AVAILABLE |||
    //====>>> To be deployed hopefully in versions greater than 0.8.0 <><><><><><><><><><><>
}

void DActionsListModel::w_autoSelectNext(bool keepers)
{
    QMutexLocker lock(&dataMutex);
    emit workerProcessStarted();
    AnyProcessRunning.store(true);

    QPair<int, long long> best;
    best.first = 0;
    best.second = 0;
    const int total = viewIt.size();
    int counter = -1;
    int quarters = total / 10;
    for(auto const& k : viewIt)
    {
        ++counter;
        if(k.item->isGroupHeader)
            continue;
        long long value;

        if(keepers)
            value = recommender.getKeepingWeight( k.item->property.getFilePath() );
        else
            value = recommender.getDeletionWeight( k.item->property.getFilePath() );

        if(best.second <= value)
        {
            best.second = value;
            best.first = counter;
        }
        if(counter >= quarters)
        {
            emit workerProcessProgressPercentage( static_cast<int>( total / quarters ) );
            quarters += 10;
        }
    }
    emit bestIndex(best.first);
    AnyProcessRunning.store(false);
    emit workerProcessFinished();
}

void DActionsListModel::bestIndexScrollTo(int k)
{
    emit scrollTo( createIndex(k, 0) );
}

void DActionsListModel::selectNextGroupKeeps(QModelIndex index, bool selectNextGroup)
{
   p_selectNextGroup(index, selectNextGroup, true);
}

void DActionsListModel::selectNextGroupDeletes(QModelIndex index, bool selectNextGroup)
{
    p_selectNextGroup(index, selectNextGroup, false);
}

void DActionsListModel::p_selectNextGroup(QModelIndex index, bool selectNextGroup, bool keepers)
{
    if(!index.isValid())
        return;
    Q_ASSERT_X(index.row() < viewIt.size(), "SelectNext_Routines", "FATAL ERROR: Index out of Range");
    //First detect the range of Values we need to sample... || Upper and Lower bounds
        int idx_lower = index.row(), idx_upper = idx_lower;

        if(selectNextGroup)
        {
            int t_track = idx_lower;
            while( ( ++t_track < viewIt.size() ) && (viewIt[t_track].item != viewIt[t_track].header) );
            idx_lower = idx_upper = t_track;
        }

        for(int i = idx_lower; i >= 0; i--)
            if(viewIt[i].item == viewIt[i].header)
            {
                idx_lower = i + 1;
                break;
            }

        for(int i = idx_upper + 1; i < viewIt.size(); i++)
            if(viewIt[i].item == viewIt[i].header)
            {
                idx_upper = i;
                break;
            }

        if(!(idx_upper - idx_lower))
            return;
        QList<int> ranges;
        for(int i = idx_lower; i < idx_upper; i++)
            ranges.append( i );

        if(keepers)
            qSort(ranges.begin(), ranges.end(), [&](int lhs, int rhs)
            {
                return (  recommender.getKeepingWeight( viewIt[lhs].item->property.getFilePath() )
                        > recommender.getKeepingWeight( viewIt[rhs].item->property.getFilePath() ) );
            });
        else
            qSort(ranges.begin(), ranges.end(), [&](int lhs, int rhs)
            {
                return (  recommender.getDeletionWeight( viewIt[lhs].item->property.getFilePath() )
                        > recommender.getDeletionWeight( viewIt[rhs].item->property.getFilePath() ) );
            });

        QModelIndexList indexes;
        QModelIndex ScrolledTo = createIndex( idx_lower - 1 , 0 );
        //Loop... selecting everything within this range whilest skipping the last item
        for(int i = 0; i < ranges.size() - 1; i++)
        {
            indexes.append( createIndex( ranges[i], 0 ) );
        }

        emit makeSelection(indexes, ScrolledTo);
}

QString DActionsListModel::formartForLog(const QString &str)
{
    return "<font color='#4389A4'>><b>DupscanGUIModel: -</b></font>" + str;
}

DLS::DuplicatesContainer DActionsListModel::iDataListToDuplicatesContainer(const QList<iData> &list) const
{
    //Assuming iData list is correct. . . Any incorrectness is catastrophe!

    DLS::DuplicatesContainer dups;
    for(int i = 0; i < list.size(); i++)
    {
        const iData& d = list[i];
        Q_ASSERT_X(d.header->isGroupHeader, "containerMaker", "Serious Logic Error!");
        int limit = d.header->header.itemCount() + i;

        DLS::ptrVEC_FileProperty vecPtr(new DLS::VEC_FileProperty);
        for(; (i < list.size() && (list[i].header == d.header)); i++)
        {
            if(list[i].header == list[i].item)
                continue;
            vecPtr->push_back(list[i].item->property);
        }
        if(!vecPtr->empty())
            dups.insert( vecPtr );
    }
    return dups;
}

DLS::DuplicatesContainer DActionsListModel::getCurrentDuplicates() const
{
    return iDataListToDuplicatesContainer( viewIt );
}
