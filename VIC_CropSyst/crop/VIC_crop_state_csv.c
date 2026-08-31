#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vicNl.h>
#include "VIC_crop_state_csv.h"
#include "agronomic/VIC_land_unit_C_interface.h"

/*********************************************************************
  crop/VIC_crop_state_csv.c
  See VIC_crop_state_csv.h for design notes and known limitations.

  2026 correction: the first version of this file read biomass,
  growth stage, water stress and days-in-growing-season straight off
  crop_data_struct (crop->biomass_current, crop->growth_stage, etc.),
  and read GAI/root depth/accumulated degree days via
  VIC_CropSyst_get(VC_...) using the codes in
  VIC_crop_variable_codes.h. Both of those turned out to be wrong for
  the V3 driver: crop_data_struct's growth-related fields are only
  ever populated by the legacy V2 driver (call_crop_model.c), and
  VIC_CropSyst_get() in the V3 build dispatches on a *different*,
  independently-numbered enum (VIC::CropSyst_Variables in
  agronomic/VIC_land_unit_simulation.h) than VIC_crop_variable_codes.h's
  Variable_code -- so those calls would have silently read nothing or
  the wrong value. This version instead uses the dedicated
  VIC_land_unit_get_*_for_state()-style accessors added to
  agronomic/VIC_land_unit_C_interface.h specifically for this purpose,
  which query the active crop object directly with no enum involved.
*********************************************************************/

#define VIC_CROP_STATE_CSV_MAX_LINE (4096)

/*_____________________________________________________________________*/
void write_crop_state_csv_header(FILE *fp)
{
  if (fp == NULL) return;
  if (ftell(fp) != 0) return; /* header already written */

  fprintf(fp,
          "date,cellnum,veg_index,crop_code,is_active,growth_stage,"
          "days_in_growing_season,biomass_current_kg_m2,"
          "root_biomass_kg_m2,GAI,root_depth_m,"
          "accum_thermal_time_deg_day,water_stress_index,"
          "active_phenology_modifier,modifier_relative_elapsed,"
          "CropSystHandle\n");
}
/*_____________________________________________________________________*/
void write_crop_state_csv(FILE               *fp,
                           int                 cellnum,
                           int                 veg_index,
                           crop_data_struct   *crop,
                           dmy_struct         *current_date)
{
  int    is_active   = (crop != NULL && crop->CropSystHandle != 0);
  int    crop_code   = 0;
  int    growth_stage = 0;
  double biomass_current_kg_m2       = 0.0;
  double root_biomass_kg_m2          = 0.0;
  double GAI                         = 0.0;
  double root_depth_m                = 0.0;
  double accum_thermal_time_deg_day  = 0.0;
  double water_stress_index          = 0.0;
  int    active_phenology_modifier   = 0;
  double modifier_relative_elapsed   = 0.0;

  if (fp == NULL) return;
  write_crop_state_csv_header(fp);

  if (is_active) {
    crop_code = crop->code;
    /* Activate this specific crop object before querying it, since
       every VIC_land_unit_get_*_for_state() accessor below operates
       on whichever crop is currently "active" (a single global
       pointer -- see agronomic/VIC_land_unit_C_interface.cpp).
       Without this, looping over multiple veg tiles' crops (as
       dist_prec.c does) would silently read the wrong crop's values
       for every tile except whichever was processed last that day. */
    VIC_land_unit_activate(crop->CropSystHandle);
    /* Note: this call leaves crop->CropSystHandle activated as a side
       effect. That is safe here because this function only runs once
       per cell on the configured STATEDAY, after that cell's daily
       processing (VIC_land_unit_process_day()) has already completed
       for the timestep -- and process_day() explicitly (re)activates
       the correct land unit before doing any work on it, so no other
       code path relies on a particular crop remaining "active"
       between calls. */
    growth_stage                = VIC_land_unit_get_growth_stage_code();
    biomass_current_kg_m2       = VIC_land_unit_get_canopy_biomass_kg_m2();
    root_biomass_kg_m2          = VIC_land_unit_get_root_biomass_kg_m2();
    GAI                         = VIC_land_unit_get_GAI();
    root_depth_m                = VIC_land_unit_get_root_depth_m();
    accum_thermal_time_deg_day  = VIC_land_unit_get_accum_degree_days();
    water_stress_index          = VIC_land_unit_get_water_stress_index();
    /* 2026: which phenology "modifier" period (accrescence/
       culminescence/senescence) is active today, and how far through
       it -- see VIC_crop_state_csv.h's struct comment and
       Crop_complete::restore_state()'s documentation
       (crop_cropsyst.h/.cpp) for the full rationale. */
    active_phenology_modifier   = VIC_land_unit_get_active_phenology_modifier();
    modifier_relative_elapsed   = VIC_land_unit_get_modifier_relative_elapsed();
  }

  fprintf(fp, "%04d-%02d-%02d,%d,%d,%d,%d,%d,%d,%f,%f,%f,%f,%f,%f,%d,%f,%lu\n",
          current_date->year, current_date->month, current_date->day,
          cellnum, veg_index, crop_code, is_active,
          growth_stage,
          0, /* days_in_growing_season: not currently sourced from a
                confirmed V3 accessor; crop_data_struct::days_in_Gseason
                is only populated by the legacy V2 driver. Left at 0
                (informational field only, not used by restore_state()). */
          biomass_current_kg_m2,
          root_biomass_kg_m2, GAI, root_depth_m,
          accum_thermal_time_deg_day,
          water_stress_index,
          active_phenology_modifier,
          modifier_relative_elapsed,
          is_active ? crop->CropSystHandle : 0UL);
  fflush(fp);
}
/*_____________________________________________________________________*/
int read_crop_state_csv(char *filename, crop_state_record **out_records)
{
  FILE  *fp;
  char   line[VIC_CROP_STATE_CSV_MAX_LINE];
  char   dummy_date[32];
  int    capacity = 64;
  int    count = 0;
  crop_state_record *records;

  *out_records = NULL;

  fp = fopen(filename, "r");
  if (fp == NULL) return -1;

  if (fgets(line, VIC_CROP_STATE_CSV_MAX_LINE, fp) == NULL) {
    fclose(fp);
    return 0; /* empty file: no crop state saved (e.g. no crop model
                 enabled for this run) -- not an error. */
  }

  records = (crop_state_record *)malloc(capacity * sizeof(crop_state_record));
  if (records == NULL) { fclose(fp); return -1; }

  while (fgets(line, VIC_CROP_STATE_CSV_MAX_LINE, fp) != NULL) {
    crop_state_record rec;
    unsigned long handle;
    int n = sscanf(line,
                    "%31[^,],%d,%d,%d,%d,%d,%d,%lf,%lf,%lf,%lf,%lf,%lf,%d,%lf,%lu",
                    dummy_date, &rec.cellnum, &rec.veg_index,
                    &rec.crop_code, &rec.is_active, &rec.growth_stage,
                    &rec.days_in_growing_season,
                    &rec.biomass_current_kg_m2, &rec.root_biomass_kg_m2,
                    &rec.GAI, &rec.root_depth_m,
                    &rec.accum_thermal_time_deg_day,
                    &rec.water_stress_index,
                    &rec.active_phenology_modifier,
                    &rec.modifier_relative_elapsed,
                    &handle);
    if (n != 16) continue; /* malformed / blank line; skip */
    rec.CropSystHandle = handle;

    if (count == capacity) {
      capacity *= 2;
      records = (crop_state_record *)realloc(records,
                    capacity * sizeof(crop_state_record));
      if (records == NULL) { fclose(fp); return -1; }
    }
    records[count++] = rec;
  }

  fclose(fp);
  *out_records = records;
  return count;
}
/*_____________________________________________________________________*/
const crop_state_record *find_crop_state(const crop_state_record *table,
                                          int n_records,
                                          int cellnum,
                                          int veg_index)
{
  int i;
  if (table == NULL) return NULL;
  for (i = 0; i < n_records; i++) {
    if (table[i].cellnum == cellnum && table[i].veg_index == veg_index) {
      if (!table[i].is_active) return NULL; /* nothing to restore */
      return &table[i];
    }
  }
  return NULL;
}
/*_____________________________________________________________________*/
int apply_crop_state(const crop_state_record *saved)
{
  if (saved == NULL || !saved->is_active) return FALSE;

  /* biomass_current_kg_m2 is already in kg/m2 (see write_crop_state_csv()),
     matching the units used by CropSyst's own Canopy_leaf_growth::
     restart_with(). active_phenology_modifier/modifier_relative_elapsed
     -- see VIC_crop_state_csv.h's struct comment and
     Crop_complete::restore_state()'s documentation (crop_cropsyst.h/.cpp)
     for why these matter: they let the crop's canopy-growth-curve state
     resume from the correct remaining degree-day budget within its
     actual current phenological period, instead of assuming every
     earlier period was already fully complete. */
  return VIC_land_unit_restore_crop_state(
            saved->biomass_current_kg_m2,
            saved->GAI,
            saved->root_depth_m,
            saved->growth_stage,
            saved->accum_thermal_time_deg_day,
            saved->active_phenology_modifier,
            saved->modifier_relative_elapsed);
}
