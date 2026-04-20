#include <QApplication>
#include <QFont>
#include "mainwindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    app.setApplicationName("MUF Calculator");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("IAEA Safeguards Educational Tools");

    // Clean system font fallback
    QFont f("Segoe UI", 10);
    f.setStyleHint(QFont::SansSerif);
    app.setFont(f);

    MainWindow w;
    w.show();
    return app.exec();
}
