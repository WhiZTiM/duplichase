#ifndef FILTERWIDGETS_HPP
#define FILTERWIDGETS_HPP

#include <QDialog>

QT_BEGIN_NAMESPACE
class QLabel;
class QFrame;
class QLineEdit;
class QLineEdit;
class QGroupBox;
class QStringList;
class QPushButton;
QT_END_NAMESPACE
class FileSizeInputLineEdit;

class FilterDialog : public QDialog
{
    Q_OBJECT
public:
    explicit FilterDialog(QWidget *parent = nullptr);

    void hideEvent(QHideEvent *event) override;
signals:
    void filterByExtension(QStringList);
    void filterBySize(ulong min, ulong max);
    void filterByPath(QString);
    void filterByRegex(QRegExp);
    void statusBarErrorMessage(QString);
    void aboutToHide();
private slots:
    void approveButton_clicked();
    void cancelButton_clicked();
private:
    QPushButton* knownEssentialsPushButton;
    QPushButton* documentTypesPushButton;
    QPushButton* archiveTypesPushButton;
    QPushButton* musicTypesPushButton;
    QPushButton* imageTypesPushButton;
    QPushButton* videoTypesPushButton;
    QPushButton* filterApprovePushButton;
    QPushButton* filterRejectPushButton;

    QLabel* minFileSizeLabel;
    QLabel* maxFileSizeLabel;
    QFrame* lineWidget;
    FileSizeInputLineEdit* minFileSizeLineEdit;
    FileSizeInputLineEdit* maxFileSizeLineEdit;
    QLineEdit* regexLineEdit;
    QLineEdit* folderPathLineEdit;
    QGroupBox* pathGroupBox;
    QGroupBox* regexGroupBox;
    QGroupBox* typesGroupBox;
    QGroupBox* sizeRangeGroupBox;

    bool validateInputs();
    bool validateSizeInputs();
    bool validateRegexInput();
    bool validatePathInput();
    void signalTypesSelection();
    void fireErrorMessageBox(const QString &msg);
    unsigned long sizzleValue(bool &isGood, FileSizeInputLineEdit *fslEdit, const QString &tag);
    void setUpCompleter();
};

class FileSizeInputLineEdit : public QWidget
{
    Q_OBJECT
public:
    enum class SM { good, underflow, overflow, bad };
    FileSizeInputLineEdit(QWidget* parent = nullptr, int minimumInputLimit = 2048);
    bool isValid();
    inline SM status()
        { return st; }

    unsigned long value();
public slots:

private:
    QLineEdit* lineEdit;
    QRegExp regexp;
    SM st = SM::good;
    unsigned long minlimit;
};

#endif // FILTERWIDGETS_HPP
