#ifndef VIC_CROP_STATE_CSV_H
#define VIC_CROP_STATE_CSV_H
/*********************************************************************
  crop/VIC_crop_state_csv.h

  Author: Claude (Anthropic), added 2026 to support scenario-branching
          "initial state" runs for VIC-CropSyst (companion to
          ../vic_state_csv.h, which handles the soil/snow side).

  WHAT THIS CAPTURES
  ------------------
  One row per (grid cell, veg/land-unit tile) describing the
  prognostic crop state on a given day:
      crop code, whether a crop is currently active, growth stage,
      biomass (current + root), green area index (GAI/LAI), rooting
      depth, accumulated thermal time (degree-days), days in the
      growing season, water stress index, and the crop's
      CropSystHandle bookkeeping fields already tracked in
      crop_data_struct (see vicNl_def.h).

  These are exactly the fields already used for VIC-CropSyst's own
  per-day crop diagnostic output (see the "Accum_DD,...,Root_depth_mm"
  header written in agronomic/VIC_land_unit_C_interface.cpp), obtained
  through the same confirmed public accessors (get_accum_degree_days(),
  get_GAI(), get_recorded_root_depth_m(), ...), now also routed through
  the VIC_CropSyst_get()/VIC_land_unit_get() variable-code interface
  (see VIC_crop_variable_codes.h) so this module only needs the plain-C
  interface, not C++ engine internals directly.

  HOW RESTORE WORKS, AND ITS LIMITS
  ----------------------------------
  Reading a crop state CSV does not, by itself, create crop objects:
  in VIC-CropSyst V3 a crop becomes active when the rotation/event
  engine reaches its scheduled sowing date (see
  CropSyst::Land_unit_simulation / agronomic/VIC_land_unit_simulation.cpp).
  If a run starts (via INIT_STATE) on a day after that sowing date --
  which is exactly the warm-restart scenario this module exists for --
  the normal calendar-driven sowing event for a crop that should
  already be growing will never fire, because the days before the
  restart are never simulated.

  This module therefore provides the data (read_crop_state_csv()) and
  the restore call (apply_crop_state(), which wraps
  VIC_land_unit_restore_crop_state()); it is the responsibility of the
  land-unit/rotation integration point to (a) recognize that a crop
  should already be active on the run's start day, per the loaded
  state, (b) force it into existence for that date, and then
  (c) call apply_crop_state(). Step (a)/(b) requires either forcing the
  rotation engine's sowing event for that date (a CropSyst engine
  change, not attempted here) or configuring the run's rotation
  parameters so that the crop of interest is created starting on the
  restart date directly (a scenario/parameter-file level workaround
  that requires no code change and works today). See the project
  README for a full discussion.

  Once a crop is active, apply_crop_state() restores growth stage,
  biomass, GAI and rooting depth exactly (using CropSyst's own
  restart_with() / initialize() / activate_*() hooks -- see
  VIC_CropSyst_proper_crop::restore_state()). Exact within-stage
  degree-day accumulation is not currently restorable (no public
  setter exists in the CropSyst phenology engine for that); the value
  is still recorded in the CSV for inspection/validation.
*********************************************************************/

#include <vicNl_def.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  int    cellnum;
  int    veg_index;
  int    crop_code;
  int    is_active;                 /* 1 if a crop is currently growing here */
  int    growth_stage;              /* Normal_crop_event_sequence value */
  int    days_in_growing_season;
  double biomass_current_kg_m2;
  double root_biomass_kg_m2;
  double GAI;
  double root_depth_m;
  double accum_thermal_time_deg_day;
  double water_stress_index;
  /* 2026: which phenology "modifier" period (accrescence/culminescence/
     senescence -- these track independently of growth_stage above, and
     are what determine the crop's canopy-growth-curve behavior) was
     active on this day, and how far through it, as a 0..1 fraction of
     that period's own configured duration. 0=none, 1=accrescence,
     2=culminescence, 3=senescence. Restoring these lets a later run
     reproduce the correct remaining degree-day budget within the
     crop's current phenological period, instead of assuming every
     earlier period was already fully complete -- see
     Crop_complete::restore_state()'s own documentation
     (crop_cropsyst.h/.cpp) for the full rationale. */
  int    active_phenology_modifier;
  double modifier_relative_elapsed;
  unsigned long int CropSystHandle; /* 0 if not applicable/unknown */
} crop_state_record;

/* Writes (or appends) the CSV header row.  Safe to call multiple
   times; only writes if the file is currently empty. */
void write_crop_state_csv_header(FILE *fp);

/* Writes one row describing the current state of the crop growing (if
   any) in grid cell `cellnum`, veg tile `veg_index`, on `current_date`.
   `crop` may be NULL or crop->CropSystHandle == 0, in which case a row
   with is_active=0 is written (no crop currently growing there).
   Call at the same STATEYEAR/STATEMONTH/STATEDAY trigger used for
   write_model_state_csv() -- see dist_prec.c. */
void write_crop_state_csv(FILE               *fp,
                           int                 cellnum,
                           int                 veg_index,
                           crop_data_struct   *crop,
                           dmy_struct         *current_date);

/* Reads an entire crop state CSV file into a newly malloc'd array of
   crop_state_record.  Returns the number of records read (>= 0), or
   -1 on error.  Caller owns *out_records and must free() it. */
int read_crop_state_csv(char *filename, crop_state_record **out_records);

/* Linear lookup of the record for (cellnum, veg_index) within a table
   returned by read_crop_state_csv(). Returns NULL if not present or
   not active. */
const crop_state_record *find_crop_state(const crop_state_record *table,
                                          int n_records,
                                          int cellnum,
                                          int veg_index);

/* Applies a saved crop_state_record to the currently active crop
   object (see the header comment above for the prerequisite that a
   crop object already exists / has just been created for this land
   unit on the restart day).  Thin wrapper around
   VIC_land_unit_restore_crop_state().  Returns TRUE on success. */
int apply_crop_state(const crop_state_record *saved);

#ifdef __cplusplus
}
#endif

#endif /* VIC_CROP_STATE_CSV_H */
