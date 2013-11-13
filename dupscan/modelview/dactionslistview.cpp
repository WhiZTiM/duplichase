#include "dactionslistview.hpp"
#include "dupscan/modelview/ditem.hpp"
#include <boost/filesystem.hpp>
#include <QMenu>
#include <QCursor>
#include <QAction>
#include <QToolTip>
#include <QPainter>
#include <QFileInfo>
#include <QKeyEvent>
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

DActionsListView::DActionsListView(QWidget *parent) :
    QListView(parent)
{
    //
}

void DActionsListView::processGroupHeaderSelected(QModelIndex index)
{
    Q_UNUSED(index)
    //
}

DActionsListDelegate::DActionsListDelegate(QWidget *parent) :
    QStyledItemDelegate(parent)
{
    //
    contextMenu.addAction("&Open Directory", this, SLOT(action_openDirectory()));
    contextMenu.addAction("Open &File", this, SLOT(action_openFile()));
    contextMenu.addSeparator();
    contextMenu.addAction("Mark for keep");
    contextMenu.addAction("Mark for Deletion");
    contextMenu.addSeparator();
    contextMenu.addAction("Delete Now");
}

void DActionsListDelegate::action_openFile()
{
    DItem item = currentIndex.data().value<DItem>();
    if(item.isGroupHeader)
        return;
    QString fname(QString::fromStdString( item.property.getFilePath() ));
    QDesktopServices::openUrl( QUrl::fromLocalFile( fname ) );
}

void DActionsListDelegate::action_openDirectory()
{
    DItem item = currentIndex.data().value<DItem>();
    if(item.isGroupHeader)
        return;

    QString fname( parent_of_generic_path( item.property.getFilePath() ));
    QDesktopServices::openUrl( QUrl::fromLocalFile( fname ) );
}

bool DActionsListDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
#if 1
    if(event->type() == QEvent::MouseButtonPress)
    {
        //
        std::cerr << "\nAccepted Mouse Release" << std::endl;
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

        if(mouseEvent->button() == Qt::RightButton)
        {
            QRect rect(mouseEvent->globalPos(), contextMenu.sizeHint());
            contextMenu.setGeometry( rect );
            currentIndex = index;
            contextMenu.show();
        }
        //if( )
    }
    if(event->type() == QEvent::MouseMove)
    {
        if(QToolTip::isVisible())
            return false;
        QString data = index.data(Qt::ToolTipRole).toString();
        QToolTip::showText(QCursor::pos(), data);
    }
    if(event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease)
    {
        std::cerr << "KEY Pressed " << std::endl;
    }
    if(event->type() == QEvent::ContextMenu)
    {
        std::cerr << "ContextMenu" << std::endl;
    }
#endif
    return false;
}

void DActionsListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //QStyledItemDelegate::paint(painter, option, index);
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
    return QString::fromStdString(boost::filesystem::path(name).native());
}

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
    QVBoxLayout *mainLayout         = new QVBoxLayout(this);
    filter_pushButton               = new QPushButton("&Filter", this);
    sort_pushButton                 = new QPushButton("&Sort", this);
    reset_pushButton                = new QPushButton("&Reset", this);
    autoSelectKeep_pushButton       = new QPushButton("AutoSelect &Keep", this);
    autoSelectDelete_pushButton     = new QPushButton("AutoSelect &Deletion", this);
    sort_pushButton->setCheckable(true);

    QFrame* line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);

    mainLayout->addWidget(filter_pushButton);
    mainLayout->addWidget(sort_pushButton);
    //mainLayout->addWidget(line);
    mainLayout->addWidget(reset_pushButton);
    mainLayout->addWidget(line);
    mainLayout->addWidget(autoSelectKeep_pushButton);
    mainLayout->addWidget(autoSelectDelete_pushButton);
    mainLayout->addStretch();

    sortContextMenu.addAction("&Descending Order of File size", this, SLOT(sortByDescendingFileSize()));
    sortContextMenu.addAction("&Ascending Order of File size", this, SLOT(sortByAscendingFileSize()));

    connect(sort_pushButton, SIGNAL(clicked()), this, SLOT(processSortRequest()));
    connect(filter_pushButton, SIGNAL(clicked()), this, SIGNAL(filteringRequested()));
    connect(reset_pushButton, SIGNAL(clicked()), this, SIGNAL(resetRequested()));
    connect(&sortContextMenu, SIGNAL(aboutToHide()), this, SLOT(sortContextMenuAboutToHide()));
}

void ActionsButtonPanel::processSortRequest()
{
    sortContextMenu.setGeometry( QRect( QCursor::pos(), sortContextMenu.sizeHint() ) );
    sort_pushButton->setChecked(true);
    sortContextMenu.show();
    sortContextMenu.exec();
}

void ActionsButtonPanel::sortByDescendingFileSize()
{
    sortingRequested(Qt::DescendingOrder);
}

void ActionsButtonPanel::sortByAscendingFileSize()
{
    sortingRequested(Qt::AscendingOrder);
}

void ActionsButtonPanel::sortContextMenuAboutToHide()
{
    sort_pushButton->setChecked(false);
}
