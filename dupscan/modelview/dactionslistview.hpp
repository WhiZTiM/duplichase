#ifndef DACTIONSLISTVIEW_HPP
#define DACTIONSLISTVIEW_HPP

#include <QListView>
#include <QStyledItemDelegate>
#include <QMenu>
#include <QAction>

QT_BEGIN_NAMESPACE
class QPushButton;
QT_END_NAMESPACE

class DActionsListDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    DActionsListDelegate(QWidget *parent = 0);
    QString fileNameAsString(const QString& name) const;
    QString fileNameAsString(const std::string& name) const;
    QString fileSizeAsString(const ulong size) const;
    //bool eventFilter(QObject *object, QEvent *event);
   // bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
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

signals:
    void deleteFileNow(QModelIndexList);
    void markForKeep(QModelIndexList);
    void markForDelete(QModelIndexList);
    void unmarkForKeep(QModelIndexList);
    void unmarkForDelete(QModelIndexList);

public slots:
    void processGroupHeaderSelected(QModelIndex index);
    void action_openFile();
    void action_openDirectory();
    void action_deletFileNow();
    void action_markForKeep();
    void action_markForDelete();

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
    void prepareContextMenu();
};


class ActionsButtonPanel : public QWidget
{
    Q_OBJECT
public:
    explicit ActionsButtonPanel(QWidget* parent = nullptr);

signals:
    void sortingRequested(Qt::SortOrder);
    void filteringRequested();
    void resetRequested();
    void commitRequested();
    void unmarkAllKeeps();
    void unmarkAllDeletes();
    void unmarkAll();
    void autoSelectNextKeeps();
    void autoSelectNextDeletes();
    void autoSelectNextPossibilities();
public slots:

private slots:
    void sortByDescendingFileSize();
    void sortByAscendingFileSize();

    void processSortRequest();
    void processMarkingsClicked();
    void processAutoSelectionClicked();

    void sortContextMenuAboutToHide();
    void autoSelectionContextMenuAboutToHide();
    void markingsContextMenuAboutToHide();

private:
    QPushButton* autoSelection_pushButton;
    QPushButton* markings_pushButton;
    QPushButton* reset_pushButton;
    QPushButton* sort_pushButton;
    QPushButton* filter_pushButton;
    QPushButton* commit_pushButton;
    QMenu sortContextMenu;
    QMenu autoSelectionContextMenu;
    QMenu markingsContextMenu;
};

#endif // DACTIONSLISTVIEW_HPP
