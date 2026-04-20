# MUF Calculator v1.0 — Qt Creator / C++ Project
## IAEA Nuclear Material Accounting System

---

## Quick Build Instructions

### Requirements
- Qt 5.12+ or Qt 6.x (with Widgets module)
- Qt Creator 7+ (recommended) OR qmake + make
- C++17-capable compiler (GCC 8+, Clang 7+, MSVC 2019+)

---

### Option A — Qt Creator (Recommended)

1. Open **Qt Creator**
2. File → Open File or Project → select `MUFCalc.pro`
3. Configure kit (e.g. Desktop Qt 5.15.2 GCC 64bit)
4. Press **Ctrl+B** to build
5. Press **Ctrl+R** to run

---

### Option B — Command Line (Linux/macOS)

```bash
# Ubuntu / Debian
sudo apt install qt5-default qtbase5-dev build-essential

cd MUFCalc
qmake MUFCalc.pro
make -j$(nproc)
./MUFCalc
```

### Option C — Command Line (Windows with MinGW)

```bat
cd MUFCalc
qmake MUFCalc.pro
mingw32-make -j4
MUFCalc.exe
```

---

## Project Structure

```
MUFCalc/
├── MUFCalc.pro                 Qt project file (open this in Qt Creator)
├── include/
│   ├── datatypes.h             Core data structures (MBPInput, MUFResult…)
│   ├── styles.h                Application-wide dark theme stylesheet
│   ├── mufengine.h             Main calculation orchestrator
│   ├── validationengine.h      IAEA physical consistency validation
│   ├── uncertaintyengine.h     σ(MUF) propagation + Cᵢ explainability
│   ├── diagnosticsengine.h     Safeguards diagnostic rules
│   ├── reportgenerator.h       HTML + text report export
│   ├── dataentrywizard.h       5-page data entry wizard
│   ├── resultspanel.h          4-tab results display
│   ├── historymanager.h        Calculation history (up to 20 entries)
│   ├── mainwindow.h            Application main window
│   └── aboutdialog.h
├── src/
│   ├── main.cpp
│   ├── mainwindow.cpp
│   ├── mufengine.cpp
│   ├── validationengine.cpp
│   ├── uncertaintyengine.cpp
│   ├── diagnosticsengine.cpp
│   ├── reportgenerator.cpp
│   ├── dataentrywizard.cpp
│   ├── resultspanel.cpp
│   ├── historymanager.cpp
│   └── aboutdialog.cpp
└── resources/
    └── resources.qrc
```

---

## IAEA Equations Implemented

| Step | Equation | Description |
|------|----------|-------------|
| 1 | `IB = BI + ΣInputs − ΣOutputs` | Book Inventory |
| 2 | `MUF = IB − EI` | Material Unaccounted For |
| 3 | `σ(MUF) = √[σ²(BI) + σ²(ΣIn) + σ²(ΣOut)]` | Uncertainty propagation |
| 4 | `Cᵢ = [σ²(i) / σ²(MUF)] × 100%` | Explainability contribution |

---

## Diagnostic Rules

| Code | Condition | Severity |
|------|-----------|----------|
| MUF-NORMAL | `|MUF| ≤ 1σ` | OK |
| MUF-EXCEED-1σ | `1σ < |MUF| ≤ 2σ` | INFO |
| MUF-EXCEED-2σ | `2σ < |MUF| ≤ 3σ` | WARNING |
| MUF-EXCEED-3σ | `|MUF| > 3σ` | CRITICAL |
| NEGATIVE-MUF | `MUF < 0` | ALERT |
| DOMINANT-UNCERTAINTY-SOURCE | `Cᵢ ≥ 50%` | WARNING |
| HIGH-MUF/THROUGHPUT | `|MUF|/throughput > 5%` | ALERT |
| ZERO-SIGMA | `σ(MUF) = 0` | CRITICAL |

---

## License

MIT License — Open source, free for educational and research use.

**Disclaimer:** Not intended for operational IAEA safeguards reporting.
For educational and training use at nuclear facility laboratories only.

---

**Developer:** Samuel Nartey Teye, Student Group 0AM4Ф  
**Supervisor:** Roman Pushkarskiy, Senior Lecturer  
**Institution:** Tomsk, 2025
