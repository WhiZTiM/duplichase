/*******************************************************************************************
**  (C) Copyright September 2013 - December 2013 by
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
    void statusBarMessage(QString);

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
