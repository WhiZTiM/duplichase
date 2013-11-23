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
#include <atomic>

class DActionsListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit DActionsListModel(QObject *parent = 0);
    void setDuplicates(DLS::DuplicatesContainer container);

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

public slots:
    void sortModel(Qt::SortOrder sortOrder = Qt::DescendingOrder);
    void filterModelByExtension(QStringList extensionList);
    void filterModelBySize(ulong min, ulong max);
    void filterModelByPath(QString parentPath );
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
    /*! TODO!!!
    */

private slots:
    void bestIndexScrollTo(int);
    void p_deleteFilesNow(QModelIndexList indexes, bool RemoveFromModel = false, bool MoveToTrash = false);
    void p_selectNextGroup(QModelIndex index, bool selectNextGroup, bool keepers);

private:
    bool freshPreparation = true;
    std::vector<DLS::ptrVEC_FileProperty> vec_duplicates;
    DLS::DuplicatesContainer duplicates;
    DLS::PathRecommender recommender;
    QList<int> viewIndexes;
    QList<DItem> items;
    QLinkedList<DItem> dItems;
    struct iData {
        QLinkedList<DItem>::iterator item;
        QLinkedList<DItem>::Iterator header;
    };

    QList<iData> viewIt;
    ExtraPropertyHandler extraPropertyHandle;

    void prepareModel();
    QString formartForLog(const QString& str);

    QMutex dataMutex;
    QFuture<void> w_selectionFuture;
    std::atomic<bool> AnyProcessRunning;
    void w_autoSelectNextKeeps();
    void w_autoSelectNextDeletes();
    void w_autoSelectNextPossibilities();
    void w_autoSelectNext(bool keepers);
};

#endif // DACTIONSLISTMODEL_HPP
