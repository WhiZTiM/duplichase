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
#ifndef DLSMAINWINDOW_H
#define DLSMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class DLSMainWindow;
}

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QStackedLayout;
QT_END_NAMESPACE

class DupScanSetupWidget;
class DupScanFindWidget;
class DupScanActionWidget;
class ActionsButtonPanel;

class DLSMainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit DLSMainWindow(QWidget *parent = 0);
    ~DLSMainWindow();
    
public slots:
    void stageRadioClickEvent();
    void processStart_Suspend();
    void aboutDuplichasePopup();
    void openHelpFile();
    void action_reportAProblem();
    void loadResultsFromFile(QString openPath);
    void startScanner();
    void StopScanner();
private slots:
    void processFinishedScanning(bool succeeded);
    void writeToStatusBar7(QString msg);
    void writeToLogger(QString msg);

    void saveResultsToFile();
    void loadResultsFromFile();

private:
    Ui::DLSMainWindow *ui;
    DupScanSetupWidget*     dpSetupWidget;
    DupScanFindWidget*      dpFindWidget;
    DupScanActionWidget*    dpActionWidget;
    ActionsButtonPanel*     dpActionsPanelWidget;

    QVBoxLayout* dupScanSetupFrameLayout;
    QVBoxLayout* dupScanFindFrameLayout;
    QVBoxLayout* dupScanActionsFrameLayout;

    QStackedLayout* dupScanFrameStackedLayout;

    bool canStartScanning = false;
    void setupMainWindowElements();
    void makeScanningContinue();
    void makeScanningStartable();
};

#endif // DLSMAINWINDOW_H
