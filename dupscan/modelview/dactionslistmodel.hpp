#ifndef DACTIONSLISTMODEL_HPP
#define DACTIONSLISTMODEL_HPP

#include <QList>
#include <QLinkedList>
#include <QAbstractListModel>
#include <QSortFilterProxyModel>
#include <dupscan/modelview/ditem.hpp>
#include <dupscan/modelview/extrafileproperty.hpp>
#include <backend/include/container_helpers/fp_holders.hpp>
#include <backend/include/recommendation/path_recommender.hpp>

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

public slots:
    void sortModel(Qt::SortOrder sortOrder = Qt::DescendingOrder);
    void filterModelByExtension(const QStringList& extensionList);
    void resetViewItems();

    void commitMarkings();
    void selectForDeletion(QModelIndexList indexes);
    void selectForKeep(QModelIndexList indexes);
    void deselectForDeletion(QModelIndexList indexes);
    void deselectForKeep(QModelIndexList indexes);
    void deleteFilesNow(QModelIndexList indexes);

    void unmarkAllKeeps();
    void unmarkAllDeletes();
    void unmarkAll();
    /*! TODO!!!
    void filterModelByPath(const QString& parentPath );
    void filterModelBySize(ulong min, ulong max);
    */


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
};

#endif // DACTIONSLISTMODEL_HPP
