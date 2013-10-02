#include <QApplication>
#include "dlsmainwindow.hpp"

int main(int argc, char* argv[])
{
    QApplication App(argc, argv);

    DLSMainWindow window;
    window.show();

    return App.exec();
}
