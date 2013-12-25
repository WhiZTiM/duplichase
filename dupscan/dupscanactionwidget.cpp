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
#include <QLabel>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QApplication>
#include <QProgressDialog>
#include "dupscanactionwidget.hpp"
#include "dupscan/modelview/dactionslistview.hpp"
#include "dupscan/modelview/dactionslistmodel.hpp"
#include "dupscan/modelview/filepropertyserializer.hpp"
#include "test/fileproperty/sample_fileProperty.hpp"


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

    emptyViewLabel = new QLabel("<qt><h2>No Duplicate Files Found</h2></qt>", this);
    stackedWidget = new QStackedWidget(this);
    stackedWidget->addWidget( emptyViewLabel );
    stackedWidget->addWidget( view );

    setLayout( new QHBoxLayout );
    layout()->addWidget( stackedWidget );

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
    connect(actionsButton, SIGNAL(filterByPath(QString)), model, SLOT(filterModelByPath(QString)));
    connect(actionsButton, SIGNAL(filterByRegex(QRegExp)), model, SLOT(filterModelByRegex(QRegExp)));
    connect(actionsButton, SIGNAL(filterBySize(ulong,ulong)), model, SLOT(filterModelBySize(ulong,ulong)));
    connect(actionsButton, SIGNAL(filterByExtension(QStringList)), model, SLOT(filterModelByExtension(QStringList)));
    connect(actionsButton, SIGNAL(statusBarErrorMessage(QString)), this, SIGNAL(statusBarMessage(QString)));

    connect(model, SIGNAL(logMessage(QString)), this, SIGNAL(logMessage(QString)));
    connect(model, SIGNAL(scrollTo(QModelIndex)), view, SLOT(scrollToIndex(QModelIndex)));
    connect(model, SIGNAL(makeSelection(QModelIndexList,QModelIndex)), view,
            SLOT(makeSelection(QModelIndexList,QModelIndex)));

    connect(model, SIGNAL(modelWasJustSet()), this, SLOT(modelWasJustSet()));
    //model->setDuplicates( sampleDuplicateContainer() );

}

void DupScanActionWidget::setDuplicates(DLS::DuplicatesContainer duplicates)
{
    model->setDuplicates( duplicates );
    update();
}

void DupScanActionWidget::modelWasJustSet()
{
    if(model->isEmpty())
    {
        stackedWidget->setCurrentIndex( 0 );
    }
    else
        stackedWidget->setCurrentIndex( 1 );
}

bool DupScanActionWidget::saveResultsToFile(const QString &fileName)
{
    FilePropertySerializer serializer;
    if(serializer.toDisk(fileName, model->getCurrentDuplicates()))
    {
        logMessage("Successfully saved!");
        return true;
    }
    QMessageBox::critical(this, tr("File Error"), serializer.error());
    return false;
}

bool DupScanActionWidget::loadResultsFromFile(const QString &fileName)
{
    FilePropertySerializer serializer;
    DLS::DuplicatesContainer duplicates = serializer.fromDisk( fileName );
    if(serializer.error().isEmpty())
    {
        update();
        QProgressDialog progress(tr("Verifying Files in Place.."), tr("Cancel"), 0, 100, this);
        progress.setMaximumWidth( 600 );
        progress.setWindowTitle(tr("Please Wait - - Verifying Files in Place.."));
        progress.setWindowModality(Qt::WindowModal);
        progress.setMinimumDuration(1000);

        std::function<void(QString)> setNameCallBack =
                [&](const QString& name) -> void
        {
            progress.setLabelText( "<qt><b>Verifying: </b></qt>" + name);
            QApplication::processEvents();
        };

        std::function<void(int)> setProgressCallBack =
                std::bind(&QProgressDialog::setValue, std::ref(progress), std::placeholders::_1);

        std::function<bool()> isTaskCancelledCallBack =
                std::bind(&QProgressDialog::wasCanceled, std::ref(progress));

        FilePropertySerializer::VerificationCallbacks vc;
        vc.CurrentFileCallback = setNameCallBack;
        vc.PercentageProgressCallback = setProgressCallBack;
        vc.isTaskCancelledCallback = isTaskCancelledCallBack;


        FilePropertySerializer::verifyAndFilter_inPlace(duplicates, vc);

        if(progress.wasCanceled())
            return false;
        int rtn = QMessageBox::information(this, tr("Write Back Changes"),
                                 tr("Do you wish to write back verified changes to the file"),
                                           QMessageBox::No | QMessageBox::Yes, QMessageBox::No);
        if(rtn == QMessageBox::Yes)
            saveResultsToFile( fileName );

        model->setDuplicates( duplicates );
        logMessage("Successfully Loaded!");
        return true;
    }
    QMessageBox::critical(this, tr("File Error"), serializer.error());
    return false;
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
