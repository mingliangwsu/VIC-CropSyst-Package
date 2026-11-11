#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vicNl.h>
#include "vic_state_csv.h"

/*********************************************************************
  vic_state_csv.c

  See vic_state_csv.h for design notes.  The row layout mirrors
  write_model_state.c / read_initial_model_state.c exactly (same
  fields, same semantics) but is written as a "wide" CSV row per
  (cellnum, veg tile, snow band) instead of a packed binary/ASCII
  stream.  This makes it possible to:
    - inspect/validate a state snapshot with any spreadsheet or
      pandas/R,
    - diff two scenarios' states at the branch day to confirm they
      are identical,
    - join it (by cellnum + date) with the companion crop state CSV
      produced by crop/VIC_crop_state_csv.c.
*********************************************************************/

#define VIC_STATE_CSV_MAX_LINE (65536)

/*_____________________________________________________________________*/
void write_state_csv_header(FILE *fp)
{
  extern option_struct options;
  int lidx, nidx, dist;

  if (fp == NULL) return;
  if (ftell(fp) != 0) return; /* header already written */

  fprintf(fp, "date,cellnum,Nveg,Nbands,Nlayer,Nnode,NDIST,veg,band,"
              "mu,still_storm,dry_time");
  for (dist = 0; dist < options.NDIST; dist++)
    for (lidx = 0; lidx < options.Nlayer; lidx++)
      fprintf(fp, ",moist_d%d_l%d", dist, lidx);
  for (dist = 0; dist < options.NDIST; dist++)
    for (lidx = 0; lidx < options.Nlayer; lidx++)
      fprintf(fp, ",ice_d%d_l%d", dist, lidx);
  for (dist = 0; dist < options.NDIST; dist++)
    fprintf(fp, ",Wdew_d%d", dist);
  fprintf(fp, ",snow_last_snow,snow_melting,snow_coverage,snow_swq,"
              "snow_surf_temp,snow_surf_water,snow_pack_temp,"
              "snow_pack_water,snow_density,snow_coldcontent,"
              "snow_snow_canopy");
  for (nidx = 0; nidx < options.Nnode; nidx++)
    fprintf(fp, ",soil_T_n%d", nidx);
  fprintf(fp, "\n");
}
/*_____________________________________________________________________*/
void write_model_state_csv(FILE                *fp,
                            dist_prcp_struct    *prcp,
                            int                  Nveg,
                            int                  cellnum,
                            soil_con_struct     *soil_con,
                            char                *STILL_STORM,
                            int                 *DRY_TIME,
                            dmy_struct          *current_date)
{
  extern option_struct options;
  int veg, band, lidx, nidx, dist;
  int Nbands = options.SNOW_BAND;

  cell_data_struct  ***cell    = prcp->cell;
  veg_var_struct    ***veg_var = prcp->veg_var;
  snow_data_struct   **snow    = prcp->snow;
  energy_bal_struct  **energy  = prcp->energy;

  if (fp == NULL) return;

  write_state_csv_header(fp);

  for (veg = 0; veg <= Nveg; veg++) {
    for (band = 0; band < Nbands; band++) {

      fprintf(fp, "%04d-%02d-%02d,%d,%d,%d,%d,%d,%d,%d,%d,%f,%d,%d",
              current_date->year, current_date->month, current_date->day,
              cellnum, Nveg, Nbands, options.Nlayer, options.Nnode,
              options.NDIST, veg, band,
              prcp->mu[veg], (int)STILL_STORM[veg], DRY_TIME[veg]);

      for (dist = 0; dist < options.NDIST; dist++)
        for (lidx = 0; lidx < options.Nlayer; lidx++)
          fprintf(fp, ",%f", cell[dist][veg][band].layer[lidx].moist);

      for (dist = 0; dist < options.NDIST; dist++)
        for (lidx = 0; lidx < options.Nlayer; lidx++)
#if SPATIAL_FROST
          /* Store the area-weighted average ice content across frost
             sub-areas; per-sub-area detail is not required to warm
             start a scenario run and would otherwise blow up the
             column count. Full-fidelity restarts that need the
             binary format can still use BINARY_STATE_FILE. */
        {
          double avg_ice = 0.0;
          int    frost_area;
          for (frost_area = 0; frost_area < FROST_SUBAREAS; frost_area++)
            avg_ice += cell[dist][veg][band].layer[lidx].ice[frost_area];
          avg_ice /= FROST_SUBAREAS;
          fprintf(fp, ",%f", avg_ice);
        }
#else
          fprintf(fp, ",%f", cell[dist][veg][band].layer[lidx].ice);
#endif

      for (dist = 0; dist < options.NDIST; dist++) {
        double Wdew = (veg < Nveg) ? veg_var[dist][veg][band].Wdew : 0.0;
        fprintf(fp, ",%f", Wdew);
      }

      fprintf(fp, ",%d,%d,%f,%f,%f,%f,%f,%f,%f,%f,%f",
              snow[veg][band].last_snow, (int)snow[veg][band].MELTING,
              snow[veg][band].coverage, snow[veg][band].swq,
              snow[veg][band].surf_temp, snow[veg][band].surf_water,
              snow[veg][band].pack_temp, snow[veg][band].pack_water,
              snow[veg][band].density, snow[veg][band].coldcontent,
              snow[veg][band].snow_canopy);

      for (nidx = 0; nidx < options.Nnode; nidx++)
        fprintf(fp, ",%f", energy[veg][band].T[nidx]);

      fprintf(fp, "\n");
    }
  }
  fflush(fp);
}
/*_____________________________________________________________________*/
int read_initial_model_state_csv(char                *filename,
                                  dist_prcp_struct    *prcp,
                                  int                  Nveg,
                                  int                  Nbands,
                                  int                  cellnum,
                                  soil_con_struct     *soil_con,
                                  char                *init_STILL_STORM,
                                  int                 *init_DRY_TIME)
{
  extern option_struct options;
  FILE  *fp;
  char   line[VIC_STATE_CSV_MAX_LINE];
  char   ErrStr[MAXSTRING];
  int    found = FALSE;
  int    csv_cellnum, csv_Nveg, csv_Nbands, csv_Nlayer, csv_Nnode, csv_NDIST;
  int    veg, band, lidx, nidx, dist;
  double mu_val;
  int    still_storm_val, dry_time_val;
  char   dummy_date[32];

  cell_data_struct  ***cell    = prcp->cell;
  veg_var_struct    ***veg_var = prcp->veg_var;
  snow_data_struct   **snow    = prcp->snow;
  energy_bal_struct  **energy  = prcp->energy;

  fp = fopen(filename, "r");
  if (fp == NULL) {
    sprintf(ErrStr, "read_initial_model_state_csv: unable to open state "
                     "file \"%s\".", filename);
    nrerror(ErrStr);
  }

  /* skip header line */
  if (fgets(line, VIC_STATE_CSV_MAX_LINE, fp) == NULL) {
    fclose(fp);
    return FALSE; /* empty file */
  }

  while (fgets(line, VIC_STATE_CSV_MAX_LINE, fp) != NULL) {
    char *p = line;
    int   n;

    n = sscanf(p, "%31[^,],%d,%d,%d,%d,%d,%d,%d,%d,%lf,%d,%d",
               dummy_date, &csv_cellnum, &csv_Nveg, &csv_Nbands,
               &csv_Nlayer, &csv_Nnode, &csv_NDIST, &veg, &band,
               &mu_val, &still_storm_val, &dry_time_val);
    if (n != 12) continue; /* malformed / blank line; skip */

    if (csv_cellnum != cellnum) continue;

    if (csv_Nveg != Nveg) {
      sprintf(ErrStr, "The number of vegetation types in cell %d (%d) in "
                       "the CSV state file does not equal that defined in "
                       "the vegetation parameter file (%d).",
                       cellnum, csv_Nveg, Nveg);
      nrerror(ErrStr);
    }
    if (csv_Nbands != Nbands) {
      sprintf(ErrStr, "The number of snow bands in cell %d (%d) in the CSV "
                       "state file does not equal that defined in the snow "
                       "band file (%d).", cellnum, csv_Nbands, Nbands);
      nrerror(ErrStr);
    }
    if (csv_Nlayer != options.Nlayer || csv_Nnode != options.Nnode ||
        csv_NDIST  != options.NDIST) {
      sprintf(ErrStr, "The Nlayer/Nnode/NDIST recorded in the CSV state "
                       "file for cell %d (%d/%d/%d) do not match the "
                       "current run's configuration (%d/%d/%d).",
                       cellnum, csv_Nlayer, csv_Nnode, csv_NDIST,
                       options.Nlayer, options.Nnode, options.NDIST);
      nrerror(ErrStr);
    }
    if (veg < 0 || veg > Nveg || band < 0 || band >= Nbands) continue;

    prcp->mu[veg]         = mu_val;
    init_STILL_STORM[veg] = (char)still_storm_val;
    init_DRY_TIME[veg]    = dry_time_val;

    /* advance p past the 12 already-parsed comma-separated fields */
    for (n = 0; n < 12 && *p; p++) if (*p == ',') n++;

    for (dist = 0; dist < options.NDIST; dist++)
      for (lidx = 0; lidx < options.Nlayer; lidx++) {
        sscanf(p, "%lf", &cell[dist][veg][band].layer[lidx].moist);
        while (*p && *p != ',') p++;
        if (*p == ',') p++;
      }

    for (dist = 0; dist < options.NDIST; dist++)
      for (lidx = 0; lidx < options.Nlayer; lidx++) {
        double ice_val;
        sscanf(p, "%lf", &ice_val);
        while (*p && *p != ',') p++;
        if (*p == ',') p++;
#if SPATIAL_FROST
        {
          int frost_area;
          for (frost_area = 0; frost_area < FROST_SUBAREAS; frost_area++)
            cell[dist][veg][band].layer[lidx].ice[frost_area] = ice_val;
        }
#else
        cell[dist][veg][band].layer[lidx].ice = ice_val;
#endif
      }

    for (dist = 0; dist < options.NDIST; dist++) {
      double dew_val;
      sscanf(p, "%lf", &dew_val);
      while (*p && *p != ',') p++;
      if (*p == ',') p++;
      if (veg < Nveg) veg_var[dist][veg][band].Wdew = dew_val;
    }

    {
      int    last_snow_val, melting_val;
      double coverage_val, swq_val, surf_temp_val, surf_water_val,
             pack_temp_val, pack_water_val, density_val,
             coldcontent_val, snow_canopy_val;
      sscanf(p, "%d,%d,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf",
             &last_snow_val, &melting_val, &coverage_val, &swq_val,
             &surf_temp_val, &surf_water_val, &pack_temp_val,
             &pack_water_val, &density_val, &coldcontent_val,
             &snow_canopy_val);
      for (n = 0; n < 11 && *p; p++) if (*p == ',') n++;

      snow[veg][band].last_snow    = last_snow_val;
      snow[veg][band].MELTING      = (char)melting_val;
      snow[veg][band].coverage     = coverage_val;
      snow[veg][band].swq          = swq_val;
      snow[veg][band].surf_temp    = surf_temp_val;
      snow[veg][band].surf_water   = surf_water_val;
      snow[veg][band].pack_temp    = pack_temp_val;
      snow[veg][band].pack_water   = pack_water_val;
      snow[veg][band].density      = density_val;
      snow[veg][band].coldcontent  = coldcontent_val;
      snow[veg][band].snow_canopy  = snow_canopy_val;
    }

    for (nidx = 0; nidx < options.Nnode; nidx++) {
      sscanf(p, "%lf", &energy[veg][band].T[nidx]);
      while (*p && *p != ',') p++;
      if (*p == ',') p++;
    }

    found = TRUE;
  }

  fclose(fp);
  return found;
}
