#include "dactionslistmodel.hpp"
Q_DECLARE_METATYPE(DLS::FileProperty)
Q_DECLARE_METATYPE(DItem)

DActionsListModel::DActionsListModel(QObject *parent) :
    QAbstractListModel(parent)
{
    qRegisterMetaType<DLS::FileProperty>("DLS::FileProperty");
    qRegisterMetaType<DItem> ("DItem");
}

void DActionsListModel::setDuplicateData(DLS::DuplicatesContainer container)
{
    duplicates = container;
    prepareModel();
}

int DActionsListModel::rowCount(const QModelIndex &parent) const
{
    //if(!parent.isValid())    //toplevel
    //    return duplicates.size();
    return items.size();
}


//Private Functions
void DActionsListModel::prepareModel()
{
    //Assing unique numbers and headers
    items.clear();
    int i = 1;
    for(auto& item : duplicates)
    {
        DItem itm;
        itm.isGroupHeader = true;
        itm.header.topString("Category <<( " + QString::number(i) + " )>>");
        items.push_back(itm);
        DItem& itmHeader_ref = items.back();

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
            ++itemCount;
            Set_header_size = true;
        }
        itmHeader_ref.header.itemCount( itemCount );

        //sets the group size as from the last element in the loop
        if(Set_header_size)
            itmHeader_ref.header.value( items.back().property.getSize() );

        ++i;
    }
}

QVariant DActionsListModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(role);
    if(index.row() >= items.size())
        return QVariant();
    return QVariant::fromValue<DItem>( items[index.row()] );
}
