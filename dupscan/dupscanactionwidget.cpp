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
    connect(view, SIGNAL(removeFromView(QModelIndexList)), model, SLOT(removeIndexes(QModelIndexList)));
    connect(view, SIGNAL(removeFromScanner(QModelIndexList)), model, SLOT(removeIndexesSession(QModelIndexList)));
    connect(view, SIGNAL(selectNextGroupKeeps(QModelIndex,bool)), model, SLOT(selectNextGroupDeletes(QModelIndex,bool)));
    connect(view, SIGNAL(selectNextGroupDeletes(QModelIndex,bool)), model, SLOT(selectNextGroupDeletes(QModelIndex,bool)));

    connect(actionsButton, SIGNAL(sortingRequested(Qt::SortOrder)), model, SLOT(sortModel(Qt::SortOrder)));
    connect(actionsButton, SIGNAL(commitRequested()), model, SLOT(commitMarkings()));
    connect(actionsButton, SIGNAL(resetRequested()), model, SLOT(resetViewItems()));
    connect(actionsButton, SIGNAL(unmarkAllKeeps()), model, SLOT(unmarkAllKeeps()));
    connect(actionsButton, SIGNAL(unmarkAllDeletes()), model, SLOT(unmarkAllDeletes()));
    connect(actionsButton, SIGNAL(unmarkAll()), model, SLOT(unmarkAll()));
    connect(actionsButton, SIGNAL(autoSelectNextKeeps()), model, SLOT(autoSelectNextKeeps()));
    connect(actionsButton, SIGNAL(autoSelectNextDeletes()), model, SLOT(autoSelectNextDeletes()));
    connect(actionsButton, SIGNAL(autoSelectNextPossibilities()), model, SLOT(autoSelectNextPossibilities()));
    connect(actionsButton, SIGNAL(selectGroupKeeps()), view, SLOT(selectKeepsGroup()));
    connect(actionsButton, SIGNAL(selectGroupDeletes()), view, SLOT(selectDeletesGroup()));
    connect(actionsButton, SIGNAL(selectNextGroupKeeps()), view, SLOT(selectNextKeepsGroup()));
    connect(actionsButton, SIGNAL(selectNextGroupDeletes()), view, SLOT(selectNextDeletesGroup()));

    connect(model, SIGNAL(logMessage(QString)), this, SIGNAL(logMessage(QString)));
    connect(model, SIGNAL(scrollTo(QModelIndex)), view, SLOT(scrollToIndex(QModelIndex)));
    connect(model, SIGNAL(makeSelection(QModelIndexList,QModelIndex)), view,
            SLOT(makeSelection(QModelIndexList,QModelIndex)));

    //model->setDuplicates( sampleDuplicateContainer() );

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
