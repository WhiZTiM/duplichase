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
#include <QSplashScreen>
#include <QApplication>
#include <QMessageBox>
#include <QTimer>
#include <QStyle>
#include <QFile>
#include <iostream>
#include "dlsmainwindow.hpp"
#include "dglobals.hpp"

/* 1 Thessalonians 5:9 - For God hath not appointed us to wrath,
 *   but to obtain salvation by our Lord Jesus Christ.
 *  <-||--> What an AmaZinG TRUTH!!!
 */
int main(int argc, char* argv[])
{
    QApplication App(argc, argv);
    std::ios::sync_with_stdio(false);
    QApplication::setApplicationVersion(PROGRAM_VERSION_FULL_STRING);
    QApplication::setApplicationDisplayName("DupLichaSe");
    QApplication::setApplicationName("DupLichaSe");
    QApplication::setOrganizationName("DeltoidCoorp");
    QApplication::setOrganizationDomain("blog.whiztim.com");

    QString fileName;
    const QStringList arguments = App.arguments();
    if(arguments.size() > 2)
    {
        std::cerr << QObject::tr("Invalid Argumentss passed!\n"
                     "NOTE: Only One parameter Needed; and that is"
                     "\ta path to an existing .dlsr file(DupLichaSe Result file)\nor\n"
                     "NOTE: Start the Program without arguments!\n").toStdString();
        return -1;
    }
    else if(arguments.size() == 2)
    {
        fileName = arguments.at(1);
        if(!QFile::exists(fileName))
        {
            QString msg(QObject::tr("Failed to access: \"") + fileName +
                                    QObject::tr("\"\n\tmaybe DupLichaSe does not have read permission here"));
            std::cerr << msg.toStdString() << std::endl;
            return -1;
        }
        if(not fileName.endsWith(".dlsr"))
        {
            QMessageBox msg(QMessageBox::Critical,
                            QObject::tr("DupLichaSe -- Error"),
                            QObject::tr("Invalid DupLichaSe Result file... Aborting The program"),
                            QMessageBox::Ok);
            return msg.exec();
        }
    }

    QPixmap splashPixmap(":/main/splashScreen.png");
    QSplashScreen splashScreen(splashPixmap);
    splashScreen.show();

    DLSMainWindow window;
    splashScreen.show();
    QTimer::singleShot(3300, &window, SLOT(show()));
    QTimer::singleShot(4400, &splashScreen, SLOT(close()));

    if(not fileName.isEmpty())
        window.loadResultsFromFile(fileName);

    return App.exec();
}
