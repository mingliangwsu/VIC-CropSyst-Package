#include <stdio.h>
#include <stdlib.h>
#include <vicNl.h>
#include <math.h>

static char vcid[] = "$Id: dist_prec.c,v 5.10.2.6 2012/02/05 00:15:44 vicadmin Exp $";
#if VIC_CROPSYST_VERSION>=3
#include "agronomic/VIC_land_unit_C_interface.h"
#include "agronomic/VIC_land_unit_simulation.h"
//150930LML int init_grid_cell_crop_fluxes(Grid_cell_crop_output &grid_cell_crop_output);    //150929LML
#endif

int  dist_prec(atmos_data_struct   *atmos,
               dist_prcp_struct    *prcp,
               soil_con_struct     *soil_con,
               veg_con_struct      *veg_con,
               lake_con_struct     *lake_con,
               dmy_struct          *dmy,
               global_param_struct *global_param,
               filep_struct        *filep,
               out_data_file_struct *out_data_files,
               out_data_struct     *out_data,
               save_data_struct    *save_data,
               int                  rec,
               int                  cellnum,
               char                 NEWCELL,
               char                 LASTREC,
               char                *init_still_storm,
               int                 *init_dry_time
#ifdef VIC_CROPSYST_VERSION
               #if VIC_CROPSYST_VERSION==2
               ,crop_data_struct *crops
               #endif
               #ifndef FULL_IRRIGATION
               //,irrigation_pattern_struct *irrig_patt //keyvan 130604
               ,std::map<int,std::map<int,irrigation_pattern_struct>> &irrig_patt
               #endif
               ,int spinup_run                                                   //160922LML = 0                                               //150819LML
#endif
           ) {
/**********************************************************************
  dist_prec        Keith Cherkauer        October 9, 1997

  This subroutine calls the solution routines for a single grid cell
  for one time step.  It also controls the distribution of precipitation
  and reassembles grid cell data for output.

  The fractional coverage of precipitation over an area or grid cell,
  mu, is estimated using the equation from Fan et. al. (1996).  The
  coefficient, 0.6, was selected for the Arkansas - Red River Basin and
  was found using precipitation records on a 100km x 100km area.  It
  may not be applicable to all regions, please check the reference

  References:

  Modifications:
  11-30-98 Added counter to assure that a storm has been stopped
           for at least one day, before allowing the model to
       average soil moisture when a new precipitation event
       arrives.                                             KAC
  03-05-01 Fixed error in which distributed precipitation accounting
           variables (dry_time, still_storm, any_snow) were used
           within the vegetation loop, but did not store separate
           values for each vegetation type.                     KAC
  03-12-03 Modifed to add AboveTreeLine to soil_con_struct so that
           the model can make use of the computed treeline.     KAC
  03-27-03 Modified calculation of dry_time.  Originally the check
           to see if a new storm was warranted checked if dry_time
           was greater than 24/dt.  However, dry_time is incremented
           by dt, so it was checking hours against time steps.  The
           division by dt has been removed, so a new storm starts if
           the cell has been drying for a full 24 hours.     RS & KAC
  04-10-03 Modified to store still_storm and dry_time in the model
           statefile, so that full conditions will be preserved.  KAC
  01-Nov-04 Added support for state files containing SPATIAL_FROST and
        LAKE_MODEL state variables.                    TJB
  02-Feb-05 Modified to save state file at the end of the final timestep
        of the date indicated by STATEYEAR, STATEMONTH, and STATEDAY
        in the global parameter file.                TJB
  2005-Mar-24 Modified parameter list of put_data() to accomodate support
          for ALMA variables.                    TJB
  2006-Sep-23 Implemented flexible output configuration; uses new out_data,
          out_data_files, and save_data structures.            TJB
  2006-Oct-16 Merged infiles and outfiles structs into filep_struct.    TJB
  2006-Nov-07 Removed LAKE_MODEL option.                TJB
  2007-Apr-04 Modified to handle grid cell errors by returning to the
              main subroutine, rather than ending the simulation.    GCT/KAC
  2008-Oct-23 Modified call to put_data() to store ErrorFlag.        TJB
  2009-Mar-03 Modified routine to store put_data() error in ErrorFlag2 and
          return a single ERROR value if an error occurs.        KAC via TJB
  2009-Jun-19 Added T flag to indicate whether TFALLBACK occurred.    TJB
  2009-Sep-28 Added logic for initial (pre-simulation) call to put_data.TJB
  2012-Jan-16 Removed LINK_DEBUG code                    BN
  2012-Nov-13 passed crop structure to full_energy.c and put_data.c
                structures for coupling                             KM
**********************************************************************/

  extern option_struct   options;
  extern veg_lib_struct *veg_lib;
  static char still_storm[MAX_VEG];
  static int  dry_time[MAX_VEG];
  char    any_snow[MAX_VEG];
  int     month;
  int     ErrorFlag, ErrorFlag2;
  /**************************************************
    If rec < 0, initialize the storage terms for water and energy balances
  **************************************************/
  if (rec < 0) {
    ErrorFlag2 = put_data(prcp, atmos, soil_con, veg_con,
              lake_con, out_data_files, out_data, save_data,
              &dmy[0],
              #if VIC_CROPSYST_VERSION==2
              crops,
              #endif
              rec );
    if ( ErrorFlag2 == ERROR ) ErrorFlag = ERROR;
    return (0);
  }
  //**************************************************
  //  If rec >= 0, proceed with simulation
  //**************************************************
  // check if state file has been used to initialize storm tracking
  //if ( init_dry_time >= 0 ) {
  if ( init_dry_time ) {
    // initialize storm tracking
    for (int veg = 0; veg <= veg_con[0].vegetat_type_num; veg++ ) {
      dry_time[veg]    = init_dry_time[veg];
      still_storm[veg] = init_still_storm[veg];
    }
  }
  if(options.DIST_PRCP) {
    //*******************************************
    //  Controls Distributed Precipitation Model
    //*******************************************
    double new_mu = 1.0 - exp(-options.PREC_EXPT * atmos->prec[NR]);
    for (int veg = 0; veg <= veg_con[0].vegetat_type_num; veg++ ) {
      #if VIC_CROPSYST_VERSION>=3
      int is_crop = iscrop(veg_con[veg].VCS.veg_class_code);
      #endif
      any_snow[veg] = FALSE;
      for (int i = 0; i < options.SNOW_BAND; i++ ) {
        //* Check for snow on ground or falling
        if ((prcp->snow[veg][i].swq > 0) || (prcp->snow[veg][i].snow_canopy > 0.))
          any_snow[veg] = TRUE;
      } //i
      if ( any_snow[veg] || atmos->snowflag[NR] ) {
        //* If snow present, mu must be set to 1.
        new_mu = 1.;
        if ( rec == 0 ) {
          //* Set model variables if first time step
          prcp->mu[veg] = new_mu;
          if ( atmos->prec[NR] > 0 ) still_storm[veg] = TRUE;
          else                       still_storm[veg] = FALSE;
          dry_time[veg] = 0;
        }
        any_snow[veg] = TRUE;
      } else {
        if ( rec == 0 ) {
          if ( atmos->prec[NR] == 0 ) {
            //* If first time step has no rain, than set mu to 1.
            prcp->mu[veg]    = 1.;
            new_mu           = 1.;
            still_storm[veg] = TRUE;
            dry_time[veg]    = 24;
          } else {
            /* If first time step has rain, then set mu based on intensity */
            prcp->mu[veg]    = new_mu;
            still_storm[veg] = TRUE;
            dry_time[veg]    = 0;
          }
        } else if((atmos->prec[NR] == 0) && (dry_time[veg] >= 24.)) {
          //* Check if storm has ended
          new_mu           = prcp->mu[veg];
          still_storm[veg] = FALSE;
          dry_time[veg]    = 0;
        } else if ( atmos->prec[NR] == 0 ) {
          /* May be pause in storm, keep track of pause length */
          new_mu         = prcp->mu[veg];
          dry_time[veg] += global_param->dt;
        }
      }
      #if VIC_CROPSYST_VERSION>=3
      if (is_crop) prcp->mu[veg] = 1.0;                                          //LML 150429
      #endif
      if ( (!still_storm[veg]) && (atmos->prec[NR] > STORM_THRES
                                   || any_snow[veg])) {
        //** Average soil moisture before a new storm **
        ErrorFlag = initialize_new_storm(prcp->cell,prcp->veg_var,
                                         veg,veg_con[0].vegetat_type_num,rec,
                                         prcp->mu[veg],new_mu);
        if ( ErrorFlag == ERROR ) return ( ERROR );
        still_storm[veg] = TRUE;
        prcp->mu[veg]    = new_mu;
      } else if ((new_mu != prcp->mu[veg]) && still_storm[veg]) {
        //** Redistribute soil moisture during the storm if mu changes **
        int month = 0;
        if ( dmy[rec].day == 1 && dmy[rec].hour == 0 ) {
          month = dmy[rec].month - 2;
          if ( month < 0 ) month = 11;
        } else {
          month = dmy[rec].month - 1;
        }
        double Wdmax = 0;
        if (veg < veg_con[0].vegetat_type_num) {
          Wdmax = veg_lib[veg_con[veg].veg_class].Wdmax[month];
          #if VIC_CROPSYST_VERSION>=3
          if (is_crop) {
            if (rec > 0) Wdmax = VIC_land_unit_get(VIC::WDEWMAX);
            else         Wdmax = 0;
          }
          #endif
        } else {
          Wdmax = 0;
        }
        redistribute_during_storm(prcp->cell, prcp->veg_var, veg,
            veg_con[0].vegetat_type_num, rec, Wdmax,
            prcp->mu[veg], new_mu, soil_con->max_moist
            #if VIC_CROPSYST_VERSION>=3
            ,is_crop
            #endif
            );
        prcp->mu[veg] = new_mu;
      }
    }  //veg
    /** Solve model time step **/
    ErrorFlag = full_energy(NEWCELL, cellnum, rec, atmos, prcp, &dmy[rec]/*150609LML dmy*/,global_param,
        lake_con, soil_con, veg_con
        #ifdef VIC_CROPSYST_VERSION
        #if VIC_CROPSYST_VERSION==2
        ,crops
        #endif
        #ifndef FULL_IRRIGATION
        ,irrig_patt //keyvan 130604
        #endif
        #endif //VIC_CROPSYST_VERSION
        );

  } else {
    //**************************************************
    //  Controls Grid Cell Averaged Precipitation Model
    //**************************************************
    ErrorFlag = full_energy(NEWCELL, cellnum, rec, atmos, prcp, &dmy[rec]/*150609LML dmy*/,global_param,
        lake_con, soil_con, veg_con
        #ifdef VIC_CROPSYST_VERSION
        #if VIC_CROPSYST_VERSION==2
        ,crops
        #endif
        #ifndef FULL_IRRIGATION
        ,irrig_patt //keyvan 130604
        #endif
        #endif //VIC_CROPSYST_VERSION
        );
  }
  //**************************************************
  //  Write cell average values for current time step
  //**************************************************
  #if (VIC_CROPSYST_VERSION>=3)
  if (!spinup_run) {
  #endif
  ErrorFlag2 = put_data(prcp, atmos, soil_con, veg_con,
      lake_con, out_data_files, out_data, save_data,
      &dmy[rec],
      #if VIC_CROPSYST_VERSION==2
      crops, //passing crop structure to the put_data.c code
      #endif
      rec);
  if ( ErrorFlag2 == ERROR ) ErrorFlag = ERROR;
  #if (VIC_CROPSYST_VERSION>=3)
  }
  #endif
  //************************************
  //  Save model state at assigned date
  //  (after the final time step of the assigned date)
  //************************************
  if ( filep->statefile != NULL
       &&  ( dmy[rec].year == global_param->stateyear
             && dmy[rec].month == global_param->statemonth
             && dmy[rec].day == global_param->stateday
             && ( rec+1 == global_param->nrecs
                  || dmy[rec+1].day != global_param->stateday ) ) )
    write_model_state(prcp, veg_con[0].vegetat_type_num,
              soil_con->gridcel, filep, soil_con,
              still_storm, dry_time, *lake_con);
  return ( ErrorFlag );
}
