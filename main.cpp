#include <QApplication>
#include "mainwindow.h"
#include <time.h>

#include <QLocale>
#include <QTranslator>

QString _locale;

int main(int argc, char *argv[]) {
    srand((unsigned)time(0));
    QApplication a(argc, argv);
    _locale = QLocale::system().name();
    QTranslator translator;
    translator.load(QString("qigsaw_") + _locale);
    a.installTranslator(&translator);
    int index = _locale.indexOf('_');
    if (index != -1) _locale = _locale.mid(0,index);
    MainWindow w;
    w.show();
    return a.exec();
}
