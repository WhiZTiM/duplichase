#ifndef DUPSCANFINDWIDGET_HPP
#define DUPSCANFINDWIDGET_HPP

#include <chrono>
#include <QTime>
#include <QWidget>
#include <QFuture>
#include <QStyledItemDelegate>
#include "backend/include/container_helpers/fp_holders.hpp"

QT_BEGIN_NAMESPACE
class QLabel;
class QPainter;
class QListWidget;
class QModelIndex;
//class QFutureWatcher;
class QStackedLayout;
class QStyleOptionViewItem;
QT_END_NAMESPACE

namespace DLS {
    class FileProperty;
    //class DuplicatesContainer;
}

enum class LOGType
{
    DLSCoreWarning,
    DLSCoreError,
    DLSCoreInfo,
    Info,
    Error,
    Warning,
    None
};

class FileViewDelegate : public QStyledItemDelegate
{
public:
    FileViewDelegate(QWidget *parent = 0);
    QString fileNameAsString(const QString& name) const;
    QString fileNameAsString(const std::string& name) const;
    QString fileSizeAsString(const ulong size) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    QString _name;
    QString _size;
};


typedef QList<DLS::FileProperty> QList_FileProperty;

class DupScanFindWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DupScanFindWidget(QWidget *parent = 0);
    ~DupScanFindWidget();
    void startScanner(const QStringList& scanFolders, const QStringList& exclusionFolders);
    DLS::DuplicatesContainer getDuplicateContainer();
    //void testSuite();
    
signals:
    void finishedPathTransversal(bool, unsigned long);
    void filePropertyAddSignal(const DLS::FileProperty&, int);
    void finishedScanning(bool);
    void scanProgress(int);
    void logMessage(QString);
    void logMessage(LOGType logType, QString header, QString body);

public slots:
    bool scanningJobPaused() const;
    bool scanningJobRunning() const;
    bool scanningJobFinished() const;
    bool resultsReady() const;
    void stopScanning();
    void pauseScanning();
    void continueScanning();
    void setViewLimit(unsigned long limit);
    void toggleSuperSpeed(bool);
    void setScanSpeed(int);
    void test();
    void testUpdateSlot(int);

private slots:
    void processFinshedPathTransversal(bool, unsigned long);
    void processLogMessage(LOGType logtype, QString header, QString body);
    void addDLSFileProperty(const DLS::FileProperty& fileproperty, int id = 0);
    //void outOfThreadLogMessage(QString);

private:
    FileViewDelegate *fileViewDelegate;
    QStackedLayout *mainStackedLayout;
    QListWidget *fileListingWidget;
    QLabel *superSpeedLabel;
    QFuture<bool> scannerFuture;
    QFuture<bool> transversalFuture;
    QTime progressTimer;
    QTime processRunningtime;
    QTime processRunningTimeStart;

    unsigned long viewLimit;
    unsigned long transversedFileCounts = 0;
    bool cancelTransversal = false;
    bool cancelSCanning = false;
    volatile bool superSpeedSet = false;
    volatile int scannerSleepInterval = 0;

    bool startInternalScanner(const QStringList scanFolders, const QStringList exclusionFolders);
    void updateScanProgressSignal(unsigned long value);
    void setUpAuxilliaries();

    DLS::DuplicatesContainer duplicates;
};

#endif // DUPSCANFINDWIDGET_HPP
