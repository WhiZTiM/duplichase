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
    view->setSelectionMode(DActionsListView::ExtendedSelection);
    actionsButton = new ActionsButtonPanel(this);
    view->setModel( model );
    view->setItemDelegate( delegate );

    setLayout( new QHBoxLayout );
    layout()->addWidget( view );

    connect(view, SIGNAL(markForKeep(QModelIndexList)), model, SLOT(selectForKeep(QModelIndexList)));
    connect(view, SIGNAL(unmarkForKeep(QModelIndexList)), model, SLOT(deselectForKeep(QModelIndexList)));
    connect(view, SIGNAL(markForDelete(QModelIndexList)), model, SLOT(selectForDeletion(QModelIndexList)));
    connect(view, SIGNAL(unmarkForDelete(QModelIndexList)), model, SLOT(deselectForDeletion(QModelIndexList)));
    connect(view, SIGNAL(deleteFileNow(QModelIndexList)), model, SLOT(deleteFilesNow(QModelIndexList)));

    connect(actionsButton, SIGNAL(sortingRequested(Qt::SortOrder)), model, SLOT(sortModel(Qt::SortOrder)));
    connect(actionsButton, SIGNAL(resetRequested()), model, SLOT(resetViewItems()));

    model->setDuplicates( sampleDuplicateContainer() );

}

void DupScanActionWidget::setDuplicates(DLS::DuplicatesContainer duplicates)
{
    model->setDuplicates( duplicates );
    update();
}

QWidget* DupScanActionWidget::actionsButtonPanel() const
{
    return actionsButton;
}

DupScanActionWidget::~DupScanActionWidget()
{
    //delete actionsButton;
    actionsButton = nullptr;
}
