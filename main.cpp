#include <QApplication>
#include "dlsmainwindow.hpp"
/* 1 Thessalonians 5:9 - For God hath not appointed us to wrath,
 *   but to obtain salvation by our Lord Jesus Christ.
 *  <-||--> What an AmaZinG TRUTH!!!
 */
int main(int argc, char* argv[])
{
    QApplication App(argc, argv);

    DLSMainWindow window;
    window.show();

    return App.exec();
}
