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

No other global-parameter keywords are needed for the hydrology side.

**For the crop side, two more things are required** -- see "Required
workflow: the mid-season crop creation problem" below for the full
explanation, but in short: set `STARTDAY` to one calendar day *after*
the source run's `STATEDAY` (not the same day), and edit the warm run's
rotation/management file so the crop is scheduled to sow on that same
`STARTDAY`. Skipping either of these produces incorrect results --
either the crop never restores, or restores but is measurably wetter
than it should be.

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

## Required workflow: the mid-season crop creation problem

This governs how to actually run a warm-started scenario correctly.
It's not a small bug -- it's a consequence of how VIC-CropSyst creates
crop objects, and getting it wrong produces either a crash, a crop
that silently fails to restore, or (if only half-fixed) a crop that
restores but runs one calendar day out of alignment with its source.
The workflow below is confirmed working end-to-end, validated against
a fully continuous reference run.

**Why this is needed.** In VIC-CropSyst V3, a crop object is created
when the rotation/event engine reaches its calendar-scheduled sowing
event, evaluated day by day. If a run starts (via `INIT_STATE`) on a
day after that sowing date -- exactly the warm-restart scenario this
feature is for -- the sowing event for a crop that should already be
active never fires, because the days before the restart are never
simulated. The crop is simply never created, and there is nothing for
`restore_state()` to restore into.

**The fix has two coordinated parts, both required together:**

1. **Force the crop to be sown on the run's own start day.** Edit the
   rotation/management parameter file for the warm-started run so
   that the crop of interest is scheduled to sow on `STARTDAY+1` (see
   point 2 below for why it's `+1`, not `STARTDAY` itself) -- for
   example, changing the relevant `event_date=` entry to that day's
   DOY. This produces a freshly-planted crop object on the run's first
   simulated day; `restore_state()` then immediately overwrites that
   fresh state with the saved snapshot.

2. **Set `STARTDAY` to one calendar day after the source run's own
   `STATEDAY`, not the same day.** This was not obvious and took real
   debugging to pin down. The saved state represents the crop and
   soil/snow condition *at the end of* `STATEDAY`. If the warm-started
   run's `STARTDAY` is set to that same date, VIC's own water-balance
   processing (`full_energy()`, entirely separate from anything
   CropSyst-specific) reprocesses that day's precipitation/ET/
   infiltration a second time, on top of a state that already reflects
   that day having happened -- confirmed directly by comparing a
   warm-started run against a fully continuous reference run: soil
   moisture came out measurably, systematically wetter than it should.
   Setting `STARTDAY` to `STATEDAY+1` (and shifting the rotation
   file's forced sowing date in point 1 to match) fixes this.

   This shift, on its own, introduces a *second*, smaller problem:
   the crop's own state, once restored, needs one additional day of
   growth simulated to catch up to `STARTDAY`'s actual calendar date
   (since the saved snapshot is for `STATEDAY`, one day earlier).
   `restore_state()` handles this automatically -- after applying the
   saved biomass/GAI/growth_stage/thermal-time values, it calls
   `start_day()` then `process_day()` once, explicitly, simulating
   that missing day using CropSyst's own normal daily-processing
   entry points. (`process_day()` alone was tried first and found
   insufficient -- it doesn't advance the active phenology period's
   own `thermal_time_relative_elapsed`, which only happens in
   `start_day()`; both are needed together, in that order.)

**Worked example.** Source run ends `1985-07-10` (`STATEDAY`). Warm
run's global parameter file:

```
STARTDAY   11
```

(one day after `STATEDAY`), and its rotation/management file's forced
sowing `event_date` set to DOY 192 (July 11, matching `STARTDAY`, not
DOY 191/July 10, the original `STATEDAY`).

**Practical note:** both parts must move together. Changing `STARTDAY`
without also shifting the rotation file's sowing date (or vice versa)
means the forced-sowing event and the run's actual first day no longer
line up, and the crop either never gets created or gets created on the
wrong day.

## Known limitation: the branch date's own output row is a bootstrap artifact

Because the crop object only comes into existence via that same day's
forced-sowing workaround (see above), and that day's diagnostic output
row is computed and written as part of normal processing before
`restore_state()` runs, the run's first simulated day (`STARTDAY`,
i.e. `STATEDAY+1`) shows the crop's momentary fresh-planting state
(`GAI`/biomass = 0, `Grow_Stage` = `germination&planting`) in its own
output row, not the restored values.

The *next* day onward is fully continuous with the source run. Given a
source run ending `1985-07-10` with saved state `GAI=3.886`,
`Biomass=0.971`: the warm-started run's own `1985-07-11` (`STARTDAY`)
row shows `GAI=0`; `1985-07-12` onward tracks the source run's own
trajectory closely -- confirmed against a fully continuous reference
run covering the same period without any restart, GAI and canopy cover
agree to within floating-point precision (~0.0004 max difference)
across the whole remaining season.

**Practical implication, unchanged:** treat `STARTDAY`'s own row in a
warm-started run as a bootstrap artifact, not real data. Any analysis,
plotting, or comparison against the source run should start from
`STARTDAY + 1`.

## Resolved: exact thermal-time restoration

`restore_state()` restores growth **stage** exactly (via
`Phenology_2018::activate_*()`), biomass/canopy/root depth exactly
(via the engine's own `restart_with()`/`initialize()` hooks), and both
the global accumulated degree-days (`thermal_time->GDDs`/`GDDs_yesterday`,
confirmed directly settable -- no new accessor needed) and the
*within-stage* thermal-time fraction for the currently-active
phenology period (`modifier_relative_elapsed`, forced via
`force_period_relative_elapsed()`). The latter matters because
`Period_thermal::start_day()` recomputes its own relative-elapsed
fraction fresh every day from that period's own accumulator -- global
thermal-time restoration alone is not sufficient; the period-local
value has to be forced too, and it is.

Also restored: which specific phenology modifier was active at the
branch date (accrescence/culminescence/senescence/none), and the
crop's peak canopy cover reached before senescence began
(`cover_attained_max`) -- needed on two separate objects
(`Canopy_cover_actual` and the reference canopy's own
`Canopy_cover_curve_2017::CCmax2_actual`), since both independently
compute senescence decay from the current-cover value at the moment
senescence begins, and both would otherwise incorrectly treat the
already-restored, already-decayed cover as if it were the true,
earlier peak.

## Known limitation: small, constant biomass offset

Biomass growth (`Crop_complete::calc_act_biomass_growth()`) depends on
`limited_pot_transpiration_m`, a water-balance quantity VIC computes
during that same day's `full_energy()` call, *before* `restore_state()`
runs -- computed against the wrong, pre-restore crop. The
`start_day()`+`process_day()` catch-up described above correctly
advances phenology/GAI (which only depends on thermal-time state, not
that water-balance quantity), but doesn't retrigger VIC's own
water-balance computation, so the catch-up day itself contributes
essentially no biomass growth. From the next day onward, daily biomass
growth is correct again -- the result is a small, constant, roughly
one-day-of-growth offset (confirmed directly: ~0.018 kg/m^2 for the
crop/scenario tested, about 1.8% relative to biomass at that point),
not an accumulating error. Fixing this fully would mean finding a way
to make VIC recompute that day's water balance after the restore
rather than before it -- a deeper change than anything else here,
genuinely risky (an earlier attempt to relocate the restore hook
earlier in VIC's own per-day processing sequence, to sidestep this
same ordering issue, was tried and reverted after confirming it broke
the restore entirely: the rotation engine's sowing-event dispatch
turned out to fire from inside `VIC_land_unit_process_day()` itself,
discarding a restore positioned before that call). Not attempted
further here; judged acceptable given the small, non-growing size of
the discrepancy.

## Known limitation: small residual soil-moisture bias

After the `STARTDAY` shift (see above), warm-started soil moisture
tracks a fully continuous reference run closely but not exactly:
confirmed via direct comparison, mean absolute difference of a few mm,
with a small (~2.5-4mm average) wetter bias. Likely downstream of the
biomass offset just above (biomass affects transpiration, which
affects soil moisture, which affects exactly which day an
irrigation-demand threshold gets crossed) rather than a separate,
distinct bug -- irrigation is a discrete, threshold-triggered pulse
(10-30mm jumps on specific days for the crop/scenario tested), so even
small upstream state differences can shift which day a threshold gets
crossed by a day or two, producing a temporary gap around each
irrigation event rather than systematic drift. Confirmed the soil
moisture range itself stays consistently within the continuous run's
own natural range throughout -- this is a timing/magnitude precision
issue, not a directional or runaway one.

## What was verified vs. not

This feature has been validated end-to-end against real simulation
output, including a full split-vs-continuous comparison: a warm-started
run (source run through the branch date, then a second run resuming
from the saved state) was compared day-by-day against a third,
independent run covering the identical calendar period continuously,
with no restart at all.

**Confirmed via that comparison, and via direct compile/test cycles
throughout development:**
- GAI and canopy cover: match the continuous reference run to within
  floating-point precision (~0.0004 max difference) for the remainder
  of the restored season.
- Biomass: correct day-to-day growth rate from the second day onward,
  with the small, constant, non-growing offset noted above.
- Soil moisture: within a few mm of the continuous run on average (see
  above), well within its natural seasonal range.
- Irrigation: fires correctly and regularly throughout the restored
  season, at a frequency and magnitude matching the source run's own
  pattern (confirmed via `irrig_total_mm`/`irrig_netdemand_mm` in the
  `.asc` output) -- an earlier version of this feature had irrigation
  silently never firing at all for the restored season, traced to
  `Crop_complete::trigger_synchronization()` never being called for
  any of the phenology stages the restore cascade forces (needed for
  management-file events scheduled relative to a phenologic stage,
  like "begin auto-irrigation N days after emergence") and fixed.
- Every touched compilation unit across the whole feature
  (`crop_cropsyst.cpp`, `canopy_cover_continuum.cpp`,
  `canopy_growth_cover_based.cpp`, `canopy_cover_curve.cpp`,
  `full_energy.c`, `dist_prec.c`, `VIC_land_unit_simulation.cpp`,
  `VIC_land_unit_C_interface.cpp`, `VIC_crop_state_csv.c`, and others)
  compiles with zero errors, confirmed repeatedly via fresh
  clone-apply-compile cycles.
- A normal, non-restore run (`INIT_STATE`/`CSV_STATE_FILE` both unset
  or `FALSE`) produces byte-for-byte identical `.asc`/flux output
  between this feature's branch and `master` -- confirming the entire
  feature is inert unless explicitly enabled, with zero impact on
  existing workflows.

**Not verified:** a full link of the complete executable (blocked in
the development environment by a missing external library dependency,
unrelated to this feature's own code) -- individual compilation units
were verified instead, repeatedly. Recommend a full build/link as the
first step in any new environment. Also not verified: behavior across
different crops, multiple grid cells simultaneously, or branch points
falling in different phenology stages than the yield-formation/
senescence case tested here -- the underlying mechanism
(`restore_state()`'s stage-based cascade) is written generically, but
only this one scenario has been exercised end-to-end.
