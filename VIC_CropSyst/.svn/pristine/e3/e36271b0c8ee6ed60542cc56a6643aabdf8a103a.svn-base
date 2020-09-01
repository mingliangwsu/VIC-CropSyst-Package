/******************************************************************************
// $Id: LAKE.h,v 5.9.2.16 2011/03/07 05:14:55 vicadmin Exp $
  Modifications:
  2006-Nov-07 Removed LAKE_MODEL option.				TJB
  2007-Aug-16 Made return value of initialize_prcp an int.		JCA
  2007-Aug-21 Added features for EXCESS_ICE option.			JCA
  2007-Oct-24 Changed get_sarea, get_volume, and get_depth to return exit
	      status so that errors can be trapped and communicated up the
	      chain of function calls.					KAC via TJB
  2007-Oct-24 Changed lakeice() to return exit status.			KAC via TJB
  2007-Nov-06 New lake physics parameters.  Modified argument lists for
	      various functions.  Moved get_dist() to vicNl.h.		LCB via TJB
  2008-Apr-21 Added argument to alblake.				LCB via TJB
  2008-Sep-10 Updated values of CONDS and lamwlw to match Laura
	      Bowling's lake work.					LCB via TJB
  2009-Jul-31 Removed lakemain() and wetland_energy(); initialize_lake
	      no longer takes a snow structure as input.		TJB
  2009-Sep-28 Removed initialize_prcp and update_prcp.  Modified
	      argument list of initialize_lake.				TJB
  2009-Sep-30 Miscellaneous fixes for lake model.			TJB
  2009-Oct-05 Added functions for updating/rescaling lake and wetland
	      fluxes and storages when lake area changes.		TJB
  2010-Nov-11 Added skip_hydro flag to initialize_lake() arg list.
	      Removed rescale_lake_fluxes().				TJB
  2010-Nov-26 Changed the argument list of water_balance().		TJB
  2010-Dec-28 Added latitude to alblake() arglist.			TJB
  2011-Mar-01 Added rescale_snow_storage().  Added terms to argument
	      list of initialize_lake().				TJB
******************************************************************************/

#ifndef LAKE_H
#define LAKE_H

#define LAKE_SET
#define TMELT 0.0
#define EMICE 0.97      /* Ice emissivity */
#define EMH2O .98
#define RHOSNOW   250.  /* densities of water and snow */
#define RHOICE   917.   /* ice density*/
#define rhosurf 1.275   /* surface air density */
#define MAX_SURFACE_LAKE   .6  /* max. surface layer thickness for E-B (m) */
#define BETA 0.001       /* Curve shape parameter for lake profile. */
#define FRACMIN  0.10   /* min ice thickness in meters */
#define FRACLIM   0.02  /* lower limit on fractional ice cover */
#define CPW_ICE   4200. /* specific heat of ice */
#define DM 1.38889E-07  /* molecular diffusivity of water */
#define SNOWCRIT   0.05  /* for albedo, in m */
//#define G 9.80616
#define ZWATER 0.0045    // 0.004 - original value
#define ZSNOW 0.005
#define CONDI 2.3        /* thermal conductivity of ice */
#define CONDS 0.7       /* thermal conductivity of snow */ 

// attenuation of short and longwave radiation through ice (1/m)
#define lamisw 1.5 // 1.5 in Patterson & Hamblin
#define lamilw 20  // 20.0 in Patterson & Hamblin
// attenuation of short and longwave radiation through snow (1/m)
#define lamssw 6.0 // 6.0 in Patterson & Hamblin
#define lamslw 20  // 20.0 in Patterson & Hamblin
// attenuation of short and longwave radiation through water (1/m)
#define lamwsw .3  // San Fran Bay data: 0.31 - 29.9 1/m (visible)
#define lamwlw 1.4 // Hostetler and Bartlein assume 0.85 1/m (total)
#define PERCENT_RADIATION_VISIBLE 0.7        /* Percent of radiation in visible band. */
#define PERCENT_RADIATION_INFRARED 0.3        /* Percent of radiation in infrared band. */
#define QWTAU 86400./2.   /* D. Pollard sub-ice time constant. */
#define RADIUS 6371.228 /* Earth radius in km. */

//#endif // LAKE_SET

/*** Subroutine prototypes ***/

#ifdef __cplusplus
extern "C" {
#endif

double adjflux(double, double, double ,double, double, double, double,
	       double, double, double, double *, double *);
void advect_soil_veg_storage(const double lakefrac,
                             const double max_newfraction,
                             const double newfraction,
                             const soil_con_struct  &soil_con,
                             const veg_con_struct   &veg_con,
                             const lake_con_struct  &lake_con,
                             cell_data_struct *cell,
                             veg_var_struct   *veg_var,
                             double *delta_moist);
void advect_snow_storage(const double, const double, const double, snow_data_struct *);
void alblake(const double Tcutoff, const double Tair, const double newsnow, const double coldcontent,
          const int dt, const double swq, const double depth, const int day_in_year, const double latitude,
          double &snowalbedo, double &albs, float &albi, float &albw, int &last_snow, char &MELTING);
void alloc_atmos(int, atmos_data_struct **);
double calc_density(const double);
double CalcIcePackEnergyBalance(double Tsurf, ...);
void colavg(const double *T,const double *Ti,const float lakeprv,const int numnod,
            const double dz, const double surfdz, double *finaltemp, double *density);
float dragcoeff(float, double, double);
void eddy (const int freezeflag, const double wind, const double *vic_water_density,
           const double lat, const int numnod, const double dz, const double surfdz, double *de);
void energycalc(const double *finaltemp, const int numnod, const double dz, const double surfdz,
                const double *surface, const double *cp, const double *density, double &sumjoule);
double ErrorIcePackEnergyBalance(double Tsurf, ...);
double ErrorPrintIcePackEnergyBalance(double, va_list);
int get_depth(lake_con_struct, double, double *);
int get_depth_from_sarea(lake_con_struct, double, double *);
int get_sarea(lake_con_struct, double, double *);
int get_volume(lake_con_struct, double, double *);
void iceform(const double Tcutoff, const double fracprv, const int numnod, const int dt,
             const double dz, const double  surfdz, const double *cp, const double *surface,
             const double  lvolume, double *T, double &qfusion, double &areaadd, double &new_ice_height,
             double &new_ice_water_eq
             );
void icerad(const double  sw, const double  hi, const double  hs,
            double &avgcond, double &SWnet, double &SW_under_ice);
int ice_depth(lake_con_struct, double, double, double *);
int ice_melt(double, double, double *, double, snow_data_struct *, lake_var_struct *, int, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double *, double *, double *, double *, double *, double *, double *, double *, double *, double);
double IceEnergyBalance(double, va_list);
int initialize_lake(lake_var_struct *, lake_con_struct, const soil_con_struct *, cell_data_struct *, double, int);
int lakeice(const double sw_ice, const double fracice, const int dt, const double snowflux,
      const double qw, const double SWabsorbed, const double volume, const double sarea,
      double &energy_ice_melt_bot, double &qf, double &ice_water_eq);
void latsens (const double Tsurf, const double Tcutk, const double hice, const double tair, const double wind,
              const double pressure, const double vp, const double air_density, const double wind_h,
              double &evap, double &qsen);
float calc_lake_drag(const float Tsurf, const double Tair, const double wind, const double roughness, const double Z1);
lake_con_struct read_lakeparam(FILE *, soil_con_struct, veg_con_struct *);
void rescale_soil_veg_fluxes(const double, const double, cell_data_struct *, veg_var_struct *);
void rescale_snow_energy_fluxes(const double, const double, snow_data_struct&, energy_bal_struct&);
void rescale_snow_storage(const double, const double, snow_data_struct *);
void rhoinit(double,double&);
int solve_lake(double, double, double, double, double, double, double, double, 
		double, double, lake_var_struct *, lake_con_struct, 
		soil_con_struct, int, int, double, dmy_struct, double);
double specheat(const double);
void temp_area(const double sw_visible, const double sw_nir, const double surface_force,
               const double *T, const double *vic_water_density, const double *de,
               const int dt, const double *surface, const int numnod, const double dz, const double surfdz, const double *cp,
               double &temph, double &energy_out_bottom, double *Tnew);
void tracer_mixer (const int freezeflag, const double *surface, const int numnod,
                   const double  dz, const double surfdz, const double *cp, double *T, int &mixdepth);
void tridia(const int ne, const double *a, const double *b, const double *c, const double *y, double *x);
int water_balance(const lake_con_struct &lake_con, const int dt,
            const int rec, const int iveg,const int band, const double lakefrac, const soil_con_struct &soil_con,
            #if EXCESS_ICE
            int SubsidenceUpdate, double total_meltwater,
            #endif
            dist_prcp_struct *prcp, const veg_con_struct &veg_con, lake_var_struct &lake);
int  water_energy_balance(int, double*, double*, int, int, double, double, double, double, double, double, double, double, double, double, double, double, double, double *, double *, double *, double*, double *, double *, double *, double, double *, double *, double *, double *, double *, double);
int water_under_ice(const int     freezeflag, const double  sw_ice, const double  wind,
    double *Ti, double *vic_water_density, const double  lat, const int numnod,
    const double  dz, const double  surfdz, const double  Tcutoff, double &qw,
    double *surface, double &deltaH, double *water_cp,
    const double  hice, const double  sdepth, const double  dt, int &mixdepth, double &energy_out_bottom);

#ifdef __cplusplus
}
#endif
#endif
