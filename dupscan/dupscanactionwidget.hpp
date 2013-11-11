#ifndef DUPSCANACTIONWIDGET_HPP
#define DUPSCANACTIONWIDGET_HPP

#include <QWidget>
#include <QListView>
#include "backend/include/container_helpers/fp_holders.hpp"

class DActionsListView;
class DActionsListModel;
class DActionsListDelegate;
class ActionsButtonPanel;

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
    ~DupScanActionWidget();
    QWidget *actionsButtonPanel() const;

signals:

public slots:
    void setDuplicates(DLS::DuplicatesContainer duplicates);

private:
    DActionsListView* view;
    DActionsListModel* model;
    DActionsListDelegate* delegate;
    ActionsButtonPanel* actionsButton;
};

#endif // DUPSCANACTIONWIDGET_HPP
