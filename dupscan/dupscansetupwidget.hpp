#ifndef DUPSCANSETUPWIDGET_HPP
#define DUPSCANSETUPWIDGET_HPP

#include <QWidget>

QT_BEGIN_NAMESPACE

class QLabel;
class QWidget;
class QLineEdit;
class QGroupBox;
class QListWidget;
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QAbstractButton;

QT_END_NAMESPACE

class DupScanSetupWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DupScanSetupWidget(QWidget *parent = 0);
    void setInbuiltExclusionList(const QStringList& exclusionList);
    bool searchListIsEmpty() const;
    QStringList searchList() const;
    QStringList inbuiltExclusionList() const;
    QStringList currentExclusionList() const;
    
signals:
    void statusMessage(QString msg);

public slots:
    void add_searchFolder();
    void add_exclusionFolder();
    void modify_searchFolder();
    void modify_exclusionFolder();
    void remove_searchFolder();
    void remove_exclusionFolder();
    void showHide_inbuiltExclusionList();

private:
    QString lastSearchFolderPath;
    QString lastExclusionFolderPath;
    QStringList e_inbuiltExclusionList;

    QVBoxLayout* mainLayout;

    QGroupBox* s_groupBox;
    QVBoxLayout* s_verticalLayout;
    QHBoxLayout* s_horizontalLayout;
    QPushButton* s_modifyPushButton;
    QPushButton* s_addFolderPushButton;
    QPushButton* s_removeFolderPushButton;
    QPushButton* s_addRemovableDrivePushButton;

    QGroupBox* e_groupBox;
    QVBoxLayout* e_verticalLayout;
    QHBoxLayout* e_horizontalLayout;
    QPushButton* e_modifyPushButton;
    QPushButton* e_addPathPushButton;
    QPushButton* e_showInbuiltListPushButton;
    QPushButton* e_removeFolderPushButton;

    QListWidget* s_pathsListWidget;
    QListWidget* e_pathsListWidget;

    void initializeInbulitExclusionList();
};

#endif // DUPSCANSETUPWIDGET_HPP
