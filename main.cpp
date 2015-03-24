//#include "application.h"
#include "TadaridaMainWindow.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    TadaridaMainWindow w;
    if(w.CanContinue) w.show();
    else return(-1);
    a.connect(&w, SIGNAL(endOfMainWindow()), &a, SLOT(quit()));
    return a.exec();
}
