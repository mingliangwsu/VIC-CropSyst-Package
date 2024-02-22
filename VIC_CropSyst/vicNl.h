/* RCS Id String
 * $Id: vicNl.h,v 5.20.2.43 2012/10/25 23:15:58 vicadmin Exp $
 */
/************************************************************************
  Modifications:
  2006-Sep-23 Implemented flexible output configuration; uses the new
              out_data, out_data_files, and save_data structures.	TJB
	      Removed the following functions:
		conv_force_vic2alma
		conv_results_vic2alma
	      Added the following new functions:
		create_output_list
		free_out_data_files
		init_output_list
		parse_output_info
		set_output_defaults
		set_output_var
		zero_output_list
  2006-Oct-16 Merged infiles and outfiles structs into filep_struct.	TJB
  2006-Nov-07 Removed LAKE_MODEL option.				TJB
  2007-Jan-15 Added PRT_HEADER option.					TJB
  2007-Apr-03 Modified the data types of the following functions for
	      CONTINUE_ON_ERROR:					KAC/GTC
	      CalcAerodynamic
	      dist_prec
	      distribute_node_moisture_properties
	      full_energy
	      initialize_new_storm
	      redistribute_during_storm
	      runoff
	      snow_intercept
	      snow_melt
	      solve_T_profile
	      surface_fluxes
  2007-Apr-24 Added Ming Pan's new functions for IMPLICIT option.       JCA
              fda_heat_eqn
              newt_raph
              tridiag
              fdjac3
              solve_T_profile_implicit
  2007-Apr-21 Added functions:						TJB
	      free_dmy
	      free_out_data
	      free_veglib
  2007-Aug-09 Added features for EXCESS_ICE option.			JCA
  2007-Aug-22 Made calc_water_balance_error  type double.		JCA
  2007-Nov-06 Moved get_dist() from LAKE.h to this file.		JCA
  2008-Feb-17 Changed argument list for snow_density().			KMA via TJB
  2008-Apr-21 Added snow depth and albedo to snow_albedo() argument
	      list.							KAC via TJB
  2008-Oct-23 Modified put_data() to be type int, so that it can
	      return an error status.					TJB
  2009-Jan-16 Added avgJulyAirTemp to argument list of
	      compute_treeline().					TJB
  2009-Feb-09 Removed dz_node from several functions.			KAC via TJB
  2009-Mar-16 Added resid_moist to argument list of
	      estimate_layer_ice_content().				TJB
  2009-May-17 Added asat to argument list of surface_fluxes(),
	      full_energy(), and wetland_energy().			TJB
  2009-Jun-09 Modified argument lists of some functions that were
	      modified to use extension of veg_lib structure to contain
	      bare soil information.					TJB
  2009-Jun-09 Added compute_pot_evap().					TJB
  2009-Jun-09 Removed unnecessary functions quick_penman() and
	      compute_penman_constants().				TJB
  2009-Jun-19 Added T flag to indicate whether TFALLBACK occurred.	TJB
  2009-Jun-26 Simplified argument list of runoff() by passing all cell_data
	      variables via a single reference to the cell data structure.	TJB
  2009-Jul-07 Added soil_con.BandElev[] to read_snowband() arg list.	TJB
  2009-Jul-31 Removed unused layer_node_fract array from
	      estimate_layer_ice_content().				TJB
  2009-Sep-19 Added T fbcount to count TFALLBACK occurrences.		TJB
  2009-Sep-28 Added collect_wb_terms() and collect_eb_terms(). Changed
	      argument list of read_snowband().				TJB
  2009-Oct-08 Extended T fallback scheme to snow and ice T.		TJB
  2009-Nov-15 Removed ice0 and moist0 from argument list of solve_snow,
	      since they are never used.				TJB
  2009-Dec-11 Removed save_data structure from argument list of
	      initialize_model_state().					TJB
  2009-Dec-11 Removed min_liq and options.MIN_LIQ.			TJB
  2010-Mar-31 Added cell_area to initialize_atmos().			TJB
  2010-Apr-26 Simplified argument lists for solve_snow() and
	      snow_intercept().						TJB
  2010-Apr-28 Removed net_short, displacement, roughness, and ref_height
	      from arg list of arno_evap() as they are no longer used.	TJB
  2010-Apr-28 Removed individual soil_con variables from argument list
	      of initialize_atmos() and replaced with *soil_con.	TJB
  2010-Nov-11 Added lakefactor to collect_wb_terms() and collect_eb_terms()
	      so that these functions could handle changes in how lake
	      and wetland cell/soil/snow/energy fluxes are represented.	TJB
  2010-Dec-01 Added compute_zwt().					TJB
  2011-Jan-04 Made read_soilparam_arc() a sub-function of
	      read_soilparam().						TJB
  2011-Mar-01 Added wrap_compute_zwt().  Added compute_runoff_and_asat().
	      Changed the argument list of initialize_soil().		TJB
  2011-Mar-31 Added frost_fract to collect_wb_terms() arglist.		TJB
  2011-May-24 Replaced finish_frozen_soil_calcs() with
	      calc_layer_average_thermal_props().  Added
	      estimate_layer_ice_content_quick_flux().			TJB
  2011-Jun-03 Added options.ORGANIC_FRACT.  Soil properties now take
	      organic fraction into account.				TJB
  2011-Jun-10 Added bulk_dens_min and soil_dens_min to arglist of
	      soil_conductivity() to fix bug in commputation of kappa.	TJB
  2011-Nov-04 Updated mtclim functions to MTCLIM 4.3.                   TJB
  2012-Jan-16 Removed LINK_DEBUG code					BN
  2012-Oct-25 Changed calc_energy_balance_error to return the error to
	      the parent function.					CL via TJB

************************************************************************/
#ifndef __BCPLUSPLUS__
//RLN Borland compiler is complianing about vector
// Mingliang is using vector to lookup crop code give rotation code
// We shouldn't need to do this.
#  ifdef __cplusplus
#     include <vector>                                                               //LML 150323
#  endif
#endif

#ifdef VCS_V5
#include <vic_def.h>
#else
#include <vicNl_def.h>
#endif

#include "pubtools.h"
#include "global.h"

#ifndef VICNL_H
#  define VICNL_H
#  ifdef VIC_CROPSYST_VERSION

#     include <VCS_Nl.h>
#  endif
#endif

#include <math.h>
#include <LAKE.h>

#ifdef __cplusplus
extern "C" {
#endif

extern parameters_struct param;

/*** SubRoutine Prototypes ***/

double calc_advected_sensible_heat(double, double, double, double, double);
void alloc_atmos(int, atmos_data_struct **);
double arno_evap(const soil_con_struct &, const double, const double,
                 const double, const double, const double, const double
                 #if SPATIAL_FROST
                 ,double
                 #endif // SPATIAL_FROST
                 ,layer_data_struct*, layer_data_struct*
                 );
//160509LML soil evaporation (m/s)
double calc_soil_evap(const layer_data_struct &layer
    , const soil_con_struct &soil_con
    , const double tair, const double elevation, const double rad, const double vpd, const double ra, const double rc, const double rarc
    #if SPATIAL_FROST
    ,const double *frost_fract
    #endif
    );
unsigned char average_moisture_for_storm(double *, double *, double, double);
int   CalcAerodynamic(char, double, double, double, double, double,
               double *, double *, double *, double *, double *);
void   calc_cloud_cover_fraction(atmos_data_struct *, dmy_struct *, int,
				 int, int, double *);
double calc_energy_balance_error(int, double, double, double, double, double);
#if OUTPUT_FORCE_STATS
void   calc_forcing_stats(int, atmos_data_struct *);
#endif // OUTPUT_FORCE_STATS
double calc_longwave(const double tskc, const double  air_temp_celsius, const double vp_pa);
void   calc_netlongwave(double *, double, double, double);
double calc_netshort(double, int, double, double *);
double calc_rainonly(const double,const double,const double,const double,double);
double calc_rainonly_dynamic_threshold(const double,const double,const double,const double);
double calc_rc(double,double,float,double,double,double,double,char);
void   calc_root_fractions(const soil_con_struct&, veg_con_struct*);
#if SPATIAL_SNOW
double calc_snow_coverage(int *, double, double, double, double, double,
                          double, double, double *, double *, double *,
                          double *, double *);
#endif
double calc_snow_ground_flux(int, int, int, int, double, double, double,
			     double, double, double *, double *, double *,
			     double *, energy_bal_struct *,
			     snow_data_struct *, layer_data_struct *,
                             layer_data_struct *, soil_con_struct *, char *);
#if QUICK_FS
int    calc_soil_thermal_fluxes(int, double *, double *, char *, int *, double *, double *,
				double *, double *, double *,double *,
				double *, double *, double *,
				double *, double *, double *, double ***, int, int, int, int);
#else
int    calc_soil_thermal_fluxes(int, double *, double *, char *, int *, double *, double *,
				double *, double *, double *,double *,
				double *, double *, double *,
				double *, double *, double *,
#if EXCESS_ICE
				double *, double *,
#endif // EXCESS_ICE
				int, int, int, int);
#endif // QUICK_FS
double CalcSnowPackEnergyBalance(double Tsurf, ...);
double CalcBlowingSnow( const double Dt,
                        const double Tair,
                        const int LastSnow,
                        const double SurfaceLiquidWater,
                        const double Wind,
                        const double Ls,
                        const double AirDens,
                        //160516LML double Press,
                        const double EactAir,
                        const double ZO,
                        const double Zrh,
                        const double snowdepth,
                        const float lag_one,
                        const float sigma_slope_orig,
                        const double Tsnow,
                        const int iveg,
                        const int Nveg,
                        const float fe_orig,
                        const double displacement,
                        const double roughness,
                        double *TotalTransport);
double calc_atmos_energy_bal(double InOverSensible, double InUnderSensible, double LatentHeatOver, double LatentHeatUnder,
                 double LatentHeatSubOver, double LatentHeatSubUnder, double Lv, double NetLongOver, double NetLongUnder, double NetShortOver, double NetShortUnder, const double  Ra,
                 const double Tair, const double atmos_density, double vp, double vpd, double *Error, double *LatentHeat, double *LatentHeatSub, double *NetLongAtmos, double *NetShortAtmos, double *SensibleHeat, double *VPcanopy, double *VPDcanopy, char *Tcanopy_fbflag, int *Tcanopy_fbcount);
int    calc_layer_average_thermal_props(energy_bal_struct *, layer_data_struct *,
					layer_data_struct *, layer_data_struct *,
					soil_con_struct *, int, int, double *);
double calc_surf_energy_bal(double             Le,
                double             LongUnderIn,
                double             NetLongSnow,                                  // net LW at snow surface
                double             NetShortGrnd,                                 // net SW transmitted thru snow
                double             NetShortSnow,                                 // net SW at snow surface
                double             OldTSurf,
                double             ShortUnderIn,
                double             SnowAlbedo,
                double             SnowLatent,
                double             SnowLatentSub,
                double             SnowSensible,
                double             Tair,                                         // T of canopy or air
                double             VPDcanopy,
                double             VPcanopy,
                //160516LML double             advection,
                //160516LML double             coldcontent,
                double             delta_coverage, // change in coverage fraction
                double             dp,
                double             ice0,
                double             melt_energy,
                double             moist,
                double             mu,
                double             snow_coverage,
                double             snow_depth,
                double             BareAlbedo,
                double             surf_atten,
                //160516LML double             vapor_flux,
                double            *aero_resist,
                double            *aero_resist_used,
                double            *displacement,
                double            *melt,
                double            *ppt,
                double            *rainfall,
                double            *ref_height,
                double            *roughness,
                #if SPATIAL_SNOW
                double            *snowfall,
                #endif
                double            *wind,
                float             *root,
                int                INCLUDE_SNOW,
                int                UnderStory,
                int                Nnodes,
                int                Nveg,
                int                band,
                int                dt,                                           //(hr)
                int                hour,                                         //hidx
                int                iveg,
                //160516LML int                nlayer,
                int                overstory,
                int                rec,
                int                veg_class,
                atmos_data_struct *atmos,
                dmy_struct        *dmy,
                energy_bal_struct *energy,
                layer_data_struct *layer_dry,
                layer_data_struct *layer_wet,
                snow_data_struct  *snow,
                soil_con_struct   *soil_con,
                veg_var_struct    *veg_var_dry,
                veg_var_struct    *veg_var_wet,
                int                nrecs
#ifdef VIC_CROPSYST_VERSION
                #if (VIC_CROPSYST_VERSION>=3)
                ,veg_con_struct     *veg_con
                #endif
                ,cell_data_struct  *cell_wet
                ,cell_data_struct  *cell_dry
                ,int                veg_class_code
                ,double           **aero_resist_ET
                #ifndef FULL_IRRIGATION
                ,irrigation_pattern_struct *irrig_patt
                #endif
#endif //keyvan 11152012
                );
double calc_trans(double, double);
double calc_veg_displacement(const double);
double calc_veg_height(const double);
double calc_veg_roughness(const double);
double calc_water_balance_error(int, double, double, double);

/*170412LML moved to VCS_Nl.h
#if VIC_CROPSYST_VERSION==2 ///Keyvan added
int call_crop_model(crop_data_struct *,
                   // #ifdef CROPSYST_PROVIDED_SOIL_HYDROLOGY
                   soil_con_struct  *,
                   //#endif
                   atmos_data_struct *,
                   //4.1.1 cell_data_struct *,  //At this time, only wet cell is sent.  Only transpiration is updated
                   layer_data_struct *,
                   dmy_struct *,
                   int, int //double *, double,
                   ,cell_data_struct *
                   ,int
                   #ifdef CROPSYST_PROVIDED_SOIL_HYDROLOGY
                   ,double
                   #endif
                   #if (VIC_CROPSYST_VERSION>=3)
                   ,int current_crop_counter
                   ,veg_con_struct *veg_con
                   #endif
                   );
#endif
*/
double canopy_evap(const soil_con_struct   &,
                   const char,
                   const int,
                   const int,
                   const double,
                   const double,      /*seconds*/
                   const double,
                   const double,
                   const double,
                   const double,
                   const double,
                   const double,
                   const double *,
#if SPATIAL_FROST
                   const double *,
#endif
                   const float *
#ifdef VIC_CROPSYST_VERSION
                   #if (VIC_CROPSYST_VERSION>=3)
                   ,const int
                   ,const int
                   ,/*190918LML const*/ veg_con_struct *
                   ,const double
                   ,const double
                   ,const int
                   #endif
                   ,const int
                   ,const atmos_data_struct *
                   ,const dmy_struct       *
                   ,const int
                   ,const double          * const *
                   ,cell_data_struct *
                   ,cell_data_struct *
#endif
                   ,double            *
                   ,layer_data_struct *
                   ,layer_data_struct *
                   ,veg_var_struct    *
                   ,veg_var_struct    *);
double overstory_canopy_evap(layer_data_struct *layer_wet,
                             layer_data_struct *layer_dry,
                             veg_var_struct    *veg_var_wet,
                             veg_var_struct    *veg_var_dry,
                             int                veg_class,
                             int                month,
                             double             mu,
                             double            *Wdew,
                             double             delta_t,
                             double             rad,
                             double             vpd,
                             double             net_short,
                             double             air_temp,
                             double             ra,
                             double             elevation,
                             double            *prec,
                             double            *dryFrac);
void   check_files(filep_struct *, filenames_struct *);
FILE  *check_state_file(char *, int *);
void   close_files(filep_struct *, out_data_file_struct *, filenames_struct *);
filenames_struct cmd_proc(int argc, char *argv[]);
void   collect_eb_terms(const energy_bal_struct&, const snow_data_struct&, const cell_data_struct&,
                        const double, const double, const double,
                        const int, const int, const double, const int, const int,
                        int *, int *, int *, int *, int *,
                        out_data_struct *
#if SPATIAL_FROST
                        ,double *, double
#endif
                        );
void   collect_wb_terms(const cell_data_struct&, const veg_var_struct&, const snow_data_struct&,
                        const double, const double, const double, const double,
                        const int, const int, const double, const int, double *,
#if SPATIAL_FROST
                        double *,
#endif
                        out_data_struct *);
void   compress_files(char string[]);
void compute_dz(const int Nnodes, const double dp, double *dz, double *thermdepths);
void   correct_precip(const double, const double, const double, const double,double *);
#warning RLN reached

void compute_pot_evap(int veg_class,
                      const dmy_struct *dmy,
                      int rec,
                      int dt,
                      double shortwave,
                      double net_longwave,
                      double tair,
                      double vpd,
                      double elevation,
                      const double * const *aero_resist,
                      double *pot_evap,
                      double *pot_evap_daily = 0,
                      double run_time_hours  = 0
                      );
//void   compute_runoff_and_asat(const soil_con_struct &, const double *, const double, double *, double *);
void   compute_runoff_and_asat(const soil_con_struct *, double *, double, double *, double *);
void   compute_soil_layer_thermal_properties(layer_data_struct *, double *,
					     double *, double *, double *,
					     double *, double *, double *,
#if SPATIAL_FROST
                                             double *,
#endif
					     int);
void   compute_treeline(const atmos_data_struct *, const dmy_struct *, const double, const double *, char *);
double compute_zwt(const soil_con_struct *, int, double);
out_data_struct *create_output_list();

void   display_current_settings(int, filenames_struct *);
int    dist_prec(atmos_data_struct *,dist_prcp_struct *,soil_con_struct *,
		 veg_con_struct *, lake_con_struct *,
         dmy_struct *, global_param_struct *,
		 filep_struct *, out_data_file_struct *,
		 out_data_struct *, save_data_struct *,
		 int, int, char, char, char *, int *
#ifdef VIC_CROPSYST_VERSION
        #if VIC_CROPSYST_VERSION==2
		, crop_data_struct * //keyvan added crop_data_struct
        #endif
        #ifndef FULL_IRRIGATION
        //,irrigation_pattern_struct *
        ,std::map<int,std::map<int,irrigation_pattern_struct>> &irrig_patt
        #endif
        ,int spinup_run = 0                                                      //150819LML
#endif
		 );
int distribute_node_moisture_properties(const soil_con_struct &soil_con, const double *moist, energy_bal_struct &energy);
void   distribute_soil_property(double *,double,double,
				double **l_param,
				int, int, double *, double *);

double error_calc_atmos_energy_bal(double Tcanopy, ...);
double error_calc_atmos_moist_bal(double , ...);
double error_calc_canopy_energy_bal(double, ...);
double error_calc_snow_ground_flux(double Tsurf, ...);
double error_calc_surf_energy_bal(double Tsurf, ...);
double error_print_atmos_energy_bal(double, va_list);
double error_print_atmos_moist_bal(double, va_list);
double error_print_canopy_energy_bal(double, va_list);
double error_print_snow_ground_flux(double, va_list);
double error_print_solve_T_profile(double, va_list);
double error_print_surf_energy_bal(double, va_list);
double error_solve_T_profile(double Tsurf, ...);
double estimate_dew_point(double, double, double, double, double);
#if QUICK_FS
int estimate_layer_ice_content(layer_data_struct *, double *, double *,
			       double *, double ***, double *,
			       double *, double ***,
#if SPATIAL_FROST
			       double *, double,
#endif // SPATIAL_FROST
			       int, int, char);
#else
int estimate_layer_ice_content(layer_data_struct *, double *, double *,
			       double *, double *, double *, double *,
			       double *, double *, double *,
#if SPATIAL_FROST
			       double *, double,
#endif // SPATIAL_FROST
#if EXCESS_ICE
			       double *, double *,
#endif // EXCESS_ICE
			       int, int, char);
#endif
int estimate_layer_ice_content_quick_flux(
                   const double            *depth,
                   const double             Dp,
                   const double             Tsurf,
                   const double             T1,
                   const double             Tp,
                   const double            *max_moist,
#if QUICK_FS
                   const double          ***ufwc_table_layer,
#else
                   const double            *expt,
                   const double            *bubble,
#endif // QUICK_FS
#if SPATIAL_FROST
                   const double            *frost_fract,
                   const double             frost_slope,
#endif // SPATIAL_FROST
#if EXCESS_ICE
                   const double            *porosity,
                   const double            *effective_porosity,
#endif // EXCESS_ICE
                   const char               FS_ACTIVE,
                   layer_data_struct *layer);
double estimate_T1(double, double, double, double, double, double, double,
		   double, double, double, double);
double exp_interp(double,double,double,double,double);

double f(double, double, double, double, double, double, double, double,
         double, double, int, double *, double, double, double, double *,
         double *, double *, double *, double *, double *);
void   fda_heat_eqn(double *, double *, int, int, ...);
void   fdjac3(double *, double *, double *, double *, double *,
            void (*vecfunc)(double *, double *, int, int, ...),
            int);
void   find_0_degree_fronts(const double *, const double *, const int, energy_bal_struct *);
//160511LML layer_data_struct find_average_layer(layer_data_struct *, layer_data_struct *,
//				     double, double);
bool calc_average_layer_moisture(const layer_data_struct &wet,
                     const layer_data_struct &dry, const double mu,
                     layer_data_struct *avg_layer);                              //160511LML
void   find_sublayer_temperatures(layer_data_struct *, double *, double *,
				  double *, double, double, int, int);
void   free_atmos(const int nrecs, atmos_data_struct *atmos);
void   free_dist_prcp(dist_prcp_struct *, int);
void   free_dmy(dmy_struct **dmy);
void   free_vegcon(veg_con_struct **);
void   free_veglib(veg_lib_struct **);
void   free_out_data_files(out_data_file_struct **);
void   free_out_data(out_data_struct **);
int    full_energy(char, int, int, atmos_data_struct *, dist_prcp_struct *,
                   dmy_struct *, global_param_struct *, const lake_con_struct *,
                   soil_con_struct *, veg_con_struct *
#ifdef VIC_CROPSYST_VERSION
            #if VIC_CROPSYST_VERSION==2
            ,crop_data_struct * //keyvan added crop_data_struct
            #endif
            #ifndef FULL_IRRIGATION
            //, irrigation_pattern_struct * //keyvan 130604
            ,std::map<int,std::map<int,irrigation_pattern_struct>> &
            #endif
#endif
                   );
double func_aero_resist(double,double,double,double,double);
double func_atmos_energy_bal(double Tcanopy, va_list ap);
double func_atmos_moist_bal(double VPcanopy, va_list ap);
double func_canopy_energy_bal(double Tfoliage, va_list ap);
double func_snow_ground_flux(double, va_list);
double func_surf_energy_bal(double, va_list);

double get_avg_temp(double, double, double *, double *, int);
double get_dist(double, double, double, double);
void   get_force_type(char *, int, int *);
global_param_struct get_global_param(filenames_struct *, FILE *);
void   get_next_time_step(int *, int *, int *, int *, int *, int);

double hermint(const double, const int, const double *, const double *,
               const double *, const double *, const double *);
void   hermite(const int, const double *, const double *, const double *, double *, double *);
void   HourlyT(const int, const int, const int *, const double *, const int *, const double *, double *);

void   init_output_list(out_data_struct *, int, char *, int, float);
void   initialize_atmos(atmos_data_struct *, dmy_struct *, FILE **,
#if OUTPUT_FORCE
			soil_con_struct *, out_data_file_struct *, out_data_struct *);
#else
			soil_con_struct *);
#endif

/*170412LML moved to VCS_Nl.h
#ifdef VIC_CROPSYST_VERSION
void initialize_crop(crop_data_struct *); // **08172010 KJC *
#endif
*/

void   initialize_global();
int   initialize_model_state(dist_prcp_struct *, dmy_struct,
                  filep_struct,
                  int, int, int,
			      double, soil_con_struct *,
                              veg_con_struct *, lake_con_struct,
			      char **, int **);
int    initialize_new_storm(cell_data_struct ***, veg_var_struct ***,
			    int, int, int, double, double);
void   initialize_snow(snow_data_struct **, int, int);
void   initialize_soil(cell_data_struct **, soil_con_struct *, veg_con_struct *, int);
void   initialize_veg( veg_var_struct **, veg_con_struct *,
                       int
                       //150929LML #if VIC_CROPSYST_VERSION>=3
                       //150929LML ,crop_data_struct    ***
                       //150929LML #endif
                       #if VIC_CROPSYST_VERSION>=3
                       ,soil_con_struct *                                        //150929LML
                       //150930LML ,Grid_cell_crop_output *                                  //150929LML
                       #endif
                       );

void   latent_heat_from_snow(double, double, double, double, double,
                             double, double, double *, double *,
                             double *, double *, double *);
double linear_interp(double,double,double,double,double);

cell_data_struct **make_cell_data(int, int);
dist_prcp_struct make_dist_prcp(int
                                //150929LML #if VIC_CROPSYST_VERSION>=3
                                //150929LML ,veg_con_struct *
                                //150929LML #endif
                                );
dmy_struct *make_dmy();
energy_bal_struct **make_energy_bal(int);
void make_in_and_outfiles(filep_struct *, filenames_struct *,
			  soil_con_struct *, out_data_file_struct *);
out_data_struct *make_out_data(int);
snow_data_struct **make_snow_data(int);
veg_var_struct **make_veg_var(int);
void   MassRelease(double *,double *,double *,double *);
double maximum_unfrozen_water(const double T, const double max_moist
                              #if QUICK_FS
                              ,double **table
                              #else
                              ,const double bubble, const double expt
                              #endif
                              );
double calc_ice_content(const double moist, const double temperature, const double max_moist,
                        const double bubble, const double expt);               //160516LML
double modify_Ksat(double);
void mtclim_wrapper(int, int, double, double, double, double,
                      double, double, double, double,
                      int, dmy_struct *, double *,
                      double *, double *, double *, double *, double *
                    #if defined(VIC_CROPSYST_VERSION)
                                  ,double *
                    #endif
                    );

double new_snow_density(double);
int    newt_raph(void (*vecfunc)(double *, double *, int, int, ...),
               double *, int);

FILE  *open_file(char string[], char type[]);
FILE  *open_state_file(filenames_struct);

void parse_output_info(filenames_struct *, FILE *, out_data_file_struct **, out_data_struct *);
double penman(double tair, double elevation, double rad, double vpd, double ra, double rc, double rarc);
void   prepare_full_energy(int, int, int, dist_prcp_struct *,
			   soil_con_struct *, double *, double *);
double priestley(double, double);
int    put_data(dist_prcp_struct *, atmos_data_struct *,
		soil_con_struct *, veg_con_struct *,
                lake_con_struct *, out_data_file_struct *,
		out_data_struct *, save_data_struct *,
 	        dmy_struct *,
#if VIC_CROPSYST_VERSION==2
               crop_data_struct *,
#endif
 	        int
 	        );

double read_arcinfo_value(char *, double, double);
int    read_arcinfo_info(char *, double **, double **, int **);
void   read_atmos_data(FILE *, int, int, double **);
double **read_forcing_data(FILE **);
void   read_initial_model_state(FILE *, dist_prcp_struct *,
                int, int, int,
                soil_con_struct *, char *,
				int *, lake_con_struct);
void   read_snowband(FILE *, soil_con_struct *);
void   read_snowmodel(atmos_data_struct *, FILE *, int, int, int, int);
soil_con_struct read_soilparam(FILE *, char *, int *, char *, char *);
soil_con_struct read_soilparam_arc(FILE *, char *, int *, char *, int);
veg_lib_struct *read_veglib(FILE *, int *);
veg_con_struct *read_vegparam(FILE *, int, int);
/*170412LML moved to VCS_Nl.h
#if (VIC_CROPSYST_VERSION>=3)
#ifdef USE_IRRIGATION_PARAM_FILE
void read_irrigparam(FILE *irrigparam, const int gridcel, Irrigation_Types_In_Each_Cell &irrigation_list);
#endif
bool find_irrigation_type(const Irrigation_Types_In_Each_Cell &irrig_lib,
                          const int crop_code, Irrigation_Type &irrigation_type, bool &full_irrigation);
CropCodeLib *read_cropcodelib(FILE *, int&);                                     //161102LML
#endif
*/
/*170412LML moved to VCS_Nl.h
#if (VIC_CROPSYST_VERSION==2)
void free_croplib(crop_lib_struct **);
#endif
*/
//LML 150413 #if (VIC_CROPSYST_VERSION>=3)
//LML 150413 crop_rotation_lib_struct *read_crop_rotation_lib(FILE *, int *);
//LML 150413 #endif
/*170412LML moved to VCS_Nl.h
#ifdef VIC_CROPSYST_VERSION
#if VIC_CROPSYST_VERSION==2
crop_data_struct *read_cropparam(FILE *, int );
#else
crop_data_struct *provide_cropparam(const veg_con_struct *veg_con_array, const int gridcel);
#endif
void free_crop(crop_data_struct *);
#endif
*/

//LML 150413 #if (VIC_CROPSYST_VERSION>=3)
//LML 150413 void free_crop_rotation_lib(crop_rotation_lib_struct **);
//LML 150413 #endif
int    redistribute_during_storm(cell_data_struct ***, veg_var_struct ***,
                 int, int, int, double, double, double, double *
                 #if VIC_CROPSYST_VERSION>=3
                 ,const int is_crop                                               //LML 150428 array
                 #endif
                 );
void   redistribute_moisture(layer_data_struct *, double *, double *,
			     double *, double *, double *, int);
unsigned char redistribute_moisture_for_storm(double *, double *, double,
                          double, double);
double root_brent(double, double, char *, double (*Function)(double, va_list), ...);
int    runoff(cell_data_struct *, cell_data_struct *,
              energy_bal_struct *, soil_con_struct *, double *,
              #if EXCESS_ICE
              int,
              #endif
              #if SPATIAL_FROST
              double *,
              #endif
              double, int, int, int
              #if (VIC_CROPSYST_VERSION>=3)
              ,veg_var_struct *
              ,veg_var_struct *
              ,bool
              #endif
              );
double get_total_soil_moisture(const layer_data_struct* vic_layer,
                               int from_top_layer = 0,
                               int to_bottom_layer = options.Nlayer-1);          //(mm) 150707LML
double get_total_soil_thickness_mm(const soil_con_struct *soil, int from_top_layer = 0,
                                   int to_bottom_layer = options.Nlayer-1);          //(mm) 150714LML
double get_total_transpiration(const cell_data_struct &cell);                    //150707LML
double cal_baseflow(const double rel_moist_bottom_layer,const soil_con_struct &soil_con); //160510LML
bool cascade_saterated_moisture_from_certain_layer(
    const int from_layer, const double *max_moist,
    double *Q12, double *liq, double *ice, double *runoff);                      //160511LMLbool drainage_between_upper_layers(const soil_con_struct &soil_con, const double *min_liq, const double *liq,
bool drainage_between_upper_layers(const soil_con_struct &soil_con, const double *min_liq, const double *liq,
                                   const double evap[][FROST_SUBAREAS], const int frost_area, double *Q12); //160511LML
void set_max_min_hour(const double *, const int, int *, int *);
void set_node_parameters(double *, double *, double *, double *, double *, double *,
                         double *, double *, double *, double *, double *,
                         double *, double *,
#if QUICK_FS
			 double ***,
#endif
#if EXCESS_ICE
			 double *, double *, double *, double *,
#endif
			 int, int, char);
out_data_file_struct *set_output_defaults(out_data_struct *);
int set_output_var(out_data_file_struct *, int, int, out_data_struct *, char *, int, char *, int, float);
double snow_albedo(double, double, double, double, double, double, int, char);
double snow_density(snow_data_struct *, double, double, double, double, double);
int    snow_intercept(double, double, double, double, double, double,
                      double, double, double, double, double, double,
                      double *, double *, double *, double *, double *,
                      double *, double *, double *, double *, double *,
                      double *, double *, double *, double *, double *,
                      double *, char *, int *, double *, double *, double *,
                      double *, double *, double *, float *,
                      int, int, int, int, int, int, int, int,
                      double *dryFrac, atmos_data_struct *, layer_data_struct *,
                      layer_data_struct *, soil_con_struct *,
                      veg_var_struct *, veg_var_struct *
#ifdef VIC_CROPSYST_VERSION  //keyvan 130212
                      #if (VIC_CROPSYST_VERSION>=3)
                      ,veg_con_struct *
                      ,double                                                    //150624LML
                      #endif
                      //150929LML ,crop_data_struct *
                      ,const dmy_struct*
                      ,cell_data_struct *
                      ,cell_data_struct *                                        //150929LML
                      ,int
                      ,double **
#endif
                      );
int    snow_melt(double, double, double, double, double *, double, double *, double,
		 double, double, double, double, double, double, double,
                 double, double, double, double, double, double,
                 double *, double *, double *, double *, double *, double *,
                 double *, double *, double *, double *, double *, double *,
                 int, int, int, int, snow_data_struct *, soil_con_struct *);
double SnowPackEnergyBalance(double, va_list);
double soil_conductivity(const double moist, const double ice, const double soil_dens_min,
                         const double bulk_dens_min, const double quartz,
                         const double soil_density, const double bulk_den, const double organic);
/*void   soil_thermal_calc(soil_con_struct *, layer_data_struct *,
			 energy_bal_struct, double *, double *, double *,
			 int, int);*/
double soil_thermal_eqn(double, va_list);
double solve_snow(char, double, double, double, double, double, double,
                  double, double, double, double, double, double,
                  double *, double *, double *, double *, double *,
                  double *, double *, double *, double *, double *,
                  double *, double *, double *, double *, double *, double *,
                  double *, double *, double *, double *, double *, double *,
                  double *, double *, double *, double *, double *, double *,
                  float *, int, int, int, int, int, int, int, int, int, int *,
                  double *, const dmy_struct *, atmos_data_struct *, energy_bal_struct *,
                  layer_data_struct *, layer_data_struct *,
                  snow_data_struct *, soil_con_struct *,
                  veg_var_struct *, veg_var_struct *
#ifdef VIC_CROPSYST_VERSION
                  #if (VIC_CROPSYST_VERSION>=3)
                  ,veg_con_struct *
                  ,double                                                        //150624LML
                  #endif
                  ,cell_data_struct * //keyvan 130218
                  ,cell_data_struct *                                            //150929LML
                  //150929, crop_data_struct * //keyvan 130212
                  ,int
                  ,double **
#endif
                  );
double solve_atmos_energy_bal(double Tcanopy, ...);
#ifdef __BCPLUSPLUS__
//190130RLN I this is what is actually needed
double solve_atmos_moist_bal(double VPcanopy,...);
#else
double solve_atmos_moist_bal(double VPcanopy);
#endif
double solve_canopy_energy_bal(double Tfoliage, ...);
double solve_snow_ground_flux(double Tsurf, ...);
double solve_surf_energy_bal(double Tsurf, ...);
#if QUICK_FS
int    solve_T_profile(double *, double *, char *, int *, double *, double *,double *,
		       double *, double, double *, double *, double *,
		       double *, double *, double *, double *, double, double *, double ***,
		       int, int *, int, int, int, int);
#else
int    solve_T_profile(double *, double *, char *, int *, double *, double *,double *,
		       double *, double, double *, double *, double *,
		       double *, double *, double *, double *, double, double *,
#if EXCESS_ICE
		       double *, double *,
#endif
		       int, int *, int, int, int, int);

#endif
int   solve_T_profile_implicit(double *, double *, double *, double *, double *,
			       double *, double, double *, double *, double *,
#if EXCESS_ICE
			       double *, double *,
#endif
			       double *, double *, double *, double *, double, int, int *,
			       int, int, int, int,
			       double *, double *, double *, double *, double *, double *, double *);
double StabilityCorrection(double, double, double, double, double, double);
int surface_fluxes(char, double, double, double, double,
                   #if EXCESS_ICE
                   int, double *, double *,
                   #endif
                   double, double, double *, double *, double **,
                   double *, double *, double *, double *,
                   double *, double *, double *, double *, double *,
                   float *, int, int, int, int, int,
                   int, int, int, int, atmos_data_struct *, dmy_struct *,
                   energy_bal_struct *, global_param_struct *,
                   cell_data_struct *, cell_data_struct *,
                   snow_data_struct *, soil_con_struct *,
                   veg_var_struct *, veg_var_struct *, float, float, float
                   #ifdef VIC_CROPSYST_VERSION
                   #if (VIC_CROPSYST_VERSION>=3)
                   ,veg_con_struct *
                   #endif
                   #ifndef FULL_IRRIGATION
                   ,irrigation_pattern_struct *
                   #endif
                   #endif
                   );

double svp(double);
double svp_slope(double);
void transpiration(layer_data_struct *
                    , const soil_con_struct&
                    ,const veg_lib_struct&
                    , int
                    , double
                    , double
                    , double
                    , double
                    , double
                    , double
                    , double
                    , double
                    ,double *
                    #if SPATIAL_FROST
                    ,double *
                    #endif
                    ,const float *
                    );
double cal_liquid_moist_for_certain_layer(const layer_data_struct &layer
                        #if SPATIAL_FROST
                        ,double *frost_fract
                        #endif
                        );                                                       //160510LML
double cal_moist_stress_factor(const double liq_moist
                               ,const int layer_index
                               ,const soil_con_struct &soil_con
                               );                                                //160510LML
bool is_approximately(const double &value,const double &target,const double tolerance=0.0000007); //160510LML
void tridag(double *,double *,double *,double *,double *,int);
void tridiag(double *, double *, double *, double *, unsigned);
int update_thermal_nodes(dist_prcp_struct *,
			  int, int, int, soil_con_struct *, veg_con_struct *);
void usage(char *);

void   vicerror(char *);
double volumetric_heat_capacity(double,double,double,double);

void wrap_compute_zwt(const soil_con_struct *, cell_data_struct *);
void write_data(out_data_file_struct *, out_data_struct *, dmy_struct *, int);
void write_dist_prcp(dist_prcp_struct *);
#if OUTPUT_FORCE
void write_forcing_file(atmos_data_struct *, int, out_data_file_struct *, out_data_struct *);
#endif
void write_header(out_data_file_struct *, out_data_struct *, dmy_struct *);
void write_layer(layer_data_struct *, int, int,
#if SPATIAL_FROST
                 double *,
#endif
                 double *);
void write_model_state(dist_prcp_struct *, int,
		       int, filep_struct *, soil_con_struct *, char *,
		       int *, lake_con_struct);
void write_snow_data(const snow_data_struct&, const int, const int);
void write_soilparam(const soil_con_struct&);
void write_vegparam(const veg_con_struct*);
void write_vegvar(const veg_var_struct&, int);

void zero_output_list(out_data_struct *);
/*Crop model functions */
/*170412LML moved to VCS_Nl.h
#ifdef VIC_CROPSYST_VERSION
#if (VIC_CROPSYST_VERSION>=3)
#ifndef  __BCPLUSPLUS__
int find_index_in_sorted_vector(const std::vector<int> &data,const int key, int imin, int imax);
int is_cropping_type_exist(const int cropping_type);              //LML 141020 NIY
int get_current_veg_type(const int veg_class, const dmy_struct *dmy);    //LML 141015
int get_crop_counter(const veg_con_struct *veg_con, const int target_cropping_code);
int get_veg_lib_index(const int veg_class_code);
void free_cropcodelib(CropCodeLib **);
#ifdef DEBUG_SPECIFIC_VEG_BAND
void print_state_flux_specific_veg_band(int dist,
                                        double mu,
                                        int band,
                                        #ifdef CHECK_WATER_BALANCE
                                        double init_total_water_mm,
                                        #endif
                                        const veg_lib_struct &veglib_rec,
                                        const cell_data_struct *cell,
                                        const snow_data_struct *snow,
                                        const veg_var_struct   *veg_var,
                                        const veg_con_struct   *veg_con,
                                        //150929LML const crop_data_struct *crop,
                                        const soil_con_struct  *soil,
                                        const atmos_data_struct *atmos);
#endif
#endif
#endif //(VIC_CROPSYST_VERSION>=3)

int get_crop_lib_index(int veg_class_code);      //LML 141015
int iscrop(int veg_class_code_in_veg_con_file);
bool IsRotationWithMultipleCycles(int veg_class_code_in_veg_con_file);           //LML 150526
void DecomposeVegclassCode(int veg_class_code_runtime,
                           int &rotation_or_crop_veg_class_code,
                           int &rotation_cycle_index);                           //LML 150526

#if (VIC_CROPSYST_VERSION == 2)
double return_MAD(int veg_class_code);
//150413RLN in V3 we get MAD from irrigation paramters
//Use VIC_land_unit_get_irrigation_parameters()
#endif
//double VIC_calc_ET_pot(double ET_reference_crop);
//double calc_pot_evap(double);
//double today_is_irrigation(double MAD_crop_specific);

//double VIC_calc_soil_evaporation_actual_mm(
  //                                                double soil_evap_pot_mm,
  //                                                layer_data_struct *layer_wet);

double evaporation_from_irrigation_systems(double ET0, double ET0_open_water, int irrigation_index, double crop_h = 0.5);
//150702LML void   clear_crop_water_fluxes(crop_data_struct &current_crop);
void clear_cell_irrigation_water_fluxes(cell_data_struct *current_cell);         //150702LML
// * Crop model functions *
int iscrop(int veg_class);
#if (FULL_IRRIGATION==FALSE)
void alloc_irrig_pattern(int, irrigation_pattern_struct **,filep_struct *, filenames_struct *,
			  soil_con_struct *);
double *read_irrig_pattern(FILE *, irrigation_pattern_struct*, int nrecs);

#endif
CO2_conc_struct *read_CO2_conc(FILE *);
double solve_penman_for_T (double, va_list);
double solve_wet_bulb(double, va_list);
#endif
*/


#ifdef __cplusplus
 }
#endif


