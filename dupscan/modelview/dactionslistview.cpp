#include <QPainter>
#include <QStaticText>
#include "dactionslistview.hpp"
#include "dupscan/modelview/ditem.hpp"
#include <boost/filesystem.hpp>

Q_DECLARE_METATYPE(DItem)

DActionsListView::DActionsListView(QWidget *parent) :
    QListView(parent)
{
}


DActionsListDelegate::DActionsListDelegate(QWidget *parent) :
    QStyledItemDelegate(parent)
{
    //
}

void DActionsListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);
    DItem item = index.data().value<DItem>();
    DLS::FileProperty fprop = item.property;
    QString name;
    QString size;

    QFontMetrics metrics(option.font);
    QRect rect_major = option.rect;
    rect_major.translate( 3, 3 );
    //Need to trim... but hey!, translation alone serves us for now...
    QRect rect_minor = rect_major;
    const QPen originalPen = painter->pen();

    QPen borderHighlightPen(originalPen);
    borderHighlightPen.setColor( QColor(Qt::red) );
    painter->setPen( borderHighlightPen );
    painter->drawRect( option.rect );
    painter->setPen(originalPen);

    const QFont font = painter->font();
    QFont fnt = font;
    fnt.setBold(true);

    if(item.isGroupHeader)
    {
        name = item.header.topString();
        int headerC = item.header.itemCount();

        if(item.header.value())
            size = fileSizeAsString( item.header.value() * headerC ) + " in total";
        else
            size =  "Not Available";

        if(name.isEmpty())
            return;
        QString freeable( fileSizeAsString( (headerC - 1) * item.header.value()) + " can be freed" );
        QString fileCounts( QString::number( headerC ) + " Files ::  " + QString::number( headerC - 1)
            + " duplicates" );
        //qreal dx = rect_major.width() / 3.0;
        //painter->translate( dx, 0 );

        painter->setFont( fnt );
        painter->setPen( QColor(20, 40, 30) );
        painter->drawText(rect_major, Qt::AlignLeft | Qt::AlignTop, name, &rect_minor);
        rect_minor.translate(0, rect_minor.height() );
        painter->setPen( QColor(120, 50, 70) );
        painter->drawText(rect_minor, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, size, &rect_major);
        painter->setFont( font );
        rect_major.translate( rect_major.width() + 10, 0);
        rect_major.setWidth( rect_major.width() + 600 );
        painter->drawText(rect_major, Qt::AlignTop | Qt::AlignLeft, fileCounts, &rect_minor);
        //rect_major.translate( metrics.width( fileNameStatic.text() ) , 0);
        rect_minor.translate( rect_minor.width() + 10, 0 );
        rect_minor.setWidth( rect_minor.width() + 30 );
        painter->drawText(rect_minor, Qt::AlignLeft | Qt::AlignTop, freeable);
        return;
    }

    name = fileNameAsString( fprop.getFilePath() );
    size = fileSizeAsString( fprop.getSize() );
    if(name.isEmpty())
        return;

    QStaticText fileSizeStatic( "File Size: ");
    QStaticText fileNameStatic( "File Name: ");
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
    painter->drawText(rect_minor, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, size, &rect_major);
    painter->setFont( font );

    //restore original
    painter->setPen( originalPen );
}

QSize DActionsListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    (void)index;
    QFontMetrics metrics( option.font );
    return QSize(option.rect.width(), (metrics.height() * 2) + 6);
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
