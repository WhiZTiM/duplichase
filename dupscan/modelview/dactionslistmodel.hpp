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
#ifndef DACTIONSLISTMODEL_HPP
#define DACTIONSLISTMODEL_HPP

#include <QList>
#include <QMutex>
#include <QFuture>
#include <QStringList>
#include <QLinkedList>
#include <QAbstractListModel>
#include <QSortFilterProxyModel>
#include <dupscan/modelview/ditem.hpp>
#include <dupscan/modelview/extrafileproperty.hpp>
#include <backend/include/container_helpers/fp_holders.hpp>
#include <backend/include/recommendation/path_recommender.hpp>
#include <functional>
#include <atomic>

class DActionsListModel : public QAbstractListModel
{
    Q_OBJECT
public:

    enum class TRemoveFrom : int
    {
        None = 0x0,                     //! 0000
        View = 0x1,                     //! 0001
        Model= 0x3,                     //! 0011
        FileSystem_toRecycleBin = 0x7,  //! 0111
        FileSystem_permanently = 0xB    //! 1011
    };


    explicit DActionsListModel(QObject *parent = 0);
    void setDuplicates(DLS::DuplicatesContainer container);
    DLS::DuplicatesContainer getCurrentDuplicates() const;
    inline bool isEmpty() const
    {
        return viewIt.isEmpty();
    }

    //overriden functions
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;

signals:
    void logMessage(QString message);
    void statusBarMessage(QString message);
    void scrollTo(QModelIndex index);
    void makeSelection(QModelIndexList, QModelIndex scrolledTo);

    void workerProcessStarted();
    void workerProcessFinished();
    void workerProcessProgressPercentage(int);

    //!private stuff
    void bestIndex(int);
    void modelWasJustSet();

public slots:
    void sortModel(Qt::SortOrder sortOrder = Qt::DescendingOrder);
    void filterModelByExtension(QStringList extensionList);
    void filterModelBySize(ulong min, ulong max);
    void filterModelByPath(QString filePath);
    void filterModelByRegex(QRegExp regex);
    void resetViewItems();

    void commitMarkings();
    void selectForDeletion(QModelIndexList indexes);
    void selectForKeep(QModelIndexList indexes);
    void deselectForDeletion(QModelIndexList indexes);
    void deselectForKeep(QModelIndexList indexes);
    void deleteFilesNow(QModelIndexList indexes);
    void removeIndexes(QModelIndexList indexes);
    void removeIndexesSession(QModelIndexList indexes);

    void unmarkAllKeeps();
    void unmarkAllDeletes();
    void unmarkAll();

    void selectNextGroupKeeps(QModelIndex index, bool selectNextGroup = false);
    void selectNextGroupDeletes(QModelIndex index, bool selectNextGroup = false);

    void autoSelectNextKeeps();
    void autoSelectNextDeletes();
    void autoSelectNextPossibilities();

private slots:
    void bestIndexScrollTo(int);

private:
    void p_deleteFilesNow(QModelIndexList indexes, bool RemoveFromModel = false, bool MoveToTrash = false);
    void p_selectNextGroup(QModelIndex index, bool selectNextGroup, bool keepers);
    bool freshPreparation = true;
    std::vector<DLS::ptrVEC_FileProperty> vec_duplicates;
    DLS::DuplicatesContainer duplicates;
    DLS::PathRecommender recommender;
    QLinkedList<DItem> dItems;
    struct iData {
        QLinkedList<DItem>::iterator item;
        QLinkedList<DItem>::Iterator header;
    };
    QMutex dataMutex;
    QList<iData> viewIt;
    QFuture<void> w_selectionFuture;
    std::atomic<bool> AnyProcessRunning;
    ExtraPropertyHandler extraPropertyHandle;

    void prepareModel();
    void w_autoSelectNextKeeps();
    void w_autoSelectNextDeletes();
    void w_autoSelectNextPossibilities();
    void w_autoSelectNext(bool keepers);
    QString formatForLog(const QString& str);
    DLS::DuplicatesContainer iDataListToDuplicatesContainer(const QList<iData>& list) const;
    int scanDataViewAndremoveIndex_if(TRemoveFrom removeType, std::function<bool(int)> Callable);
    int scanDataRangeAndRemoveIndex_if(QList<int> indexRange, TRemoveFrom removeType,
                                       std::function<bool(int)> Callable);
};

inline DActionsListModel::TRemoveFrom operator & (DActionsListModel::TRemoveFrom a, DActionsListModel::TRemoveFrom b)
{
    using underlying_type = std::underlying_type<DActionsListModel::TRemoveFrom>::type;
    return static_cast<DActionsListModel::TRemoveFrom>
            ( static_cast<underlying_type>(a) & static_cast<underlying_type>(b));
}

#endif // DACTIONSLISTMODEL_HPP
