/*******************************************************************************************
**  (C) Copyright September 2013 - December 2013 by
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
#include "dactionslistmodel.hpp"
#include "dupscan/safe_deletion.hpp"
#include <QtConcurrent/QtConcurrent>
#include <boost/algorithm/string.hpp>
#include <QApplication>
#include <QStringList>
#include <QVector>
#include <QPair>
#include <QFile>
#include <iostream>
#include <array>
#include <QMutexLocker>
#include <QThread>

Q_DECLARE_METATYPE(DLS::FileProperty)
Q_DECLARE_METATYPE(DItem)

QString deleteFileToRecycleBin(const QString& filePath);
QString deleteFilePermanently(const QString& filePath);

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
    beginResetModel();

    viewIt.clear();
    QLinkedList<DItem>::iterator pIterHeader = dItems.begin();
    int mk = 0;
    for(QLinkedList<DItem>::iterator iter = dItems.begin(); iter != dItems.end(); iter++)
    {
        if(iter->isGroupHeader)
        {
            pIterHeader = iter;
            mk = 0;
        }
        else
            pIterHeader->header.itemCount(++mk);

        iData kItem;
        kItem.header = pIterHeader;
        kItem.item = iter;
        viewIt.push_back( kItem );
    }

    endResetModel();
}

void DActionsListModel::filterModelByExtension(QStringList extensionList)
{
    std::unordered_set<std::string> extensionSet;
    for(auto& extension : extensionList)
        extensionSet.insert('.' + extension.toLower().toStdString());
    scanDataViewAndremoveIndex_if(TRemoveFrom::View,
                                  ([&](int indx) -> bool
    {
        return (extensionSet.find( boost::to_lower_copy( viewIt[indx].item->property.getFileExtension()) )
                == extensionSet.end());
    }
    ));

}

void DActionsListModel::filterModelBySize(ulong min, ulong max)
{
    scanDataViewAndremoveIndex_if(TRemoveFrom::View,
                                  [&](int indx) -> bool
    {
        unsigned long fsize = viewIt[indx].item->property.getSize();
        return !((min <= fsize) && (fsize <= max));
    }
    );
}

void DActionsListModel::filterModelByRegex(QRegExp regex)
{
    if(!regex.isValid())
        return;
    scanDataViewAndremoveIndex_if(TRemoveFrom::View,
                                  [&](int indx)
    {
        return !regex.exactMatch( QString::fromStdString( viewIt[indx].item->property.getFilePath() ) );
    });
}

void DActionsListModel::filterModelByPath(QString filePath)
{
    if(filePath.isEmpty())
        return;
    scanDataViewAndremoveIndex_if(TRemoveFrom::View,
                                  [&](int indx)
    {
        return ! QString::fromStdString(viewIt[indx].item->property.getFilePath()).contains(filePath, Qt::CaseInsensitive) ;
    });
}

/// THis function exposes the poor design and loose cohesion of this Software. Runs at O(2n + nlogn) worst case
/// EDIT: FIXED! Now nuns at (Sorting cost + copying cost) --> (nlogn + n) --> nlogn;
void DActionsListModel::sortModel(Qt::SortOrder sortOrder)
{
    beginResetModel();

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
    return viewIt.size();
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

    recommender.clear();
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

        dItems.push_back(itm);  pLast = dItems.end() - 1;
        iData kItem;
        kItem.header = pLast;
        kItem.item = pLast;
        viewIt.push_back( kItem );
        DItem& itmHeader_reference = dItems.back();                 //! REMOVE

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

            dItems.push_back(itm);  pLast = dItems.end() - 1;
            kItem.item = pLast;
            viewIt.push_back( kItem );

            ++itemCount;
            Set_header_size = true;
        }

        itmHeader_reference.header.itemCount( itemCount );

        //sets the group size as from the last element in the loop
        if(Set_header_size)
        {
            itmHeader_reference.header.value( dItems.back().property.getSize() );
        }

        ++i;
    }
    endResetModel();
}

QVariant DActionsListModel::data(const QModelIndex &index, int role) const
{
    if(index.row() >= viewIt.size())
        return QVariant();

    Q_ASSERT_X( index.row() < viewIt.size(), "dataRequest", " Out of range index requested! " );
    DItem item = (*viewIt[ index.row() ].item);

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
        Q_ASSERT_X( index.row() < viewIt.size(), "selectionForDeletion", " Out of range index requested! " );
        viewIt[ index.row() ].item->isDeleteChecked = true;
        viewIt[ index.row() ].item->isKeepChecked = false;
        emit dataChanged(index, index);
    }
}

void DActionsListModel::selectForKeep(QModelIndexList indexes)
{
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
    for(auto const& index : indexes)
    {
        Q_ASSERT_X( index.row() < viewIt.size(), "deselectionForDeletion", " Out of range index requested! " );
        viewIt[ index.row() ].item->isDeleteChecked = false;
        emit dataChanged(index, index);
    }
}

void DActionsListModel::deselectForKeep(QModelIndexList indexes)
{
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

//! TODO: Adapt this function to the newly created \fn scanDataViewAndRemoveIndex_if()
void DActionsListModel::p_deleteFilesNow(QModelIndexList indexes, bool RemoveFromModel, bool MoveToTrash)
{

    QList<int> indexList;
    for(int i = 0; i < indexes.size(); i++)
        indexList.append( indexes[i].row() );
    std::function<bool(int)> deleters = [](int){ return true; };

    TRemoveFrom removalType = RemoveFromModel ?
                (MoveToTrash ? TRemoveFrom::FileSystem_toRecycleBin : TRemoveFrom::Model)
              : TRemoveFrom::View;

    scanDataRangeAndRemoveIndex_if(indexList, removalType, deleters);
}

void DActionsListModel::commitMarkings()
{
    beginResetModel();
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
    for(auto& k : dItems)
        k.isKeepChecked = false;
    endResetModel();
}

void DActionsListModel::unmarkAllDeletes()
{
    beginResetModel();
    for(auto& k : dItems)
        k.isDeleteChecked = false;
    endResetModel();
}

void DActionsListModel::unmarkAll()
{
    beginResetModel();
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

void DActionsListModel::autoSelectDeletePossibilities()
{
    if(AnyProcessRunning.load())
    {
        emit statusBarMessage(QString("Please Be Patient With the Current Operation"));
        return;
    }
    if(!w_selectionFuture.isRunning())
        w_selectionFuture = QtConcurrent::run(this, &DActionsListModel::w_autoSelectDeletePossibilities);
}

void DActionsListModel::w_autoSelectNextKeeps()
{
    w_autoSelectNext(true);
}

void DActionsListModel::w_autoSelectNextDeletes()
{
    w_autoSelectNext(false);
}

void DActionsListModel::w_autoSelectDeletePossibilities()
{
    //---> Some cranky stuff here...    ||| NOT YET AVAILABLE |||
    //====>>> To be deployed hopefully in versions greater than 0.8.0 <><><><><><><><><><><>

    //Very crude implementation. . .
    for(int i = 0; i < viewIt.size(); i++)
    {
        QModelIndex indx = createIndex(i, 0);
        p_selectNextGroup(indx, false, false);
    }
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
            while(((t_track + 1) < viewIt.size() ) && (viewIt[t_track].item != viewIt[t_track].header) )
                ++t_track;
            idx_lower = idx_upper = t_track;
        }

        for(int i = idx_lower; i >= 0; i--)
            if(viewIt[i].item == viewIt[i].header)
            {
                idx_lower = i + 1;
                Q_ASSERT(idx_lower < viewIt.size());
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

QString DActionsListModel::formatForLog(const QString &str)
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

#define TESTOUTTA_RANGE

inline int DActionsListModel::scanDataViewAndremoveIndex_if(TRemoveFrom removeType, std::function<bool (int)> Callable)
{
    QList<int> wholeList;
    for(int i=0; i < viewIt.size(); i++)
        wholeList.append(i);

    return scanDataRangeAndRemoveIndex_if(wholeList, removeType, Callable);
}

/*! This algorithm runs at O(2N + Mlog(M)) ...where M == \a indexRange.size() and N = \a viewIt.size()
//! First, is to sort the index in ascending order then (1st LOOP) pick each index in \a indexRange and
//! pass it to \a Callable(); if Callable returns true... it goes ahead to remove the index according to
//! \a removeType...
//!
//! Some removals will cause One item to be left in the group Header, and we do not want that
//! So, we scan and remove that item(2nd LOOP)
//!
//! Finally, we scan and remove empty Headers
//!
//!*/
int DActionsListModel::scanDataRangeAndRemoveIndex_if(QList<int> indexRange, TRemoveFrom removeType,
                                                      std::function<bool (int)> Callable)
{
    if(removeType == TRemoveFrom::None)
        return 0;

    if(AnyProcessRunning.load())
    {
        emit statusBarMessage(QString("Please Be Patient With the Current Operation"));
        return 0;
    }

    AnyProcessRunning.store(true);
    QModelIndex parent;
    //Sort the model first
    qSort(indexRange);

    const int rangeLimit = indexRange.size();
    const int ten_percent = (rangeLimit / 10) > 0 ? (rangeLimit / 10) : 1;
    int t_track = 0;
    for(int i = 0; i < rangeLimit; i++)
    {
        /* Remove This! during code cleanup! The assumption of this code fails in some scenerios
        const int index = i - t_track;
        const int idx = indexRange[index];
        */
        const int idx = indexRange[i] - t_track;

        if(viewIt[idx].item == viewIt[idx].header)
            continue;
        else if(not Callable(idx))
            continue;


        beginRemoveRows( parent, idx, idx );
        const QString filePath(QString::fromStdString( viewIt[idx].item->property.getFilePath() ));
        if((removeType & TRemoveFrom::Model) == TRemoveFrom::Model)
        {
            dItems.erase( viewIt[idx].item );
        }
        if((removeType & TRemoveFrom::View) == TRemoveFrom::View)
        {
            viewIt[idx].header->header.itemCount( viewIt[idx].header->header.itemCount() - 1 );
            viewIt.removeAt( idx );
            ++t_track;
        }
        endRemoveRows();


        if((removeType & TRemoveFrom::FileSystem_toRecycleBin) == TRemoveFrom::FileSystem_toRecycleBin)
        {
            recommender.setWeights( filePath.toStdString(), std::string() );
            emit logMessage( formatForLog( deleteFileToRecycleBin( filePath ) ) );
        }
        if((removeType & TRemoveFrom::FileSystem_permanently) == TRemoveFrom::FileSystem_permanently)
        {
            recommender.setWeights( filePath.toStdString(), std::string() );
            emit logMessage( formatForLog( deleteFilePermanently( filePath ) ));
        }
        if((i % ten_percent) == 0)  //update at multiples of ten_percent only
            QApplication::processEvents(QEventLoop::AllEvents, 50);
    }


    //Deal with single last entries;
    const int s_rangeLimit = viewIt.size();
    int s_track = 0;
    for(int i = 0; i < s_rangeLimit; i++)
    {
        const int idx = i - s_track;
        if(viewIt[idx].item == viewIt[idx].header)
            continue;
        if((viewIt[idx].header->header.itemCount() < 2))
        {
            beginRemoveRows(parent, idx, idx);
            viewIt[idx].header->header.itemCount(0);
            if((removeType & TRemoveFrom::Model) == TRemoveFrom::Model)
            {
                dItems.erase( viewIt[idx].item );
            }
            if((removeType & TRemoveFrom::View) == TRemoveFrom::View)
            {
                viewIt.removeAt( idx );
                ++s_track;
            }
            endRemoveRows();
        }
    }


    //Deal With empty headers
    const int h_rangeLimit = viewIt.size();
    int ht_track = 0;
    for(int i = 0; i < h_rangeLimit; i++)
    {
        const int idx = i - ht_track;
        if((viewIt[idx].item == viewIt[idx].header) && (viewIt[idx].header->header.itemCount() < 1))
        {
            beginRemoveRows(parent, idx, idx);
            if((removeType & TRemoveFrom::Model) == TRemoveFrom::Model)
            {
                dItems.erase( viewIt[idx].item );
            }
            if((removeType & TRemoveFrom::View) == TRemoveFrom::View)
            {
                viewIt.removeAt( idx );
                ++ht_track;
            }
            endRemoveRows();
        }
    }

    AnyProcessRunning.store(false);
    return t_track;
}

QString deleteFileToRecycleBin(const QString& filePath)
{
    QString deletionReply = DeletionAgent::toTrash( filePath );
    if(!deletionReply.isEmpty())
        return QString(QObject::tr("Problem With \"") + filePath + "\" " + deletionReply);
    return QString(QObject::tr("The File \"") + filePath + QObject::tr("\" was SUCSESSFULLY deleted"));
}

QString deleteFilePermanently(const QString& filePath)
{
    bool s = QFile::remove( filePath );
    if(not s)
        return QString(QObject::tr("Problem With \"") + filePath +
                       QObject::tr(" Failed to permanently delete!"));
    return QString(QObject::tr("The File \"") + filePath +
                   QObject::tr("\" has been permanently deleted"));
}
