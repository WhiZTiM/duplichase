#ifndef DACTIONSLISTVIEW_HPP
#define DACTIONSLISTVIEW_HPP

#include <QListView>
#include <QStyledItemDelegate>

class DActionsListDelegate : public QStyledItemDelegate
{
public:
    DActionsListDelegate(QWidget *parent = 0);
    QString fileNameAsString(const QString& name) const;
    QString fileNameAsString(const std::string& name) const;
    QString fileSizeAsString(const ulong size) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    QString _name;
    QString _size;
};

class DActionsListView : public QListView
{
    Q_OBJECT
public:
    explicit DActionsListView(QWidget *parent = 0);

signals:

public slots:

};

#endif // DACTIONSLISTVIEW_HPP
