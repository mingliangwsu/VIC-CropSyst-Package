#ifndef VIC_STATE_CSV_H
#define VIC_STATE_CSV_H
/*********************************************************************
  vic_state_csv.h

  Author: Claude (Anthropic), added 2026 to support scenario-branching
          "initial state" runs for VIC-CropSyst.

  This module adds a tabular (CSV, and optionally Apache Parquet)
  alternative to VIC's existing binary/ASCII model-state mechanism
  (see write_model_state.c / read_initial_model_state.c).

  WHY
  ---
  The existing INIT_STATE / SAVE_STATE mechanism already lets VIC save
  its full hydrologic state (soil moisture, soil ice, soil thermal
  node temperatures, snow pack, dew storage, distributed-precipitation
  bookkeeping) on a specific date and reload it to warm-start a later
  run.  That mechanism is preserved unchanged (BINARY_STATE_FILE /
  ASCII).  This module adds a companion CSV format that is:
    * human readable / diffable / analyzable with pandas, R, Excel...
    * easy to combine with the new crop/vegetation state file
      (see crop/VIC_crop_state_csv.h) so that a single "initial state"
      snapshot describes both the soil physical state AND the crop
      state at the same simulated day, which is what is needed to run
      multiple weather/management SCENARIOS forward from one shared
      starting point.

  The CSV row layout is "wide": one row per (grid cell, veg tile,
  snow band), with one column per soil layer / thermal node.  Column
  counts are therefore run-specific (they depend on options.Nlayer,
  options.Nnode, options.SNOW_BAND); the header is written once, from
  the values in effect for the run, and is echoed back when reading a
  CSV state file so that Nlayer/Nnode/Nbands mismatches are caught
  early instead of silently corrupting data.

  This module does not implement the distinction between wet/dry
  fractions used internally by VIC's optional distributed
  precipitation model; like the legacy state writer, soil moisture is
  written out per distribution index (dist = 0 wet / 1 dry when
  NDIST==2) as separate columns, and mu is written once per veg tile
  so that a restart can rebuild both fractions exactly.

  USAGE
  -----
  Enable by adding to the global parameter file:
      CSV_STATE_FILE   TRUE
  in addition to the existing INIT_STATE / SAVE_STATE / STATEYEAR /
  STATEMONTH / STATEDAY / STATENAME keywords.  When CSV_STATE_FILE is
  TRUE, VIC additionally reads/writes a CSV file at the same
  path as the existing statefile / init_state file name, with a
  "_soil.csv" suffix appended, e.g.:
      output/state/state_2005_soil.csv
  and, if a crop/land-cover model is enabled, a companion
      output/state/state_2005_crop.csv
  (see crop/VIC_crop_state_csv.h).

  PARQUET
  -------
  A Parquet backend is a natural addition (columnar, compressed,
  ideal for large multi-cell state files) but requires linking Apache
  Arrow/Parquet C++, which is not part of this project's existing
  build system.  The functions below are written against a small,
  self-contained tabular in-memory record type ("VIC_state_table")
  specifically so that a Parquet backend can be dropped in later by
  implementing write_state_table_parquet()/read_state_table_parquet()
  with the same signatures as the CSV versions -- see the
  VIC_ENABLE_PARQUET guard at the bottom of this file.
*********************************************************************/

#include <vicNl_def.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Writes (or appends) the CSV header row for the soil/hydrology state
   table.  Safe to call multiple times; only writes the header if the
   file is currently empty (ftell(fp) == 0). */
void write_state_csv_header(FILE *fp);

/* Writes one CSV block (one row per veg tile x snow band) describing
   the current hydrologic state of grid cell `cellnum` on `current_date`.
   Mirrors the data captured by write_model_state() (see
   write_model_state.c) but in tabular form.  Call at the same point
   in the daily loop where write_model_state() is called (see
   dist_prec.c), guarded by the same STATEYEAR/STATEMONTH/STATEDAY
   trigger. */
void write_model_state_csv(FILE                *fp,
                            dist_prcp_struct    *prcp,
                            int                  Nveg,
                            int                  cellnum,
                            soil_con_struct     *soil_con,
                            char                *STILL_STORM,
                            int                 *DRY_TIME,
                            dmy_struct          *current_date);

/* Scans a previously written CSV state file for the row block
   belonging to `cellnum` and, if found, initializes `prcp`'s cell,
   snow, energy and veg_var structures (soil moisture, soil ice,
   thermal node temperatures, snow pack, dew, mu/STILL_STORM/DRY_TIME)
   from it, exactly as read_initial_model_state() does for the binary
   format.  Returns TRUE (1) if the cell was found and state was
   applied, FALSE (0) if the cell was not present in the file (in
   which case the caller should fall back to
   initialize_model_state()'s normal cold-start initialization for
   that cell). */
int read_initial_model_state_csv(char                *filename,
                                  dist_prcp_struct    *prcp,
                                  int                  Nveg,
                                  int                  Nbands,
                                  int                  cellnum,
                                  soil_con_struct     *soil_con,
                                  char                *init_STILL_STORM,
                                  int                 *init_DRY_TIME);

#ifdef VIC_ENABLE_PARQUET
/* Optional Parquet backend.  Not built by default: requires linking
   Apache Arrow C++ (arrow/parquet).  See vic_state_parquet.cpp for a
   reference implementation stub with the same table model used by
   the CSV functions above. */
void write_model_state_parquet(const char *filename, /* ... same table ... */ void *table);
int  read_initial_model_state_parquet(const char *filename, void *table);
#endif

#ifdef __cplusplus
}
#endif

#endif /* VIC_STATE_CSV_H */
