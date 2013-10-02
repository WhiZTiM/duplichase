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

class DLSMainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit DLSMainWindow(QWidget *parent = 0);
    ~DLSMainWindow();
    
public slots:
    void stageRadioClickEvent();
    void aboutDuplichasePopup();
private slots:
    void writeToStatusBar7(QString msg);
    void writeToLogger(QString msg);

private:
    Ui::DLSMainWindow *ui;
    DupScanSetupWidget*     dpSetupWidget;
    DupScanFindWidget*      dpFindWidget;
    DupScanActionWidget*    dpActionWidget;

    QVBoxLayout* dupScanSetupFrameLayout;
    QVBoxLayout* dupScanFindFrameLayout;
    QVBoxLayout* dupScanActionsFrameLayout;

    QStackedLayout* dupScanFrameStackedLayout;

    void setupMainWindowElements();
};

#endif // DLSMAINWINDOW_H
