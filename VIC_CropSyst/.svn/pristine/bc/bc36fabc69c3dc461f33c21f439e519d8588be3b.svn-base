#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <list>
#include <iomanip>
#include <vicNl.h>
//#include <global.h>
#include <malloc.h>
#ifdef VCS_V5
#include "vic_def.h"
#else
#include "vicNl_def.h"
#endif
#include "vic_run.h"
#include "vic_driver_shared_all.h"
#ifdef VIC_CROPSYST_VERSION
#include "VCS_Nl.h"
#include "corn/application/explaination/explainations.h"
#endif
#ifdef CROP_DAILY_OUTPUT_MEMFIRST
#include "VIC_crop_output.h"
#endif

parameters_struct   param;                                                       //170406LML
size_t              NR; /* array index for atmos struct that indicates
                           the model step avarage or sum */
size_t              NF; /* array index loop counter limit for atmos
                           struct that indicates the SNOW_STEP values */

#define PRINT_BOOLEAN_AS_STRING(bvalue) (bvalue == 1 ? "True" : "False")
/*170413LML moved to VCS_Nl.c
#ifdef VIC_CROPSYST_VERSION
#include <assert.h>
extern int get_cropping_system_numbers(const veg_con_struct *veg_con_array);          //150928LML
extern int set_output_for_crop_numbers(out_data_struct *out_data,const int numcrops); //150928LML
int copy_veg_lib_record(const veg_lib_struct &from_veg, veg_lib_struct &to_veg);      //151001LML
int make_veg_lib_for_crops(veg_con_struct *veg_con,veg_lib_struct *veg_lib);     //151001LML
#endif
*/
static char vcid[] = "$Id: vicNl.c,v 5.14.2.20 2012/02/05 00:15:44 vicadmin Exp $";
soil_con_struct   *soil_con_global = 0;   //km 130124
/*170413LML moved to VCS_Nl.c
#if (FULL_IRRIGATION==FALSE)
irrigation_pattern_struct *irrig_patt;
#endif
*/
/** Main Program **/
/*170413LML moved to VCS_Nl.c
//LML 140808
#if (VIC_CROPSYST_VERSION>=3)
#include "corn/OS/file_system_logical.h"
namespace CORN { namespace OS {
File_system &file_system() { return CORN::OS::file_system_logical; }
}}
#endif
*/
//extern int global_rec;

//#ifdef CROPSYST_ON2
//VIC_main
//#else
//#endif
#ifdef VIC_CROPSYST_VERSION
int run_CropSyst_crop = 0;                                                       //LML 150521
#endif

#ifdef CROP_DAILY_OUTPUT_MEMFIRST
std::stringstream crop_output_head;
bool created_head = false;
char out_file_name[MAXSTRING];
std::list<Crop_output_struct> crop_output_list;
#endif


int main (int argc, char *argv[])
/**********************************************************************
    vicNl.c        Dag Lohmann        January 1996

  This program controls file I/O and variable initialization as well as
  being the primary driver for the model.

  For details about variables, input files and subroutines check:
    http://ce.washington.edu/~hydro/Lettenmaier/Models/VIC/VIC_home.html

  UNITS: unless otherwise marked:
         all water balance components are in mm
     all energy balance components are in mks
     depths, and lengths are in m

  modifications:
  1997-98 Model was updated from simple 2 layer water balance to
          an extension of the full energy and water balance 3 layer
      model.                                                  KAC
  02-27-01 added controls for lake model                          KAC
  11-18-02 Updated storage of lake water for water balance
           calculations.                                          LCB
  03-12-03 Modifed to add AboveTreeLine to soil_con_struct so that
           the model can make use of the computed treeline.     KAC
  04-10-03 Modified to initialize storm parameters using the state
           file.                                                KAC
  04-10-03 Modified to start the model by skipping records until the
           state file date is found.  This replaces the previous method
           of modifying the global file start date, which can change
           the interpolation of atmospheric forcing data.        KAC
  04-15-03 Modified to store wet and dry fractions when intializing
           water balance storage.  This accounts for changes in model
           state initialization, which now stores wet and dry fractions
           rather than just averagedvalues.                      KAC
  29-Oct-03 Modified the version display banner to print the version
        string defined in global.h.                    TJB
  01-Nov-04 Updated arglist for make_dist_prcp(), as part of fix for
        QUICK_FLUX state file compatibility.            TJB
  02-Nov-04 Updated arglist for read_lakeparam(), as part of fix for
        lake fraction readjustment.                    TJB
  2005-Apr-13 OUTPUT_FORCE option now calls close_files().        TJB
  2006-Sep-23 Implemented flexible output configuration; uses the new
              out_data, out_data_files, and save_data structures.    TJB
  2006-Oct-16 Merged infiles and outfiles structs into filep_struct;
          This included merging builtnames into filenames.        TJB
  2006-Nov-07 Removed LAKE_MODEL option.                TJB
  2006-Nov-07 Changed statefile to init_state in call to
          check_state_file().                    TJB
  2007-Jan-15 Added PRT_HEADER option; added call to
          write_header().                        TJB
  2007-Apr-04 Added option to continue run after a cell fails.         GCT/KAC
  2007-Apr-21 Added calls to free_dmy(), free_out_data_files(),
          free_out_data(), and free_veglib().  Added closing of
          all parameter files.                    TJB
  2007-Aug-21 Return ErrorFlag from initialize_model_state.        JCA
  2007-Sep-14 Excluded calls to free_veglib() and closing of parameter
          files other than the soil param file for the case
          when OUTPUT_FORCE=TRUE.                    TJB
  2007-Nov-06 Moved computation of cell_area from read_lakeparam() to
          read_soilparam() and read_soilparam_arc().        TJB
  2008-May-05 Added prcp fraction (mu) to initial water storage
          computation.  This solves water balance errors for the
          case where DIST_PRCP is TRUE.                TJB
  2009-Jan-16 Added soil_con.avgJulyAirTemp to argument list of
          initialize_atmos().                    TJB
  2009-Jun-09 Modified to use extension of veg_lib structure to contain
          bare soil information.                    TJB
  2009-Jul-07 Added soil_con.BandElev[] to read_snowband() arg list.    TJB
  2009-Jul-31 Replaced references to N+1st veg tile with references
          to index of lake/wetland tile.                TJB
  2009-Sep-28 Replaced initial water/energy storage computations and
          calls to calc_water_balance_error/calc_energy_balance_error
          with an initial call to put_data.  Modified the call to
          read_snowband().                        TJB
  2009-Dec-11 Removed save_data structure from argument list of
          initialize_model_state().                    TJB
  2010-Mar-31 Added cell_area to initialize_atmos().            TJB
  2010-Apr-28 Removed individual soil_con variables from argument list
          of initialize_atmos() and replaced with *soil_con.    TJB
  2010-Nov-10 Added closing of state files.                TJB
  2011-Jan-04 Made read_soilparam_arc() a sub-function of
          read_soilparam().                        TJB
  2012-Jan-16 Removed LINK_DEBUG code                    BN
  2012-Nov-13 CropSyst arguments are included           KM
  2012-Nov-13 snow band is 1 when we have crop in cell KM
**********************************************************************/
{
  extern veg_lib_struct *veg_lib;
  #if VIC_CROPSYST_VERSION >=3
  extern CropCodeLib *cropcode_lib;                                              //161102LML
  extern int account_crop_types;                                                 //161102LML
  #endif
  #if VIC_CROPSYST_VERSION==2
  //150413RLN V3 works with rotations not specific crops
  extern crop_lib_struct *crop_lib;
  #endif
  //LML 150413 #if (VIC_CROPSYST_VERSION>=3)
  //LML 150413 extern crop_rotation_lib_struct *crop_rotation_lib;   //LML 141104
  //LML 150413 #endif

  extern option_struct options;
  extern Error_struct Error;
  extern global_param_struct global_param;
  /** Variable Declarations **/
  char                     NEWCELL;
  char                     LASTREC;
  char                     MODEL_DONE;
  char                     RUN_MODEL;
  char                    *init_STILL_STORM;
  char                     ErrStr[MAXSTRING];
  int                      rec, i, j;
  int                      veg;
  int                      dist;
  int                      band;
  int                      Ndist;
  int                      Nveg_type;
  int                      cellnum;
  int                      index;
  int                     *init_DRY_TIME;
  int                      Ncells;
  int                      cell_cnt;
  int                      startrec;
  int                      ErrorFlag;
  float                    mu;
  double                   storage;
  double                   veg_fract;
  double                   band_fract;
  double                   Clake;
  dmy_struct              *dmy;
  atmos_data_struct       *atmos;
  veg_con_struct          *veg_con;                                              //array
  soil_con_struct          soil_con;
  dist_prcp_struct         prcp; /* stores information about distributed
                    precipitation */
  #if (VIC_CROPSYST_VERSION>=3)
  #else
  filenames_struct         filenames;
  #endif
  filep_struct             filep;
  lake_con_struct          lake_con;
  out_data_file_struct     *out_data_files;
  out_data_struct          *out_data;
  save_data_struct         save_data;

  #if (!defined(__GNUC__) && !defined(__BCPLUSPLUS__))
  //LML 150309 MinGW doesn't have this function
  //RLN 190130 BC++ also does not have this.
  mallopt(M_MMAP_THRESHOLD,524288);
  #endif
  fprintf(stderr,"initialize_parameters...\n");
  initialize_parameters();                                                       //170406LML

#ifdef VIC_CROPSYST_VERSION
  run_CropSyst_crop = 1;                                                         //LML 150528
  soil_con_global  = &soil_con; //120811
  // layer_wet_global = layer_wet; //120814
  #if VIC_CROPSYST_VERSION==2
  crop_data_struct         *crops = 0;
  #endif
  int                      Ncrop_type; //Similar to Nveg_type but counts the number of crps in the crop library -KJC 02132011
  int                      Ncrop_rotation_type;     //LML 141104
  CO2_conc_struct          *CO2_str; //Keyvan added
  double *irrigation_data;
  double *CO2_conc;
#endif

  /** Read Model Options **/
  initialize_global();
  filenames = cmd_proc(argc, argv);

#if VERBOSE
  display_current_settings(DISP_VERSION,(filenames_struct*)NULL);
#endif

  /** Read Global Control File **/
  fprintf(stderr,"filep.globalparam...\n");
  filep.globalparam = open_file(filenames.global,"r");
  global_param = get_global_param(&filenames, filep.globalparam);

  /** Set up output data structures **/
  out_data = create_output_list();
  out_data_files = set_output_defaults(out_data);
  fclose(filep.globalparam);
  filep.globalparam = open_file(filenames.global,"r");
  parse_output_info(&filenames, filep.globalparam, &out_data_files, out_data);

  /** Check and Open Files **/
  check_files(&filep, &filenames);

#if !OUTPUT_FORCE
  /** Read Vegetation Library File **/
  veg_lib = read_veglib(filep.veglib,&Nveg_type);
#if VIC_CROPSYST_VERSION==2
  /** Read Crop Library File **/
  crop_lib = read_croplib(filep.croplib,&Ncrop_type);
  //LML 150413 #if (VIC_CROPSYST_VERSION>=3)
  //LML 150413 crop_rotation_lib = read_crop_rotation_lib(filep.crop_rotation_lib,&Ncrop_rotation_type);
  //LML 150413 #endif
#endif

#if VIC_CROPSYST_VERSION>=3
  cropcode_lib = read_cropcodelib(filep.VCS.cropcodelib,account_crop_types);
#endif


#endif // !OUTPUT_FORCE
  /** Initialize Parameters **/
  Ndist = options.NDIST;
  #ifdef VIC_CROPSYST_VERSION
  if (Ndist == 2) assert(false);                                                  //LML 150416 current version does not surport this option.
  //LML 150423 if (global_param.dt != 24) assert(false);                                     //LML 150417 current version only support daily time-step
  #endif

  cellnum = -1;

  /** Make Date Data Structure **/
  dmy      = make_dmy();
  fprintf(stderr,"alloc_atmos...\n");
  /** allocate memory for the atmos_data_struct **/
  alloc_atmos(global_param.nrecs, &atmos);

#if VIC_CROPSYST_VERSION
  /**Set irrigation library**/
#if (!defined(IRRIGATION_MANAGEMENT_QUALITY))||(IRRIGATION_MANAGEMENT_QUALITY==1)
  set_irrigation_efficiency_of_irrigation_library(1);
#elif (IRRIGATION_MANAGEMENT_QUALITY==2)
  set_irrigation_efficiency_of_irrigation_library(2);
#elif (IRRIGATION_MANAGEMENT_QUALITY==3)
  set_irrigation_efficiency_of_irrigation_library(3);
#endif
  if (run_CropSyst_crop/*LML 150528 crops*/){
  /************************************************
  deficit irrigation calculations Keyvan 130604
  ************************************************/
#if (FULL_IRRIGATION==FALSE)
    alloc_irrig_pattern(global_param.nrecs, &irrig_patt, &filep, &filenames, &soil_con);
    irrigation_data = read_irrig_pattern(filep.irrigation_pattern, irrig_patt, global_param.nrecs);
    for(rec=0; rec<global_param.nrecs; rec++)
      irrig_patt[rec].irrig_amount= irrigation_data[rec]; //keyvan 130604
#endif
    if(options.VCS.CO2_trans){
      CO2_str= read_CO2_conc(filep.VCS.CO2_PPM);
      //free((double *)irrigation_data); // keyvan 130604
    }
  }
#endif
  /** Initial state **/
  startrec = 0;
#if !OUTPUT_FORCE
  if ( options.INIT_STATE )
    filep.init_state = check_state_file(filenames.init_state, &startrec);
  /** open state file if model state is to be saved **/
  if ( options.SAVE_STATE && strcmp( filenames.statefile, "NONE" ) != 0 )
    filep.statefile = open_state_file(filenames);
  else filep.statefile = NULL;
#endif // !OUTPUT_FORCE
  /************************************
    Run Model for all Active Grid Cells
    ************************************/
  MODEL_DONE = FALSE;
  cell_cnt=0;
  while(!MODEL_DONE) {
#ifdef CROP_DAILY_OUTPUT_MEMFIRST
    crop_output_list.clear();
#endif
    fprintf(stderr,"read_soilparam...\n");
    soil_con = read_soilparam(filep.soilparam, filenames.soil_dir, &cell_cnt, &RUN_MODEL, &MODEL_DONE);

    if(RUN_MODEL) {

#if QUICK_FS
      /** Allocate Unfrozen Water Content Table **/
      if(options.FROZEN_SOIL) {
    for(i=0;i<MAX_LAYERS;i++) {
      soil_con.ufwc_table_layer[i] = (double **)malloc((QUICK_FS_TEMPS+1)*sizeof(double *));
      for(j=0;j<QUICK_FS_TEMPS+1;j++)
        soil_con.ufwc_table_layer[i][j] = (double *)malloc(2*sizeof(double));
    }
    for(i=0;i<MAX_NODES;i++) {
      soil_con.ufwc_table_node[i] = (double **)malloc((QUICK_FS_TEMPS+1)*sizeof(double *));

      for(j=0;j<QUICK_FS_TEMPS+1;j++)
        soil_con.ufwc_table_node[i][j] = (double *)malloc(2*sizeof(double));
    }
      }
#endif /* QUICK_FS */

      NEWCELL=TRUE;
      cellnum++;

#if !OUTPUT_FORCE
#if VIC_CROPSYST_VERSION==2
      crops = read_cropparam(filep.cropparam, soil_con.gridcel);
#endif

      /** Read Grid Cell Vegetation Parameters **/
      fprintf(stderr,"read_vegparam...\n");
      veg_con = read_vegparam(filep.vegparam, soil_con.gridcel, Nveg_type);
      fprintf(stderr,"end read_vegparam...\n");
      #if VIC_CROPSYST_VERSION>=3
      #ifdef USE_IRRIGATION_PARAM_FILE
      read_irrigparam(filep.VCS.irrigparam, soil_con.gridcel, soil_con.VCS.irrigation_type_list); //160609LML
      #endif
      fprintf(stderr,"make_veg_lib_for_crops...\n");
      make_veg_lib_for_crops(veg_con,veg_lib);                                   //151001LML
      #endif
      calc_root_fractions(soil_con,veg_con);
      //150928LML #if VIC_CROPSYST_VERSION>=3
      //150928LML crops = provide_cropparam(veg_con, soil_con.gridcel);
      //150928LML #endif
      if ( options.LAKES )
        lake_con = read_lakeparam(filep.lakeparam, soil_con, veg_con);
#endif // !OUTPUT_FORCE
      /** Build Gridded Filenames, and Open **/
      make_in_and_outfiles(&filep, &filenames, &soil_con, out_data_files);
#ifdef VIC_CROPSYST_VERSION
      //To reassign the the nelem in the outdata to crop paramters to the actual number of crops
      int numcrops = get_cropping_system_numbers(veg_con);                       //150928LML
      //170925LML if (numcrops > 0)
        set_output_for_crop_numbers(out_data,numcrops);                          //150928LML
#endif
      if (options.PRT_HEADER) {
        /** Write output file headers **/
        write_header(out_data_files, out_data, dmy);
      }
#if !OUTPUT_FORCE
      /** Read Elevation Band Data if Used **/
      read_snowband(filep.snowband, &soil_con);
      /** Make Precipitation Distribution Control Structure **/
#if defined(ONE_SNOWBAND_FOR_CROP) && defined(VIC_CROPSYST_VERSION)
  //assume one snowband for cells that contain crop 180117LML
  if (numcrops > 0) {
      soil_con.AreaFract[0] = 1.0;
      soil_con.Pfactor[0]   = 1.0;
      soil_con.Tfactor[0]   = 0.0;
      soil_con.BandElev[0]  = soil_con.elevation;
      options.SNOW_BAND = 1;
  }
#endif
      fprintf(stderr,"make_dist_prcp...\n");
      prcp     = make_dist_prcp(veg_con[0].vegetat_type_num
                                //150929LML #if VIC_CROPSYST_VERSION
                                //150929LML ,veg_con
                                //150929LML #endif
              );
      fprintf(stderr,"make_dist_prcp done...\n");
#endif // !OUTPUT_FORCE

      /**************************************************
         Initialize Meteological Forcing Values That
         Have not Been Specifically Set
       **************************************************/
#if VERBOSE
      fprintf(stderr,"Initializing Forcing Data\n");
#endif /* VERBOSE */

      initialize_atmos(atmos, dmy, filep.forcing,
#if OUTPUT_FORCE
                       &soil_con, out_data_files, out_data);
#else /* OUTPUT_FORCE */
                       &soil_con);
#endif /* OUTPUT_FORCE */

#if !OUTPUT_FORCE

      /**************************************************
        Initialize Energy Balance and Snow Variables
      **************************************************/

#if VERBOSE
      fprintf(stderr,"Model State Initialization\n");
#endif /* VERBOSE */
      ErrorFlag = initialize_model_state(&prcp, dmy[0], filep,
                                         soil_con.gridcel, veg_con[0].vegetat_type_num,
                                         options.Nnode,
                                         atmos[0].air_temp[NR],
                                         &soil_con, veg_con, lake_con,
                                         &init_STILL_STORM, &init_DRY_TIME);
      if ( ErrorFlag == ERROR ) {
        if ( options.CONTINUEONERROR == TRUE ) {
          // Handle grid cell solution error
          fprintf(stderr, "ERROR: Grid cell %i failed in record %i so the simulation has not finished.  An incomplete output file has been generated, check your inputs before rerunning the simulation.\n", soil_con.gridcel, rec);
          break;
        } else {
          // Else exit program on cell solution error as in previous versions
          sprintf(ErrStr, "ERROR: Grid cell %i failed in record %i so the simulation has ended. Check your inputs before rerunning the simulation.\n", soil_con.gridcel, rec);
          vicerror(ErrStr);
        }
      }

#if VERBOSE
      fprintf(stderr,"Running Model\n");
#endif /* VERBOSE */

      /** Update Error Handling Structure **/
      Error.filep = filep;
      Error.out_data_files = out_data_files;

      /** Initialize the storage terms in the water and energy balances **/
      /** Sending a negative record number (-global_param.nrecs) to dist_prec() will accomplish this **/
      ErrorFlag = dist_prec(&atmos[0], &prcp, &soil_con, veg_con,
                            &lake_con, dmy, &global_param, &filep, out_data_files,
                            out_data, &save_data, -global_param.nrecs, cellnum,
                            NEWCELL, LASTREC, init_STILL_STORM, init_DRY_TIME
#ifdef VIC_CROPSYST_VERSION
                           #if VIC_CROPSYST_VERSION==2
                           ,crops
                           #endif
                           #if (FULL_IRRIGATION==FALSE)
                           ,irrig_patt /**keyvan added this for deficit irrigation**/
                           #endif
                           ,0
#endif
                  );

      /******************************************
    Run Model in Grid Cell for all Time Steps
    ******************************************/
#if (VIC_CROPSYST_VERSION>=3)
      int spinups = 0;
      int this_spin_years = 0;
      int this_spin_days  = 0;
      global_param.VCS.is_spinup_run = 1;
      if (global_param.VCS.number_spinups_for_soil == 0)
        global_param.VCS.is_spinup_run = 0;
      int total_years_spinup = global_param.VCS.number_spinups_for_soil * global_param.VCS.spinup_years;

      if (global_param.VCS.is_spinup_run) global_simdate //200827RLN global_today
          .set_YMD(dmy[0].year-total_years_spinup,dmy[0].month,dmy[0].day);
      if(!CS::emanators_global) CS::instantiate_emanators(global_simdate/*200827RLN global_today*/);                //180111LML
#endif
      for ( rec = startrec ; rec < global_param.nrecs; rec++ ) {
        #ifdef VIC_CROPSYST_VERSION
        #if (VIC_CROPSYST_VERSION>=3)
        if (!global_param.VCS.is_spinup_run) global_simdate/*200827 global_today*/
            .set_YMD(dmy[rec].year,dmy[rec].month,dmy[rec].day); //150723LML to synchronize
        global_rec = rec;
#ifdef LIU_DEBUG
        if (global_simdate /*200727RLN global_today*/.get_DOY() == 1 || !global_param.VCS.is_spinup_run)
            std::clog<<"NREC:"      <<global_param.nrecs
                 <<" rec:"      <<rec
                 <<" spinup?:"  <<PRINT_BOOLEAN_AS_STRING(global_param.VCS.is_spinup_run)
                 <<" spin#:"    <<(global_param.VCS.is_spinup_run ? (spinups + 1) : 0)
                 <<" Year:"     <<(int)global_simdate/*200827 global_today*/.get_year()
                 <<" Mon:"      <<(int)global_simdate/*200827 global_today*/.get_month()
                 <<" Day:"      <<(int)global_simdate/*200827 global_today*/.get_DOM()
                 <<" DOY:"      <<(int)global_simdate/*200827 global_today*/.get_DOY()
                 <<std::endl;

        if (global_simdate /*200727RLN global_today*/.get_year() == 1979 && global_simdate /*200727RLN global_today*/.get_DOY() == 250){
            std::clog << "Debug_DOY:" << global_simdate/*200827 global_today*/.get_DOY() << std::endl;
        }

#endif
        //std::clog<<"NREC:"      <<global_param.nrecs
        //         <<" Year:"     <<(int)global_today.get_year()
        //         <<" Mon:"      <<(int)global_today.get_month()
        //         <<" Day:"      <<(int)global_today.get_DOM()
        //         <<" DOY:"      <<(int)global_today.get_DOY()
        //         <<std::endl;
        #endif
        if (options.VCS.CO2_trans){ ///updating co2 concentration for transient co2 condition
          for ( i = 0; i < options.VCS.CO2_Nyear; i++) {
            if (dmy[rec].year==CO2_str[i].year)
              options.VCS.CO2_PPM=CO2_str[i].CO2_conc;
          }
        }
        #endif

        if ( rec == global_param.nrecs - 1 ) LASTREC = TRUE;
        else LASTREC = FALSE;

        //if (rec == 125) {
        //    std::clog << "DEBUGGING main rec = 125\n";
        //}

        ErrorFlag = dist_prec(&atmos[rec], &prcp, &soil_con, veg_con,
                              &lake_con, dmy, &global_param, &filep,
                              out_data_files, out_data, &save_data, rec, cellnum,
                              NEWCELL, LASTREC, init_STILL_STORM, init_DRY_TIME
#ifdef VIC_CROPSYST_VERSION
                              #if VIC_CROPSYST_VERSION==2
                              ,crops
                              #endif
                              #if (FULL_IRRIGATION==FALSE)
                              ,irrig_patt /**keyvan added this for deficit irrigation**/
                              #endif
                              ,global_param.VCS.is_spinup_run                        //150819LML
#endif
                    ); //crop structure added

        if ( ErrorFlag == ERROR ) {
          if ( options.CONTINUEONERROR == TRUE ) {
            // Handle grid cell solution error
            fprintf(stderr, "ERROR: Grid cell %i failed in record %i so the simulation has not finished.  An incomplete output file has been generated, check your inputs before rerunning the simulation.\n", soil_con.gridcel, rec);
            break;
          } else {
            // Else exit program on cell solution error as in previous versions
            sprintf(ErrStr, "ERROR: Grid cell %i failed in record %i so the simulation has ended. Check your inputs before rerunning the simulation.\n", soil_con.gridcel, rec);
            vicerror(ErrStr);
          }
        }
        NEWCELL=FALSE;
        for ( veg = 0; veg <= veg_con[0].vegetat_type_num; veg++ )
          init_DRY_TIME[veg] = -999;
        #if VIC_CROPSYST_VERSION>=3
        int end_of_day = (global_param.dt == 24) || (dmy[rec].hour == (24 - global_param.dt));
        if (global_param.VCS.is_spinup_run) {
          if (end_of_day) {
            this_spin_days++;
            if (this_spin_days == global_simdate /*200727RLN global_today*/.days_in_this_year()) {
              this_spin_years++;
              this_spin_days = 0;
            }
          }
          if (end_of_day                                     &&
            ((this_spin_years >= global_param.VCS.spinup_years ) ||
             (rec == (global_param.nrecs - 1))))
          {
            spinups++;
            rec = startrec - 1;
            this_spin_years = 0;
            this_spin_days  = 0;
          }
          if(spinups == global_param.VCS.number_spinups_for_soil)
            global_param.VCS.is_spinup_run = 0;
          if (end_of_day)
            global_simdate /*200727RLN global_today*/.inc_day();
        }
        #endif
      }    /* End Rec Loop */

#endif /* !OUTPUT_FORCE */
      close_files(&filep,out_data_files,&filenames);
#if !OUTPUT_FORCE
#if QUICK_FS
      if(options.FROZEN_SOIL) {
        for(i=0;i<MAX_LAYERS;i++) {
          for(j=0;j<6;j++)
            free((char *)soil_con.ufwc_table_layer[i][j]);
          free((char *)soil_con.ufwc_table_layer[i]);
        }
        for(i=0;i<MAX_NODES;i++) {
          for(j=0;j<6;j++)
            free((char *)soil_con.ufwc_table_node[i][j]);
          free((char *)soil_con.ufwc_table_node[i]);
        }
      }
#endif /* QUICK_FS */
      free_dist_prcp(&prcp,veg_con[0].vegetat_type_num);
      free_vegcon(&veg_con);
#ifdef VIC_CROPSYST_VERSION
      veg_con = 0;
      #if VIC_CROPSYST_VERSION==2
      if (crops)  /*120824 RLN */
      free_crop(crops); crops=0;
      #endif
      //free((char *) (crop));
#endif
      free((char *)soil_con.AreaFract);
      free((char *)soil_con.BandElev);
      free((char *)soil_con.Tfactor);
      free((char *)soil_con.Pfactor);
      free((char *)soil_con.AboveTreeLine);
      free((char*)init_STILL_STORM);
      free((char*)init_DRY_TIME);
#endif /* !OUTPUT_FORCE */
    printf("Finished gridcell:%d\n",soil_con.gridcel);

//print out crop output
#ifdef CROP_DAILY_OUTPUT_MEMFIRST
    if (sizeof(crop_output_list.size() > 0)) {
        sprintf(out_file_name,"%s/%s_%.5f_%.5f.asc",filenames.result_dir,
            filenames.VCS.crop_output_file_name,
            soil_con.lat,
            soil_con.lng);
        std::ofstream debugout(out_file_name,std::ofstream::out);
        debugout << crop_output_head.str();
        for (std::list<Crop_output_struct>::iterator temp=crop_output_list.begin(); temp != crop_output_list.end(); ++temp) {
            //std::cout << ' ' << *it;
            debugout << temp->cell_id
                    << "," << std::setprecision(8) << temp->lon
                    << "," << std::setprecision(7) << temp->lat
                    << "," << temp->Year
                    << "," << temp->Month
                    << "," << temp->Day
                    << "," << temp->DOY
                    << "," << temp->Dist
                    << "," << temp->Band
                    << std::setprecision(8)
                    << "," << temp->Cell_fract
                    << "," << temp->CroppingSyst_code
                    << "," << temp->Crop_code
                    << "," << temp->Crop_name
                    << "," << temp->Accum_DD
                    << "," << temp->Accum_DD_clip
                    << "," << temp->Grow_Stage
                    << "," << temp->GAI
                    << "," << temp->Green_Canopy_Cover
                    << "," << temp->Biomass_kg_m2
                    << "," << temp->Yield_kg_m2
                    << "," << temp->Root_depth_mm
                    << "," << temp->VIC_PET_shortgrass_mm
                    << "," << temp->CropSyst_refET_mm
                    << "," << temp->Act_Transp_mm
                    << "," << temp->VIC_final_transp_mm
                    << "," << temp->irrig_netdemand_mm
                    << "," << temp->irrig_total_mm
                    << "," << temp->irrig_evap_mm
                    << "," << temp->irrig_runoff_mm
                    << "," << temp->irrig_intcpt_mm
                    << "," << temp->Soil_E_mm
                    << "," << temp->Crop_Canopy_E_mm
                    << "," << temp->snow_sublimation
                    << "," << temp->ET_mm
                    << "," << temp->VIC_ET_mm
                    << "," << temp->water_stress_index
                    << "," << temp->Runoff
                    << "," << temp->Baseflow
                    << "," << temp->PPT
                    << "," << temp->Tair_avg
                    << "," << temp->Tmax
                    << "," << temp->Tmin
                    << "," << temp->SWRAD_w_m2
                    << "," << temp->VP_kPa
                    << "," << temp->SPH_kg_kg
                    << "," << temp->RHUM_avg_per
                    << "," << temp->RHUM_max_per
                    << "," << temp->RHUM_min_per
                    << "," << temp->Snow_dep_mm
                    << "," << temp->Biomass_kg_m2
                    << "," << temp->profile_liq_water
                    << std::endl;
        }
        debugout.close();
        crop_output_list.clear();
        }
#endif
    }    /* End Run Model Condition */
  }     /* End Grid Loop */

  /** cleanup **/
  free_atmos(global_param.nrecs, atmos);
  free_dmy(&dmy);
  free_out_data_files(&out_data_files);
  free_out_data(&out_data);
#if !OUTPUT_FORCE
#if VIC_CROPSYST_VERSION==2
  free_croplib(&crop_lib); //keyvan added this
 //LML 150413 #if (VIC_CROPSYST_VERSION>=3)
 //LML 150413  free_crop_rotation_lib(&crop_rotation_lib);    //LML 141104
 //LML 150413 #endif
#endif
  free_veglib(&veg_lib);
#endif /* !OUTPUT_FORCE */
  fclose(filep.soilparam);
#if !OUTPUT_FORCE
  fclose(filep.vegparam);
  fclose(filep.veglib);
  if (options.SNOW_BAND>1)
    fclose(filep.snowband);
  if (options.LAKES)
    fclose(filep.lakeparam);
  if ( options.INIT_STATE )
    fclose(filep.init_state);
  if ( options.SAVE_STATE && strcmp( filenames.statefile, "NONE" ) != 0 )
    fclose(filep.statefile);

#endif /* !OUTPUT_FORCE */
  #if VIC_CROPSYST_VERSION>=3
  fclose(filep.VCS.cropcodelib);
  free_cropcodelib(&cropcode_lib);
  provide_explainations_global().delete_all();
  #endif
  return EXIT_SUCCESS;
}    /* End Main Program */

/*170413LML moved to VCS_Nl.c
//151001LML
#if VIC_CROPSYST_VERSION>=3
int make_veg_lib_for_crops(veg_con_struct *veg_con,veg_lib_struct *veg_lib)
{
  //Set veg lib for each bond
  for (int iveg = 0; iveg <= veg_con[0].vegetat_type_num; iveg++) {
    veg_con[iveg].VCS.veg_lib = new veg_lib_struct*[options.SNOW_BAND];
    int vegcode = veg_con[iveg].VCS.veg_class_code;
    int rotation_or_veg_class_code;
    int rotation_index;
    DecomposeVegclassCode(vegcode,rotation_or_veg_class_code,rotation_index);
    int veg_lib_index = get_veg_lib_index(vegcode);
    if (iscrop(vegcode)) {
      for (int band = 0; band < options.SNOW_BAND; band++) {
        veg_con[iveg].VCS.veg_lib[band] = new veg_lib_struct;
        if (veg_lib_index >= 0)
          copy_veg_lib_record(veg_lib[veg_lib_index],*veg_con[iveg].VCS.veg_lib[band]);
      }
    } else {
      for (int band = 0; band < options.SNOW_BAND; band++) {
          if (veg_lib_index >= 0)
            veg_con[iveg].VCS.veg_lib[band] = &veg_lib[veg_lib_index];
          else
            veg_con[iveg].VCS.veg_lib[band] = 0;
      }
    }
  }
  return 0;
}
//______________________________________________________________________________
int copy_veg_lib_record(const veg_lib_struct &from_veg, veg_lib_struct &to_veg)
{
  to_veg.overstory      = from_veg.overstory      ;
  to_veg.NVegLibTypes   = from_veg.NVegLibTypes   ;
  to_veg.rad_atten      = from_veg.rad_atten      ;
  to_veg.rarc           = from_veg.rarc           ;
  to_veg.rmin           = from_veg.rmin           ;
  to_veg.trunk_ratio    = from_veg.trunk_ratio    ;
  to_veg.wind_atten     = from_veg.wind_atten     ;
  to_veg.wind_h         = from_veg.wind_h         ;
  to_veg.RGL            = from_veg.RGL            ;
  to_veg.veg_class      = from_veg.veg_class      ;
  to_veg.VCS.veg_class_code = from_veg.VCS.veg_class_code ;
  strcpy(to_veg.VCS.veg_or_rotation_name,from_veg.VCS.veg_or_rotation_name);
  for (int mon = 0; mon < 12; mon++) {
    to_veg.LAI[mon]         = from_veg.LAI[mon]         ;
    to_veg.Wdmax[mon]       = from_veg.Wdmax[mon]       ;
    to_veg.albedo[mon]      = from_veg.albedo[mon]      ;
    to_veg.displacement[mon]= from_veg.displacement[mon];
    to_veg.emissivity[mon]  = from_veg.emissivity[mon]  ;
    to_veg.roughness[mon]   = from_veg.roughness[mon]   ;
  }
}
#endif
*/
//160510LML copied from CropSyst
bool is_approximately(const double &value,const double &target,const double tolerance)
{ return ((value) < ((target) + (tolerance))) && ((value) > ((target) - (tolerance)));}
//160526LML____________________________________________________________________/
