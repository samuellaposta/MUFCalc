# MUFCalc 2.0 Architecture

This document describes the layered architecture of MUFCalc v2.0 and the
rationale for each split. The headline change from v1.0 is that the calculation
pipeline has been decomposed into single-responsibility classes behind a thin
orchestrator, and the public types have been renamed to align with IAEA
terminology and GUM conventions.

## Layered overview

```
┌──────────────────────────────────────────────────────────────────────┐
│  GUI layer (Qt Widgets)                                              │
│  MainWindow → DataEntryWizard → ResultsPanel → {Summary, Equations,  │
│                                                  Uncertainty,        │
│                                                  Diagnostics}Tab     │
│              → VirtualFacilityDialog → StudentGuideDialog            │
└────────────────────────────────┬─────────────────────────────────────┘
                                 │  MufInput / MufResult
                                 ▼
┌──────────────────────────────────────────────────────────────────────┐
│  Reports layer                                                       │
│  ReportGenerator (facade) ── HtmlReport ── PlainTextReport           │
└────────────────────────────────┬─────────────────────────────────────┘
                                 │
┌────────────────────────────────▼─────────────────────────────────────┐
│  Business logic — namespace MUFCalc                                  │
│  ┌─────────────────────────────────────────────────────────────────┐ │
│  │           MufCalculator  (six-step orchestrator)                │ │
│  └──────┬──────────┬───────────┬─────────────┬───────────┬────────┘ │
│         ▼          ▼           ▼             ▼           ▼          │
│   Validator  Uncertainty  Contribution  Statistical Diagnostic     │
│              Engine       Analyzer      Evaluator   Engine         │
│   (utils/)   (GUM)        (Cᵢ XAI)      (CUSUM)     (12 flags)     │
└────────────────────────────────┬─────────────────────────────────────┘
                                 │
┌────────────────────────────────▼─────────────────────────────────────┐
│  Data layer                                                          │
│  HistoryRepository (in-memory; DAO interface, SQLite-ready)          │
└──────────────────────────────────────────────────────────────────────┘
```

## Design principles

**One responsibility per class.** v1's `DiagnosticsEngine` carried both the
sequential-test state (CUSUM/CUMUF accumulators) and the 12 flag rules. v2 splits
these so a unit test can exercise the flag logic against a hand-crafted result
without touching CUSUM state, and vice versa.

**Pure data structures crossing layer boundaries.** `MufInput` and `MufResult`
are plain aggregates with no Qt parent-child semantics. The same struct travels
from the wizard, through the engine, through the repository, into the report
renderer, with no copies or adapters.

**Repository pattern for persistence.** v2 hides the in-memory store behind
`HistoryRepository`. A SQLite-backed implementation can drop in by re-implementing
the `add` / `at` / `clear` / `size` interface, with no GUI changes.

**Format-routing facade for reports.** `ReportGenerator` only routes to
`HtmlReport` or `PlainTextReport`. New formats (CSV, PDF) are additive — they do
not require touching any existing renderer.

## Six-step calculation pipeline

`MufCalculator::calculate(const MufInput&)`:

1. **Validate** the input via `Validator`. If any field fails, return a
   `MufResult` with `success=false` and a populated `errorMessage`. Validation
   covers text-field safety (anti-injection), date sanity, weight/uncertainty
   bounds (`ValidationLimits` in `app/Constants.h`), and the physical-consistency
   check that IB ≥ 0.
2. **Propagate σ(MUF)** via `UncertaintyEngine` using the GUM Type B rule:
   `σ²(MUF) = σ²(BI) + Σ σ²(In) + Σ σ²(Out) + σ²(EI)`. v2 includes the EI term,
   which v1's report HTML had omitted.
3. **Compute IB and MUF.** `IB = BI + ΣIn − ΣOut`; `MUF = IB − EI`. The ratio
   `|MUF| / σ(MUF)` is the canonical significance score.
4. **Explainable AI.** `ContributionAnalyzer` computes
   `Cᵢ = σ²(i) / σ²(MUF) × 100 %` for every term and identifies the dominant
   uncertainty source. Sorted descending so the GUI bar chart is ready to render.
5. **Sequential statistics.** `StatisticalEvaluator` updates persistent
   accumulators across MBP periods: SITMUF (per-period), Page's CUSUM (S+, S−),
   CUMUF (running ΣMUF). State is reset only via
   `MufCalculator::resetSequentialState()`, exposed through MainWindow's
   *Clear History* action.
6. **Safeguards diagnostics.** `DiagnosticEngine` applies the 12 flag checks
   from the populated `MufResult`. Each flag carries severity, code, message,
   recommended action, and a citation to the originating IAEA / literature source.
   Severities cascade: if no flag of severity ≥ INFO fires, a synthetic
   `MUF-NORMAL` flag is added so the user always sees an explicit verdict.

## Naming alignment

The architecture document drove a public rename to align with IAEA NMA Handbook
terminology:

| v1 name        | v2 name                | Rationale |
|----------------|------------------------|-----------|
| `NuclearItem`  | `InventoryComponent`   | IAEA Services Series 15 vocabulary |
| `qty`          | `weight`               | Unambiguous physical quantity |
| `error`        | `uncertainty`          | GUM-correct (1σ standard deviation) |
| `label`        | `name`                 | Disambiguates from `QLabel` widgets |
| `MBPInput`     | `MufInput`             | Matches the calculator class name |
| `MUFResult`    | `MufResult`            | Same |
| `MUFEngine`    | `MufCalculator`        | "Calculator" reflects the role |

`bias` and `relativeUncertainty` are new additions documented in `Types.h`.
`bias` is signed (a measured systematic offset can be ±), while `uncertainty`
remains non-negative because it represents the spread of a distribution.

## Sign conventions (recommendation #1)

A common student error is to assume measurement uncertainty is signed. v2's
data model encodes the GUM-correct convention:

- `uncertainty` (1σ) is non-negative — it is a standard deviation.
- `bias` is signed — positive means the instrument reads high, negative means
  it reads low. Bias and uncertainty are independent and reported separately
  per STR-368.

The NMA Data tab in `VirtualFacilityDialog` displays both columns side-by-side,
with bias colour-coded (red = +, green = −, grey = zero) so the sign convention
is visible at a glance.

## Per-stream measurement methods (recommendation #2)

Each `VirtualFacility` carries a `methodsTableHtml` field, populated with a
table of typical 1σ values per stream and per measurement method (drawn from
STR-368). This makes the point that an enrichment plant's UF6 cylinder weighing
(0.1–0.3 % gross), a reprocessing plant's IDMS at the input accountability
tank (0.2–0.4 %), and a research reactor's serial-number counting (essentially
zero) cannot share a single uncertainty formula.

## Single primary KMP per worked example (recommendation #3)

Each facility now declares a `primaryKmp` and a `primaryKmpRationale` so the
worked-example calculation is anchored to one Key Measurement Point and yields
a consistent figure. Choices:

- UF6 enrichment → KMP-B (LEU product cylinders)
- MOX fabrication → KMP-2 (assemblies output)
- PUREX reprocessing → KMP-IAT (input accountability tank)
- Research reactor → KMP-Core (in-core inventory)

The Scenario tab renders this as a *Primary KMP for Worked Example* callout
above the methods table.

## Threading and Qt object ownership

The calculation pipeline is synchronous and lives entirely on the GUI thread.
For the per-MBP timing of a training run (microseconds), no offloading is
required. If the JSON scenario loader (future work) ever needs to ingest
multi-MB datasets, `MufCalculator::calculate` is pure-function-shaped against
its input and is trivial to move into a worker `QThread`.

GUI widgets follow standard Qt parent-child ownership. The four tab classes
(`SummaryTab`, etc.) are `QWidget` subclasses owned by the `QTabWidget` inside
`ResultsPanel`. Engine objects are value-typed members of `MainWindow` — they
have no `QObject` parent and are destroyed with the window.

## What is *not* in v2 (deliberate scope decisions)

- **No SQLite yet.** The repository pattern is in place, but the in-memory
  implementation is sufficient for single-session training. SQLite migration
  is a future-work line item.
- **No JSON scenario loader.** Scenarios are still hard-coded in
  `VirtualFacilityDialog` so the defence package is self-contained. The
  `resources/scenarios/` directory is reserved for the JSON format described
  in the architecture document.
- **No internationalisation.** `resources/i18n/` is reserved for `.ts` files;
  no translation strings are wired up yet.
- **No PDF report.** HTML and plain text are the supported export formats. PDF
  via `QPrinter` is a future-work item.
