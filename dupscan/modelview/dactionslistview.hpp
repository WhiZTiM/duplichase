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
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

signals:
    void groupHeaderSelected(QModelIndex) const;

public slots:
    void action_openFile();
    void action_openDirectory();

private:
    QString _name;
    QString _size;
    QMenu contextMenu;
    QModelIndex currentIndex;
};

class DActionsListView : public QListView
{
    Q_OBJECT
public:
    explicit DActionsListView(QWidget *parent = 0);

signals:

public slots:
    void processGroupHeaderSelected(QModelIndex index);

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
public slots:

private slots:
    void processSortRequest();
    void sortByDescendingFileSize();
    void sortByAscendingFileSize();
    void sortContextMenuAboutToHide();
private:
    QPushButton* autoSelectKeep_pushButton;
    QPushButton* autoSelectDelete_pushButton;
    QPushButton* reset_pushButton;
    QPushButton* sort_pushButton;
    QPushButton* filter_pushButton;
    QMenu sortContextMenu;
};

#endif // DACTIONSLISTVIEW_HPP
