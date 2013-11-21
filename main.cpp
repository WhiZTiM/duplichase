#include <QApplication>
#include <QSplashScreen>
#include <QTimer>
#include "dlsmainwindow.hpp"

/* 1 Thessalonians 5:9 - For God hath not appointed us to wrath,
 *   but to obtain salvation by our Lord Jesus Christ.
 *  <-||--> What an AmaZinG TRUTH!!!
 */
int main(int argc, char* argv[])
{
    QApplication App(argc, argv);

    QPixmap splashPixmap(":/main/splashScreen.png");
    QSplashScreen splashScreen(splashPixmap);
    splashScreen.show();
    App.processEvents();

    DLSMainWindow window;
    splashScreen.show();
    QTimer::singleShot(3300, &window, SLOT(show()));
    QTimer::singleShot(4400, &splashScreen, SLOT(close()));
    return App.exec();
}
