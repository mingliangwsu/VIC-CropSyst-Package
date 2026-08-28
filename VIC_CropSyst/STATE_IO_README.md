# Scenario-branching "initial state" support (2026)

## What this adds

VIC-CropSyst can now save its full soil/snow hydrologic state *and* its
crop/vegetation state on a specific simulated day, as tabular CSV files,
and warm-start a later run from that snapshot. The goal: run several
weather/management scenarios forward from one shared starting point
without re-simulating each one from the beginning.

This is additive to, and does not change, the existing binary/ASCII
`BINARY_STATE_FILE` mechanism (`write_model_state.c` /
`read_initial_model_state.c`), which already handled the hydrology side.

## Enabling it

Add to the global parameter file, alongside the existing state options:

```
INIT_STATE      <path/to/prior/run's/state>
STATENAME       <path/for/this/run's/state>
STATEYEAR       <yyyy>
STATEMONTH      <mm>
STATEDAY        <dd>
CSV_STATE_FILE  TRUE
```

`CSV_STATE_FILE` is new. When `TRUE`, in addition to whatever
`BINARY_STATE_FILE` produces, VIC reads/writes four companion CSV
files, derived by suffixing the existing `INIT_STATE`/`STATENAME`
paths:

| File                          | Contents                                   |
|--------------------------------|---------------------------------------------|
| `<STATENAME>_soil.csv`         | soil moisture/ice/temperature, snow, dew   |
| `<STATENAME>_crop.csv`         | crop growth stage, biomass, GAI, root depth, accumulated thermal time |
| `<INIT_STATE>_soil.csv`        | read on startup to warm-start hydrology    |
| `<INIT_STATE>_crop.csv`        | read on startup to warm-start crop state (see limitation below) |

No other global-parameter keywords are needed.

## New source files

- `vic_state_csv.h` / `.c` — hydrology (soil/snow) state CSV read/write.
  Mirrors `write_model_state.c`'s exact data model in a wide,
  tabular, pandas/R/Excel-friendly format (one row per grid cell x veg
  tile x snow band). Validated with a standalone round-trip test
  (see `test_csv_roundtrip.c`, not part of the VIC build, kept for
  reference during development).
- `crop/VIC_crop_state_csv.h` / `.c` — crop/vegetation state CSV
  read/write and the `apply_crop_state()` restore entry point.

## Modified files

- `vicNl_def.h` — new `option_struct.CSV_STATE_FILE` flag; new
  `filep_struct`/`filenames_struct` fields for the companion CSV paths.
- `get_global_param.c`, `initialize_global.c` — parse/default the new
  `CSV_STATE_FILE` keyword.
- `vicNl.c` — opens/closes the output CSV state files; resolves the
  input CSV state file paths.
- `initialize_model_state.c` — on `INIT_STATE`, reads from the CSV
  soil-state file instead of the binary one when `CSV_STATE_FILE` is
  set (falls back to cold-start initialization, with a warning, if a
  given cell isn't present in the CSV file).
- `dist_prec.c` — at the existing STATEYEAR/STATEMONTH/STATEDAY
  trigger (same one `write_model_state()` uses), also writes the
  soil and crop CSV rows.
- `crop/VIC_crop_variable_codes.h` — new prognostic variable codes
  (`VC_accum_degree_days`, `VC_growth_stage_code`,
  `VC_root_biomass_kg_m2`), with a note on their scope (see "Two
  separate variable-code enums" below).
- `crop/VIC_CropSyst_proper_crop.h` / `.cpp` — new `restore_state()`
  method; also **fixed a pre-existing bug**: `CropSyst_proper_crop::get()`
  was a dead stub (its entire switch body was commented out,
  referencing member names -- `canopy_cover_actual`, `biomass_actual`,
  `transpiration_actual`, `root` -- that don't exist on this class).
  Replaced with a working implementation using confirmed public
  accessors (`get_GAI()`, `get_canopy_biomass_kg_m2()`,
  `get_accum_degree_days()`, etc.).
- `agronomic/VIC_land_unit_simulation.h` / `.cpp` — `restore_state()`
  forwarding method (the real V3 entry point: `active_land_unit` is a
  `VIC::Land_unit_simulation*`, not a `CropSyst_proper_crop*` -- see
  below); seven new `..._for_state()` dedicated accessors used by the
  CSV writer.
- `agronomic/VIC_land_unit_C_interface.h` / `.cpp` — C-callable
  wrappers: `VIC_land_unit_restore_crop_state()` and the seven
  `VIC_land_unit_get_..._for_state()` accessors.
- `crop/VIC_crop_C_interface.h` / `.cpp` — equivalent
  `VIC_CropSyst_restore_state()` wrapper added to the legacy V2-style
  interface too, for consistency, though V3 is the pathway that
  actually matters (see below).

## Architecture notes worth knowing before extending this

**Two build-era crop interfaces coexist in this codebase.**
`crop/VIC_crop_C_interface.cpp` (with `active_CropSyst`) is the
legacy V2-style driver interface; `agronomic/VIC_land_unit_C_interface.cpp`
(with `active_land_unit`, of type `VIC::Land_unit_simulation*`) is the
real, active V3 interface (confirmed by checking which files the V3
Qt/Makefile build targets include, and by finding that both files
define a same-named `VIC_CropSyst_get()` function -- they can't both
be linked into the same binary). All the V3-relevant work in this
feature (the accessors used by the CSV writer, `restore_state()`) is
wired through `agronomic/VIC_land_unit_C_interface.cpp`.

**Two separate variable-code enums exist and must not be confused.**
`crop/VIC_crop_variable_codes.h` defines `Variable_code`, consumed by
`CropSyst_proper_crop::get()`. `agronomic/VIC_land_unit_simulation.h`
defines a *different*, independently-numbered `VIC::CropSyst_Variables`
enum, consumed by `Land_unit_simulation::get()` -- which is what
`VIC_land_unit_get()`/`VIC_CropSyst_get()` actually dispatch to in the
V3 build. `Land_unit_simulation::get()` does NOT forward unhandled
codes to `crop_active_or_intercrop->get()`, so passing a
`Variable_code` value into `VIC_CropSyst_get()` in a V3 build silently
does the wrong thing (interprets it as a `VIC::CropSyst_Variables`
value). This was caught and fixed during development: the crop-state
CSV writer originally called `VIC_CropSyst_get(VC_accum_degree_days)`
etc., which would have compiled but returned wrong values. It now uses
seven dedicated, unambiguous accessor functions
(`VIC_land_unit_get_accum_degree_days()` etc.) that query the active
crop object directly, with no enum involved. If you add more
prognostic fields to the CSV, follow this pattern rather than reusing
either variable-code enum across the C/C++ boundary.

**`crop_data_struct`'s growth fields are V2-only.** Fields like
`crop->biomass_current`, `crop->growth_stage`, `crop->water_stress_index`,
`crop->days_in_Gseason` (see `vicNl_def.h`) are only ever populated by
the legacy `call_crop_model.c` (V2) driver. In the V3 build they are
stale/unpopulated. `write_crop_state_csv()` therefore sources these
values from the dedicated accessors above, not from `crop_data_struct`,
except for `crop->code` and `crop->CropSystHandle` (bookkeeping fields
that are populated in both versions) and `crop->days_in_Gseason`,
which is currently just written as 0 for V3 (informational only, not
used by restore).

## Known limitation: the mid-season crop creation problem

This is the one piece of the original ask that remains genuinely open,
and it's a modeling/architecture decision, not a small bug.

In VIC-CropSyst V3, a crop object is created when the rotation/event
engine (`CropSyst::Land_unit_simulation`, in the proprietary CropSyst
engine proper) reaches its calendar-scheduled sowing event, evaluated
day by day as `process_day()` is called. If a run starts (via
`INIT_STATE`) on a day *after* that sowing date -- which is exactly
the warm-restart scenario this feature exists for -- the sowing event
for a crop that should already be active never fires, because the
days before the restart are never simulated. The crop is simply never
created, and there is nothing for `restore_state()` to restore into.

`restore_state()` and the C-level plumbing down to it
(`VIC_land_unit_restore_crop_state()`) are fully implemented and ready
to use CropSyst's own designed-for-this restart hooks
(`Canopy_leaf_growth::restart_with()`, `Crop_root::initialize()`,
`Phenology_2018::activate_*()`) once a crop object exists. What's
missing is the trigger: something that recognizes, on the run's start
day, that a crop should already exist per the loaded state, and forces
it into existence before calling `restore_state()`. Two ways to close
this gap, in order of how much they touch the proprietary engine:

1. **Recommended, no engine changes needed.** Configure the
   *rotation/management parameter files* for a branch run so that the
   crop of interest is scheduled to be sown starting on the restart
   date itself, rather than its original planting date. Combined with
   the (already-working) soil-state warm start, this gets the
   dominant benefit -- shared, consistent soil physical state across
   scenarios -- without touching the rotation engine at all. The crop
   then grows from a fresh (but immediately restore_state()-corrected)
   start rather than a continuous one; call `restore_state()` right
   after this "day-zero" sowing event fires to snap biomass/GAI/root
   depth/growth stage to the saved values.
2. **More invasive.** Extend `CropSyst::Land_unit_simulation`'s event
   scheduler with a "force-sow on an arbitrary date, bypassing the
   normal rotation schedule" entry point. This requires deep
   familiarity with the proprietary engine's `Sowing_event`/
   `Sowing_operation`/event-scheduler internals
   (`agronomic/VIC_land_unit_simulation.cpp` around the sowing-event
   generation code is the starting point) and a working build/test
   environment to validate against -- deliberately not attempted here
   without the ability to compile and test it.

## Known limitation: the branch date's own output row is not continuous

Because `restore_state()` must run *after* the crop object already
exists for the restart day (see the mid-season crop creation problem
above -- the crop only gets created via that same day's forced-sowing
workaround), and the crop's daily diagnostic output row is computed
and written as part of that same day's normal processing (inside
`full_energy()`, before `restore_state()` runs), the branch date's own
row in the warm-started run's `.asc` output reflects the crop's
fresh-planting state (`GAI`/biomass = 0, `Grow_Stage` =
`germination&planting`), not the restored values.

For example, given a branch date of 1985-07-10 with a saved state of
`GAI=3.886`, `Biomass=0.971`: the source run's own 1985-07-10 row shows
those values, but the warm-started run's 1985-07-10 row shows
`GAI=0`/`Biomass=0`. The very next day (1985-07-11) onward is fully
continuous with the source run -- `GAI`, biomass, canopy cover, root
depth, and accumulated thermal time all match the source run's branch-
day values (within normal floating-point precision) and evolve
sensibly from there.

**Practical implication:** treat the branch date's row in a warm-
started run as a bootstrap artifact, not real data. Any analysis,
plotting, or comparison against the source run should start from
`STATEDAY + 1`, not `STATEDAY` itself.

This has the same root cause, and the same two possible fixes, as the
mid-season crop creation problem above: it stems from `restore_state()`
necessarily running after that day's crop creation and output writing,
both of which happen inside the same `full_energy()` call. A genuine
fix would mean locating and re-invoking whatever function inside
`agronomic/VIC_land_unit_C_interface.cpp` populates that daily output
row (the `CROP_DAILY_OUTPUT_MEMFIRST`-guarded code paths are the
starting point) a second time, after `restore_state()` completes for
that day -- not attempted here, since it would need its own
compile/test cycle to confirm the output function is safely
re-callable (e.g. that calling it twice for one day doesn't produce a
duplicate row) in an environment where that could actually be
verified.

## Known limitation: exact thermal-time restoration

`restore_state()` restores growth **stage** exactly (via
`Phenology_2018::activate_*()`), and biomass/canopy/root depth
exactly (via the engine's own `restart_with()`/`initialize()` hooks).
It does not currently restore the *exact* within-stage accumulated
degree-days: `Phenology_2018` keeps that accumulator (`GDDs`) private
with no public setter. The value is still captured and written to the
crop CSV for inspection/validation (e.g. confirming two scenario runs
agree at the branch day), and `restore_state()` accepts it as a
parameter for exactly this purpose -- extending it to also force the
accumulator would need a small addition to `phenology_2018.h`/`.cpp`
itself (a new public setter), which was out of scope here since it
touches the proprietary CropSyst engine rather than the VIC wrapper
layer.

## What was verified vs. not

The full project (CropSyst engine + `corn`/`UED` support libraries +
VIC-CropSyst wrapper) is a large legacy C/C++ codebase targeting a
project-specific Qt/Makefile build that wasn't available to run in
this environment. What could be verified:

- The hydrology CSV read/write **algorithm** (the trickiest part --
  parsing dynamically-sized, comma-separated wide rows correctly) was
  extracted into a standalone C program with representative stub types
  and round-trip tested (`gcc`, passes).
- Every new/changed function's declaration, definition, and call sites
  were manually cross-checked for matching names, parameter counts,
  and types across all files (see the grep-based consistency checks
  used throughout development).
- Every C++ member referenced (`canopy_leaf_growth`, `roots_current`,
  `phenology`, `get_accum_degree_days()`, `get_canopy_biomass_kg_m2()`,
  `get_act_root_biomass_kg_m2()`, `get_GAI()`,
  `get_recorded_root_depth_m()`, `get_water_stress_index()`,
  `ref_phenology()`, `Phenology_2018::activate_*()`,
  `Canopy_leaf_growth::restart_with()`, `Crop_root::initialize()`) was
  located and confirmed to exist, with the correct signature and
  access specifier, by reading the actual CropSyst engine source
  rather than assumed from naming conventions.

What was **not** verified: full compilation of the actual project
build (no toolchain available here), and therefore no guarantee
against typos or minor signature mismatches that only a real compiler
pass would catch. Recommend a compile pass with the project's own
`build/Qt/Linux_gcc/VIC_CropSyst_V3.pro` or
`build/Xcc/Release/Makefile_Kamiak` as the first next step.
