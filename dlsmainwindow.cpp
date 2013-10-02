#include <QStackedLayout>
#include <QListWidget>
#include "dlsmainwindow.hpp"
#include "ui_dlsmainwindow.h"
#include "dupscan/dialogs/aboutdialog.hpp"
#include "dupscan/dupscansetupwidget.hpp"
#include "dupscan/dupscanfindwidget.hpp"
#include "dupscan/dupscanactionwidget.hpp"

DLSMainWindow::DLSMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DLSMainWindow)
{
    ui->setupUi(this);

    dpSetupWidget = new DupScanSetupWidget(this);
    dpFindWidget = new DupScanFindWidget(this);
    dpActionWidget = new DupScanActionWidget(this);
    setupMainWindowElements();
    setWindowIcon(QIcon(":/main/mainIcon"));
}

void DLSMainWindow::setupMainWindowElements()
{
    setWindowTitle(tr("DupLichaSe 2013 --version 0.01 Alpha"));
    ui->msgInfoTextBrowser->setReadOnly(true);

    ui->progressBarFinite->setMinimum(0);
    ui->progressBarFinite->setMaximum(100);
    ui->scanSpeedHorizontalSlider->setMinimum(0);
    ui->scanSpeedHorizontalSlider->setMaximum(100);

    dupScanFrameStackedLayout = new QStackedLayout(ui->dupScanFrame);
    dupScanFrameStackedLayout->addWidget(dpSetupWidget);
    dupScanFrameStackedLayout->addWidget(dpFindWidget);
    dupScanFrameStackedLayout->addWidget(dpActionWidget);
    ui->dupScanFrame->setLayout(dupScanFrameStackedLayout);

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
}

DLSMainWindow::~DLSMainWindow()
{
    delete ui;
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
