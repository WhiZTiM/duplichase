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
#ifndef DACTIONSLISTVIEW_HPP
#define DACTIONSLISTVIEW_HPP

#include <QStyledItemDelegate>
#include <QListView>
#include <QRegExp>
#include <QAction>
#include <QDialog>
#include <QMenu>

QT_BEGIN_NAMESPACE
class QPushButton;
QT_END_NAMESPACE
class FilterDialog;

class DActionsListDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    DActionsListDelegate(QWidget *parent = 0);
    QString fileNameAsString(const QString& name) const;
    QString fileNameAsString(const std::string& name) const;
    QString fileSizeAsString(const ulong size) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

signals:
    void groupHeaderSelected(QModelIndex) const;

private:
    QString _name;
    QString _size;
};

class DActionsListView : public QListView
{
    Q_OBJECT
public:
    explicit DActionsListView(QWidget *parent = 0);
    bool viewportEvent(QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void scrollTo(const QModelIndex &index, ScrollHint hint = PositionAtCenter);

signals:
    void selectNextGroupKeeps(QModelIndex index, bool SelectNextGroup);
    void selectNextGroupDeletes(QModelIndex index, bool SelectNextGroup);
    void selectDeletePossibilities(QModelIndex index);
    void deleteFileNow(QModelIndexList);
    void markForKeep(QModelIndexList);
    void markForDelete(QModelIndexList);
    void unmarkForKeep(QModelIndexList);
    void unmarkForDelete(QModelIndexList);
    void removeFromView(QModelIndexList);
    void removeFromScanner(QModelIndexList);

public slots:
    void selectKeepsGroup();
    void selectDeletesGroup();
    void selectNextKeepsGroup();
    void selectNextDeletesGroup();
    void selectNextPossibilities();
    void scrollToIndex(QModelIndex index);
    void makeSelection(QModelIndexList indexes, QModelIndex ScrolledTo);

//private slots:
    void processGroupHeaderSelected(QModelIndex index);
    void action_openFile();
    void action_openDirectory();
    void action_deletFileNow();
    void action_markForKeep();
    void action_markForDelete();
    void action_removeFromView();
    void action_removeFromScanner();

private:
    QMenu contextMenu;
    bool keepActionChecked = false;
    bool deleteActionChecked = false;

    QAction* keepAction;
    QAction* commitAction;
    QAction* deleteAction;
    QAction* deleteNowAction;
    QAction* openFileAction;
    QAction* openDirectoryAction;
    QAction* removeFromViewAction;
    QAction* removeFromScannerAction;
    void prepareContextMenu();
};


class ActionsButtonPanel : public QWidget
{
    Q_OBJECT
public:
    explicit ActionsButtonPanel(QWidget* parent = nullptr);

signals:
    void sortingRequested(Qt::SortOrder);
    void resetRequested();
    void commitRequested();
    void unmarkAllKeeps();
    void unmarkAllDeletes();
    void unmarkAll();
    void autoSelectNextKeeps();
    void autoSelectNextDeletes();
    void autoSelectDeletePossibilities();
    void autoCleanDeletes();
    void selectGroupKeeps();
    void selectGroupDeletes();
    void selectNextGroupKeeps();
    void selectNextGroupDeletes();

    void filterByExtension(QStringList);
    void filterBySize(ulong min, ulong max);
    void filterByPath(QString);
    void filterByRegex(QRegExp);
    void statusBarErrorMessage(QString);

public slots:

private slots:
    void sortByDescendingFileSize();
    void sortByAscendingFileSize();

    void processSortRequest();
    void processMarkingsClicked();
    void processAutoSelectionClicked();
    void filterContextMenuClicked();

    void sortContextMenuAboutToHide();
    void autoSelectionContextMenuAboutToHide();
    void markingsContextMenuAboutToHide();
    void filterContextMenuAboutToHide();

private:
    QPushButton* autoSelection_pushButton;
    QPushButton* markings_pushButton;
    QPushButton* reset_pushButton;
    QPushButton* sort_pushButton;
    QPushButton* filter_pushButton;
    QPushButton* commit_pushButton;
    QPushButton* auto_pushButton;
    QMenu sortContextMenu;
    QMenu autoSelectionContextMenu;
    QMenu markingsContextMenu;
    QMenu filterContextMenu;
    FilterDialog* filterDialog;
};

#endif // DACTIONSLISTVIEW_HPP
