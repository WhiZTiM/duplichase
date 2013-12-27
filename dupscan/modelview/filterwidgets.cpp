#include "ditemcontainer.hpp"
#include "filterwidgets.hpp"
#include <QRegExpValidator>
#include <QFileSystemModel>
#include <QMessageBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QCompleter>
#include <QLineEdit>
#include <QGroupBox>
#include <QRegExp>
#include <QLabel>

FilterDialog::FilterDialog(QWidget *parent) :
    QDialog(parent)
{
    setWindowFlags(Qt::Popup);

    QHBoxLayout* filterAcceptRejectHLayout = new QHBoxLayout;
    QVBoxLayout* typesButtonVLayout = new QVBoxLayout;
    QVBoxLayout* sizeRangeVLayout = new QVBoxLayout;
    QVBoxLayout* rightPaneHLayout = new QVBoxLayout;
    QVBoxLayout* leftPaneHLayout = new QVBoxLayout;
    QHBoxLayout* itemLayout = new QHBoxLayout;
    QVBoxLayout* mainLayout = new QVBoxLayout;
    QVBoxLayout* pathGroupBoxLayout = new QVBoxLayout;
    QVBoxLayout* regexGroupBoxLayout = new QVBoxLayout;

    typesGroupBox = new QGroupBox(tr("File Types"), this);
    typesGroupBox->setCheckable(true);
    typesGroupBox->setChecked(true);
    sizeRangeGroupBox = new QGroupBox(tr("File Size"), this);
    sizeRangeGroupBox->setCheckable(true);
    sizeRangeGroupBox->setChecked(false);
    pathGroupBox = new QGroupBox(tr("File Path"), this);
    pathGroupBox->setCheckable(true);
    pathGroupBox->setChecked(false);
    regexGroupBox = new QGroupBox(tr("Regular Expression"), this);
    regexGroupBox->setCheckable(true);
    regexGroupBox->setChecked(false);

    filterApprovePushButton = new QPushButton(tr("Filter"), this);
    filterRejectPushButton = new QPushButton(tr("Cancel"), this);

    knownEssentialsPushButton = new QPushButton(tr("Known Essentials"), this);
    knownEssentialsPushButton->setCheckable(true);

    documentTypesPushButton = new QPushButton(tr("Documents"), this);
    documentTypesPushButton->setCheckable(true);

    imageTypesPushButton = new QPushButton(tr("Images"), this);
    imageTypesPushButton->setCheckable(true);

    musicTypesPushButton = new QPushButton(tr("Audio"), this);
    musicTypesPushButton->setCheckable(true);

    videoTypesPushButton = new QPushButton(tr("Videos"), this);
    videoTypesPushButton->setCheckable(true);

    archiveTypesPushButton = new QPushButton(tr("Archives"), this);
    archiveTypesPushButton->setCheckable(true);

    typesButtonVLayout->addWidget(knownEssentialsPushButton);
    typesButtonVLayout->addWidget(documentTypesPushButton);
    typesButtonVLayout->addWidget(imageTypesPushButton);
    typesButtonVLayout->addWidget(musicTypesPushButton);
    typesButtonVLayout->addWidget(videoTypesPushButton);
    typesButtonVLayout->addWidget(archiveTypesPushButton);
    typesButtonVLayout->addStretch();
    typesGroupBox->setLayout(typesButtonVLayout);

    minFileSizeLabel = new QLabel(tr("Minimum File Size"), this);
    minFileSizeLineEdit = new FileSizeInputLineEdit(this);
    maxFileSizeLabel = new QLabel(tr("Maximum File Size"), this);
    maxFileSizeLineEdit = new FileSizeInputLineEdit(this);
    lineWidget = new QFrame(this);
    lineWidget->setFrameShape(QFrame::HLine);

    sizeRangeVLayout->addWidget(minFileSizeLabel);
    sizeRangeVLayout->addWidget(minFileSizeLineEdit);
    sizeRangeVLayout->addWidget(lineWidget);
    sizeRangeVLayout->addWidget(maxFileSizeLabel);
    sizeRangeVLayout->addWidget(maxFileSizeLineEdit);
    sizeRangeGroupBox->setLayout(sizeRangeVLayout);

    folderPathLineEdit = new QLineEdit(tr("Path:"), this);
    pathGroupBoxLayout->addWidget(folderPathLineEdit);
    pathGroupBox->setLayout( pathGroupBoxLayout );
    regexLineEdit = new QLineEdit(tr("Regular Expression"), this);
    regexGroupBoxLayout->addWidget(regexLineEdit);
    regexGroupBox->setLayout( regexGroupBoxLayout );

    filterAcceptRejectHLayout->addStretch();
    filterAcceptRejectHLayout->addWidget(filterApprovePushButton);
    filterAcceptRejectHLayout->addWidget(filterRejectPushButton);

    rightPaneHLayout->addWidget(typesGroupBox);
    leftPaneHLayout->addWidget(sizeRangeGroupBox);
    leftPaneHLayout->addWidget(pathGroupBox);
    leftPaneHLayout->addWidget(regexGroupBox);

    itemLayout->addLayout(rightPaneHLayout);
    itemLayout->addLayout(leftPaneHLayout);

    mainLayout->addLayout(itemLayout);
    mainLayout->addLayout(filterAcceptRejectHLayout);
    setLayout(mainLayout);

    setUpCompleter();

    connect(filterApprovePushButton, SIGNAL(clicked()), this, SLOT(approveButton_clicked()));
    connect(filterRejectPushButton, SIGNAL(clicked()), this, SLOT(hide()));
}

void FilterDialog::hideEvent(QHideEvent *event)
{
    QDialog::hideEvent(event);
    emit aboutToHide();
}

void FilterDialog::cancelButton_clicked()
{
    hide();
}

void FilterDialog::approveButton_clicked()
{
    if(validateInputs())
        hide();
}

bool FilterDialog::validateInputs()
{
    if(!validateSizeInputs())
        return false;
    if(!validateRegexInput())
        return false;
    if(!validatePathInput())
        return false;

    if(sizeRangeGroupBox->isChecked())
        emit filterBySize( minFileSizeLineEdit->value(), maxFileSizeLineEdit->value() );
    if(typesGroupBox->isChecked())
        /* fire */ signalTypesSelection();
    if(pathGroupBox->isChecked())
        emit filterByPath(folderPathLineEdit->text());
    if(regexGroupBox->isChecked())
        emit filterByRegex( QRegExp(regexLineEdit->text()) );
    return true;
}

bool FilterDialog::validateSizeInputs()
{
    if(!sizeRangeGroupBox->isChecked())
        return true;

    bool ok = false;
    auto min = sizzleValue(ok, minFileSizeLineEdit, tr("Minimum File Size"));
    if(not ok)
        return false;
    min = sizzleValue(ok, maxFileSizeLineEdit, tr("Maximum File Size"));
    if(not ok)
        return false;

    if(minFileSizeLineEdit->value() > maxFileSizeLineEdit->value())
    {
        fireErrorMessageBox(tr("<qt><b>C'mon!!!</b> Minimum File Size is <b>less</b> than "
                               "Maximum File Size!<br /><b>That's just too wrong!</qt>"));
        return false;
    }

    return true;
}

bool FilterDialog::validatePathInput()
{
    if(!pathGroupBox->isChecked())
        return true;
    QString text(folderPathLineEdit->text());
    if(text.isEmpty())
    {
        fireErrorMessageBox(tr("Ooopsie!! Cannot have empty Folder Filter Path!"));
        return false;
    }
    if(!QFile::exists(text))
    {
        fireErrorMessageBox(tr("ERROR: The folder \"") + text + tr("\" does not exist!"));
        return false;
    }
    return true;
}

bool FilterDialog::validateRegexInput()
{
    if(regexGroupBox->isChecked())
        return true;
    QString exp(regexLineEdit->text());
    if(exp.isEmpty())
    {
        fireErrorMessageBox(tr("Regular Expression Cannot be Empty!"));
        return false;
    }
    QRegExp regexp(exp);
    if(!regexp.isValid())
    {
        fireErrorMessageBox(tr("WRONG Expression! NOTE: Use Perl standard regex syntax"));
        return false;
    }
    return true;
}

void FilterDialog::signalTypesSelection()
{
    typedef ExtensionFilterTypes e;
    QStringList filter;
    if(knownEssentialsPushButton->isChecked())
    {
        QStringList types = e::getExtensionTypes();
        for(auto& m : types)
            filter += m;
        emit filterByExtension(filter);
        return;
    }
    if(documentTypesPushButton->isChecked())
        filter += e::getExtensionList("document");
    if(imageTypesPushButton->isChecked())
        filter += e::getExtensionList("image");
    if(musicTypesPushButton->isChecked())
        filter += e::getExtensionList("audio");
    if(videoTypesPushButton->isChecked())
        filter += e::getExtensionList("video");
    if(archiveTypesPushButton->isChecked())
        filter += e::getExtensionList("executable");

    if(filter.isEmpty())
        return;
    emit filterByExtension(filter);
}

void FilterDialog::fireErrorMessageBox(const QString& msg)
{
    QMessageBox::critical(this, tr("DupLichaSe - Filter Validation Error"), msg);
    show();
}

unsigned long FilterDialog::sizzleValue(bool& isGood, FileSizeInputLineEdit* fslEdit, const QString& tag)
{
    unsigned long rtn = fslEdit->value();
    if(fslEdit->status() == FileSizeInputLineEdit::SM::good)
    {
        isGood = true;
    }
    else if(fslEdit->status() == FileSizeInputLineEdit::SM::bad)
    {
        fireErrorMessageBox(tr("<qt><b>") + tag +
                            tr("</b> has an invalid input</qt>"));
        isGood = false;
    }
    else if(fslEdit->status() == FileSizeInputLineEdit::SM::underflow)
    {
        fireErrorMessageBox(tr("<qt><b>Oopsie!!! ") + tag +
                            tr("</b>'s value is less than DupLichaSe's current capacity!<br />"
                               "::Please type in a Value between <b>2KB and 4GB</b></qt>"));
        isGood = false;
    }
    else if(fslEdit->status() == FileSizeInputLineEdit::SM::overflow)
    {
        fireErrorMessageBox(tr("<qt><b>Oopsie!!! ") + tag +
                            tr("</b>'s value is greater than DupLichaSe's current capacity!<br />"
                               "::Please type in a Value between <b>2KB and 4GB</b></qt>"));
        isGood = false;
    }
    return rtn;
}

void FilterDialog::setUpCompleter()
{
    QCompleter* completer = new QCompleter(this);
    QFileSystemModel* fsmodel = new QFileSystemModel(completer);
    fsmodel->setReadOnly(true);
    fsmodel->setRootPath("");
    completer->setModel(fsmodel);
    folderPathLineEdit->setCompleter(completer);
}




////////////////////////////////////////////////////////////////////
/// \brief FileSizeInputLineEdit::FileSizeInputLineEdit
/// \param parent
///////////////////////////////////////////////////////////////////////

FileSizeInputLineEdit::FileSizeInputLineEdit(QWidget *parent, int minimumInputLimit)
    : QWidget(parent), minlimit(minimumInputLimit)
{
    setLayout(new QVBoxLayout);
    lineEdit = new QLineEdit(this);
    layout()->addWidget(lineEdit);

    lineEdit->setMaxLength(10);
    QRegExp regex("^(\\d*\\.?\\d*)(?:\\s*)(|[Bb]|(?:(?:[KMGkmg])|(?:[KMGkmg][Bb])))$", Qt::CaseInsensitive);
    regexp = regex;
    QRegExpValidator* regexValidator = new QRegExpValidator(regex, this);
    lineEdit->setValidator(regexValidator);
}

unsigned long FileSizeInputLineEdit::value()
{
    st = SM::good;
    QString text(lineEdit->text());
    if(text.isEmpty())
    {
        st = SM::bad;
        return 0;
    }
    if(!regexp.exactMatch(text))
    {
        st = SM::bad;
        return 0;
    }
    QStringList capturedTexts( regexp.capturedTexts() );

    unsigned long rtn = 0;
    float cap1 = 0;
    QString prefix;
    for(int i = 0; i < capturedTexts.size(); i++)
    {
        bool ok;
        if(i == 1)
        {
            cap1 = capturedTexts[i].toFloat(&ok);
            if(not ok)
            {
                st = SM::overflow;
                return 0;
            }
        }
        else if(i == 2)
            prefix = capturedTexts[i];
    }

    prefix = prefix.toUpper();

    //const unsigned long Limit = 1073741823UL * 4; // 4GB - 4 Limit...
    //4294967295

    //The magic numbers you see here are calculated by dividing the Limit of 4GB --> 4294967295
    // with the real value(converting to bytes) of the input
    do{
        if(prefix == "B")
        {
            rtn = static_cast<unsigned long>(cap1);
            break;
        }
        else if(prefix == "" || prefix == "KB" || prefix == "K") //Default string is KB
        {
            if (static_cast<unsigned long>(cap1) > 4194303)
            {
                st = SM::overflow;
                return 0;
            }
            rtn = static_cast<unsigned long>(cap1) * 1024;
            break;
        }
        else if (prefix == "MB" || prefix == "M")
        {
            if (static_cast<unsigned long>(cap1) > 4095)
            {
                st = SM::overflow;
                return 0;
            }
            rtn = static_cast<unsigned long>(cap1) * 1024 * 1024;
            break;
        }
        else if (prefix == "GB" || prefix == "G")
        {
            if (static_cast<unsigned long>(cap1) > 4)
            {
                st = SM::overflow;
                return 0;
            }
            rtn = static_cast<unsigned long>(cap1) * 1024 * 1024 * 1024;
            break;
        }
        break;
    }while(false);

    if(rtn < minlimit)
        st = SM::underflow;
    return rtn;
}

bool FileSizeInputLineEdit::isValid()
{
    return ( (!lineEdit->text().isEmpty()) && (regexp.exactMatch(lineEdit->text())));
}
