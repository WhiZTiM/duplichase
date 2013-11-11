#include <QFile>
#include <QListWidget>
#include <QHBoxLayout>
#include <QStackedLayout>
#include "dlsmainwindow.hpp"
#include "ui_dlsmainwindow.h"
#include "dupscan/dialogs/aboutdialog.hpp"
#include "dupscan/dupscansetupwidget.hpp"
#include "dupscan/dupscanfindwidget.hpp"
#include "dupscan/dupscanactionwidget.hpp"

/*Titus 2:11 - For the grace of God that bringeth salvation hath appeared to ALL MEN,
 * <||--> Including you looking at this code!!
 */

DLSMainWindow::DLSMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DLSMainWindow)
{
    ui->setupUi(this);

    dpSetupWidget = new DupScanSetupWidget(this);
    dpFindWidget = new DupScanFindWidget(this);
    dpActionWidget = new DupScanActionWidget(this);
    ui->dupScanParameterFrame->setLayout( new QHBoxLayout );
    setupMainWindowElements();
    setWindowIcon(QIcon(":/main/mainIcon"));
    QFile file(":/stylesheet/mainStyle");
    if(file.isOpen())
    {
        qApp->setStyleSheet( QString::fromLatin1( file.readAll() ) );
    }
    qApp->setStyleSheet(
                "QMainWindow> .QWidget{"
                //"background-color: \"black\";"
                "background-image: url(\":/images/bg01\");"
                "background-repeat: repeat-xy;"
                "}");
}

void DLSMainWindow::setupMainWindowElements()
{
    setWindowTitle(tr("DupLichaSe 2013 --version 0.01 Alpha"));
    ui->msgInfoTextBrowser->setReadOnly(true);
    //ui->msgInfoTextBrowser->set

    ui->progressBarFinite->setMinimum(0);
    ui->progressBarFinite->setMaximum(100);
    ui->scanSpeedHorizontalSlider->setMinimum(0);
    ui->scanSpeedHorizontalSlider->setMaximum(100);

    dupScanFrameStackedLayout = new QStackedLayout(ui->dupScanFrame);
    dupScanFrameStackedLayout->addWidget(dpSetupWidget);
    dupScanFrameStackedLayout->addWidget(dpFindWidget);
    dupScanFrameStackedLayout->addWidget(dpActionWidget);
    ui->dupScanFrame->setLayout(dupScanFrameStackedLayout);
    dpActionWidget->actionsButtonPanel()->setVisible(false);
    ui->dupScanParameterFrame->layout()->addWidget( dpActionWidget->actionsButtonPanel() );

    connect(ui->action_About_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(ui->stage1_setupRadioButton, SIGNAL(clicked()), this, SLOT(stageRadioClickEvent()));
    connect(ui->stage2_FindRadioButton, SIGNAL(clicked()), this, SLOT(stageRadioClickEvent()));
    connect(ui->stage3_ActionsRadioButton, SIGNAL(clicked()), this, SLOT(stageRadioClickEvent()));
    connect(ui->action_About, SIGNAL(triggered()), this, SLOT(aboutDuplichasePopup()));
    connect(ui->superSpeedCheckBox, SIGNAL(toggled(bool)), dpFindWidget, SLOT(toggleSuperSpeed(bool)));
    connect(ui->scanSpeedHorizontalSlider, SIGNAL(valueChanged(int)), dpFindWidget, SLOT(setScanSpeed(int)));
    connect(dpSetupWidget, SIGNAL(statusMessage(QString)), this, SLOT(writeToStatusBar7(QString)) );
    connect(dpFindWidget, SIGNAL(scanProgress(int)), ui->progressBarFinite, SLOT(setValue(int)));
    connect(dpFindWidget, SIGNAL(logMessage(QString)), this, SLOT(writeToLogger(QString)));
    connect(dpFindWidget, SIGNAL(finishedScanning(bool)), this, SLOT(processFinishedScanning(bool)));
    connect(ui->progressSuspendPushButton, SIGNAL(clicked()), this, SLOT(processStart_Suspend()));
    connect(ui->progressStopPushButton, SIGNAL(clicked()), this, SLOT(StopScanner()));

    processStart_Suspend();
}

DLSMainWindow::~DLSMainWindow()
{
    delete ui;
}

void DLSMainWindow::makeScanningContinue()
{
    ui->progressSuspendPushButton->setText("SUSPEND");
    ui->progressBarFiniteLabel->setText("Progress: SCANNING");
    ui->progressBarWorkingLabel->setText("Working");
    dpFindWidget->continueScanning();
}

void DLSMainWindow::makeScanningStartable()
{
    canStartScanning = true;
    ui->progressBarFinite->setValue(0);
    ui->progressSuspendPushButton->setText("START");
    ui->progressBarFiniteLabel->setText("Progress: IDLING");
    ui->progressBarWorkingLabel->setText("Resting");
}


//! PUBLIC SLOTS
void DLSMainWindow::stageRadioClickEvent()
{
    if(ui->stage1_setupRadioButton->isChecked())
    {
        dupScanFrameStackedLayout->setCurrentIndex(0);
    }
    else if(ui->stage2_FindRadioButton->isChecked())
    {
        dupScanFrameStackedLayout->setCurrentIndex(1);
    }
    else if(ui->stage3_ActionsRadioButton->isChecked())
    {
        dupScanFrameStackedLayout->setCurrentIndex(2);
        dpActionWidget->actionsButtonPanel()->show();
        return;
    }
    //ui->dupScanParameterFrame->layout()->removeWidget( dpActionWidget->actionsButtonPanel() );
    //ui->dupScanParameterFrame->layout()->takeAt(0);
    dpActionWidget->actionsButtonPanel()->hide();
}

void DLSMainWindow::processStart_Suspend()
{
    if(dpFindWidget->scanningJobRunning())
    {
        ui->progressSuspendPushButton->setText("CONTINUE");
        ui->progressBarFiniteLabel->setText("Progress: SUSPENDED");
        ui->progressBarWorkingLabel->setText("Paused");
        dpFindWidget->pauseScanning();
    }
    else if(dpFindWidget->scanningJobPaused())
    {
        makeScanningContinue();
    }
    else if(canStartScanning)
    {
        if(ui->stage1_setupRadioButton->isChecked())
            startScanner();
        else
        {
            ui->stage1_setupRadioButton->setChecked(true);
            stageRadioClickEvent();
        }
    }
    else if(dpFindWidget->scanningJobFinished())
    {
        makeScanningStartable();
    }
}

void DLSMainWindow::startScanner()
{
    if(dpSetupWidget->searchListIsEmpty())
    {
        writeToStatusBar7("Cannot Start Operation: Please Add at least One search Path");
        ui->stage1_setupRadioButton->setChecked(true);
        canStartScanning = false;
        stageRadioClickEvent();
        processStart_Suspend();
        return;
    }
    ui->stage2_FindRadioButton->setChecked(true);
    stageRadioClickEvent();
    dpFindWidget->startScanner(dpSetupWidget->searchList(), dpSetupWidget->currentExclusionList());
    makeScanningContinue();
}

void DLSMainWindow::StopScanner()
{
    if(dpFindWidget->stopScanning())
    {
        ui->stage3_ActionsRadioButton->setChecked(true);
        canStartScanning = false;
        stageRadioClickEvent();
        makeScanningStartable();
    }
    else
    {
        writeToStatusBar7("The scanner is not in progress. Please click Start");
    }
}

void DLSMainWindow::writeToLogger(QString msg)
{
    ui->msgInfoTextBrowser->append(msg);
}

void DLSMainWindow::aboutDuplichasePopup()
{
    AboutDialog about(this);
    about.exec();
}


//! PRIVATE SLOTS
void DLSMainWindow::writeToStatusBar7(QString msg)
{
    ui->statusbar->showMessage(msg, 7000);
}

void DLSMainWindow::processFinishedScanning(bool succeeded)
{
    Q_UNUSED(succeeded)
    canStartScanning = false;
    makeScanningStartable();
    ui->stage3_ActionsRadioButton->setChecked(true);
    dpActionWidget->setDuplicates( dpFindWidget->getDuplicateContainer() );
    stageRadioClickEvent();
}
