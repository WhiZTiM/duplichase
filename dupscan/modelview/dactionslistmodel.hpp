#ifndef DACTIONSLISTMODEL_HPP
#define DACTIONSLISTMODEL_HPP

#include <QList>
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

    void selectForDeletion(QModelIndex index);
    void selectForKeep(QModelIndex index);
    void deselectForDeletion(QModelIndex index);
    void deselectForKeep(QModelIndex index);
    /*! TODO!!!
    void filterModelByPath(const QString& parentPath );
    void filterModelBySize(ulong min, ulong max);
    */


private:
    DLS::DuplicatesContainer duplicates;
    DLS::PathRecommender recommender;
    QList<int> viewIndexes;
    QList<DItem> items;
    ExtraPropertyHandler extraPropertyHandle;

    void prepareModel();
};

#endif // DACTIONSLISTMODEL_HPP
