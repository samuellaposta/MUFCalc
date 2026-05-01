QT      += core gui widgets printsupport network
CONFIG  += c++17

TARGET   = MUFCalc
TEMPLATE = app
VERSION  = 2.0.0

# ── Source organisation ─────────────────────────────────────────────────
INCLUDEPATH += src

SOURCES += \
    src/main.cpp \
    src/app/Application.cpp \
    src/core/SignificantQuantity.cpp \
    src/core/UncertaintyEngine.cpp \
    src/core/ContributionAnalyzer.cpp \
    src/core/StatisticalEvaluator.cpp \
    src/core/DiagnosticEngine.cpp \
    src/core/MufCalculator.cpp \
    src/utils/Validator.cpp \
    src/database/HistoryRepository.cpp \
    src/reports/HtmlReport.cpp \
    src/reports/PlainTextReport.cpp \
    src/reports/ReportGenerator.cpp \
    src/gui/MainWindow.cpp \
    src/gui/DataEntryWizard.cpp \
    src/gui/SummaryTab.cpp \
    src/gui/EquationsTab.cpp \
    src/gui/UncertaintyTab.cpp \
    src/gui/DiagnosticsTab.cpp \
    src/gui/ResultsPanel.cpp \
    src/gui/VirtualFacilityDialog.cpp \
    src/gui/StudentGuideDialog.cpp \
    src/gui/AboutDialog.cpp \
    src/gui/FeedbackDialog.cpp

HEADERS += \
    src/app/Application.h \
    src/app/Constants.h \
    src/app/Styles.h \
    src/core/Types.h \
    src/core/SignificantQuantity.h \
    src/core/UncertaintyEngine.h \
    src/core/ContributionAnalyzer.h \
    src/core/StatisticalEvaluator.h \
    src/core/DiagnosticEngine.h \
    src/core/MufCalculator.h \
    src/utils/Validator.h \
    src/utils/MathUtils.h \
    src/utils/FormatUtils.h \
    src/database/HistoryRepository.h \
    src/reports/HtmlReport.h \
    src/reports/PlainTextReport.h \
    src/reports/ReportGenerator.h \
    src/gui/MainWindow.h \
    src/gui/DataEntryWizard.h \
    src/gui/SummaryTab.h \
    src/gui/EquationsTab.h \
    src/gui/UncertaintyTab.h \
    src/gui/DiagnosticsTab.h \
    src/gui/ResultsPanel.h \
    src/gui/VirtualFacilityDialog.h \
    src/gui/StudentGuideDialog.h \
    src/gui/AboutDialog.h \
    src/gui/FeedbackDialog.h \
    src/gui/WatermarkWidget.h \
    src/gui/MeasurementMethodDb.h \
    src/gui/NuclearTransferItemDb.h

# ── Compiler warnings ───────────────────────────────────────────────────
*g++*|*clang*: QMAKE_CXXFLAGS += -Wall -Wextra -Wpedantic

# ── Resources (images, qrc-bundled) ─────────────────────────────────────
RESOURCES += resources/resources.qrc
