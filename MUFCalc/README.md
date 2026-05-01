# MUFCalc — Nuclear Material Accountancy Training System

**Version 2.0 (Professional Edition)**
A free, open-source, cross-platform desktop application for university-level training in
Nuclear Material Accountancy (NMA), implementing IAEA INFCIRC/153 safeguards concepts
including MUF, σ(MUF), CUSUM, SITMUF, CUMUF, and 12 safeguards diagnostic flags.

> **Educational use only.** MUFCalc is a training tool. It is not an operational
> safeguards system, does not connect to any State or facility data source, and
> must not be used for real material accounting submissions.

---

## Project Information

| | |
|---|---|
| **Author** | Samuel Nartey Teye (Posta) |
| **Institution** | Tomsk Polytechnic University, School of Nuclear Science and Engineering |
| **Programme** | M.Sc. Nuclear Physics and Technology / Nuclear Safety & Security |
| **Supervisor** | Roman Pushkarsky, Senior Lecturer |
| **Academic year** | 2025–2026 |
| **License** | MIT |
| **Framework** | C++17 / Qt 6.5+ |
| **Target platforms** | Linux (primary), macOS, Windows |

---

## What's new in v2.0

The v1 codebase has been re-architected to a layered, one-class-per-file structure
matching the architecture specification. The engine is now decomposed into
single-responsibility modules behind a thin orchestrator.

| v1.0 | v2.0 |
|---|---|
| `MUFEngine` (monolithic) | `MufCalculator` orchestrator + 5 single-responsibility engines |
| `NuclearItem` / `MBPInput` / `MUFResult` | `InventoryComponent` / `MufInput` / `MufResult` (IAEA-aligned naming) |
| `.qty` / `.error` / `.label` | `.weight` / `.uncertainty` / `.name` (+ new `.bias`, `.relativeUncertainty`) |
| One-file `DiagnosticsEngine` (CUSUM + 12 flags + state) | `StatisticalEvaluator` (CUSUM/SITMUF/CUMUF state) + `DiagnosticEngine` (12 flag checks) |
| `UncertaintyEngine::buildContributions()` (private helper) | `ContributionAnalyzer` (Explainable AI layer, standalone class) |
| `HistoryManager` (in-memory) | `HistoryRepository` DAO interface (in-memory now, SQLite-ready) |
| Single `ResultsPanel` builds 4 tabs inline | 4 standalone `QWidget` subclasses (`SummaryTab`, `EquationsTab`, `UncertaintyTab`, `DiagnosticsTab`) |
| One `ReportGenerator` class | `HtmlReport` + `PlainTextReport` + thin `ReportGenerator` facade |
| `qmake`-only | `CMakeLists.txt` + `MUFCalc.pro` (both supported) |
| No automated tests | Qt Test unit tests for the engine layer |
| Global namespace | Everything in `namespace MUFCalc` |

The σ(MUF) HTML report now correctly includes the EI variance term (a v1 oversight).

---

## Building from source

### Linux (Ubuntu 22.04 / RHEL 9)

```bash
sudo apt install qt6-base-dev qt6-tools-dev cmake g++
git clone <repo>
cd MUFCalc
./scripts/build_linux.sh
./build-linux/MUFCalc
```

### Qt Creator (any platform)

Open `MUFCalc.pro` in Qt Creator and click *Build*.

### Manual CMake

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --test-dir build --output-on-failure
```

---

## Project layout

```
MUFCalc/
├── CMakeLists.txt          # Primary build file
├── MUFCalc.pro             # qmake alternative
├── README.md               # This file
├── docs/
│   └── architecture.md     # Detailed architecture
├── src/
│   ├── main.cpp
│   ├── app/                # Application shell, constants, styles
│   │   ├── Application.{h,cpp}
│   │   ├── Constants.h
│   │   └── Styles.h
│   ├── core/               # Business logic (one class per file)
│   │   ├── Types.h                    # InventoryComponent, MufInput, MufResult, …
│   │   ├── MufCalculator.{h,cpp}      # Orchestrator
│   │   ├── UncertaintyEngine.{h,cpp}  # σ(MUF) propagation (GUM)
│   │   ├── ContributionAnalyzer.{h,cpp}# Explainable AI (Cᵢ)
│   │   ├── StatisticalEvaluator.{h,cpp}# CUSUM, SITMUF, CUMUF
│   │   ├── DiagnosticEngine.{h,cpp}   # 12 safeguards flags
│   │   └── SignificantQuantity.{h,cpp}# IAEA SQ resolver
│   ├── utils/
│   │   ├── Validator.{h,cpp}          # Input validation
│   │   ├── MathUtils.h
│   │   └── FormatUtils.h
│   ├── database/
│   │   └── HistoryRepository.{h,cpp}  # DAO (in-memory, SQLite-ready)
│   ├── reports/
│   │   ├── HtmlReport.{h,cpp}
│   │   ├── PlainTextReport.{h,cpp}
│   │   └── ReportGenerator.{h,cpp}    # Format-routing facade
│   └── gui/                # Qt widgets
│       ├── MainWindow.{h,cpp}
│       ├── DataEntryWizard.{h,cpp}
│       ├── ResultsPanel.{h,cpp}       # Thin shell
│       ├── SummaryTab.{h,cpp}
│       ├── EquationsTab.{h,cpp}
│       ├── UncertaintyTab.{h,cpp}
│       ├── DiagnosticsTab.{h,cpp}
│       ├── VirtualFacilityDialog.{h,cpp}
│       ├── StudentGuideDialog.{h,cpp}
│       ├── AboutDialog.{h,cpp}
│       ├── WatermarkWidget.h
│       ├── MeasurementMethodDb.h      # IAEA STR-368 measurement methods
│       └── NuclearTransferItemDb.h
├── tests/
│   ├── CMakeLists.txt
│   └── unit/
│       ├── TestMufCalculator.cpp
│       ├── TestDiagnosticEngine.cpp
│       └── TestSignificantQuantity.cpp
├── resources/              # Future: scenarios JSON, lessons MD, i18n .ts files
└── scripts/
    └── build_linux.sh
```

---

## IAEA standards implemented

| Standard | Use in MUFCalc |
|---|---|
| INFCIRC/153 (Corrected) | MUF reporting, MBA structure, Article 25/29/76 references |
| IAEA Services Series 15 (NMA Handbook) | MUF, IB, σ(MUF) calculation methodology |
| STR-368 (ITV 2010) | Per-stream measurement uncertainties (Type B) |
| IAEA Safeguards Glossary 2022 | Definitions, SQ values (§3.14), timeliness goals (§3.17) |
| GUM JCGM 100:2008 | Uncertainty propagation, sign conventions |
| Avenhaus 1977; Burr et al. 2013 | CUSUM, SITMUF sequential-test theory |

---

## Educational scope and disclaimer

MUFCalc is intended as a teaching aid for nuclear engineering courses. It models
the *concepts* of nuclear material accounting using realistic-but-fictional
virtual facility data. It is not connected to any operator or State-level
accounting system, and must not be used to produce, replace, or audit actual
safeguards records. Numerical agreement between MUFCalc and any IAEA worked
example is intentional and pedagogical, not operational.
