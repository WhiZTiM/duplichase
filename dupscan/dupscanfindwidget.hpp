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
#ifndef DUPSCANFINDWIDGET_HPP
#define DUPSCANFINDWIDGET_HPP

#include <chrono>
#include <atomic>
#include <QTime>
#include <QWidget>
#include <QMutex>
#include <QFuture>
#include "logformatter.hpp"
#include <QStyledItemDelegate>
#include <QWaitCondition>
#include "backend/include/container_helpers/fp_holders.hpp"

QT_BEGIN_NAMESPACE
class QLabel;
class QThread;
class QPainter;
class QListWidget;
class QModelIndex;
//class QFutureWatcher;
class QStackedLayout;
class QStyleOptionViewItem;
QT_END_NAMESPACE

class DupScanFindWorker;

namespace DLS {
    class FileProperty;
    //class DuplicatesContainer;
}

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
    void startScanner(const QStringList& scanFolders, const QStringList& exclusionFolders);
    bool scanningJobPaused() const;
    bool scanningJobRunning() const;
    bool scanningJobFinished() const;
    bool resultsReady() const;
    bool stopScanning();
    void pauseScanning();
    void continueScanning();
    void setViewLimit(unsigned long limit);
    void toggleSuperSpeed(bool);
    void setScanSpeed(int);
    void test();
    void testUpdateSlot(int);

private slots:
    void processFinshedPathTransversal(bool, unsigned long);
    void processFinshedScanning(bool succeeded);
    //void processLogMessage(LOGType logtype, QString header, QString body);
    void addDLSFileProperty(const DLS::FileProperty& fileproperty, int id = 0);
    //void outOfThreadLogMessage(QString);

private:
    DupScanFindWorker *worker;
    FileViewDelegate *fileViewDelegate;
    QStackedLayout *mainStackedLayout;
    QListWidget *fileListingWidget;
    QLabel *superSpeedLabel;
    QThread *mainThread;
    QFuture<bool> scannerFuture;
    QFuture<bool> transversalFuture;
    QTime progressTimer;
    QTime processRunningtime;
    QTime processRunningTimeStart;
    QMutex scannerMutex;
    QMutex counterMutex;
    QWaitCondition scannerWaitCondition;
    QWaitCondition counterWaitContition;

    unsigned long viewLimit;
    unsigned long transversedFileCounts = 0;
    std::atomic<bool> scannerPaused;
    std::atomic<bool> counterPaused;
    std::atomic<bool> cancelTransversal;
    std::atomic<bool> cancelSCanning;
    std::atomic<bool> superSpeedSet;
    std::atomic<bool> suspendAllScanners;
    std::atomic<int> scannerSleepInterval;

    bool startInternalScanner(const QStringList scanFolders, const QStringList exclusionFolders);
    void updateScanProgressSignal(unsigned long value);
    void setUpAuxilliaries();
    void attempt_serialization_to_disk();

    DLS::DuplicatesContainer duplicates;
    LogFormatter logFormatter;
};

#endif // DUPSCANFINDWIDGET_HPP
