#include "aboutdialog.hpp"
#include "ui_aboutdialog.h"
#include <QMessageBox>

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    setAboutQtText();
    //setWindowFlags();
    ui->tabWidget->setCurrentIndex(0);  //Always
    ui->about_briefLabel->setOpenExternalLinks(true);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::setAboutQtText()
{
    QString translatedTextAboutQtCaption = QString(
            "<h3>About Qt</h3>"
            "<p>This program uses Qt version %1.</p>"
            ).arg(QLatin1String(QT_VERSION_STR));
    QString bodyDetail =
            "<p>Qt is a C++ toolkit for cross-platform application "
            "development.</p>"
            "<p>Qt provides single-source portability across all major desktop "
            "operating systems. It is also available for embedded Linux and other "
            "embedded and mobile operating systems.</p>"
            "<p>Qt is available under three different licensing options designed "
            "to accommodate the needs of our various users.</p>"
            "<p>Qt licensed under our commercial license agreement is appropriate "
            "for development of proprietary/commercial software where you do not "
            "want to share any source code with third parties or otherwise cannot "
            "comply with the terms of the GNU LGPL version 2.1 or GNU GPL version "
            "3.0.</p>"
            "<p>Qt licensed under the GNU LGPL version 2.1 is appropriate for the "
            "development of Qt applications provided you can comply with the terms "
            "and conditions of the GNU LGPL version 2.1.</p>"
            "<p>Qt licensed under the GNU General Public License version 3.0 is "
            "appropriate for the development of Qt applications where you wish to "
            "use such applications in combination with software subject to the "
            "terms of the GNU GPL version 3.0 or where you are otherwise willing "
            "to comply with the terms of the GNU GPL version 3.0.</p>"
            "<p>Please see <a href=\"http://qt.digia.com/Product/Licensing/\">qt.digia.com/Product/Licensing</a> "
            "for an overview of Qt licensing.</p>"
            "<p>Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies) and other "
            "contributors.</p>"
            "<p>Qt and the Qt logo are trademarks of Digia Plc and/or its subsidiary(-ies).</p>"
            "<p>Qt is developed as an open source project on "
            "<a href=\"http://qt-project.org/\">qt-project.org</a>.</p>"
            "<p>Qt is a Digia product. See <a href=\"http://qt.digia.com/\">qt.digia.com</a> "
            "for more information.</p>"
            ;
    aboutQtStolenString = translatedTextAboutQtCaption + bodyDetail;
    QLabel* label = new QLabel(ui->aboutQtScrollArea);
    label->setTextFormat(Qt::RichText);
    label->setWordWrap(true);
    label->setText(aboutQtStolenString);
}
