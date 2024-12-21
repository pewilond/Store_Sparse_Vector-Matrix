#include <QApplication>
#include "display_window.h"
// #include "test.h"

int main(int argc, char *argv[])
{
    // run_tests();
    QApplication app(argc, argv);

    DisplayWindow window;
    window.setWindowTitle("Data Structures Tester");
    window.resize(800, 600);
    window.show();

    return app.exec();

}
