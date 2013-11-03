#include <QHBoxLayout>
#include "dupscanactionwidget.hpp"
#include "dupscan/modelview/dactionslistview.hpp"
#include "dupscan/modelview/dactionslistmodel.hpp"
#include "test/fileproperty/sample_fileProperty.hpp"

DuplicatesListView::DuplicatesListView(QWidget *parent)
    :   QListView(parent)
{
    //
}


DupScanActionWidget::DupScanActionWidget(QWidget *parent) :
    QWidget(parent)
{
    delegate = new DActionsListDelegate(this);
    model = new DActionsListModel(this);
    view = new DActionsListView(this);
    view->setModel( model );
    view->setItemDelegate( delegate );

    setLayout( new QHBoxLayout );
    layout()->addWidget( view );

    model->setDuplicateData( sampleDuplicateContainer() );

}
