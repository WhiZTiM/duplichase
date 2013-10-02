#include "dupscansetupwidget.hpp"
#include "useful_qt_functions.hpp"
#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include <QPushButton>
#include <QFileDialog>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QProxyStyle>
#include <QStandardPaths>

bool checkAndShowOverLappingDialog(const QStringList& newList, const QStringList& oldList, QWidget *parent = 0);

DupScanSetupWidget::DupScanSetupWidget(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    initializeInbulitExclusionList();

    s_groupBox = new QGroupBox(tr("-->Folders to be Searched"), this);
    s_horizontalLayout = new QHBoxLayout(s_groupBox);
    s_verticalLayout = new QVBoxLayout;
    s_pathsListWidget = new QListWidget(s_groupBox);
    s_modifyPushButton = new QPushButton(tr("Modify"), s_groupBox);
    s_addFolderPushButton = new QPushButton(tr("Add Folder"), s_groupBox);
    s_removeFolderPushButton = new QPushButton(tr("Remove Path"), s_groupBox);
    s_addRemovableDrivePushButton = new QPushButton(tr("Add Removable Drive"), s_groupBox);

    e_groupBox = new QGroupBox(tr("-->Exclusion Folders"), this);
    e_horizontalLayout = new QHBoxLayout(e_groupBox);
    e_verticalLayout = new QVBoxLayout;
    e_pathsListWidget = new QListWidget(e_groupBox);
    e_addPathPushButton = new QPushButton(tr("Add Exclusion"), e_groupBox);
    e_modifyPushButton = new QPushButton(tr("Modify Exclusion"), e_groupBox);
    e_showInbuiltListPushButton = new QPushButton(tr("Show Inbuilt"), e_groupBox);
    e_removeFolderPushButton = new QPushButton(tr("Remove Exclusion"), e_groupBox);
    e_showInbuiltListPushButton->setCheckable(true);
    e_showInbuiltListPushButton->setChecked(false);

    //! Layouts
    // layouts for search path
    s_verticalLayout->addWidget(s_addRemovableDrivePushButton);
    s_verticalLayout->addWidget(s_addFolderPushButton);
    s_verticalLayout->addWidget(s_modifyPushButton);
    s_verticalLayout->addWidget(s_removeFolderPushButton);
    s_verticalLayout->addStretch();
    s_horizontalLayout->addWidget(s_pathsListWidget);
    s_horizontalLayout->addLayout(s_verticalLayout);

    // layouts for the exclusion path
    e_verticalLayout->addWidget(e_addPathPushButton);
    e_verticalLayout->addWidget(e_modifyPushButton);
    e_verticalLayout->addWidget(e_removeFolderPushButton);
    e_verticalLayout->addWidget(e_showInbuiltListPushButton);
    e_verticalLayout->addStretch();
    QMargins e_margin = e_verticalLayout->contentsMargins();
    e_margin.setRight( e_margin.right() + 20 );
    e_verticalLayout->setContentsMargins(e_margin);

    e_horizontalLayout->addWidget(e_pathsListWidget);
    e_horizontalLayout->addLayout(e_verticalLayout);

    mainLayout->addWidget(s_groupBox);
    mainLayout->addWidget(e_groupBox);

    //! Connections
    connect(s_addFolderPushButton, SIGNAL(clicked()), this, SLOT(add_searchFolder()));
    connect(s_modifyPushButton, SIGNAL(clicked()), this, SLOT(modify_searchFolder()));
    connect(s_removeFolderPushButton, SIGNAL(clicked()), this, SLOT(remove_searchFolder()));

    connect(e_addPathPushButton, SIGNAL(clicked()), this, SLOT(add_exclusionFolder()));
    connect(e_modifyPushButton, SIGNAL(clicked()), this, SLOT(modify_exclusionFolder()));
    connect(e_removeFolderPushButton, SIGNAL(clicked()), this, SLOT(remove_exclusionFolder()));
    connect(e_showInbuiltListPushButton, SIGNAL(clicked()), this, SLOT(showHide_inbuiltExclusionList()));

}


//! PUBLIC FUNCTIONS
void DupScanSetupWidget::setInbuiltExclusionList(const QStringList& exclusionList)
{
    e_inbuiltExclusionList = exclusionList;
}

QStringList DupScanSetupWidget::inbuiltExclusionList() const
{
    return e_inbuiltExclusionList;
}

QStringList DupScanSetupWidget::currentExclusionList() const
{
    QStringList rtn;
    for(int i(0); i < e_pathsListWidget->count(); i++)
        rtn.append(e_pathsListWidget->item(i)->text());

    return e_showInbuiltListPushButton->isChecked() ? rtn : rtn + e_inbuiltExclusionList;
}

//! PUBLIC SLOTS
void DupScanSetupWidget::add_searchFolder()
{
    QStringList fpathList;
    QFileDialog fdialog(this, tr("Add Folder to scan"), lastSearchFolderPath);

    fdialog.setOption(QFileDialog::ShowDirsOnly, true);
    fdialog.setFileMode(QFileDialog::Directory);
    if(fdialog.exec())
        fpathList = fdialog.selectedFiles();
    else
        return;

    QStringList oldList;
    for(int i(0); i < s_pathsListWidget->count(); i++)
        oldList.append(s_pathsListWidget->item(i)->text());

    if(checkAndShowOverLappingDialog(fpathList, oldList, this) ||
            checkAndShowOverLappingDialog(oldList, fpathList, this))
        return;

    lastSearchFolderPath = fpathList.first();
    s_pathsListWidget->addItems(fpathList);
}

void DupScanSetupWidget::add_exclusionFolder()
{
    QStringList fpathList;
    QFileDialog fdialog(this, tr("Add Folders to Exclude"), lastExclusionFolderPath);

    fdialog.setOption(QFileDialog::ShowDirsOnly, true);
    fdialog.setFileMode(QFileDialog::Directory);
    if(fdialog.exec())
        fpathList = fdialog.selectedFiles();
    else
        return;

    QStringList oldList;
    for(int i(0); i < e_pathsListWidget->count(); i++)
        oldList.append(e_pathsListWidget->item(i)->text());

    if(checkAndShowOverLappingDialog(fpathList, oldList, this) ||
            checkAndShowOverLappingDialog(oldList, fpathList, this))
        return;

    lastExclusionFolderPath = fpathList.first();
    e_pathsListWidget->insertItems(0, fpathList);
}


void DupScanSetupWidget::remove_searchFolder()
{
    if((s_pathsListWidget->count() == 0) || (s_pathsListWidget->currentItem() == NULL))
    {
        emit statusMessage(QString("INVALID REMOVE Operation: ") +
                                   (s_pathsListWidget->count() ? "No path selected: Select an item on the list that you want to delete" :
                                                                "C'mon! Seriously? What are you trying to remove from an empty list?"));
        return;
    }
    delete s_pathsListWidget->currentItem();
}


void DupScanSetupWidget::remove_exclusionFolder()
{
    if((e_pathsListWidget->count() == 0) || (e_pathsListWidget->currentItem() == NULL))
    {
        emit statusMessage(QString("INVALID REMOVE Operation: ") +
                                   (s_pathsListWidget->count() ? "No path selected: Select an item on the list that you want to delete" :
                                                                "C'mon! Seriously? What are you trying to remove from an empty list?"));
        return;
    }
    delete e_pathsListWidget->currentItem();
}


void DupScanSetupWidget::modify_searchFolder()
{
    if((s_pathsListWidget->count() == 0) || (s_pathsListWidget->currentItem() == NULL))
        return;
    QStringList fpathList;
    QFileDialog fdialog(this, tr("Modify a Search Folder"), s_pathsListWidget->currentItem()->text());

    fdialog.setOption(QFileDialog::ShowDirsOnly, true);
    fdialog.setFileMode(QFileDialog::Directory);
    if(fdialog.exec())
        fpathList = fdialog.selectedFiles();
    else
        return;

    QStringList oldList;
    for(int i(0); i < s_pathsListWidget->count(); i++)
        oldList.append(s_pathsListWidget->item(i)->text());

    if(checkAndShowOverLappingDialog(fpathList, oldList, this) ||
            checkAndShowOverLappingDialog(oldList, fpathList, this))
        return;

    int row = s_pathsListWidget->currentRow();
    delete s_pathsListWidget->currentItem();
    s_pathsListWidget->insertItem(row, fpathList.first());
}

void DupScanSetupWidget::modify_exclusionFolder()
{
    if((e_pathsListWidget->count() == 0) || (e_pathsListWidget->currentItem() == NULL))
        return;
    QStringList fpathList;
    QFileDialog fdialog(this, tr("Modify an esclusion folder"), e_pathsListWidget->currentItem()->text());

    fdialog.setOption(QFileDialog::ShowDirsOnly, true);
    fdialog.setFileMode(QFileDialog::Directory);
    if(fdialog.exec())
        fpathList = fdialog.selectedFiles();
    else
        return;

    QStringList oldList;
    for(int i(0); i < e_pathsListWidget->count(); i++)
        oldList.append(e_pathsListWidget->item(i)->text());

    if(checkAndShowOverLappingDialog(fpathList, oldList, this) ||
            checkAndShowOverLappingDialog(oldList, fpathList, this))
        return;

    int row = e_pathsListWidget->currentRow();
    delete e_pathsListWidget->currentItem();
    e_pathsListWidget->insertItem(row, fpathList.first());
}


void DupScanSetupWidget::showHide_inbuiltExclusionList()
{
    if(e_showInbuiltListPushButton->isChecked())
    {
        e_pathsListWidget->addItems(e_inbuiltExclusionList);
        for(auto& str : e_inbuiltExclusionList)
        {
            QList<QListWidgetItem*> lst = e_pathsListWidget->findItems(str, Qt::MatchExactly);

            //lst is definitely 1 item anyways
            for(QListWidgetItem* item : lst)
            {
                if(item)
                    item->setBackground(QBrush(QColor::fromRgb(30, 200, 20)));
            }
        }
    }

    else
    {
        for(auto& str : e_inbuiltExclusionList)
        {
            QList<QListWidgetItem*> lst = e_pathsListWidget->findItems(str, Qt::MatchExactly);
            for(QListWidgetItem* item : lst)
            {
                if(item)
                    delete item;
            }

        }
    }
}




//! Free Functions
bool checkAndShowOverLappingDialog(const QStringList& newList, const QStringList& oldList, QWidget* parent)
{
    QList<QPair<QString, QString> > overlaps = overlappingList(newList, oldList);
    if(overlaps.empty())
    {
        for(auto& m1 : newList)
            for(auto& m2 : oldList)
                if(m1 == m2)
                    return true;
        return false;
    }

    QString msg("<qt>Sorry for the pun above, but we've got some 'problems'.<br />"
                "There is a problem with some paths being ancestors of another...<br />"
                "<i>please check your current selection or the already listed folders</i><br />");
    msg += "<i>These Includes:</i><br /><br />";

    for(auto& k : overlaps)
    {
        msg += "<b>" + k.second + "</b><br />is an ancestor of <br /><b>" + k.first + "</b><br />";
    }
    msg += "</qt>";

    QMessageBox msgBox(parent);
    msgBox.setWindowTitle("Oopsie! Ancestor Problem");
    //QProxyStyle style;
    //msgBox.setIcon( QIcon::fromTheme("dialog-warning", style.standardIcon(QStyle::SP_MessageBoxWarning))_);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText("<qt><b>Grrr :{, C'mon, In real life, Do your ancestors live with you?</b></qt>");
    msgBox.setInformativeText(msg);
    msgBox.exec();
    return true;
}

//! PRIVATE FUNCTIONS
void DupScanSetupWidget::initializeInbulitExclusionList()
{
    QStringList tempLocations = QStandardPaths::standardLocations(QStandardPaths::TempLocation);
    if(not tempLocations.isEmpty())
        for(auto& k : tempLocations)
            e_inbuiltExclusionList.append(k);

#ifdef Q_OS_LINUX
    //! At least on my Ubuntu... these are system folders and shouldn't be touched under normal circumstances
    e_inbuiltExclusionList.append("/etc");
    e_inbuiltExclusionList.append("/lib");
    e_inbuiltExclusionList.append("/lib64");
    e_inbuiltExclusionList.append("/usr");
    e_inbuiltExclusionList.append("/var");
    e_inbuiltExclusionList.append("/dev");
    e_inbuiltExclusionList.append("/proc");
    e_inbuiltExclusionList.append("/root");
    e_inbuiltExclusionList.append("/run");
    //e_inbuiltExclusionList.append("/tmp");    //hopefuly taken care of above
    e_inbuiltExclusionList.append("/sys");
    e_inbuiltExclusionList.append("/bin");
    e_inbuiltExclusionList.append("/sbin");
    e_inbuiltExclusionList.append("/selinux");
    e_inbuiltExclusionList.append("/lost+found");
#endif  // Q_OS_LINUX

#ifdef Q_OS_WIN32
    e_inbuiltExclusionList.append("C:/Program Files");
    e_inbuiltExclusionList.append("C:/Windows");
#endif
}
