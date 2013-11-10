#ifndef DACTIONSLISTVIEW_HPP
#define DACTIONSLISTVIEW_HPP

#include <QListView>
#include <QStyledItemDelegate>
#include <QMenu>
#include <QAction>

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

private:
    QString _name;
    QString _size;
    QMenu contextMenu;
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

#endif // DACTIONSLISTVIEW_HPP
