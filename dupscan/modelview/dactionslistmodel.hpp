#ifndef DACTIONSLISTMODEL_HPP
#define DACTIONSLISTMODEL_HPP

#include <QAbstractListModel>
#include <QList>
#include <dupscan/modelview/ditem.hpp>
#include <backend/include/container_helpers/fp_holders.hpp>
#include <backend/include/recommendation/path_recommender.hpp>

class DActionsListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit DActionsListModel(QObject *parent = 0);
    void setDuplicateData(DLS::DuplicatesContainer container);

    //overriden functions
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;

signals:
    void logMessage(QString message);

public slots:

private:
    DLS::DuplicatesContainer duplicates;
    DLS::PathRecommender recommender;
    QList<DItem> items;
    //QHash parentsMap;

    void prepareModel();
};

#endif // DACTIONSLISTMODEL_HPP
