#include "Application.h"
#include <QFont>

namespace MUFCalc {

Application::Application(int& argc, char** argv)
    : QApplication(argc, argv)
{
    setApplicationName("MUFCalc");
    setApplicationDisplayName("MUFCalc — Nuclear Material Accountancy Training");
    setApplicationVersion("2.0.0");
    setOrganizationName("Tomsk Polytechnic University");
    setOrganizationDomain("tpu.ru");

    QFont f("Segoe UI", 10);
    f.setStyleHint(QFont::SansSerif);
    setFont(f);
}

}  // namespace MUFCalc
