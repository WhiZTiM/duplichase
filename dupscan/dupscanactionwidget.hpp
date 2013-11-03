#ifndef DUPSCANACTIONWIDGET_HPP
#define DUPSCANACTIONWIDGET_HPP

#include <QWidget>
#include <QListView>

class DActionsListView;
class DActionsListModel;
class DActionsListDelegate;

class DuplicatesListView : public QListView
{
    Q_OBJECT
public:
    explicit DuplicatesListView(QWidget* parent = nullptr);
};


class DupScanActionWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DupScanActionWidget(QWidget *parent = 0);

signals:

public slots:

private:
    DActionsListView* view;
    DActionsListModel* model;
    DActionsListDelegate* delegate;
};

#endif // DUPSCANACTIONWIDGET_HPP
