#pragma once
// ─────────────────────────────────────────────────────────────────────────
//  MUFCalc :: Application
//  Slim wrapper over QApplication that centralises app-level setup
//  (font, organisation/version metadata, locale).
// ─────────────────────────────────────────────────────────────────────────

#include <QApplication>

namespace MUFCalc {

class Application : public QApplication {
    Q_OBJECT
public:
    Application(int& argc, char** argv);
};

}  // namespace MUFCalc
