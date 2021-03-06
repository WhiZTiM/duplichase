/*******************************************************************************************
**  (C) Copyright September 2013 - November 2013 by
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
#include "dupscan/modelview/filterwidgets.hpp"
#include "dupscan/modelview/ditem.hpp"
#include "dactionslistview.hpp"
#include <boost/filesystem.hpp>
#include <QMenu>
#include <QCursor>
#include <QAction>
#include <QToolTip>
#include <QPainter>
#include <QFileInfo>
#include <QKeyEvent>
#include <QMessageBox>
#include <QStaticText>
#include <QMouseEvent>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDesktopServices>

Q_DECLARE_METATYPE(DItem)

QString parent_of_generic_path(const std::string path)
{
    return QString::fromStdString( path.substr(0, path.rfind('/')) );
}


///////////////////////////////////////////////////////////////
/// \brief DActionsListView::DActionsListView
/// \param parent
//////////////////////////////////////////////////////////////////
DActionsListView::DActionsListView(QWidget *parent) :
    QListView(parent), contextMenu(this)
{
    openDirectoryAction     = contextMenu.addAction("&Open Directory", this, SLOT(action_openDirectory()));
    openFileAction          = contextMenu.addAction("Open &File", this, SLOT(action_openFile()));
    contextMenu.addSeparator();
    keepAction              = contextMenu.addAction("Mark for keep", this, SLOT(action_markForKeep()));
    deleteAction            = contextMenu.addAction("Mark for Deletion", this, SLOT(action_markForDelete()));
    contextMenu.addSeparator();
    removeFromViewAction    = contextMenu.addAction("Remove From View", this, SLOT(action_removeFromView()));
    removeFromScannerAction = contextMenu.addAction("Remove From DupScan", this, SLOT(action_removeFromScanner()));
    contextMenu.addSeparator();
    deleteNowAction         = contextMenu.addAction("Delete Now", this, SLOT(action_deletFileNow()));


    openDirectoryAction->setToolTip(tr("Opens The directory where this file is contained"));
    openFileAction->setToolTip(tr("Opens the File with default Application"));
    deleteNowAction->setToolTip("Deletes the File Now");
    keepAction->setCheckable(true);
    deleteAction->setCheckable(true);
}

void DActionsListView::selectKeepsGroup()
{
    QModelIndex indx = currentIndex();
    clearSelection();
    selectNextGroupKeeps( indx, false );
}

void DActionsListView::selectDeletesGroup()
{
    QModelIndex indx = currentIndex();
    clearSelection();
    selectNextGroupDeletes( indx, false);
}

void DActionsListView::selectNextKeepsGroup()
{
    QModelIndex indx = currentIndex();
    clearSelection();
    selectNextGroupKeeps( indx, true);
}

void DActionsListView::selectNextDeletesGroup()
{
    QModelIndex indx = currentIndex();
    clearSelection();
    selectNextGroupDeletes( indx, true);
}

void DActionsListView::action_openFile()
{
    if(!currentIndex().isValid())
        return;
    DItem item = currentIndex().data().value<DItem>();
    if(item.isGroupHeader)
        return;
    QString fname(QString::fromStdString( item.property.getFilePath() ));
    QDesktopServices::openUrl( QUrl::fromLocalFile( fname ) );
}

void DActionsListView::action_openDirectory()
{
    if(!currentIndex().isValid())
        return;
    DItem item = currentIndex().data().value<DItem>();
    if(item.isGroupHeader)
        return;

    QString fname( parent_of_generic_path( item.property.getFilePath() ));
    QDesktopServices::openUrl( QUrl::fromLocalFile( fname ) );
}

void DActionsListView::action_markForKeep()
{

    //if(keepAction->isChecked())
    if(keepActionChecked)
    {
        keepAction->setChecked(false);
        keepActionChecked = false;
        emit unmarkForKeep(selectionModel()->selectedIndexes());
    }
    else
    {
        keepAction->setChecked(true);
        keepActionChecked = true;
        emit markForKeep(selectionModel()->selectedIndexes());
    }
}

void DActionsListView::action_markForDelete()
{
    //if(deleteAction->isChecked())
    if(deleteActionChecked)
    {
        deleteAction->setChecked(false);
        deleteActionChecked = false;
        emit unmarkForDelete(selectionModel()->selectedIndexes());
    }
    else
    {
        deleteAction->setChecked(true);
        deleteActionChecked = true;
        emit markForDelete(selectionModel()->selectedIndexes());
    }
}

void DActionsListView::action_removeFromView()
{
    emit removeFromView( selectionModel()->selectedIndexes() );
}

void DActionsListView::action_removeFromScanner()
{
    emit removeFromScanner( selectionModel()->selectedIndexes() );
}

void DActionsListView::action_deletFileNow()
{
    if(selectionModel()->selectedIndexes().size() == 0)
        return;
    int w1 = QMessageBox::warning( this, tr("Deletion:: 1st WARNING"),
                                   tr("<qt><b>1st WARNING! </b></qt>Are you sure you want to delete the selected File(s)??"),
                                   QMessageBox::Yes | QMessageBox::No
                                   );
    if(w1 != QMessageBox::Yes)
        return;
    w1 = QMessageBox::critical( this, tr("Deletion:: FINAL WARNING!"),
                                tr("<qt><b>FINAL WARNING!  </b></qt>Are you REALLY sure you want to delete the selected File(s)??"),
                                QMessageBox::Yes | QMessageBox::No
                                );
    if(w1 != QMessageBox::Yes)
            return;
    //! We do not want the user to accidentally delete somefiles outside the view
    //! CHANGED!
    emit deleteFileNow(selectionModel()->selectedIndexes());
}

void DActionsListView::prepareContextMenu()
{
    DItem item = currentIndex().data().value<DItem>();
    if(item.isGroupHeader)
    {
        keepAction->setDisabled(true);
        deleteAction->setDisabled(true);
        deleteNowAction->setDisabled(true);
        openFileAction->setDisabled(true);
        openDirectoryAction->setDisabled(true);
    }

    keepAction->setEnabled(true);
    deleteAction->setEnabled(true);
    deleteNowAction->setEnabled(true);
    openFileAction->setEnabled(true);
    openDirectoryAction->setEnabled(true);
    if(item.isDeleteChecked)
    {
        deleteAction->setChecked(true);
        deleteActionChecked = true;
        deleteAction->setText(tr("Unmark for Deletion"));
        deleteAction->setToolTip(tr("Unmarks this file for Deletion"));
    }
    else
    {
        deleteAction->setChecked(false);
        deleteActionChecked = false;
        deleteAction->setText(tr("Mark for Deletion"));
        deleteAction->setToolTip(tr("Marks this file for Deletion"));
    }
    if(item.isKeepChecked)
    {
        keepAction->setChecked(true);
        keepActionChecked = true;
        keepAction->setText(tr("Unmark for Keep"));
        keepAction->setToolTip(tr("Unmarks this file from Keepers"));
    }
    else
    {
        keepAction->setChecked(false);
        keepActionChecked = false;
        keepAction->setText(tr("Mark for Keep"));
        keepAction->setToolTip(tr("Unmarks this file from Keepers"));
    }
}

void DActionsListView::processGroupHeaderSelected(QModelIndex index)
{
    Q_UNUSED(index)
    //
}

void DActionsListView::makeSelection(QModelIndexList indexes, QModelIndex ScrolledTo)
{
    //selectionModel()->clear();
    bool CurrentIndexHasBeenSet = false;
    for(auto& index : indexes)
    {
        if(not CurrentIndexHasBeenSet)
        {
            selectionModel()->setCurrentIndex(index, QItemSelectionModel::NoUpdate);
            CurrentIndexHasBeenSet = false;
        }
        selectionModel()->select(index, QItemSelectionModel::Select);
    }
    if(ScrolledTo.isValid())
        scrollTo(ScrolledTo);
}

void DActionsListView::scrollToIndex(QModelIndex index)
{
    //selectionModel()->clear();
    selectionModel()->select(index, QItemSelectionModel::Select);
    scrollTo(index);
}

void DActionsListView::scrollTo(const QModelIndex &index, ScrollHint hint)
{
    QListView::scrollTo(index, hint);
}

bool DActionsListView::viewportEvent(QEvent *event)
{
    QListView::viewportEvent(event);
    bool rtn = true;
    if(event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if(mouseEvent->button() == Qt::RightButton)
        {
            QRect rect(mouseEvent->globalPos(), contextMenu.sizeHint());
            contextMenu.setGeometry( rect );
            prepareContextMenu();
            contextMenu.show();
        }
    }
    else if(event->type() == QEvent::ContextMenu)
    {
        contextMenu.exec( QCursor::pos() );
    }
    return rtn;
    //return true;
}

void DActionsListView::keyPressEvent(QKeyEvent *event)
{
    std::cerr << event->text().toStdString() << std::endl;
}

void DActionsListView::selectNextPossibilities()
{
    QModelIndex indx = currentIndex();
    clearSelection();
    selectDeletePossibilities(indx);
}






///////////////////////////////////////////////////////////////
/// \brief DActionsListDelegate::DActionsListDelegate
/// \param parent
///
DActionsListDelegate::DActionsListDelegate(QWidget *parent) :
    QStyledItemDelegate(parent)
{
    //
}

void DActionsListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //QStyledItemDelegate::paint(painter, option, index);

    static const QPixmap deleteIconPixMap(":/icons/icons/delete-crosshair-icon.png");
    static const QPixmap keepIconPixMap(":/icons/icons/keep-vgood-icon.png");

    DItem item = index.data().value<DItem>();
    DLS::FileProperty fprop = item.property;
    QString name;
    QString size;

    const QFontMetrics& metrics = option.fontMetrics;
    QRect rect_major = option.rect;
    rect_major.translate( 3, 3 );
    //Need to trim... but hey!, translation alone serves us for now...
    QRect rect_minor = rect_major;
    const QPen originalPen = painter->pen();

    if(option.state & QStyle::State_Selected)
        painter->fillRect( option.rect, QColor(250, 255, 180, 250));
    else
        painter->fillRect( option.rect, QColor(255, 255, 255, 255) );   //aka WHITE

    QPen borderHighlightPen(originalPen);
    borderHighlightPen.setColor( QColor(Qt::red) );
    painter->setPen( borderHighlightPen );
    painter->drawRect( option.rect );
    painter->setPen(originalPen);

    const QFont originalFont = painter->font();
    QFont fnt = originalFont;
    fnt.setBold(true);

    if(item.isGroupHeader)
    {
        if(option.state & QStyle::State_Selected)
        {
            painter->fillRect( option.rect, QColor(130, 200, 130) );
            groupHeaderSelected(index);
        }
        else
            painter->fillRect( option.rect, QColor(100, 100, 200, 100) );

        name = item.header.topString();
        int headerC = item.header.itemCount();

        if(item.header.value())
            size = fileSizeAsString( item.header.value() * headerC ) + " in total";
        else
            size =  "Not Available";

        if(name.isEmpty())
            return;
        const QString freeable( fileSizeAsString( (headerC - 1) * item.header.value()) + " can be freed" );
        const QString fileCounts( QString::number( headerC ) + " Files ::  " + QString::number( headerC - 1)
            + " duplicates" );
        //qreal dx = rect_major.width() / 3.0;
        //painter->translate( dx, 0 );

        painter->setFont( fnt );
        painter->setPen( QColor(20, 40, 30) );
        painter->drawText(rect_major, Qt::AlignLeft | Qt::AlignTop, name, &rect_minor);
        rect_minor.translate(0, rect_minor.height() );
        painter->setPen( QColor(120, 50, 70) );
        painter->drawText(rect_minor, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, size, &rect_major);
        painter->setFont( originalFont );
        rect_major.translate( rect_major.width() + 10, 0);
        rect_major.setWidth( rect_major.width() + 600 );
        painter->drawText(rect_major, Qt::AlignTop | Qt::AlignLeft, fileCounts, &rect_minor);
        rect_minor.translate( rect_minor.width() + 10, 0 );
        rect_minor.setWidth( rect_minor.width() + 30 );
        painter->drawText(rect_minor, Qt::AlignLeft | Qt::AlignTop, freeable);
        return;
    }

    name = fileNameAsString( fprop.getFilePath() );
    size = fileSizeAsString( fprop.getSize() );
    if(name.isEmpty())
        return;

    const static QStaticText fileSizeStatic( "File Size: ");
    const static QStaticText fileNameStatic( "File Name: ");
    painter->drawStaticText(rect_major.topLeft(), fileNameStatic);
    rect_major.translate( metrics.width( fileNameStatic.text() ) , 0);
    rect_minor.translate( 0, metrics.height() );
    painter->drawStaticText(rect_minor.topLeft(), fileSizeStatic);

    painter->setFont( fnt );
    painter->setPen( QColor(20, 40, 30) );
    QString elidedName = elidedText(metrics, (rect_major.width() - (rect_major.width() * 2.25/10)), Qt::ElideLeft, name );
    painter->drawText(rect_major, Qt::AlignLeft | Qt::AlignTop, elidedName, &rect_minor);
    rect_minor.translate(0, rect_minor.height() );
    painter->setPen( QColor(120, 50, 70) );

    const static int k_align = Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap;

    painter->drawText(rect_minor, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, size, &rect_major);
    rect_major.setX( 200 );
    rect_major.setWidth( rect_major.width() + 300 );

    painter->setPen( QColor(Qt::red) );

    QString keepingWeight("Keeping Weight:  " + QString::number( item.keepingWeight ) );
    QString deletionWeight("Deletion Weight  " + QString::number( item.deletionWeight));

    painter->drawText( rect_major, k_align, deletionWeight);
    rect_major.translate( 200, 0 );
    painter->setPen( QColor(0, 220, 0) );
    painter->drawText( rect_major, k_align, keepingWeight );

    //drawPixMaps as necessary
    static const int dd = 20;
    int dx1 = option.rect.width() * 0.95;
    int dy1 = option.rect.height() * 0.2 + option.rect.top();
    QRect checkedIconArea(dx1, dy1, dd, dd);

    if(item.isKeepChecked)
        painter->drawPixmap(checkedIconArea, keepIconPixMap);
    else if(item.isDeleteChecked)
        painter->drawPixmap(checkedIconArea, deleteIconPixMap);

    //restore original
    painter->setFont( originalFont );
    painter->setPen( originalPen );
}

QSize DActionsListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    (void)index;
    return QSize(option.rect.width(), (option.fontMetrics.height() * 2) + 6);
}

QString DActionsListDelegate::fileNameAsString(const std::string &name) const
{
#ifdef Q_OS_WIN
    return QString::fromStdString(boost::filesystem::path(name).string());
#else
    return QString::fromStdString(boost::filesystem::path(name).native());
#endif
}

//! REDUNDANT!
QString DActionsListDelegate::fileSizeAsString(const ulong size) const
{
    static const int dp = 2;
    static const char f = 'f';
    QString rtn;
    if(size < 1024)                     //Bytes -> KB
        rtn = QString("%1 Bytes").arg(size);
    else if(size < 1048576L)            //KB -> MB
        rtn = QString::number((size / 1024.0), f, dp) + " KB";
    else if(size < 1073741824UL)        //MB -> GB
        rtn = QString::number((size / 1048576.0), f, dp) + " MB";
    else                                // Affirmative GB
        rtn = QString::number((size / 1073741824.0), f, dp) + " GB";
    return rtn;
}

ActionsButtonPanel::ActionsButtonPanel(QWidget *parent)
    :   QWidget(parent)
{
    //createDefaultExtensions();
    QVBoxLayout *mainLayout         = new QVBoxLayout(this);
    filter_pushButton               = new QPushButton("&Filter", this);
    sort_pushButton                 = new QPushButton("&Sort", this);
    reset_pushButton                = new QPushButton("&Reset", this);
    markings_pushButton             = new QPushButton("Markings", this);
    autoSelection_pushButton        = new QPushButton("&Auto Selection", this);
    commit_pushButton               = new QPushButton("&Commit", this);
    auto_pushButton                 = new QPushButton("A&uto", this);
    filterDialog                    = new FilterDialog(this);

    sort_pushButton->setCheckable(true);
    markings_pushButton->setCheckable(true);
    autoSelection_pushButton->setCheckable(true);

    QFrame* line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);

    QFrame* line2 = new QFrame(this);
    line2->setFrameShape(QFrame::HLine);

    QFrame* line3 = new QFrame(this);
    line3->setFrameShape(QFrame::HLine);

    mainLayout->addWidget(commit_pushButton);
    mainLayout->addWidget(line);
    mainLayout->addWidget(filter_pushButton);
    mainLayout->addWidget(sort_pushButton);
    mainLayout->addWidget(reset_pushButton);
    mainLayout->addWidget(line2);
    mainLayout->addWidget(autoSelection_pushButton);
    mainLayout->addWidget(markings_pushButton);
    mainLayout->addWidget(line3);
    mainLayout->addWidget(auto_pushButton);
    mainLayout->addStretch();
    auto_pushButton->setEnabled(false);

    sortContextMenu.addAction("&Descending Order of File size", this, SLOT(sortByDescendingFileSize()));
    sortContextMenu.addAction("&Ascending Order of File size", this, SLOT(sortByAscendingFileSize()));

    autoSelectionContextMenu.addAction("Select Group Keeps", this, SIGNAL(selectGroupKeeps()));
    autoSelectionContextMenu.addAction("Select Group Deletes", this, SIGNAL(selectGroupDeletes()));
    autoSelectionContextMenu.addAction("Select Next Group Keeps", this, SIGNAL(selectNextGroupKeeps()));
    autoSelectionContextMenu.addAction("Select Next Group Deletes", this, SIGNAL(selectNextGroupDeletes()));
    autoSelectionContextMenu.addAction("Auto Select Next &Possibilities", this, SIGNAL(autoSelectDeletePossibilities()))
           ;// ->setEnabled(false);
    autoSelectionContextMenu.addAction("Auto Select Highest &Keeps", this, SIGNAL(autoSelectNextKeeps()));
    autoSelectionContextMenu.addAction("Auto Select Highest &Deletes", this, SIGNAL(autoSelectNextDeletes()));

    markingsContextMenu.addAction("Unmark &All", this, SIGNAL(unmarkAll()));
    markingsContextMenu.addAction("Unmark All &Keeps", this, SIGNAL(unmarkAllKeeps()));
    markingsContextMenu.addAction("Unmark all &Deletes", this, SIGNAL(unmarkAllDeletes()));

    connect(reset_pushButton, SIGNAL(clicked()), this, SIGNAL(resetRequested()));
    connect(commit_pushButton, SIGNAL(clicked()), this, SIGNAL(commitRequested()));

    connect(filter_pushButton, SIGNAL(clicked()), this, SLOT(filterContextMenuClicked()));
    connect(sort_pushButton, SIGNAL(clicked()), this, SLOT(processSortRequest()));
    connect(autoSelection_pushButton, SIGNAL(clicked()), this, SLOT(processAutoSelectionClicked()));
    connect(markings_pushButton, SIGNAL(clicked()), this, SLOT(processMarkingsClicked()));
    connect(auto_pushButton, SIGNAL(clicked()), this, SIGNAL(autoCleanDeletes()));

    connect(&sortContextMenu, SIGNAL(aboutToHide()), this, SLOT(sortContextMenuAboutToHide()));
    connect(&autoSelectionContextMenu, SIGNAL(aboutToHide()), this,
            SLOT(autoSelectionContextMenuAboutToHide()));
    connect(&markingsContextMenu, SIGNAL(aboutToHide()), this,
            SLOT(markingsContextMenuAboutToHide()));
    connect(&filterContextMenu, SIGNAL(aboutToHide()), this,
            SLOT(filterContextMenuAboutToHide()));
    connect(filterDialog, SIGNAL(aboutToHide()), this, SLOT(filterContextMenuAboutToHide()));
    connect(filterDialog, SIGNAL(filterByExtension(QStringList)), this, SIGNAL(filterByExtension(QStringList)));
    connect(filterDialog, SIGNAL(filterByPath(QString)), this, SIGNAL(filterByPath(QString)));
    connect(filterDialog, SIGNAL(filterByRegex(QRegExp)), this, SIGNAL(filterByRegex(QRegExp)));
    connect(filterDialog, SIGNAL(filterBySize(ulong,ulong)), this, SIGNAL(filterBySize(ulong,ulong)));
}

void ActionsButtonPanel::triggerCommitRequested()
{
    commitRequested();
}

void ActionsButtonPanel::sortByDescendingFileSize()
{
    sortingRequested(Qt::DescendingOrder);
}

void ActionsButtonPanel::sortByAscendingFileSize()
{
    sortingRequested(Qt::AscendingOrder);
}

void ActionsButtonPanel::processSortRequest()
{
    sort_pushButton->setChecked(true);
    sortContextMenu.exec( QCursor::pos() );
}

void ActionsButtonPanel::processAutoSelectionClicked()
{
    autoSelection_pushButton->setChecked(true);
    autoSelectionContextMenu.exec(QCursor::pos());
}

void ActionsButtonPanel::processMarkingsClicked()
{
    markings_pushButton->setChecked(true);
    markingsContextMenu.exec(QCursor::pos());
}

void ActionsButtonPanel::filterContextMenuClicked()
{
    filter_pushButton->setChecked( true );
    //filterContextMenu.exec( QCursor::pos() );
    QPoint point = QCursor::pos();
    point.setY(point.y() - 80);
    filterDialog->setGeometry( QRect(point, filterDialog->sizeHint()) );
    filterDialog->show();
}

void ActionsButtonPanel::filterContextMenuAboutToHide()
{
    //filter_pushButton->setChecked(false);
    filter_pushButton->setChecked( false );
}

void ActionsButtonPanel::sortContextMenuAboutToHide()
{
    sort_pushButton->setChecked(false);
}

void ActionsButtonPanel::autoSelectionContextMenuAboutToHide()
{
    autoSelection_pushButton->setChecked(false);
}

void ActionsButtonPanel::markingsContextMenuAboutToHide()
{
    markings_pushButton->setChecked(false);
}
