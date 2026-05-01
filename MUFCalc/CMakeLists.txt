cmake_minimum_required(VERSION 3.20)
project(MUFCalc VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# ── Qt6 ──────────────────────────────────────────────────────────────────
find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Widgets PrintSupport Network)

# ── Core / Engine library ───────────────────────────────────────────────
set(MUFCALC_CORE_SOURCES
    src/core/SignificantQuantity.cpp
    src/core/UncertaintyEngine.cpp
    src/core/ContributionAnalyzer.cpp
    src/core/StatisticalEvaluator.cpp
    src/core/DiagnosticEngine.cpp
    src/core/MufCalculator.cpp
    src/utils/Validator.cpp
    src/database/HistoryRepository.cpp
    src/reports/HtmlReport.cpp
    src/reports/PlainTextReport.cpp
    src/reports/ReportGenerator.cpp
)
set(MUFCALC_CORE_HEADERS
    src/core/Types.h
    src/core/SignificantQuantity.h
    src/core/UncertaintyEngine.h
    src/core/ContributionAnalyzer.h
    src/core/StatisticalEvaluator.h
    src/core/DiagnosticEngine.h
    src/core/MufCalculator.h
    src/app/Constants.h
    src/app/Styles.h
    src/utils/Validator.h
    src/utils/MathUtils.h
    src/utils/FormatUtils.h
    src/database/HistoryRepository.h
    src/reports/HtmlReport.h
    src/reports/PlainTextReport.h
    src/reports/ReportGenerator.h
)
add_library(mufcalc_core STATIC ${MUFCALC_CORE_SOURCES} ${MUFCALC_CORE_HEADERS})
target_link_libraries(mufcalc_core PUBLIC Qt6::Core Qt6::Gui)
target_include_directories(mufcalc_core PUBLIC ${CMAKE_SOURCE_DIR}/src)

# ── GUI executable ──────────────────────────────────────────────────────
set(MUFCALC_GUI_SOURCES
    src/main.cpp
    src/app/Application.cpp
    src/gui/MainWindow.cpp
    src/gui/DataEntryWizard.cpp
    src/gui/SummaryTab.cpp
    src/gui/EquationsTab.cpp
    src/gui/UncertaintyTab.cpp
    src/gui/DiagnosticsTab.cpp
    src/gui/ResultsPanel.cpp
    src/gui/VirtualFacilityDialog.cpp
    src/gui/StudentGuideDialog.cpp
    src/gui/AboutDialog.cpp
    src/gui/FeedbackDialog.cpp
)
set(MUFCALC_GUI_HEADERS
    src/app/Application.h
    src/gui/MainWindow.h
    src/gui/DataEntryWizard.h
    src/gui/SummaryTab.h
    src/gui/EquationsTab.h
    src/gui/UncertaintyTab.h
    src/gui/DiagnosticsTab.h
    src/gui/ResultsPanel.h
    src/gui/VirtualFacilityDialog.h
    src/gui/StudentGuideDialog.h
    src/gui/AboutDialog.h
    src/gui/FeedbackDialog.h
    src/gui/WatermarkWidget.h
    src/gui/MeasurementMethodDb.h
    src/gui/NuclearTransferItemDb.h
)

add_executable(MUFCalc ${MUFCALC_GUI_SOURCES} ${MUFCALC_GUI_HEADERS}
                       resources/resources.qrc)
target_link_libraries(MUFCalc PRIVATE mufcalc_core Qt6::Widgets Qt6::PrintSupport Qt6::Network)

# ── Tests (optional) ────────────────────────────────────────────────────
option(MUFCALC_BUILD_TESTS "Build unit tests" ON)
if(MUFCALC_BUILD_TESTS)
    find_package(Qt6 COMPONENTS Test)
    if(Qt6Test_FOUND)
        enable_testing()
        add_subdirectory(tests)
    else()
        message(STATUS "Qt6::Test not found — skipping unit tests.")
    endif()
endif()

# ── Compiler hardening ──────────────────────────────────────────────────
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(MUFCalc       PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(mufcalc_core  PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ── Install ─────────────────────────────────────────────────────────────
install(TARGETS MUFCalc RUNTIME DESTINATION bin)
install(DIRECTORY resources/ DESTINATION share/mufcalc OPTIONAL)
