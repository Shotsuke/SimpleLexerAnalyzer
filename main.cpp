#include "mymodel.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MyModel w;
    w.show();
    return a.exec();
}
