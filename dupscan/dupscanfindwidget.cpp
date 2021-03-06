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
#include "backend/include/duplicatefinder.hpp"
#include "dupscan/modelview/filepropertyserializer.hpp"
#include "useful_qt_functions.hpp"
#include "dupscanfindwidget.hpp"
#include <functional>
#include <cstdlib>
#include <cmath>
#include <QUrl>
#include <QFile>
#include <QLabel>
#include <QTimer>
#include <QDebug>
#include <QFuture>
#include <QPainter>
#include <QMetaType>
#include <QListWidget>
#include <QVBoxLayout>
#include <QStaticText>
#include <QMutexLocker>
#include <QStackedLayout>
#include <QStandardPaths>
#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QtConcurrent/QtConcurrent>

// Resolving Political Dependancy
Q_DECLARE_METATYPE(DLS::FileProperty)
Q_DECLARE_METATYPE(QVector<int>)
Q_DECLARE_METATYPE(LOGType)
//END

class DupScanFindWorker : public QObject
{
    //
};


DupScanFindWidget::DupScanFindWidget(QWidget *parent) :
    QWidget(parent), viewLimit(20)
{
    qRegisterMetaType<LOGType> ("LOGType");
    qRegisterMetaType<QVector<int>> ("QVector<int>");
    qRegisterMetaType<DLS::FileProperty> ("DLS::FileProperty");

    scannerPaused = false;
    counterPaused = false;
    cancelTransversal = false;
    cancelSCanning = false;
    superSpeedSet = false;
    suspendAllScanners = false;
    scannerSleepInterval = 0;

    mainThread = QThread::currentThread();  //use it for pausing

    mainStackedLayout = new QStackedLayout(this);
    fileViewDelegate = new FileViewDelegate(this);
    fileListingWidget = new QListWidget(this);
    fileListingWidget->setItemDelegate(fileViewDelegate);
    fileListingWidget->setSelectionMode(QListWidget::SingleSelection);
    mainStackedLayout->addWidget(fileListingWidget);

    superSpeedLabel = new QLabel(this);
    mainStackedLayout->addWidget(superSpeedLabel);

    /*
    //Political Test Case
    DLS::FileProperty prop;
    prop.setSize(435325);
    prop.setFilePath("/home/whiztim/Documents/developer");
    QListWidgetItem* item = new QListWidgetItem;
    item->setData(Qt::DisplayRole, QVariant::fromValue<DLS::FileProperty>(prop));
    //item->setSizeHint(QSize(item->sizeHint().width(), item->sizeHint().height() + 30) );
    fileListingWidget->addItem(item);
    item->setBackground(QColor(70, 200, 30));
    //item->setBackground(QBrush(Qt::red));

    //fileListingWidget->addItem("<qt><b>THE REST</b></qt>");

    DLS::FileProperty prop3;
    prop3.setSize(654653255);
    prop3.setFilePath("/home/whiztim/Documents/developer/C++/Qt/eret/343533/eretdgerererer/ erere ergegergerr/33433ertrt/rgrfgfgd/fdg/er/ertrertegfdgd/dfgrthtrteer");
    QListWidgetItem* item3 = new QListWidgetItem;
    item3->setData(Qt::DisplayRole, QVariant::fromValue<DLS::FileProperty>(prop3));
    //item3->setFlags(Qt::NoItemFlags);
    fileListingWidget->addItem(item3);
    //item3->setSizeHint(QSize(item->sizeHint().width(), item->sizeHint().height() + 30) );

    DLS::FileProperty prop2;
    prop2.setSize(45465325);
    prop2.setFilePath("/home/whiztim/Documents/developer/C++/Qt");
    QListWidgetItem* item2 = new QListWidgetItem;
    item2->setData(Qt::DisplayRole, QVariant::fromValue<DLS::FileProperty>(prop2));
    fileListingWidget->addItem(item2);
    //item2->setSizeHint(QSize(item->sizeHint().width(), item->sizeHint().height() + 30) );
    //End Politics
    */
    connect(this, SIGNAL(scanProgress(int)), this, SLOT(testUpdateSlot(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(filePropertyAddSignal(DLS::FileProperty,int)), this, SLOT(addDLSFileProperty(DLS::FileProperty,int)), Qt::QueuedConnection);
    //connect(this, SIGNAL(logMessage(LOGType,QString,QString)), this, SLOT(processLogMessage(LOGType,QString,QString)));
    connect(this, SIGNAL(logMessage(LOGType,QString,QString)), &logFormatter, SLOT(formatAndWriteLogMessage(LOGType,QString,QString)), Qt::QueuedConnection);
    connect(&logFormatter, SIGNAL(logMessage(QString)), this, SIGNAL(logMessage(QString)), Qt::QueuedConnection);

    //QTimer::singleShot(8000, this, SLOT(test()));
    setUpAuxilliaries();
}

void DupScanFindWidget::startScanner(const QStringList &scanFolders, const QStringList &exclusionFolders)
{
    if(scannerFuture.isRunning() || scannerFuture.isPaused())
    {
        logMessage(LOGType::Error, "ERROR",
                   "Attempting to start a scan process while another is is currently in progress");
        return;
    }
    continueScanning();
    cancelSCanning = cancelTransversal = false;
    std::cerr << "Main THREAD: " << (int) QThread::currentThreadId() << std::endl;
    scannerFuture = QtConcurrent::run(this, &DupScanFindWidget::startInternalScanner, scanFolders, exclusionFolders);
    processRunningtime.start();
}


DLS::DuplicatesContainer DupScanFindWidget::getDuplicateContainer()
{
    std::cerr << "Main THREAD: " << (int) QThread::currentThreadId() << std::endl;
    return duplicates;
}


//Public Slots
void DupScanFindWidget::setViewLimit(unsigned long limit)
{
    viewLimit = limit;
}

void DupScanFindWidget::test()
{
    QStringList list("/home/whiztim/Documents/developer");
    QStringList ex; //("/etc/");

    startScanner(list, ex);
}


void DupScanFindWidget::toggleSuperSpeed(bool value)
{
    superSpeedSet = value;
    if(superSpeedSet)
        mainStackedLayout->setCurrentIndex(1);  //Static Text Widget
    else
        mainStackedLayout->setCurrentIndex(0);  //listing Widget

    logMessage(LOGType::Info, "Information" ,
               (QString("Super Speed Mode has been turned <b>") + (superSpeedSet ? "ON" : "OFF") + "</b>"));
}

bool DupScanFindWidget::scanningJobRunning() const
{
    return scannerFuture.isRunning() && !scannerPaused;
}

bool DupScanFindWidget::scanningJobFinished() const
{
    return scannerFuture.isFinished();
}

bool DupScanFindWidget::scanningJobPaused() const
{
    return scannerPaused;
}

bool DupScanFindWidget::stopScanning()
{
    bool rtn = false;
    if(scannerFuture.isRunning())
    {
        cancelSCanning = cancelTransversal = true;
        rtn = true;
        scannerFuture.cancel();
        continueScanning();
    }
    return rtn;
}

void DupScanFindWidget::pauseScanning()
{
    if(mainThread != QThread::currentThread())
        return;
    if(!scannerPaused)
    {
        scannerPaused = true;
    }
    if(!counterPaused)
    {
        counterPaused = true;
    }
    update();
    std::cout << "Paused" << std::endl;
}

void DupScanFindWidget::continueScanning()
{
    if(mainThread != QThread::currentThread())
        return;
    if(scannerPaused)
    {
        scannerPaused = false;
        scannerWaitCondition.wakeOne();
    }
    if(counterPaused)
    {
        counterPaused = false;
        counterWaitContition.wakeOne();
    }
    update();
    std::cout << "Continued" << std::endl;
}

bool DupScanFindWidget::resultsReady() const
{
    return scanningJobFinished();
}

//! this function inverts the input value gotten
void DupScanFindWidget::setScanSpeed(int value)
{
    if(value == 0)
    {
        scannerSleepInterval = 100;
        return;
    }
    scannerSleepInterval = (100.0 / value) + (80.0 / value);
}

void DupScanFindWidget::testUpdateSlot(int k)
{
    (void)k;
    //std::cout << std::endl << "UPDATE PROGRESS BAR: " << k << " %\n";
}


// Private Functions
bool DupScanFindWidget::startInternalScanner(const QStringList scanFolders, const QStringList exclusionFolders)
{
    try
    {
        //
        logMessage(LOGType::Error, "NEW Scan Operation: - Initializing DupLichaSe Scanning Systems", "");

        DLS::Tree::DirectoryElementHelper helper(true);
        for(QString exclusion : exclusionFolders)
        {
            QString flink = QUrl::fromLocalFile(exclusion).toString();
            QString body = "<a href=\"" + flink + "\">" + exclusion + "</a>";
            logMessage(LOGType::DLSCoreInfo, "Adding Exclusion Path:", body);
            helper.addPath(exclusion.toStdString());
        }

        DLS::ExclusionHandler exclusion( helper.getDirectoryElement() );
        DLS::DuplicateFinderConstruct construct(exclusion);

        for(QString inclusion : scanFolders)
        {
            QString flink = QUrl::fromLocalFile(inclusion).toString();
            QString body = "<a href=\"" + flink + "\">" + inclusion + "</a>";
            logMessage(LOGType::DLSCoreInfo, "Adding Search Path:", body);
            construct.addPath(inclusion.toStdString());
        }

        DLS::DuplicateFinder dfCon(construct);

        std::cerr << "WORKER THREAD main: " << (int) QThread::currentThreadId() << std::endl;
        //Lambda function for checking if COunts Prepared
        auto transversalWatch = [&] () -> bool
            {
                std::cerr << "WORKER THREAD Transversal: " << (int) QThread::currentThreadId() << std::endl;
                logMessage(LOGType::DLSCoreInfo, "DupLichaSe-Core", "initializing Ahead-Of-Time File Counting");
                try
                {
                logMessage(LOGType::DLSCoreInfo, "DupLichaSe-Core", "SubSystem Ahead-Of-Time Scanner started");
                    while(!dfCon.CountsPrepared())
                    {
                        if(cancelTransversal)
                        {
                            logMessage(LOGType::DLSCoreWarning, "Signal Fired", "FILE COUNTING ABORTED BY USER!");
                            return false;
                        }
                        QThread::msleep(200);
                    }
                    transversedFileCounts = dfCon.totalCount();
                    emit finishedPathTransversal(true, transversedFileCounts);
                    return true;
                }
                catch(...)
                {
                    logMessage(LOGType::DLSCoreError, "FATAL ERROR",
                               "An uncaught EXCEPTION was thrown from the Counting Subsystem");
                }
                transversedFileCounts = dfCon.totalCount();
                emit finishedPathTransversal(false, transversedFileCounts);
                return false;
            };
        //Lambda end

        transversalFuture = QtConcurrent::run(transversalWatch);

        int temp_counter = 0;

        //try
        {
            while(!dfCon.endend())
            {
                if(cancelSCanning)
                {
                    logMessage(LOGType::DLSCoreWarning, "DupLichaSe-Core",
                               "Duplicate Finder Subsystem was shutdown by user");
                    dfCon.endCounting( true );
                    break;
                }

                filePropertyAddSignal( dfCon.nextFile(), dfCon.ruleDepth() );
                QString error( QString::fromStdString( dfCon.getCurrentError() ) );

                if(scannerSleepInterval)
                    QThread::msleep(scannerSleepInterval);
                if(!error.isEmpty())
                {
                    logMessage(LOGType::DLSCoreWarning, "DupLichaSe-Core: Scanner Subsystem message", error);
                    continue;
                }
                if(++temp_counter > 10)     //Fire after every 10 rounds
                {
                    updateScanProgressSignal(dfCon.getCount());
                    temp_counter = 0;
                }
                if(scannerPaused)
                {
                    QMutexLocker locker(&scannerMutex);
                    scannerWaitCondition.wait(&scannerMutex);
                }
            }
            //updateScanProgressSignal(dfCon.totalCount());
            duplicates = dfCon.getDuplicatesContainer();
            attempt_serialization_to_disk();

            transversalFuture.waitForFinished();
            emit finishedScanning(true);
            return true;
        }

    }
    catch(std::exception& e)
    {
        std::cout << "\n" << e.what() << std::endl;
    }

    catch(...)
    {
        logMessage(LOGType::DLSCoreError, "DupLiChaSe-Core::FATAL ERROR", "An uncaught EXCEPTION was thrown from the Scanning Subsystem!");
    }
    emit finishedScanning(false);
    return false;
}


void DupScanFindWidget::updateScanProgressSignal(unsigned long value)
{

    // While The Counting Scanner isn't finished yet... we update the progress bar in a
    // logarithmic manner.
    if(scanningJobRunning())
    {
        if(!transversalFuture.isFinished())
        {
            auto logValue = std::log(processRunningtime.elapsed());
            if(logValue > 50)   //we do not want this to increase above 50
                return;
            int val = static_cast<int>(logValue);
            emit scanProgress(val);
            return;
        }
        else    //finished
        {
            if(!transversedFileCounts)
                return;
            qreal f_val = 100.0 * value / transversedFileCounts;
            int val = std::ceil(f_val);
            val = val > 100 ? 100 : val;
            emit scanProgress(val);
        }
    }

}


void DupScanFindWidget::setUpAuxilliaries()
{
    QString label =
            "<h1>Super Speed Mode Enabled</h1>"
            "<h3>"
            "<br /> RealTime Updates has been disabled for this mode to Work"
            "<br /> Updating You realtime is at the expense of CPU time"
            "<br /> Disabling realtime Updates reduces scan time by up to 20%"
            "<br /> If you want realtime updates, please disable Super Speed Scan"
            "</h3>";
    superSpeedLabel->setTextFormat(Qt::RichText);
    superSpeedLabel->setText(label);
    superSpeedLabel->setAlignment(Qt::AlignCenter);
    connect(this, SIGNAL(finishedPathTransversal(bool,ulong)), this, SLOT(processFinshedPathTransversal(bool,ulong)), Qt::QueuedConnection);
    connect(this, SIGNAL(finishedScanning(bool)), this, SLOT(processFinshedScanning(bool)), Qt::QueuedConnection);
}

void DupScanFindWidget::attempt_serialization_to_disk()
{
    if(duplicates.empty())
        return;
    QStringList documentsPath = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
    if(documentsPath.isEmpty())
        return;
    QString tempPath = documentsPath.first();

    boost::filesystem::path pt(tempPath.toStdString());
    tempPath = QString::fromStdString(pt.generic_string());
    tempPath += "/duplichase_last_scan.dlsr";

    FilePropertySerializer serializer;
    if(serializer.toDisk(tempPath, duplicates))
    {
        logMessage(LOGType::Info, "Scan Results Backed UP to:", tempPath);
        return;
    }
    logMessage(LOGType::Error, "Failed to Back-Up Scan Results to:", tempPath);

}


//PRIVATE SLOTS
void DupScanFindWidget::addDLSFileProperty(const DLS::FileProperty &fileproperty, int id)
{
    if(superSpeedSet)
        return;

    if((viewLimit != 0) && ( (unsigned) fileListingWidget->count() >= viewLimit))
        delete fileListingWidget->item(0);

    QListWidgetItem* item = new QListWidgetItem;
    item->setData( Qt::DisplayRole, QVariant::fromValue<DLS::FileProperty>(fileproperty) );
    fileListingWidget->addItem(item);

    switch (id) {
    case 0:
        break;
    case 1:
        item->setBackground(QColor(7, 245, 250));
        break;
    case 2:
        item->setBackground(QColor(60, 200, 20));
        break;
    case 3:
        item->setBackground(QColor(200, 120, 20));
        break;
    case 4:
        item->setBackground(QColor(10, 210, 100));
        break;
    default:
        break;
    }

    //fileListingWidget->repaint(); --> Causes a crash!
    update();
}

void DupScanFindWidget::processFinshedPathTransversal(bool completed, unsigned long counts)
{
    QString header("DupLichase-Core"), msg;
    if(completed)
    {
        msg = "Successfully completed Ahead of Time File Transversal in <b>"
                + returnTimeInString(processRunningtime.elapsed()) + "</b>";
        logMessage(LOGType::DLSCoreInfo, header, msg);
        logMessage(LOGType::Info, "Information",
                   "Total Files That would be scanned: <b>" + QString::number(counts) + "</b>" );
        logMessage(LOGType::Info, "Information", "Progress Bar is Now Assured Accurate!");
    }
    else
    {
        msg =   "OOPS! Something went wrong during Ahead of Time File Transversal!!";
        logMessage(LOGType::DLSCoreError, header, msg);
        logMessage(LOGType::Warning, "Warning",
                "Progress Bar may NOT report correct values"
                "<br />So also does DupLichaSe Core have an increased tendency to fail "
                "ANYTIME during this scan!!<br />");
        logMessage(LOGType::Info, "Blind Information",
                "If it fails Please Re-run Duplichase again! <br />"
                "On Subsequent failures, restart your Computer and Re-run DupLichaSe<br />"
                "As a Last resort, please File a Complaint! Menu -> Help -> Report a Problem");
    }
}

void DupScanFindWidget::processFinshedScanning(bool succeeded)
{
    if(succeeded)
        logMessage(LOGType::Info, "Scanning Completed",
                   "Successfully found all Duplicate Files accessible to DupLichaSe");
    else
        logMessage(LOGType::Warning, "Scanning Completed",
                   "Scanner May have encountered Errors... Please pay attention to the results");
    logMessage(LOGType::Info, "Information",
               ("<b>Scan lasted for " + returnTimeInString(processRunningtime.elapsed()) + "</b>"));
}

DupScanFindWidget::~DupScanFindWidget()
{
    cancelTransversal = true;
    cancelSCanning = true;
    scannerWaitCondition.wakeAll();
    counterWaitContition.wakeAll();
    transversalFuture.cancel();
    transversalFuture.waitForFinished();
    scannerFuture.cancel();
    scannerFuture.waitForFinished();
}


///////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////



FileViewDelegate::FileViewDelegate(QWidget *parent) :
    QStyledItemDelegate(parent)
{
    //Nathing here
}

QSize FileViewDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    (void)index;
    QFontMetrics metrics( option.font );
    return QSize(option.rect.width(), (metrics.height() * 2) + 6);
}

void FileViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);
    DLS::FileProperty fprop = index.data().value<DLS::FileProperty>();
    QString name = fileNameAsString( fprop.getFilePath() );
    QString size = fileSizeAsString( fprop.getSize() );
    if(name.isEmpty())
        return;

    const QFontMetrics& metrics = option.fontMetrics;
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

    QStaticText fileNameStatic("File Name: ");
    QStaticText fileSizeStatic("File Size: ");
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

QString FileViewDelegate::fileSizeAsString(const ulong size) const
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

QString FileViewDelegate::fileNameAsString(const std::string &name) const
{
#ifdef Q_OS_WIN32
    return QString::fromStdString(boost::filesystem::path(name).string());
#else
    return QString::fromStdString(boost::filesystem::path(name).native());
#endif
}

QString FileViewDelegate::fileNameAsString(const QString &name) const
{
#ifdef Q_OS_WIN32
    return QString::fromStdString(boost::filesystem::path(name.toStdString()).string());
#else
    return QString::fromStdString(boost::filesystem::path(name.toStdString()).native());
#endif
}


/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
///
///
