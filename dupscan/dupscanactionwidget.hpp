#ifndef DUPSCANACTIONWIDGET_HPP
#define DUPSCANACTIONWIDGET_HPP

#include <QWidget>
#include <QListView>
#include <QStackedWidget>
#include "backend/include/container_helpers/fp_holders.hpp"

class DActionsListView;
class DActionsListModel;
class DActionsListDelegate;
class ActionsButtonPanel;
class QLabel;

class DupScanActionWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DupScanActionWidget(QWidget *parent = 0);
    ~DupScanActionWidget();
    QWidget *actionsButtonPanel() const;

signals:
    void logMessage(QString);

public slots:
    void setDuplicates(DLS::DuplicatesContainer duplicates);
    void modelWasJustSet();

    bool saveResultsToFile(const QString& fileName);
    bool loadResultsFromFile(const QString& fileName);

private:
    DActionsListView* view;
    DActionsListModel* model;
    DActionsListDelegate* delegate;
    ActionsButtonPanel* actionsButton;

    QStackedWidget* stackedWidget;
    QLabel* emptyViewLabel;
};

#endif // DUPSCANACTIONWIDGET_HPP
