#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vicNl.h>

static char vcid[] = "$Id: lakes.eb.c,v 5.11.2.46 2012/02/08 00:56:32 vicadmin Exp $";

int solve_lake(double             snowfall,
	       double             rainfall,
	       double             tair, 
	       double             wind, 
	       double             vp, 
	       double             shortin, 
	       double             longin, 
	       double             vpd, 
	       double             pressure, 
	       double             air_density, 
	       lake_var_struct   *lake, 
	       lake_con_struct    lake_con, 
	       soil_con_struct    soil_con, 
	       int                dt, 
	       int                rec, 
	       double             wind_h,
	       dmy_struct         dmy,
	       double             fracprv)
{

/**********************************************************************
* This subroutine solves the energy budget for open water bodies.
*
* Parameter description :
*
  lat  	Latitiude of the lake (degrees).
  lng 	Longitude of the lake (degrees).
  tair	Air temperature (C).
  vp	Air vapor pressure (kPa).
  pressure      	Air pressure (kPa).
  wind		Wind speed (m/s).
  longwave		Downwelling long wave radiation (W/m2).
  shortwave		Incoming short wave radiation (W/m2).
  prec	         Precipitation (mm).
  lake_energy.latent	 Latent heat flux (W/m2).
  lake_energy.sensible	 Sensible heat flux (W/m2).
  lake_con.eta_a	 Decline of solar radiation input with depth (m-1).  
  lake_con.surface[numnod]	Area of the lake (m2).
  lake.temp[numnod]	Temperature of the lake water (C).
  lake.tempi                Temperature of the lake ice (C).
  lake.hice      	         Height of the lake ice (m).
  lake.fraci	         Fractional coverage of ice (-).
  lake_snow.depth	         Height of the snow on top of the lake (m).
  lake.numnod        	Number of nodes in the lake (-).
  dt		         Time step size (hrs).

  Modifications:
  2006-Oct-16 Now set mixdepth=0 for case of complete ice cover; this
	      guarantees that it is initialized for all cases.		TJB
  2006-Nov-15 Convert swq and surf_water from mm over lake to mm over ice
	      fraction at beginning of function; this was needed to avoid
	      a water budget error since swq and surf_water were being
	      converted to mm over lake at end of the function.		TJB
  2007-Apr-21 Added initialization of energy_ice_melt_bot and qf for case
	      in which fracprv >= FRACLIM but hice == 0.0.		TJB
  2007-Apr-23 Added initialization of lake_energy->Tsurf.		TJB
  2007-Nov-06 Lake snow physics now consistent with land snow pack.
	      Lake ice now limited by available lake water.		LCB via TJB
  2008-Apr-21 Corrected omissions in previous mods.  Also added code to
	      handle previously-unhandled case in deltaH computation.	TJB
  2008-Oct-23 Fixed problems with usage of uninitialized variables.	LCB via TJB
  2008-Oct-23 Deleted call to ice_depth() in section 11, since it
	      wasn't working correctly and wasn't necessary.  Now,
	      the reported ice depth is the average over the lake.	LCB via TJB
  2009-Jul-31 Removed lakemain().					TJB
  2009-Sep-28 Removed initialize_prcp() and update_prcp().  Changed
	      final units of runoff_out, baseflow_out, and evapw to
	      mm over lake (to be consistent with changes in put_data).	TJB
  2009-Nov-09 Handle lake snow when lake ice disappears.		LCB via TJB
  2010-Nov-02 Add condition of ice area > 0 to calculation of hice.	TJB
  2010-Nov-21 Added lake->volume_save and lake->swe_save.  Added storing
	      of lake snow variables over lake ice area in the lake_var
	      structure.						TJB
  2010-Nov-26 Added lake->ice_throughfall to store rainfall (over ice
	      and open water) and snowfall, so that we can wait to add
	      these to the lake volume until water_balance().		TJB
  2010-Nov-26 Added storing of lake snow variables (in depth over lake
	      ice area) to the lake_var structure.			TJB
  2010-Dec-28 Added latitude to arglist of alblake().			TJB
  2011-Mar-01 Changed units of snow and energy fluxes to be over entire
	      lake; this fixes several water/energy balance errors.	TJB
  2011-Jun-03 Added options.ORGANIC_FRACT.  Soil properties now take
	      organic fraction into account.				TJB
  2011-Sep-22 Added logic to handle lake snow cover extent.			TJB
**********************************************************************/

  double LWnetw,LWneti;
  double sw_water, sw_ice;
  double T[MAX_LAKE_NODES];   /* temp of the water column, open fraction. */
  double Tnew[MAX_LAKE_NODES];  
  double Ti[MAX_LAKE_NODES];   /* temp of the water column, ice fraction. */
  double vic_water_density[MAX_LAKE_NODES], water_cp[MAX_LAKE_NODES];
  float albi, albw;
  double Ts, tempalbs;
  double Tcutoff, Tcutk;  /* Lake freezing temperature (K). */
  double Qhw, Qhi;
  double Qgw, Qgi;
  double Qew, Qei;
  double eflux, eadd;
  double Tki, Tkw;     /* Surface temp. of ice and water in Kelvin. */
  double qbot, qw;
  int freezeflag;
  int mixdepth;
  double new_ice_area; /* Ice area formed by freezing in the open water portion. */
  int k, i, ErrorFlag;
  double tw1, tw2, r1, r2, rtu, rnet;
  double Ls, Le;
  double sumjoula, sumjoulb, sumjouli;
  double temp;
  double water_energy;
  double temphw, temphi;
  double SW_under_ice;
  int last_snow;
  double cc;
  double ice_energy;
  double error_over_water;
  double sw_underice_visible;
  double sw_underice_nir;
  double T_lower, T_upper;
  double Tsurf;
  double new_ice_height;
  double windw, windi;
  double energy_ice_formation;
  double energy_ice_melt_bot;
  double melt, deltaCC_ice, Qnet_ice;
  double joule_intermediate;
  double energy_out_bottom;
  double energy_out_bottom_ice;    
  double sw_out;
  int index;
  double tempdepth, in;
  double qf;
  double inputs, outputs, internal, phasechange;
  double new_ice_water_eq;
  double temp_refreeze_energy;
  energy_bal_struct *lake_energy;
  snow_data_struct  *lake_snow;
  
  /**********************************************************************
   * 1. Initialize variables.
   **********************************************************************/

  lake->sarea_save = lake->sarea;
  lake->volume_save = lake->volume;
  lake->swe_save = lake->swe;
 
  lake_energy = &(lake->energy);
  lake_snow = &(lake->snow);

  lake_energy->advection=0.0;
  lake_energy->deltaCC = 0.0;
  lake_energy->grnd_flux = 0.0;
  lake_energy->snow_flux = 0.0;
  lake->snowmlt = 0.0;
  qbot=qw=0.0;
  new_ice_height = new_ice_area = new_ice_water_eq = 0.0;
  lake->evapw=0.0;
  energy_ice_formation = 0.0;
  energy_out_bottom = energy_out_bottom_ice = 0.0;
  lake_snow->vapor_flux=0.0;
  lake->vapor_flux=0.0;
  lake_energy->Tsurf = lake->temp[0];
  temp_refreeze_energy = 0.0;
  lake->ice_throughfall = 0.0;

  if(lake->activenod > 0 || lake->areai > 0.0) {

    /* --------------------------------------------------------------------
     * Calculate the water freezing point.
     * ------------------------------------------------------------------- */
    rhoinit(pressure,Tcutoff);          /* degrees C */
   /* --------------------------------------------------------------------
     * Initialize liquid water and ice temperature profiles to be used
     * later in the module.
     * -------------------------------------------------------------------- */

    for ( k = 0; k < lake->activenod; k++ ) {
      T[k] = lake->temp[k];
      Ti[k] = lake->temp[k];
      vic_water_density[k] = calc_density(T[k]);
      water_cp[k] = specheat(T[k]);
    }

    energycalc( lake->temp, lake->activenod, lake->dz,
        lake->surfdz,lake->surface, water_cp, vic_water_density,sumjoulb);

    /**********************************************************************
     * 2. Calculate added precipitation and total snow height.
     **********************************************************************/

    // Convert swq from m/(lake area) to m/(ice area)
    if (lake_snow->swq > 0.0) {
      if (fracprv > 0.0)
        lake_snow->swq /= fracprv;
      else if (fracprv == 0.0) {
        lake->ice_throughfall += (lake->sarea)*(lake_snow->swq);
        lake_snow->swq = 0.0;
      }
    }
      
    if ( fracprv >= 1.0) {  /* areai is relevant */

      // If there is no snow, add the rain over ice directly to the lake.
      if(lake_snow->swq <=0.0 && rainfall > 0.0) {
        lake->ice_throughfall += (rainfall/1000.)*lake->areai;
        rainfall = 0.0;
      }
    }
    else if ( fracprv > FRACLIM && fracprv < 1.0 ) { /* sarea is relevant */

      /* Precip over open water directly increases lake volume. */
      lake->ice_throughfall += ( (snowfall/1000. + rainfall/1000.)*(1-fracprv) * lake->sarea );

      // If there is no snow, add the rain over ice directly to the lake.
      if(lake_snow->swq <= 0.0 && rainfall > 0.0) {
        lake->ice_throughfall += (rainfall/1000.)*fracprv*lake->sarea;
        rainfall = 0.0; /* Because do not want it added to snow->surf_water */
      }
    }
    else {
      lake->ice_throughfall += ((rainfall+snowfall)/1000.)*lake->sarea;
      rainfall = 0.0;
      snowfall = 0.0;
    }

    /**********************************************************************
     * 3. Calculate incoming solar radiation over water and ice.
     **********************************************************************/

    /* --------------------------------------------------------------------
     * Calculate the albedo of the lake for ice, snow and liquid water.
     * To be consistent with VIC snow model, tempalbs = NEW_SNOW_ALB if snow
     * is falling, but aging routing does not get reset.
     * -------------------------------------------------------------------- */

    alblake(Tcutoff, tair, snowfall,lake_snow->coldcontent, dt,
        lake_snow->swq, lake_snow->depth, dmy.day_in_year, (double)soil_con.lat,
        lake->SAlbedo, tempalbs, albi, albw, lake_snow->last_snow,lake_snow->MELTING);

    /* --------------------------------------------------------------------
     * Calculate the incoming solar radiaton for both the ice fraction
     * and the liquid water fraction of the lake.
     * -------------------------------------------------------------------- */

    if (lake_snow->swq > SNOWCRIT *RHOSNOW/RHO_W) {
      sw_ice = shortin * (1.-tempalbs);
      lake_energy->AlbedoLake = (fracprv) * tempalbs + (1. - fracprv) * albw;
    }
    else if (lake_snow->swq > 0. && lake_snow->swq <= SNOWCRIT*RHOSNOW/RHO_W) {
      sw_ice = shortin * (1.- (albi+tempalbs)/2.);
      lake_energy->AlbedoLake = (fracprv) * (albi+tempalbs)/2. + (1. - fracprv) * albw;
    }
    else if (fracprv > 0. && lake_snow->swq <= 0.) {
      sw_ice = shortin * (1.-albi);
      lake_energy->AlbedoLake = (fracprv) * albi + (1. - fracprv) * albw;
    }
    else /* lake->fraci = 0 */ {
      sw_ice=0.0;
      lake_energy->AlbedoLake = albw;
    }

    sw_water = shortin * (1.-albw);

    /**********************************************************************
     * 4. Calculate initial energy balance over ice-free water.
     **********************************************************************/
   
    if( (1.-fracprv) > SMALL && lake->activenod > 0) {
      freezeflag=1;         /* Calculation for water, not ice. */
      windw = wind*log((2. + ZWATER)/ZWATER)/log(wind_h/ZWATER);

      ErrorFlag = water_energy_balance( lake->activenod, lake->surface, &lake->evapw, 
					dt, freezeflag, lake->dz, lake->surfdz,
					(double)soil_con.lat, Tcutoff, tair, windw, 
					pressure, vp, air_density, longin, sw_water, 
					sumjoulb, wind_h, &Qhw, &Qew, &LWnetw, T, 
                    vic_water_density, &lake_energy->deltaH,
					&energy_ice_formation, fracprv, 
					&new_ice_area, water_cp, &new_ice_height,
					&energy_out_bottom, &new_ice_water_eq,
					lake->volume-lake->ice_water_eq);
      if ( ErrorFlag == ERROR ) return (ERROR);

      /* --------------------------------------------------------------------
       * Do the convective mixing of the lake water.
       * -------------------------------------------------------------------- */

      mixdepth = 0;        /* Set to zero for this time step. */
      tracer_mixer(freezeflag, lake->surface,
        lake->activenod, lake->dz, lake->surfdz, water_cp,T, mixdepth);

      lake_energy->AtmosLatent      = ( 1. - fracprv ) * Qew;
      lake_energy->AtmosSensible    = ( 1. - fracprv ) * Qhw;
      lake_energy->NetLongAtmos     = ( 1. - fracprv ) * LWnetw;
      lake_energy->NetShortAtmos    = ( 1. - fracprv ) * sw_water;
      lake_energy->refreeze_energy  = energy_ice_formation*(1.-fracprv);
      lake_energy->deltaH          *= ( 1. - fracprv );
      lake_energy->grnd_flux        = -1.*(energy_out_bottom*(1.-fracprv));
      lake_energy->Tsurf            = (1. - fracprv)*T[0];

    }          /* End of water fraction calculations. */
    else {
      // ice covers 100% of lake, reset open water fluxes
      mixdepth = 0;
      LWnetw = 0;
      Qew    = 0;
      Qhw    = 0;

      lake_energy->AtmosLatent      = 0.0;
      lake_energy->AtmosSensible    = 0.0;
      lake_energy->NetLongAtmos     = 0.0;
      lake_energy->NetShortAtmos    = 0.0;
      lake_energy->refreeze_energy  = 0.0;
      lake_energy->deltaH           = 0.0;
      lake_energy->grnd_flux        = 0.0;
      lake_energy->Tsurf            = 0.0;

    }
 
    /**********************************************************************
     *  6. Calculate initial energy balance over ice.
     **********************************************************************/

    if ( fracprv >= FRACLIM ) {

      freezeflag = 0;         /* Calculation for ice. */
      Le = (677. - 0.07 * tair) * JOULESPCAL * GRAMSPKG; /* ice*/
      windi = ( wind * log((2. + soil_con.snow_rough) / soil_con.snow_rough) 
		/ log(wind_h/soil_con.snow_rough) );
      if ( windi < 1.0 ) windi = 1.0;
      lake->aero_resist = (log((2. + soil_con.snow_rough) / soil_con.snow_rough)
			   * log(wind_h/soil_con.snow_rough) / (von_K*von_K)) / windi;

      /* Calculate snow/ice temperature and change in ice thickness from 
         surface melting. */
      ErrorFlag = ice_melt( wind_h+soil_con.snow_rough, lake->aero_resist, &(lake->aero_resist),
			    Le, lake_snow, lake, dt,  0.0, soil_con.snow_rough, 1.0, 
			    rainfall, snowfall,  windi, Tcutoff, tair, sw_ice, 
			    longin, air_density, pressure,  vpd,  vp, &lake->snowmlt, 
			    &lake_energy->advection, &lake_energy->deltaCC, 
			    &lake_energy->snow_flux, &Qei, &Qhi, &Qnet_ice, 
			    &temp_refreeze_energy, &LWneti, fracprv);
      if ( ErrorFlag == ERROR ) return (ERROR);

      lake_energy->refreeze_energy += temp_refreeze_energy*fracprv;
      lake->tempi = lake_snow->surf_temp;  

      /**********************************************************************
       *  7. Adjust temperatures of water column in ice fraction.
       **********************************************************************/

      /* --------------------------------------------------------------------
       * Calculate inputs to temp_area..
       * -------------------------------------------------------------------- */

      if (lake->activenod > 0) {
        ErrorFlag = water_under_ice( freezeflag, sw_ice, wind, Ti, vic_water_density,
				     (double)soil_con.lat, lake->activenod, lake->dz, lake->surfdz,
                     Tcutoff, qw, lake->surface, temphi, water_cp,
             lake->hice, lake_snow->swq*RHO_W/RHOSNOW,
                     (double)dt, mixdepth, energy_out_bottom_ice);
        if ( ErrorFlag == ERROR ) return (ERROR);
      }
      else
        temphi = -sumjoulb;
	
      /**********************************************************************
       *   8.  Calculate change in ice thickness and fraction
       *    within fraction that already has ice.
       **********************************************************************/
      /* Check to see if ice has already melted (from the top) in this time step. */
      if(lake->ice_water_eq > 0.0) {
        ErrorFlag = lakeice(sw_ice, fracprv, dt, lake_energy->snow_flux, qw,
                            lake_energy->deltaCC, lake->volume-new_ice_water_eq, lake->surface[0],
                            energy_ice_melt_bot, qf, lake->ice_water_eq);

        if ( ErrorFlag == ERROR ) return (ERROR);
      }

      lake_energy->AtmosLatent += fracprv * Qei;
      lake_energy->advection       *= fracprv;
      lake_energy->AtmosSensible   += fracprv * Qhi;
      lake_energy->NetLongAtmos    += fracprv * LWneti;
      lake_energy->NetShortAtmos   += fracprv * sw_ice;
      lake_energy->deltaH          += fracprv*temphi;
      lake_energy->grnd_flux  += -1.*(energy_out_bottom_ice*fracprv);
//      lake_energy->refreeze_energy += energy_ice_melt_bot;
      lake_energy->refreeze_energy += energy_ice_melt_bot*fracprv;
      lake_energy->Tsurf += fracprv*lake_snow->surf_temp;

    }
    else {
      /* No Lake Ice Fraction */
      LWneti = 0;
      Qei    = 0.;
      Qhi    = 0;
      qf=0.0;
      temphi =0.0;
      lake_energy->refreeze_energy=0.0;
      if(fracprv > 0.0) {
        energy_ice_melt_bot = (lake->hice*RHOICE + (snowfall/1000.)*RHO_W)*Lf/(dt*SECPHOUR);
        lake->areai = 0.0;
        lake->hice = 0.0;
        lake->ice_water_eq = 0.0;

      }
      else {
        energy_ice_melt_bot = 0.0;
        lake->areai = 0.0;
        lake->hice = 0.0;
        lake->ice_water_eq = 0.0;
      }
    }
  
    /**********************************************************************
     * 9. Average water temperature.
     **********************************************************************/

    if(lake->activenod > 0) {
      // Average ice-covered and non-ice water columns.
      colavg( T, Ti, fracprv, lake->activenod,
              lake->dz, lake->surfdz,lake->temp, lake->density);

      // Calculate depth average temperature of the lake
      lake->tempavg = 0.0;
      for(i=0; i< lake->activenod; i++) {
        lake->tempavg += lake->temp[i]/lake->activenod;
      }
    }
    else
      lake->tempavg = -99;

    /**********************************************************************
     * 10. Calculate the final water heat content and energy balance.
     **********************************************************************/

    /* Incoming energy. */ 
    inputs = (sw_ice + LWneti + lake_energy->advection + Qhi + Qei);
    outputs = energy_out_bottom_ice;
    internal = temphi;
    phasechange = -1*(lake_energy->refreeze_energy)-1.*energy_ice_melt_bot;
    
    lake_energy->error = inputs - outputs - internal - phasechange;
    
    lake_energy->snow_flux       = 0.0;

    // Sign convention
    lake_energy->deltaH *= -1;

    lake_energy->error            = ( lake_energy->NetShortAtmos 
				      + lake_energy->NetLongAtmos 
				      + lake_energy->AtmosSensible 
				      + lake_energy->AtmosLatent 		   
				      + lake_energy->deltaH 
				      + lake_energy->grnd_flux
				      + lake_energy->refreeze_energy 
				      + lake_energy->advection );

    temphw=temphi=0.0;

    /**********************************************************************
     * 11. Final accounting for passing variables back to VIC.
     **********************************************************************/
 
    // Adjust lake_snow variables to represent storage and flux over entire lake
    lake_snow->swq          *= fracprv;
    lake_snow->surf_water   *= fracprv;
    lake_snow->pack_water   *= fracprv;
    lake_snow->depth = lake_snow->swq * RHO_W / RHOSNOW;
    lake_snow->coldcontent  *= fracprv;
    lake_snow->vapor_flux   *= fracprv;
    lake_snow->blowing_flux *= fracprv;
    lake_snow->surface_flux *= fracprv;
    lake_snow->melt          = lake->snowmlt*fracprv; // in mm

    // Adjust lake_energy variables to represent storage and flux over entire lake
    lake_energy->NetShortAtmos   *= fracprv;
    lake_energy->NetLongAtmos    *= fracprv;
    lake_energy->AtmosSensible   *= fracprv;
    lake_energy->AtmosLatent     *= fracprv;
    lake_energy->deltaH          *= fracprv;
    lake_energy->grnd_flux       *= fracprv;
    lake_energy->refreeze_energy *= fracprv;
    lake_energy->advection       *= fracprv;
    lake_energy->snow_flux       *= fracprv;
    lake_energy->error           *= fracprv;

    /* Lake data structure terms */
    lake->evapw *= ( (1. - fracprv ) * dt * SECPHOUR )*0.001*lake->sarea; // in m3
    lake->vapor_flux = lake->snow.vapor_flux*lake->sarea; // in m3
    lake->swe = lake_snow->swq*lake->sarea; // in m3
    lake->snowmlt *= fracprv*0.001*lake->sarea; // in m3
    lake->pack_water = lake_snow->pack_water*lake->sarea; // in m3
    lake->surf_water = lake_snow->surf_water*lake->sarea; // in m3
    lake->sdepth = lake_snow->depth*lake->sarea;
    lake->pack_temp = lake_snow->pack_temp;
    lake->surf_temp = lake_snow->surf_temp;

    /* Update ice area to include new ice growth in water fraction. */
    lake->new_ice_area  = lake->areai;
    if(new_ice_area > 0.0 ) {
      lake->new_ice_area += new_ice_area;
      lake->ice_water_eq += new_ice_water_eq;
    }
    if(lake->ice_water_eq > 0.0 && lake->new_ice_area > 0.0) {
      lake->hice = (lake->ice_water_eq/lake->new_ice_area)*RHO_W/RHOICE;
    }
    else
      lake->hice = 0.0;

    /* Change area of ice-covered fraction if ice has thinned. */
    if(lake->hice <= 0.0) {
      lake->new_ice_area = 0.0;
      lake->hice = 0.0;
    }
    else if(lake->hice < FRACMIN) {
      lake->new_ice_area = (lake->new_ice_area * lake->hice) / FRACMIN;
      lake->hice = FRACMIN;
    }

    if (lake->snow.swq > 0)
      lake->snow.coverage = lake->new_ice_area/lake->sarea;
    else
      lake->snow.coverage = 0;

  } /* End of if activenods > 0 */
  return (0);
}   /* End of solve_lake function. */
//______________________________________________________________________________
void latsens(const double Tsurf, const double Tcutk, const double hice, const double tair, const double wind,
             const double pressure, const double vp, const double air_density, const double wind_h,
             double &evap, double &qsen)
{
/**********************************************************************
 * Calculate the partitioning of the energy balance into latent and
 * sensible heat fluxes.
 *
 * Parameters :
 *
 * Tsurf	Lake surface temperature (K).
 * Tcutk        Melting temperature (K)
 * hice		Ice height (m).
 * tair		Air temperature (C).
 * wind		Wind speed (m/s).
 * pressure	Air pressure (kPa).
 * density	Air density (kg/m3).
 * vp           Vapor pressure (kPa). 
 * evap		Evaporation rate (m/s).
 * qsen		Sensible heat flux (W/m2).
 * eog		Vapor pressure at lake level (kPa).
 **********************************************************************/

  double dragcoeff = 0;
  ///**********************************************************************
  // * Calculate the drag coefficient.
  // **********************************************************************
  if(hice > 0.)
    dragcoeff = calc_lake_drag(Tsurf, CELSIUS_TO_KELVIN(tair), wind, ZSNOW, wind_h);
  else
    dragcoeff = calc_lake_drag(Tsurf, CELSIUS_TO_KELVIN(tair), wind, ZWATER, wind_h);
  ///**********************************************************************
  // * Determine the coefficients to be used in the calculation of
  // * the vapor pressure at lake level depending on whether the lake is
  // * covered with ice or not.
  // **********************************************************************
  double eog = 0;
  if ( (hice <= 0.) && (Tsurf > Tcutk) ) {
  ///* --------------------------------------------------------------------
  // * Lake is not covered with ice (Handbook of Hydrology eq. 4.2.2).
  // * eog in kPa.
  // * --------------------------------------------------------------------
    eog = .611 * exp(17.269 * KELVIN_TO_CELSIUS(Tsurf) / (KELVIN_TO_CELSIUS(Tsurf) + 237.3 ));
  } else {
  ///* --------------------------------------------------------------------
  // * Lake is covered with ice.
  // * --------------------------------------------------------------------
    eog = .611 * exp(21.874 * KELVIN_TO_CELSIUS(Tsurf) / (Tsurf - 7.66));
  }
  ///**********************************************************************
  // * Calculate the specific humidity at lake level and the difference
  // * between this humidity and the air humidity at measurement height.
  // **********************************************************************
  double qlake = 0.622 * (eog / (pressure - 0.378 * eog));                       //* Specific humidity
  double qair  = 0.622 * (vp  / (pressure - 0.378 * vp));
  double delq  = qair - qlake;                                                   //* Difference in absolute humidity between the lake
                                                                                 //* surface and higher up (-).
  //**********************************************************************
  //* Calculate the evaporation rate.  Eq. 4 in Hostetler (1991),
  //* after Brutsaert (1982).  evap in mm/s
  //**********************************************************************
  evap = -1. * dragcoeff * wind * air_density * delq;
  ///**********************************************************************
  // * Calculate the difference in lake surface temperature and the
  // * air temperature at measurement height and consequently the
  // * sensible heat flux.  Hostetler (1991) eq 5, in W/m^2.
  // **********************************************************************
  double delT = tair + KELVIN - Tsurf;
  qsen = dragcoeff * wind * air_density * Cp;
  qsen *= delT;
}
//______________________________________________________________________________
void alblake(const double  Tcutoff,
          const double  Tair,
          const double  newsnow,
          const double  coldcontent,
          const int     dt,
          const double  swq,
          const double  depth,
          const int     day_in_year,
          const double  latitude,
          double &snowalbedo,
          double &albs,
          float  &albi,
          float  &albw,
          int    &last_snow,
          char   &MELTING)
{
  ///**********************************************************************
  // * Calculate the albedo of snow, ice and water of the lake.
  // *
  // * Parameters :
  // *
  // * tair		Air temperature (C).
  // * albs		Snow albedo (-).
  // * albi		Ice albedo (-).
  // * albw		Water albedo (-).
  // *
  // * Modifications:
  // * 2007-Nov-06 Lake snow physics now consistent with land snow pack.	LCB via TJB
  // * 2008-Apr-21 Corrected some omissions from the previous mods.		LCB via TJB
  // * 2008-Apr-21 Updated to be compatible with new snow_albedo().		KAC via TJB
  // * 2010-Dec-28 Fixed MELTING date condition to be correct in southern
  // *             hemisphere.						TJB
  // **********************************************************************
  double albgl = 0;
  double albgs = 0;
  if( (Tair-Tcutoff) > 0.0) {
    if( (Tair-Tcutoff) < 20.) {
      albgl = 0.4 - 0.011  * (Tair - Tcutoff);
      albgs = 0.6 - 0.0245 * (Tair - Tcutoff);
    } else {
      albgl = 0.4 - 0.011  * 20.;
      albgs = 0.6 - 0.0245 * 20.;
    }
  } else {
    albgl = 0.4;
    albgs = 0.6;
  }
  albi = 0.5 * albgs + 0.5 * albgl;
  // update number of days since last significant snowfall
  if ( newsnow > TraceSnow ) last_snow  = 1;
  else if ( swq == 0 )       last_snow  = 0;
  else                       last_snow += 1;
  /** Record if snowpack is melting this time step **/
  if (swq > 0.0) {
    if ( coldcontent >= 0 && (
         (latitude >= 0 && (day_in_year > 60                                     // ~ March 1
                            && day_in_year < 273))                                   // ~ October 1
         || (latitude <  0 && (day_in_year < 60                                  // ~ March 1
                               || day_in_year > 273))                            // ~ October 1
       ) ) {
      MELTING = TRUE;
    } else {
      MELTING = FALSE;
    }
  } else {
    MELTING = FALSE;
  }
  if ( MELTING && newsnow > TraceSnow ) MELTING = FALSE;
  // compute snow surface albedo
  if(swq > 0.0)                             snowalbedo = snow_albedo(newsnow, swq, depth, snowalbedo, coldcontent, dt, last_snow, MELTING);
  else if(swq == 0.0 && newsnow > 0.0)      snowalbedo = NEW_SNOW_ALB;
  else                                      snowalbedo = 0.0;
  if(newsnow > 0.0)     albs = NEW_SNOW_ALB;
  else                  albs = snowalbedo;
  albw = 0.15;
}
//______________________________________________________________________________
void colavg(const double *T,const double *Ti,const float lakeprv,const int numnod,
            const double dz, const double surfdz, double *finaltemp, double *density)
{
  ///**********************************************************************
  // * Calculate the temperature of the lake water at the different node
  // * levels and calculate the water density.
  // *
  // * Parameters :
  // *
  // * finaltemp    Averaged water column temperature (C).
  // * T		Water temperature for water fraction(C).
  // * Ti		Water temperature for ice fraction(C).
  // * lakeprv	Fraction of lake covered with ice (-).
  // * density  	Water density at each node (kg/m3).
  // * numnod	Number of nodes in the lake (-).
  // * dz           Thickness of each water layer (m).
  // * surfdz       Thickness of the surface water layer.
  // **********************************************************************
  for (int j = 0;  j< numnod; j++) {
    ///* --------------------------------------------------------------------
    // * Calculate the densities of the ice and water fractions.
    // * --------------------------------------------------------------------
    double water_densityw = calc_density(T[j]);
    double water_densityi = calc_density(Ti[j]);
    water_densityw = water_densityw + 1000.;
    water_densityi = water_densityi + 1000.;
    ///* --------------------------------------------------------------------
    // * Calculate the depth differences.
    // * --------------------------------------------------------------------
    double z = dz;
    if (j == 0) z = surfdz;
    ///* --------------------------------------------------------------------
    // * Calculate the lake (water) temperature as a weight of ice and water
    // * temperatures.
    // * --------------------------------------------------------------------
    //*       temp=((1.-lakeprv)*T[j]*z*water_densityw*sh_water+
    //         lakeprv*Ti[j]*z*water_densityi*sh_ice)/
    //         ((z*water_densityw*sh_water+z*water_densityi*sh_ice)*0.5);
    //*/
    finaltemp[j] = ((1. - lakeprv) * T[j] * z * water_densityw +
                   lakeprv * Ti[j] * z * water_densityi) /
                  ((1. - lakeprv) * z * water_densityw + lakeprv * z * water_densityi);
    ///* --------------------------------------------------------------------
    //* Recalculate the water density at the different nodes.
    //* --------------------------------------------------------------------
    density[j] = calc_density(finaltemp[j]);
  }
}
//______________________________________________________________________________
double calc_density(const double ts)
{
    //**********************************************************************
    // * Calculate the water density.
    // *
    // * Parameters :
    // *
    // * ts		Temperature at the node (C).
    // *
    // * Returns:
    // *
    // * rhostps	Water density at the node (kg/m3).
    // **********************************************************************
    // * --------------------------------------------------------------------
    // * Calculate the water density as a function of temperature.
    // * -------------------------------------------------------------------- *
    double rhow = 999.842594 + .06793952 * ts - .009095290 * pow(ts,2) +
                  .0001001685 * pow(ts,3) - .000001120083 * pow(ts,4) + .000000006536332 * pow(ts,5);
    //* --------------------------------------------------------------------
    // * Return the difference between 1000 kg/m3 and the calculated
    // * density, not the actual value.
    // * -------------------------------------------------------------------- */
     return rhow - 1000.;
}
//______________________________________________________________________________
void eddy(const int freezeflag, const double wind, const double *vic_water_density,
          const double lat, const int numnod, const double dz, const double surfdz, double *de)
{
  //**********************************************************************
  // * Calculate the eddy diffusivity.
  // *
  // * Parameters :
  // *
  // * freezeflag	= 1 if liquid water, 0 if ice.
  // * wind		Wind speed at 2 meter (m/s).
  // * T		Water temperature at the different nodes (C).
  // * density		Water density at the different nodes (C).
  // * de		Eddy diffusivity at each node (m2/d).
  // * lat	         Latitude of the pixel (degrees).
  // * numnod	         Number of nodes in the lake (-).
  // **********************************************************************
  double zhalf[MAX_LAKE_NODES];
  //**********************************************************************
  // * Calculate the density at all nodes and the distance between all nodes.
  // **********************************************************************
  for (int k = 0; k < numnod; k++)
    zhalf[k] = dz;
  // * Calculate the distance between the first node and the surface node.
  zhalf[0] = (surfdz + dz) * 0.5;
  //**********************************************************************
  //* If there is ice only molecular diffusivity is taken in account,
  //* no eddy diffusivity.
  if (freezeflag != 1) {
    for(int k = 0; k < numnod; k++)
      de[k] = DM;
  } else {
    //**********************************************************************
    // * Avoid too low wind speeds for computational stability.
    // **********************************************************************
    double twind = wind < 1.0 ? 1. : wind;
    //**********************************************************************
    //*Determine the latitudinaly dependent parameter of the Ekman
    //* profile, the surface value of the friction velocity and the neutral
    //* value of the Prandtl number (Hostetler and Bartlein eq. 6 and 7).
    //**********************************************************************
    double ks = 6.6 * pow(sin((double)fabs(lat) * PI / 180.),0.5) * pow(twind,-1.84);
    double ws = 0.0012 * twind;
    double Po = 1.0;
    //**********************************************************************
    // * Determine the maximum value of the second term in the calculation
    // * of the Richardson number for computational stability.
    // **********************************************************************
    double radmax = 4.e4;
    for (int k = 0; k < numnod-1; k++) {
      //**********************************************************************
      //     * Calculate the eddy diffusivity for each node.
      // **********************************************************************
      //* --------------------------------------------------------------------
      // * Calculate the derivative of density with depth, the Brunt-Vaisala
      // * frequency and the depth of the node (Hostetler and Bartlein eq. 9).
      // * -------------------------------------------------------------------- *
      double dpdz = (vic_water_density[k+1] - vic_water_density[k]) / zhalf[k];
      double N2   = (dpdz / (1.e3 + vic_water_density[k])) * 9.8;
      double z    = surfdz + ((float)k) * dz;

      //* --------------------------------------------------------------------
      // * Calculate the second term in the calculation of the Richardson
      // * number, make sure this number does not get too large for
      // * computational stability.  Also make sure this term does not go
      // * below 1 to avoid negative Richardson numbers (Hostetler and Bartlein eq. 8).
      // * --------------------------------------------------------------------
      double rad = 0;
      if ((z * exp(ks * z) / ws) > 1.e8) {
        rad = radmax;
      } else {
        rad = 1.+40.*N2*(von_K*z)*(von_K*z)/(ws*ws*exp(-2.*ks*z));
        if (rad > radmax) rad=radmax;
        if (rad < 1.0) rad=1.0;
      }
      // * --------------------------------------------------------------------
      // * Calculate the Richardson number and the eddy diffusivity.
      // * (Hostetler and Bartlein eq. 8 and 5.
      // * --------------------------------------------------------------------
      double Ri = (-1.0 + sqrt(rad)) / 20.0;                                     //Richardson's number.
      de[k] = DM + (von_K * ws * z / Po) * exp(-ks * z)
                   /(1.0 + 37.0 * Ri * Ri);
    } //k
    //* --------------------------------------------------------------------
    //* The eddy diffusivity of the last node is assumed to equal the
    //* eddy diffusivity of the second last node.
    //* --------------------------------------------------------------------
    de[numnod-1] = de[numnod-2];
  } //freezeflag
}
//______________________________________________________________________________
void iceform(const double  Tcutoff,
             const double  fracprv,
             const int     numnod,
             const int     dt,
             const double  dz,
             const double  surfdz,
             const double *cp,
             const double *surface,
             const double  lvolume,
             double *T,
             double &qfusion,
             double &areaadd,
             double &new_ice_height,
             double &new_ice_water_eq
             )
{
  // **********************************************************************
  // * Calculate the form of new ice in the lake as long as the fractional
  // * coverage of the ice is not 1 yet.
  // *
  // * Parameters :
  // *
  // * T		Water temperatures for all the nodes (C).
  // * Tcutoff	Temperature at which water freezes (C).
  // * fracprv	Fractional coverage of ice before this calculation (-).
  // * fracadd	Added fractional ice coverage (-).
  // * lake.fraci	New fractional ice coverage (-).
  // * qfusion      Heat flux absorbed into the ice (W/m2).
  // * hice		Ice height (m).
  // * dt		Time step (s).
  // * numnod	Number of nodes in the lake (-).
  // * cp           Specific heat (J/Kg K)
  // *
  // * Modifications:
  // * 2007-Nov-06 Ice formation is now limited by available liquid water.
  // *             ice_water_eq is now the state variable for lake ice water
  // *             storage (used to be hice).				LCB via TJB
  // **********************************************************************

/**********************************************************************
 * Calculate the newly added ice to the ice layer.
 **********************************************************************/
  qfusion = 0.0;
  double sum = 0.0;
  new_ice_water_eq = 0.0;
  for (int j = 0; j < numnod;j++) {
    if (T[j] < Tcutoff) {
      //* --------------------------------------------------------------------
      //* Calculate the ice growth (extra in J).
      //* --------------------------------------------------------------------
      double extra = 0;
      if (j == 0)
        extra = ( (Tcutoff-T[j]) * surfdz * RHO_W
                  * cp[j] * (1.0-fracprv) * (surface[j] + surface[j+1]) / 2.);
      else if (j == (numnod - 1))
        extra = ( (Tcutoff-T[j]) * dz * RHO_W
                  * cp[j]  * (1.0-fracprv) * surface[j]);
      else
        extra = ( (Tcutoff-T[j]) * dz * RHO_W
                  * cp[j]  * (1.0-fracprv) * (surface[j] + surface[j+1]) / 2.);
      ///* --------------------------------------------------------------------
      // * If ice, the temperature of the water is the freezing temperature.
      // * --------------------------------------------------------------------
      T[j] = Tcutoff;
      sum += extra;
    } //if
  } //j

  //**********************************************************************
  // * Calculate the heat flux absorbed into the ice (in W/m2) and the
  // * thickness of the new ice.
  //**********************************************************************
  new_ice_water_eq = sum / (RHO_W * Lf);
  double di = 0;
  if (lvolume > new_ice_water_eq) {
    qfusion = (sum / (dt * SECPHOUR * surface[0] * (1.0 - fracprv)));            //W/m2
    di = sum / (Lf * RHOICE);                                                    //m^3 of ice formed
  } else if (lvolume > 0.0 ) {
    new_ice_water_eq = lvolume;
    di = new_ice_water_eq*RHO_W/RHOICE;
    // NEED TO CHANGE ICE TEMPERATURE TO ACCOUNT FOR EXTRA qfusion
    qfusion=(new_ice_water_eq*RHO_W/RHOICE)/(dt*SECPHOUR*surface[0]*(1.0-fracprv)); /*W/m2*/
    //      *deltaCC=(sum - *new_ice_water_eq*RHO_W/RHOICE)/(dt*SECPHOUR*surface[0]*(1.0-fracprv)); /*W/m2*/
  } else {
    new_ice_water_eq = 0.0;
    di = 0.0;
    qfusion=0.0;                                                                 //W/m2
  }
  // **********************************************************************
  // * Calculate the added fractional coverage of ice, make sure the
  // * total fractional coverage does not exceed 1.
  // **********************************************************************
  new_ice_height = FRACMIN;
  areaadd = di / (FRACMIN);
  double temparea = (1.0 - fracprv) * surface[0];
  if (areaadd > temparea) {
    new_ice_height = di / temparea;
    areaadd        = temparea;
  }
}
//______________________________________________________________________________
void icerad(const double  sw, const double  hi, const double  hs,
         double &avgcond, double &SWnet, double &SW_under_ice)
{
  ///**********************************************************************
  // * Calculate the radiation balance over ice.
  // *
  // * Paramterers :
  // *
  // * sw		Net solar radiation at top of snowpack (W/m2).
  // * hi		Ice depth (m).
  // * hs		Snow depth (m).
  // * avgcond	Thermal conductivity of the ice and snow pack
  // *		combined (W/mK).
  // * SWnet	Net short wave radiation at the top of the lake ice.
  // * SW_under_ice	Incoming short wave radiation at the bottom of
  // *		the snow-ice layer.
  // **********************************************************************
  const double a1 = PERCENT_RADIATION_VISIBLE;                                        //LML 150408
  const double a2 = PERCENT_RADIATION_INFRARED;                                       //LML 150408
  ///**********************************************************************
  // * Calculate the thermal conductivity of the combined snow and ice
  // * layer.
  // **********************************************************************
  avgcond = (hs  *CONDI + hi * CONDS) / (CONDI * CONDS);
  ///**********************************************************************
  // * Calculate the incoming radiation at different levels in the
  // * ice-snow layer.
  // **********************************************************************/
  // * Calculation of constants. Patterson and Hamblin eq, 7
  double a = -1. * (1. - exp(-lamssw * hs)) / (CONDS * lamssw);
  double b = -1. * exp(-lamssw * hs) * (1 - exp(-lamisw * hi))/(CONDI * lamisw);
  double c = -1. * (1. - exp(-lamslw * hs))/(CONDS*lamslw);
  double d = -1. * exp(-lamslw * hs) * (1 - exp(-lamilw * hi))/(CONDI * lamilw);
  // * Solar radiation at bottom of snow pack. RHS of Patterson and Hamblin, eq. 7
  SWnet = sw * a1 * (a + b) + sw * a2 * (c + d);
  // * Solar radiation at bottom of snow/ice layer.
  // * Patterson and Hamblin eq. 8 (qf-qo)
  SW_under_ice = (a1 * sw * (1. - exp(-(lamssw * hs + lamisw * hi))) +
                  a2 * sw * (1. - exp(-(lamslw * hs + lamilw * hi))));
}
//______________________________________________________________________________
int lakeice(const double sw_ice, const double fracice, const int dt, const double snowflux,
      const double qw, const double SWabsorbed, const double volume, const double sarea,
      double &energy_ice_melt_bot, double &qf, double &ice_water_eq)
/**********************************************************************
 * Calculate the growth and decrease in the lake ice cover.
 * Changed from original model since ice_melt() (based on VIC/DHSVM snow_melt())
 * now handles melt of snow and ice from the surface, for consistency with
 * the rest of VIC.  This routine now handles melt/freeze at the
 * bottom of the ice pack.
 *
 * Parameters :
 *
 * tempi      Temperature of the ice (K).
 * Tcutoff    Temperature at which water freezes (K).
 * sw_ice     Net short wave radiation over ice (W/m2).
 * hice               Ice height (m).
 * twater       Temperature of the top water layer (K).
 * *fracice   Revised fraction of the lake covered by ice due to growth/decrease of exisitng cover (-).
 * dt         Time step (s).
 * *qbot      Incoming short wave radiation at bottom of the ice (W/m2).
 * *qw                Heat storage in the lake ice (J/m3).
 * ds           Amount of snowmelt (m)
 * qnetice;   Heat flux absorbed into the ice (W/m2).
 *
 * Modifications:
 * 2007-Nov-06 Changed from original model since ice_melt() (based on
 *             VIC/DHSVM snow_melt()) now handles melt of snow and ice
 *             from the surface, for consistency with the rest of VIC.
 *             This routine now handles melt/freeze at the bottom of
 *             the ice pack.						LCB via TJB
 **********************************************************************/
{
  //**********************************************************************
  // * Calculate fluxes at the base of the ice.
  // **********************************************************************/
  // * Flux of heat in the ice at the ice/water interface (P & H, eq. 8)
  qf = snowflux + sw_ice - SWabsorbed;
  /* --------------------------------------------------------------------
   * Amount of heat used to melt the ice (positive means freezing).
   * -------------------------------------------------------------------- */
  energy_ice_melt_bot = qf - qw; // from Hostetler 1991
  /* --------------------------------------------------------------------
   * Calculate the growth of the ice pack at the bottom (in meters).
   * -------------------------------------------------------------------- */
  double dibot=(energy_ice_melt_bot / (RHOICE * Lf)) * dt * SECPHOUR;            //* change in ice surface at the bottom of the pack.
  /* --------------------------------------------------------------------
   * Calculate the water equivalent of the ice volume (in cubic meters).
   * Freezing occurs over surface area of water, not ice, if ice area exceeds
   * water area.
   * -------------------------------------------------------------------- */
  double new_water_eq = dibot * sarea * (fracice) * RHOICE / RHO_W;
  /**********************************************************************
   * Calculate the new height of the ice pack and the fractional ice
   * cover.
   **********************************************************************/
  if(dibot > 0.0) { /*Freezing; check if enough unfrozen water is available. */
    if((volume - ice_water_eq) >= new_water_eq) {
      ice_water_eq += new_water_eq;
    } else {                                                                     //* Freezing is restricted by available water.
      if( (volume - ice_water_eq) > 0.0) {
        dibot = (volume - ice_water_eq) / (sarea * fracice * RHOICE/RHO_W);
        ice_water_eq = volume;
      } else {
        dibot = 0.0;
      }
    }
  } else {                                                                       //* Melt
    ice_water_eq += new_water_eq;
    // check that ice not completely melted in the current time step
    if ( ice_water_eq <= 0.0 ) ice_water_eq = 0.0;
  }
  // *energy_ice_melt_bottom is not currently adjusted if there is not enough water to freeze or ice to melt.
  // This energy should go to cooling the ice and warming the water, respectively.
  return (0);
}
//______________________________________________________________________________
float calc_lake_drag(const float Tsurf, const double Tair, const double wind,
                     const double roughness, const double Z1)
{
  //**********************************************************************
  // * Calculate the lake drag coefficient.
  // *
  // * Parameter :
  // *
  // * Tsurf   	Lake surface temperature (K).
  // * Tair		Air temperature (K).
  // * wind		Wind speed (m/s).
  // * dragcoeff	Drag coefficient (-).
  // **********************************************************************/
  //*********************************************************************
  //* Calculate the Richardson number.
  //**********************************************************************
  double cdrn = (von_K / log(Z1 / roughness)) * (von_K / log(Z1 / roughness)); /* dimensionless */
  double ribn = Z1 * GRAVITY * (1. - Tsurf / Tair);           /* m2/s2 */
  double ribd = 0;
  if ((Tsurf / Tair) <= 1.0) ribd = wind * wind + 0.1 * 0.1;
  else                       ribd = wind * wind + 1.0 * 1.0;
  double rib = ribn / ribd;                /* dimensionless */
  ///**********************************************************************
  // * Calculate the drag coefficient using the Richardson number.
  // **********************************************************************
  double cdr = 0;
  if (rib < 0.) cdr = cdrn * (1.0 + 24.5 * sqrt(-cdrn * rib));
  else          cdr = cdrn / (1.0 + 11.5 * rib);
  double cdrmin = 0;
  if((cdrn / 4.) > 6.e-4) cdrmin = cdrn / 4.;
  else                    cdrmin = 6.e-4;
  if (cdr < cdrmin) cdr = cdrmin;
  return(cdr);
}
//______________________________________________________________________________
void rhoinit(const double pressure, double &tfsp)
{
  ///**********************************************************************
  // * Calculate the temperature at which water
  // * freezes depending on salinity and air pressure.
  // *
  // * Paramters :
  // *
  // * tfsp		Lake freezing point (C).
  // * pressure     Air pressure (Pa)
  // **********************************************************************
  double salinity = 0;                                                           //Salinity is assumed to be zero.
  tfsp = ( -0.0575 * salinity + 1.710523e-3 * pow(salinity,1.5)
           - 2.154996e-4 * salinity * salinity - 7.53e-3 * (pressure) / 100. );  //(bars)
}
//______________________________________________________________________________
double specheat(const double t)
{
/**********************************************************************
 * Calculate the specific heat of the water depending on water
 * temperature.  Salinity is assumed to be zero.
 *
 * Paramterers :
 *
 * t	Water temperature (C).
 **********************************************************************/
  return 4217.4 - 3.720283 * t + 0.1412855 * t * t - 2.654387e-3 * t * t * t
         + 2.093236e-5 * t * t * t * t;                                          // Specific heat (J/Kg K)
}
//______________________________________________________________________________
void temp_area(const double sw_visible, const double sw_nir, const double surface_force,
               const double *T, const double *vic_water_density, const double *de,
               const int dt, const double *surface, const int numnod, const double dz, const double surfdz, const double *cp,
               double &temph, double &energy_out_bottom, double *Tnew)
{
/********************************************************************** 				       
  Calculate the water temperature for different levels in the lake.
 
  Parameters :
 
  sw_visible   Shortwave rad in visible band entering top of water column
  sw_nir       Shortwave rad in near infrared band entering top of water column
  surface_force The remaining rerms i nthe top layer energy balance 
  T		Lake water temperature at different levels (K).
  water_density		Water density at different levels (kg/m3).
  de		Diffusivity of water (or ice) (m2/d).
  dt		Time step size (s).
  surface	Area of the lake at different levels (m2).
  numnod	Number of nodes in the lake (-).
  dz        Thickness of the lake layers. 

  Modifications:
  2007-Apr-23 Added initialization of temph.				TJB
  2007-Oct-24 Modified by moving closing bracket for if ( numnod==1 ) up
	      so that the code actually calls energycalc() even if the
	      lake is represented by only one node.			KAC via TJB
  2010-Nov-21 Fixed bug in definition of zhalf.				TJB

 **********************************************************************/
  double z[MAX_LAKE_NODES], zhalf[MAX_LAKE_NODES];
  double a[MAX_LAKE_NODES], b[MAX_LAKE_NODES], c[MAX_LAKE_NODES];
  double d[MAX_LAKE_NODES];

/**********************************************************************
 * Calculate the distance between the centers of the surface and first
 * lake layers.
 **********************************************************************/
/**********************************************************************
 * Initialize the water density at all the nodes and the depth of all
 * and distance between all nodes.
 **********************************************************************/
  for (int k = 0; k < numnod; k++) {
    if (k == 0) z[k] = surfdz;
    else        z[k] = dz;
    zhalf[k] = dz;
  }
  if (numnod > 1) zhalf[0] = 0.5 * (z[0] + z[1]);
  else            zhalf[0] = 0.5 * z[0];
  double energyinput = 0.0;
  double energymixed = 0.0;
/**********************************************************************
 * Calculate the right hand side vector in the tridiagonal matrix system
 * of equations.
 **********************************************************************/
  double surface_1 = surface[0];
  double surface_2 = surface[1];
  double surface_avg = (surface_1 + surface_2) / 2.;

  double T1 = (sw_visible * (1. * surface_1 - surface_2 * exp(-lamwsw * surfdz)) +
               sw_nir     * (1. * surface_1 - surface_2 * exp(-lamwlw * surfdz))) / surface_avg
              + (surface_force * surface_1) / surface_avg;                       //* W/m2
  energyinput += T1 * surface_avg;
  double cnextra = 0.5 * (surface_2 / surface_avg) * (de[0] / zhalf[0]) * ((T[1] - T[0]) / z[0]);
  energymixed += cnextra;
  temph = 0.0;
  if (numnod == 1) {
    Tnew[0] = T[0]+(T1*dt*SECPHOUR)/((1.e3+vic_water_density[0])*cp[0]*z[0]);
  } else {
    ///* --------------------------------------------------------------------
    // * First calculate d for the surface layer of the lake.
    // * --------------------------------------------------------------------
    d[0] = T[0] + (T1 * dt * SECPHOUR) / ((1.e3 + vic_water_density[0]) * cp[0] * z[0]) + cnextra * dt * SECPHOUR;
    energy_out_bottom = (surface_1 - surface_2) * (sw_visible * exp(-lamwsw * surfdz) +
                                                   sw_nir     * exp(-lamwlw * surfdz));
    ///* --------------------------------------------------------------------
    // * Calculate d for the remainder of the column.
    // * --------------------------------------------------------------------
    //All nodes but the deepest node.
    for (int k = 1; k < numnod-1; k++) {
      int top = (surfdz + (k - 1) * dz);/* The depth of the top and the bottom of the current water layer. */
      int bot = (surfdz + k * dz);
      surface_1 = surface[k]; 
      surface_2 = surface[k+1];
      surface_avg =(surface[k] + surface[k+1]) / 2.;
      T1 = (sw_visible * (surface_1 * exp(-lamwsw * top) - surface_2 * exp(-lamwsw * bot)) +
            sw_nir     * (surface_1 * exp(-lamwlw * top) - surface_2 * exp(-lamwlw * bot)))
           / surface_avg;
      energyinput += T1 * surface_avg;
      double term1 = 0.5 * (1.  / surface_avg) *((de[k]   / zhalf[k])  *((T[k+1] - T[k])   /z[k])) * surface_2;
      double term2 = 0.5 * (-1. / surface_avg) *((de[k-1] / zhalf[k-1])*((T[k]   - T[k-1]) /z[k])) * surface_1;
      cnextra = term1 + term2;
      energymixed += cnextra;
      d[k]= T[k] + (T1 * dt * SECPHOUR) / ((1.e3 + vic_water_density[k]) * cp[k] * z[k])
                 + cnextra * dt * SECPHOUR;
      energy_out_bottom += (surface_1 - surface_2) * (sw_visible * exp(-lamwsw * bot) +
                                                      sw_nir     * exp(-lamwlw * bot));
    }
    /* ....................................................................
     * Calculation for the deepest node.
     * ....................................................................*/
    int k = numnod - 1;
    surface_1   = surface[k];
    surface_2   = surface[k];
    surface_avg = surface[k];
    int top = (surfdz + (k-1) * dz);
    int bot = (surfdz + k     * dz);
    //       T1 = (sw_visible*(exp(-lamwsw*top)) + 
    //	    sw_nir*(exp(-lamwlw*top)))*surface_1/surface_avg; 
    T1 = (sw_visible * (surface_1 * exp(-lamwsw * top) - surface_2 * exp(-lamwsw * bot)) +
          sw_nir     * (surface_1 * exp(-lamwlw * top) - surface_2 * exp(-lamwlw * bot)))
         / surface_avg;
    energyinput += T1 * surface_avg;
    energyinput /= surface[0];
    cnextra = 0.5 * (-1. * surface_1 / surface_avg) * ((de[k-1] / zhalf[k-1]) * ((T[k] - T[k-1]) / z[k]));
    energy_out_bottom = 0.;
    energy_out_bottom += surface_2 * (sw_visible * exp(-lamwsw * bot) + sw_nir * exp(-lamwlw * bot));
    energy_out_bottom /= surface[0];
    energymixed += cnextra;
    d[k] = T[k] + (T1 * dt * SECPHOUR) / ((1.e3 + vic_water_density[k]) * cp[k] * z[k])
                + cnextra * dt * SECPHOUR;
    ///**********************************************************************
    // * Calculate arrays for tridiagonal matrix.
    // **********************************************************************/
    // * Top node of the column.
    surface_2   = surface[1];
    surface_avg = (surface[0] + surface[1] ) / 2.;
    b[0] = -0.5 * (de[0] / zhalf[0])
          * ( dt*SECPHOUR / z[0] ) * surface_2 / surface_avg;
    a[0] = 1. - b[0];
    //Second to second last node of the column.
    for (int k = 1; k < numnod-1; k++) {
      surface_1 = surface[k];
      surface_2 = surface[k+1];
      surface_avg = (surface[k]  + surface[k+1]) / 2.;
      b[k] = -0.5 * (de[k]   / zhalf[k] )  * (dt * SECPHOUR / z[k]) * surface_2 / surface_avg;
      c[k] = -0.5 * (de[k-1] / zhalf[k-1]) * (dt * SECPHOUR / z[k]) * surface_1 / surface_avg;
      a[k] = 1. - b[k] - c[k];
    }
    ///* --------------------------------------------------------------------
    // * Deepest node of the column.
    // * --------------------------------------------------------------------
    surface_1   = surface[numnod-1];
    surface_avg = surface[numnod-1];
    c[numnod-1] = -0.5 * (de[numnod-1] / zhalf[numnod-1]) * (dt * SECPHOUR / z[numnod-1]) * surface_1 / surface_avg;
    a[numnod-1] = 1. - c[numnod-1];
    // **********************************************************************
    // * Solve the tridiagonal matrix.
    // **********************************************************************
    tridia(numnod,c,a,b,d,Tnew);
  }
  /**********************************************************************
   * Adjust energy fluxes for change in density -> should be fixed by
   * moving to lagrangian scheme
   **********************************************************************/
  double joulenew = 0;
  energycalc(Tnew, numnod,dz, surfdz, surface, cp, vic_water_density,joulenew);
  temph = joulenew;
}
//______________________________________________________________________________
void tracer_mixer(const int freezeflag, const double *surface, const int numnod,
                   const double  dz, const double surfdz, const double *cp, double *T, int &mixdepth)
{
/**********************************************************************
 * Simulate the convective mixing in the lake.
 *
 * Paramters :
 *
 * T		Water temperatures (K).
 * water_density		Water densities (kg/m3).
 * mixdepth	         Top depth of local instability (node number, -).
 * freezeflag	         0 for ice, 1 for liquid water.
 * surface	Area of the lake per node number (m2).
 * numnod	         Number of nodes in the lake (-).
 **********************************************************************/
  double vic_water_density[MAX_LAKE_NODES];
  for (int k = 0; k < numnod; k++ )
    vic_water_density[k] = calc_density(T[k]);
  //**********************************************************************
  // * Initialize the top depth of local instability.
  // **********************************************************************
  int mixprev = 0;
  for (int k = 0; k < (numnod - 1); k++) {
    ///**********************************************************************
    // * Check for instability at each slice in water column.
    // **********************************************************************
    double avet = 0.0;
    double avev = 0.0;
    if (vic_water_density[k] > vic_water_density[k + 1]) {
      // * If there is instability apply the mixing scheme.
      if (mixprev == 0 && (k + 1) > mixdepth)
        mixdepth = k + 1;                                                        // * Correct the top depth of local instability.
        //Mix from mixprev to k+1
      for (int m = mixprev; m <= (k + 1); m++) {
        // * Apply the mixing scheme from the previous depth to the instability
        // * up to this node.
        double heatcon = 0;                                                      //*( Heat content of the surface layer (formerly vol).
        double vol;                                                              //* Volume of the surface layer (formerly vol_tr).
        if (m == 0) {
          //* Calculate the heat content and volume of the surface layer.
          heatcon = surfdz*(1.e3+vic_water_density[m])*cp[m]*surface[m];
          vol = surfdz * surface[m];
        } else {
          //* Calculate the heat content and volume of all layers but the surface layer.
          heatcon = dz*(1.e3+vic_water_density[m])*cp[m]*surface[m];
          vol = dz * surface[m];
        }
        //* Calculate the volumetric weighed average lake temperature.
        avet += T[m] * heatcon;
        avev += heatcon;
      }
      double Tav = avet / avev;
      // * Calculate the density of the surface layer.
      double densnew = calc_density(Tav);
      //* Calculate the maximum density up to the local level.
      double rho_max = 0.0;
      for (int j = 0; j < mixprev; j++ ) {
        if ( rho_max < (1000. + vic_water_density[j]))
          rho_max = 1000. + vic_water_density[j];
      }
      ///* --------------------------------------------------------------------
      // * Adjust temperatures and density in the mixed part of column.
      // * --------------------------------------------------------------------
      for (int j = mixprev; j <= (k + 1); j++) {
        T[j] = Tav;
        vic_water_density[j] = densnew;
      }
      // * --------------------------------------------------------------------
      // * Check to make sure that the mixing has not generated new instabilities
      // * above the previous depth to the instabilities.
      // * --------------------------------------------------------------------
      if (rho_max > (1000. + densnew)) {
        //* If there are still instabilities iterate again..
        mixprev = 0;
        k = -1;
      }
    } else {
    ///**********************************************************************
    // * If there are no instabilities up to now then the depth to the
    // * instability has to be increased by 1 node.
    // **********************************************************************
      mixprev = k + 1;
    }
  } //k
  //**********************************************************************
  // * Recalculate the water density.
  // **********************************************************************
  //160513LML unused for (int k = 0; k < numnod; k++ )
  //160513LML unused   water_density[k] = calc_density(T[k]);
}      
//______________________________________________________________________________
void tridia(const int ne, const double *a, const double *b, const double *c, const double *y, double *x)
{
  ///**********************************************************************
  // * Solve a tridiagonal system of equations.
  // * Parameters :
  // * ns		The number of systems to be solved.
  // * nd		First dimension of arrays (larger than or equal to ns).
  // * ne		The number of unknowns in each system. This must
  // *		be larger than or equal to 2.
  // * a		The sub diagonals of the matrices are stored in locations
  // *		a(j,2) through a(j,ne).
  // * b		The main diagonals of the matrices are stored in
  // *		locations b(j,1) through b(j,ne).
  // * c		The super diagonals of the matrices are stored in
  // *		locations c(j,1) through c(j,ne-1).
  // * y		The right hand side of the equations is stored in
  // *		y(j,1) through y(j,ne).
  // * x		The solutions of the systems are returned in
  // *		locations x(j,1) through x(j,ne).
  // *
  // * History : Based on a streamlined version of the old NCAR ULIB subroutine
  // * TRDI used in the PHOENIX climate model of Schneider and Thompson
  // * (J.G.R., 1981). Revised by Starley Thompson to solve multiple systems
  // * and vectorize well on the CRAY-1. Later revised to include a PARAMETER
  // * statement to define loop limits and thus enable Cray short vector
  // * loops.
  // *
  // * Algorithm:  LU decomposition followed by solution.  NOTE: This
  // * subroutine executes satisfactorily if the input matrix is diagonally
  // * dominant and non-singular.  The diagonal elements are used to pivot, and
  // * no tests are made to determine singularity. If a singular or numerically
  // * singular matrix is used as input a divide by zero or floating point
  // * overflow will result.
  // **********************************************************************
  double alpha[MAX_LAKE_NODES], gamma[MAX_LAKE_NODES];                           //* Work arrays dimensioned (nd,ne).
  int nm1 = ne - 1;
  //**********************************************************************
  //* Obtain the LU decompositions.
  //**********************************************************************
  alpha[0] = 1. / b[0];
  gamma[0] = c[0] * alpha[0];
  for (int i = 1; i < nm1; i++) {
    alpha[i] = 1. / (b[i] - a[i] * gamma[i-1]);
    gamma[i] = c[i] * alpha[i];
  }
  ///**********************************************************************
  // * Solve the system.
  // **********************************************************************
  x[0] = y[0] * alpha[0];
  for (int i = 1; i < nm1; i++)
    x[i] = (y[i] - a[i] * x[i-1]) * alpha[i];
  x[nm1] = (y[nm1] - a[nm1] * x[nm1-1]) / (b[nm1] - a[nm1] * gamma[nm1 - 1]);
  for(int i = nm1 - 1; i >= 0; i--)
    x[i] = x[i] - gamma[i] * x[i+1];
}  
//______________________________________________________________________________
void energycalc(const double *finaltemp, const int numnod, const double dz, const double surfdz,
                const double *surface, const double *cp, const double *density, double &sumjoule)
{
  /**********************************************************************
   * Calculate the thermal energy in the column.
   * finaltemp[numnodes]   Average temp for each node of the water column.
   * sumjoule              Thermal energy of water column, in joules 
   * numnod   
   **********************************************************************/
  sumjoule = 0.0;
  for (int k = 0; k < numnod; k++) {
      double energy = 0;
      if (k == 0)
        energy = (finaltemp[k] + KELVIN) * surfdz   *(1000. + density[k]) * cp[k] *(surface[k] + surface[k+1]) / 2.;
      else if (k == (numnod - 1))
        energy = (finaltemp[k] + KELVIN) * dz       *(1000. + density[k]) * cp[k] * surface[k] / 2.;
      else
        energy = (finaltemp[k] + KELVIN) * dz       *(1000. + density[k]) * cp[k] * (surface[k] + surface[k+1]) / 2.;
      sumjoule += energy;
  }
}
//______________________________________________________________________________
int water_balance(const lake_con_struct &lake_con, const int dt,
            const int rec, const int iveg,const int band, const double lakefrac, const soil_con_struct &soil_con,
            #if EXCESS_ICE
            int SubsidenceUpdate, double total_meltwater,
            #endif
            dist_prcp_struct *prcp, const veg_con_struct &veg_con, lake_var_struct &lake)
/**********************************************************************
 * This routine calculates the water balance of the lake
  Modifications:
  2007-Aug-09 Added features for EXCESS_ICE option.				JCA
  2007-Oct-24 Added rec to call for water_balance so that error and warning
	      messages can report the model record number.			KAC via TJB
  2007-Oct-24 Modified loop for get_sarea to include the active node.		KAC via TJB
  2007-Nov-06 Ice water content now tracked via ice_water_eq.  Lake area is
	      now the larger of lake->areai and lake->sarea.  Drainage is now
	      modeled as flow over a broad-crested wier.			LCB via TJB
  2008-Apr-21 Corrected some omissions from the previous mods.			LCB via TJB
  2009-Mar-16 Inserted missing logic for SPATIAL_FROST and replaced resid_moist
	      with min_liq.							TJB
  2009-Sep-30 Miscellaneous fixes for lake model.				TJB
  2009-Oct-05 Modified to update/rescale lake and wetland storages and fluxes
	      to account for changes in lake area.				TJB
  2009-Nov-09 Modified to fix wb errors and case when lake fraction goes to 0.	LCB via TJB
  2009-Nov-30 Removed loops over dist in delta_moist assignments and elsewhere.
	      Clarified conditions for rescaling/advecting.			TJB
  2009-Dec-11 Removed min_liq and options.MIN_LIQ.				TJB
  2010-Sep-24 Added channel_in to store channel inflow separately from
	      incoming runoff from the catchment.				TJB
  2010-Nov-02 Changed units of lake_var moisture fluxes to volume (m3).		TJB
  2010-Nov-09 Added conditional check on lake area before dividing by it.	TJB
  2010-Nov-11 Fixed incorrect computation of lake->recharge for case in which
	      lake does not contain enough water to saturate the soil.  Changed
	      the units of lake and wetland cell (or soil) moisture fluxes
	      (layer.evap, baseflow, runoff, inflow) and energy & snow fluxes
	      to be in mm / (lake-wetland tile area) instead of mm / lake or 
	      mm / wetland area.  This fixed bugs in reporting of grid-cell
	      average fluxes to output files.  Removed rescale_lake_fluxes().	TJB
  2010-Nov-21 Removed rescaling of lake->snow.swq to tile area.			TJB
  2010-Nov-26 Changed argument list to remove unneeded terms.  Now all precip
	      that didn't get added to the snowpack in solve_lake() is added
	      to lake volume here.						TJB
  2011-Mar-01 All calls to get_depth() now use *liquid* volume instead of
	      total volume, and lake ice area is not allowed to increase if
	      ice area > liquid area.  Ice cover is now treated like a "skin"
	      on top of the liquid (regardless of its thickness).  Ice is
	      assumed to be completely buoyant, i.e. none of the ice is below
	      the water line.  If ice completely covers the lake and the
	      liquid part of the lake shrinks, the ice is assumed to bend
	      like a blanket, so that the outer edges now rest on land but
	      the center rests on top of the liquid.  The lake "area" is still
	      considered to be the ice cover in this case.  Baseflow out of the
	      lake is only allowed in the area of the lake containing liquid.
	      The edges of the ice sheet are assumed to be vertical, i.e. the
	      surface area of the top of the ice sheet is equal to the surface
	      area of the bottom of the ice sheet.  This fixes water balance
	      errors arising from inconsistent estimates of lake ice area.	TJB
  2011-Mar-01 Fixed bugs in computation of lake->recharge.			TJB
  2011-Mar-01 Changed units of lake.snow and lake.energy fluxes/storages to be
	      over lake area (rather than just ice or over entire tile).	TJB
  2011-Mar-07 Fixed bug in computation of lake->soil.runoff, baseflow, etc .	TJB
  2011-Mar-31 Fixed typo in declaration of frost_fract.				TJB
  2011-Sep-22 Added logic to handle lake snow cover extent.			TJB
**********************************************************************/
{
  extern option_struct   options;
  double Tnew[MAX_LAKE_NODES];
  cell_data_struct    &cur_cell    = prcp->cell[WET][iveg][band];
  veg_var_struct      &cur_veg_var = prcp->veg_var[WET][iveg][band];
  snow_data_struct    &cur_snow    = prcp->snow[iveg][band];
  energy_bal_struct   &cur_energy  = prcp->energy[iveg][band];

  #if SPATIAL_FROST
  double *frost_fract = soil_con.frost_fract;
  #endif
  double *delta_moist = (double*)calloc(options.Nlayer,sizeof(double));
  double *moist       = (double*)calloc(options.Nlayer,sizeof(double));
  /**********************************************************************
   * 1. Preliminary stuff
   **********************************************************************/
  int isave_n = lake.activenod;   /* save initial no. of nodes for later */
  double inflow_volume = lake.runoff_in + lake.baseflow_in + lake.channel_in;
  #if EXCESS_ICE
  if(SubsidenceUpdate > 0 ) 
    inflow_volume += total_meltwater*lakefrac * 0.001 * soil_con.cell_area*lake_con.Cl[0];
  #endif
 
  /**********************************************************************
   * 2. calculate change in lake level for lake outflow calculation
   *     grid cell runoff (m3/TS for non-lake area)
   *     snow meltwater (mm)
   *     open water evaporation (mm)
   *     precip (added in solvelake)  
   **********************************************************************/
  // Add runoff from rest of grid cell and wetland to lake, remove evaporation
  // (precip was added in solve_lake, to allow for snow interception)
  // Evaporation is not allowed to exceed the liquid volume of the lake (after incoming runoff & baseflow are added)
  if (fabs(lake.evapw) > SMALL && lake.evapw > ((lake.volume - lake.ice_water_eq) + lake.ice_throughfall + inflow_volume + lake.snowmlt)) {
    lake.evapw = (lake.volume - lake.ice_water_eq) + lake.ice_throughfall + inflow_volume + lake.snowmlt;
    lake.volume = lake.ice_water_eq;
  } else {
    lake.volume += (lake.ice_throughfall + inflow_volume + lake.snowmlt - lake.evapw);
  }
  // Estimate new surface area of liquid water for recharge calculations
  double volume_save = lake.volume;
  double ldepth = 0;
  int ErrorFlag = get_depth(lake_con, lake.volume - lake.ice_water_eq, &ldepth);
  if ( ErrorFlag == ERROR ) {
    fprintf(stderr, "Something went wrong in get_depth; record = %d, volume = %f, depth = %e\n",rec,lake.volume,ldepth);
    return ( ErrorFlag );
  }
  double surfacearea = 0;
  ErrorFlag = get_sarea(lake_con, ldepth, &surfacearea);
  if ( ErrorFlag == ERROR ) {
    fprintf(stderr, "Something went wrong in get_sarea; record = %d, depth = %f, sarea = %e\n",rec,ldepth,surfacearea);
    return ( ErrorFlag );
  }
  // Estimate the new lake fraction (before recharge)
  if(lake.new_ice_area > surfacearea) surfacearea = lake.new_ice_area;
  double newfraction = surfacearea / lake_con.basin[0];
   // Save this estimate of the new lake fraction for use later
  double max_newfraction = newfraction;
  /**********************************************************************
   * 3. calculate recharge to wetland
   **********************************************************************/
  // Based on the above initial estimate of lake area, the lake
  // will either inundate some of the wetland or will recede and
  // expose new wetland.  In the case of inundation, we will
  // take all above-ground moisture storage (snowpack, etc) and
  // give it to the lake, but at the same time we will take water
  // from the lake and fill the inundated soil to saturation.
  // In the case of a receding lake, newly-exposed wetland is
  // assumed to have saturated soil and 0 above-ground storage.

  // The redistribution of moisture within the wetland will happen
  // during the final step of this function.

  // Note:
  // This does not account for phase changes and temperature
  // changes, so will generate some energy balance errors.
  // In addition, the moisture exchange with the wetland
  // will make our initial estimate of the new lake area
  // somewhat inaccurate (the initial estimate will be an
  // upper bound on the new lake area, resulting in some of
  // the wetland being "splashed" by the lake, causing the
  // snow to run off and the soil to moisten, but not being
  // inundated).  However, lake dimensions will be recalculated
  // after runoff and baseflow are subtracted from the lake.
  lake.recharge = 0.0;
  for (int j = 0; j < options.Nlayer; j++)
    delta_moist[j] = 0; // mm over (1-lakefrac)
  if(max_newfraction > lakefrac) {
    // Lake must fill soil to saturation in the newly-flooded area
    for (int j = 0; j < options.Nlayer; j++)
      delta_moist[j] += (soil_con.max_moist[j]-cur_cell.layer[j].moist)*(max_newfraction-lakefrac)/(1-lakefrac); // mm over (1-lakefrac)
    for (int j = 0; j < options.Nlayer; j++)
      lake.recharge += (delta_moist[j]) / 1000. * (1-lakefrac) * lake_con.basin[0]; // m^3
    // Above-ground storage in newly-flooded area is liberated and goes to lake
    double abovegrnd_storage = (cur_veg_var.Wdew / 1000. + cur_snow.snow_canopy + cur_snow.swq) * (max_newfraction-lakefrac) * lake_con.basin[0];
    lake.recharge -= abovegrnd_storage;
    // Fill the soil to saturation if possible in inundated area
    // Subtract the recharge (which may be negative) from the lake
    if(lake.volume - lake.ice_water_eq > lake.recharge) { // enough liquid water to support recharge
      lake.volume -= lake.recharge;
    } else { // not enough liquid water to support recharge; fill soil as much as allowed by available liquid water in lake and above-ground storage in newly-flooded area; lake will recede back from this point after recharge is taken out of it
      lake.recharge = lake.volume - lake.ice_water_eq;
      lake.volume   = lake.ice_water_eq;
      double Recharge = 1000.*lake.recharge/((max_newfraction-lakefrac)*lake_con.basin[0]) + (cur_veg_var.Wdew + cur_snow.snow_canopy*1000. + cur_snow.swq*1000.); // mm over area that has been flooded
      for (int j = 0; j<options.Nlayer; j++) {
        if(Recharge > (soil_con.max_moist[j] - cur_cell.layer[j].moist)) {
          Recharge -= (soil_con.max_moist[j] - cur_cell.layer[j].moist);
          delta_moist[j] = (soil_con.max_moist[j] - cur_cell.layer[j].moist) * (max_newfraction - lakefrac) / (1. - lakefrac); // mm over (1-lakefrac)
        } else {
          delta_moist[j] = Recharge * (max_newfraction - lakefrac)/(1. - lakefrac); // mm over (1-lakefrac)
          Recharge = 0.0;
        }
      }
    }
  }
  /**********************************************************************
   * 4. Calculate outflow from lake.  Runoff estimate is based on the
   *    equation for flow over a broad crested weir.  Baseflow estimate
   *    is the ARNO formulation, using the ice content of the adjacent
   *    wetland.  Outgoing runoff and baseflow are in m3.
   **********************************************************************/
  double Dsmax = soil_con.Dsmax / 24.;
  int lindex = options.Nlayer - 1;
  double liq = 0;
  #if SPATIAL_FROST
  for (frost_area=0; frost_area<FROST_SUBAREAS; frost_area++) {
    liq += (soil_con.max_moist[lindex] - cur_cell.layer[lindex].ice[frost_area])*frost_fract[frost_area];
  }
  #else
  liq = soil_con.max_moist[lindex] - cur_cell.layer[lindex].ice;
  #endif
  double resid_moist = soil_con.resid_moist[lindex] * soil_con.depth[lindex] * 1000.;
  /** Compute relative moisture **/
  double rel_moist = (liq-resid_moist) / (soil_con.max_moist[lindex]-resid_moist);
  /** Compute baseflow as function of relative moisture **/
  double frac = Dsmax * soil_con.Ds / soil_con.Ws;
  double baseflow_out_mm = frac * rel_moist;
  if (rel_moist > soil_con.Ws) {
    frac = (rel_moist - soil_con.Ws) / (1 - soil_con.Ws);
    baseflow_out_mm += Dsmax * (1 - soil_con.Ds / soil_con.Ws) * pow(frac,soil_con.c);
  }	    
  if(baseflow_out_mm < 0) baseflow_out_mm = 0;
  // extract baseflow volume from the lake m^3
  // baseflow will only come from under the liquid portion of the lake
  ErrorFlag = get_depth(lake_con, lake.volume-lake.ice_water_eq, &ldepth);
  if ( ErrorFlag == ERROR ) {
    fprintf(stderr, "Something went wrong in get_depth; record = %d, volume = %f, depth = %e\n",rec,lake.volume,ldepth);
    return ( ErrorFlag );
  }
  ErrorFlag = get_sarea(lake_con, ldepth, &surfacearea);
  if ( ErrorFlag == ERROR ) {
    fprintf(stderr, "Error in get_sarea; record = %d, depth = %f, sarea = %e\n",rec,ldepth,surfacearea);
    return ( ErrorFlag );
  }
  lake.baseflow_out = baseflow_out_mm*surfacearea/1000.;
  if(lake.volume -lake.ice_water_eq >= lake.baseflow_out) {
    lake.volume -= lake.baseflow_out;
  } else {
    lake.baseflow_out = lake.volume - lake.ice_water_eq;
    lake.volume -= lake.baseflow_out;
  }
  // Find new lake depth for runoff calculations
  ErrorFlag = get_depth(lake_con, lake.volume-lake.ice_water_eq, &ldepth);
  if ( ErrorFlag == ERROR ) {
    fprintf(stderr, "Something went wrong in get_depth; record = %d, volume = %f, depth = %e\n",rec,lake.volume,ldepth);
    return ( ErrorFlag );
  }
  // Compute runoff volume in m^3 and extract runoff volume from lake
  if(ldepth <= lake_con.mindepth ) {
    lake.runoff_out = 0.0;
  } else {
    double circum = 2*PI*pow(surfacearea/PI,0.5);
    lake.runoff_out = lake_con.wfrac*circum*SECPHOUR*((double)dt)*1.6*pow(ldepth-lake_con.mindepth, 1.5);
    if((lake.volume - lake.ice_water_eq) >= lake.runoff_out) {
      /*liquid water is available */
      if( (lake.volume - lake.runoff_out) < lake_con.minvolume )
  lake.runoff_out = lake.volume - lake_con.minvolume;
      lake.volume -= lake.runoff_out;
    } else {
      lake.runoff_out = lake.volume - lake.ice_water_eq;
      if( (lake.volume - lake.runoff_out) < lake_con.minvolume )
  lake.runoff_out = lake.volume - lake_con.minvolume;
      lake.volume -= lake.runoff_out;
    }
  }
  // Check that lake volume does not exceed our earlier estimate.
  // This will prevent runaway lake growth for the case in which
  // the lake recharge to wetland is negative and large enough to
  // more than compensate for runoff and baseflow out of the lake.
  if (lake.volume > volume_save) {
    lake.runoff_out += lake.volume - volume_save;
    lake.volume = volume_save;
  }
  // check that lake volume does not exceed its maximum
  if (lake.volume - lake_con.maxvolume > SMALL) {
    if(lake.ice_water_eq > lake_con.maxvolume) {
      lake.runoff_out += (lake.volume - lake.ice_water_eq);
      lake.volume = lake.ice_water_eq;
    } else {
      lake.runoff_out += (lake.volume - lake_con.maxvolume);
      lake.volume = lake_con.maxvolume;
    }
  }
  else if (lake.volume < SMALL)
    lake.volume = 0.0;
  /**********************************************************************/
  /* End of runoff calculation */
  /**********************************************************************/
  // Recalculate lake depth to define surface[] for next time step
  // Here, we only want depth of liquid water (below ice bottom), since surface[] array only applies to liquid water
  ErrorFlag = get_depth(lake_con, lake.volume-lake.ice_water_eq, &(lake.ldepth));
  if ( ErrorFlag == ERROR ) {
    fprintf(stderr, "Something went wrong in get_depth; record = %d, volume = %f, depth = %e\n",rec,lake.volume,lake.ldepth);
    return ( ErrorFlag );
  }
  /**********************************************************************
   *  4. Adjust the activenodes and lake area array. 
   **********************************************************************/
  if(lake.ldepth > MAX_SURFACE_LAKE && lake.ldepth < 2*MAX_SURFACE_LAKE) {
    /* Not quite enough for two full layers. */
    lake.surfdz = lake.ldepth/2.;
    lake.dz = lake.ldepth/2.;
    lake.activenod = 2;
  }
  else if(lake.ldepth >= 2* MAX_SURFACE_LAKE) {
    /* More than two layers. */	
    lake.surfdz = MAX_SURFACE_LAKE;
    lake.activenod = (int) (lake.ldepth/MAX_SURFACE_LAKE);
    if(lake.activenod > MAX_LAKE_NODES)
      lake.activenod = MAX_LAKE_NODES;
    lake.dz = (lake.ldepth-lake.surfdz)/((float)(lake.activenod-1));
  }	
  else if(lake.ldepth > SMALL) {
    lake.surfdz = lake.ldepth;
    lake.dz = 0.0;
    lake.activenod = 1;
  } else {
    lake.runoff_out += (lake.volume-lake.ice_water_eq);
    lake.volume = lake.ice_water_eq;
    lake.surfdz = 0.0;
    lake.dz = 0.0;
    lake.activenod = 0;
    lake.ldepth = 0.0;
  } 	
  // lake_con.basin equals the surface area at specific depths as input by
  // the user in the lake parameter file or calculated in read_lakeparam(), 
  // lake->surface equals the area at the top of each dynamic solution layer 
  
  /* Re-calculate lake surface area  */
  for (int k = 0; k<=lake.activenod; k++) {
    if(k==0)
      ldepth = lake.ldepth;
    else
      ldepth = lake.dz*(lake.activenod - k);
    ErrorFlag = get_sarea(lake_con, ldepth, &(lake.surface[k]));
    if ( ErrorFlag == ERROR ) {
      fprintf(stderr, "Something went wrong in get_sarea; record = %d, depth = %f, sarea = %e\n",rec,ldepth,lake.surface[k]);
      return ( ErrorFlag );
    }
  }
  // Final lakefraction
  if (lake.new_ice_area > lake.surface[0])
    lake.sarea = lake.new_ice_area;
  else 
    lake.sarea = lake.surface[0];
  newfraction = lake.sarea/lake_con.basin[0];

  /*******************************************************************/  
  /* Adjust temperature distribution if number of nodes has changed. 
     Note:  This approach (and the lake model in general) does not preserve 
     the thermal energy of the water column. */
 
  double findex = 0.;
  if ( lake.activenod != isave_n ) {
    for (int k = 0; k< lake.activenod; k++) {
      Tnew[k] = 0.0;
      for (int i = 0; i < isave_n; i++) {
  findex += (1./lake.activenod);
  Tnew[k] += lake.temp[(int)floor(findex)];
      }
    }
    for (int k = 0; k< lake.activenod; k++) {
      if(isave_n > 0)
  lake.temp[k] = Tnew[k]/isave_n;
      else
  lake.temp[k] = cur_energy.Tsurf;
    }	
  }
  if(lake.activenod == isave_n && isave_n == 0)
    lake.temp[lake.activenod] = cur_energy.Tsurf;
  /**********************************************************************
      5. Rescale the fluxes in the lake and the wetland by the change in lake area;
         Advect the storages.
         Final units of storages and fluxes should be in mm/final lake or wetland area.
   **********************************************************************/
  // Wetland
  if (newfraction < 1.0) { // wetland exists at end of time step
    advect_soil_veg_storage(lakefrac, max_newfraction, newfraction, soil_con, veg_con,lake_con, &cur_cell, &(cur_veg_var), delta_moist);
    rescale_soil_veg_fluxes((1-lakefrac), (1-newfraction), &cur_cell, &(cur_veg_var));
    advect_snow_storage(lakefrac, max_newfraction, newfraction, &(cur_snow));
    rescale_snow_energy_fluxes((1-lakefrac), (1-newfraction), cur_snow, cur_energy);
    for (int j=0; j<options.Nlayer; j++) moist[j] = cur_cell.layer[j].moist;
    ErrorFlag = distribute_node_moisture_properties(soil_con, moist, cur_energy);
    if ( ErrorFlag == ERROR ) return (ERROR);
  } else if (lakefrac < 1.0) { // wetland is gone at end of time step, but existed at beginning of step
    if (lakefrac > 0.0) { // lake also existed at beginning of step
      double adj = (1.-lakefrac)*lake_con.basin[0];
      for (int j = 0; j < options.Nlayer; j++)
        lake.evapw += cur_cell.layer[j].evap * 0.001 * adj;
      lake.evapw +=cur_veg_var.canopyevap    * 0.001 * adj;
      lake.evapw +=cur_snow.canopy_vapor_flux * adj;
      lake.evapw +=cur_snow.vapor_flux        * adj;
    }
  }
  // Lake
  if (newfraction > 0.0) { // lake exists at end of time step
    // Copy moisture fluxes into lake.soil structure, mm over end-of-step lake area
    double adj = 1000. / (newfraction * lake_con.basin[0]);
    lake.soil.runoff   = lake.runoff_out   * adj;
    lake.soil.baseflow = lake.baseflow_out * adj;
    lake.soil.inflow   = lake.baseflow_out * adj;
    for (int lindex=0; lindex<options.Nlayer; lindex++)
      lake.soil.layer[lindex].evap = 0;
    lake.soil.layer[0].evap += lake.evapw * adj;
    // Rescale other fluxes and storages to mm over end-of-step lake area
    if (lakefrac > 0.0) { // lake existed at beginning of time step
      rescale_snow_storage(lakefrac, newfraction, &(lake.snow));
      rescale_snow_energy_fluxes(lakefrac, newfraction, lake.snow, lake.energy);
      if (lake.snow.swq>0)
        lake.snow.coverage = lake.new_ice_area / lake.sarea;
      else
        lake.snow.coverage = 0;
    }
    else { // lake didn't exist at beginning of time step; create new lake
      initialize_lake(&lake, lake_con, &soil_con, &cur_cell, cur_energy.T[0], 1);
    }
  } else if (lakefrac > 0.0) {                                                   // lake is gone at end of time step, but existed at beginning of step
    if (lakefrac < 1.0) {                                                        // wetland also existed at beginning of step
      double adj = 1000. / (newfraction * lake_con.basin[0]);
      cur_cell.layer[0].evap   += 1000. * lake.evapw          / adj;
      cur_cell.runoff          += 1000. * lake.runoff_out     / adj;
      cur_cell.baseflow        += 1000. * lake.baseflow_out   / adj;
      cur_cell.inflow          += 1000. * lake.baseflow_out   / adj;
    }
  }
  free((char*)delta_moist);
  free((char*)moist);
  return(0);
}
//______________________________________________________________________________
void advect_soil_veg_storage(const double lakefrac,
                             const double max_newfraction,
                             const double newfraction,
                             const soil_con_struct  &soil_con,
                             const veg_con_struct   &veg_con,
                             const lake_con_struct  &lake_con,
                             cell_data_struct *cell,
                             veg_var_struct   *veg_var,
                             double *delta_moist
)
/**********************************************************************
  advect_soil_veg_storage	Ted Bohn	2009

  Function to update moisture storage in the wetland soil and veg
  to account for changes in wetland area.

  Modifications:
  2009-Nov-09 Removed advection of ice from lake to wetland.		LCB via TJB
  2009-Nov-22 Corrected calculation of asat.				TJB
  2010-Dec-01 Added calculation of zwt.					TJB
  2011-Mar-01 Corrected the addition of delta_moist to soil moisture.
	      Added calculation of zwt2, zwt3.				TJB
  2012-Feb-07 Removed OUT_ZWT2 and OUT_ZWTL; renamed OUT_ZWT3 to
	      OUT_ZWT_LUMPED.						TJB
**********************************************************************/
{
  extern option_struct   options;
  double new_moist[MAX_LAYERS];
  double tmp_moist[MAX_LAYERS];
  double tmp_runoff;
  if (lakefrac < 1.0) { // wetland existed during this step
    // Add delta_moist to wetland, using wetland's initial area (1-lakefrac)
    for (int lidx = 0; lidx < options.Nlayer; lidx++) {
      new_moist[lidx] = cell->layer[lidx].moist+delta_moist[lidx]; // mm over (1-lakefrac)
      delta_moist[lidx] = 0;
      if (new_moist[lidx] > soil_con.max_moist[lidx]) {
        if (lidx < options.Nlayer-1) {
          delta_moist[lidx+1] += new_moist[lidx]-soil_con.max_moist[lidx];
        } else {
          delta_moist[lidx]   += new_moist[lidx]-soil_con.max_moist[lidx];
        }
        new_moist[lidx] = soil_con.max_moist[lidx];
      }
    }
    for (int lidx = options.Nlayer-1; lidx >= 0; lidx--) {
      new_moist[lidx] += delta_moist[lidx]; // mm over (1-lakefrac)
      delta_moist[lidx] = 0;
      if (new_moist[lidx] > soil_con.max_moist[lidx]) {
        if (lidx > 0) {
          delta_moist[lidx-1] += new_moist[lidx] - soil_con.max_moist[lidx];
        } else {
          delta_moist[lidx]   += new_moist[lidx] - soil_con.max_moist[lidx];
        }
        new_moist[lidx] = soil_con.max_moist[lidx];
      }
    }
    // Any recharge that cannot be accomodated by wetland goes to baseflow
    if (delta_moist[0] > 0) {
      cell->baseflow += delta_moist[0]*0.001*(1-lakefrac)*lake_con.basin[0]; // m^3
      delta_moist[0] = 0;
    }
    // Rescale wetland moisture to wetland's final area (= 1-newfraction)
    for (int lidx = 0; lidx < options.Nlayer; lidx++) {
      new_moist[lidx] *= (1-lakefrac); // mm over lake/wetland tile
      new_moist[lidx] += soil_con.max_moist[lidx]*(lakefrac-newfraction); // Add the saturated portion between lakefrac and newfraction; this works whether newfraction is > or < or == lakefrac
      new_moist[lidx] /= (1-newfraction); // mm over final wetland area (1-newfraction)
      cell->layer[lidx].moist = new_moist[lidx]; // mm over (1-newfraction)
    }
    // Recompute saturated areas
    for(int lidx = 0; lidx < options.Nlayer; lidx++) {
      tmp_moist[lidx] = cell->layer[lidx].moist;
    }
    //compute_runoff_and_asat(soil_con, tmp_moist, 0, &(cell->asat), &tmp_runoff);
    compute_runoff_and_asat(&soil_con, tmp_moist, 0, &(cell->asat), &tmp_runoff);
    // Recompute zwt's
    wrap_compute_zwt(&soil_con, cell);
    // Update Wdew
    if (max_newfraction <= lakefrac) { // lake only receded
      if (veg_var != NULL) {
        veg_var->Wdew *= (1-lakefrac)/(1-newfraction);
      }
    } else {
      if (veg_var != NULL)
        veg_var->Wdew *= (1-max_newfraction)/(1-newfraction);
    }
  } else { // Wetland didn't exist until now; create new wetland
    for (int lidx = 0; lidx < options.Nlayer; lidx++) {
      cell->layer[lidx].moist = soil_con.max_moist[lidx];
      #if SPATIAL_FROST
      for (k=0; k<FROST_SUBAREAS; k++) {
        cell->layer[lidx].ice[k]     = 0.0;
      }
      #else
      cell->layer[lidx].ice      = 0.0;
      #endif
    }
    cell->asat = 1.0;
    cell->zwt = 0;
    cell->zwt_lumped = 0;
    if (veg_var != NULL) veg_var->Wdew = 0.0;
  }
  // Compute rootmoist and wetness
  cell->rootmoist = 0;
  cell->wetness = 0;
  for (int lidx = 0; lidx < options.Nlayer; lidx++) {
    if (veg_con.root[lidx] > 0)
      cell->rootmoist += cell->layer[lidx].moist;
    #if EXCESS_ICE
    cell->wetness += (cell->layer[lidx].moist - soil_con.Wpwp[lidx]) /
                     (soil_con.effective_porosity[lidx] * soil_con.depth[lidx] * 1000. - soil_con.Wpwp[lidx]);
    #else
    cell->wetness += (cell->layer[lidx].moist - soil_con.Wpwp[lidx]) /
                     (soil_con.porosity[lidx] * soil_con.depth[lidx] * 1000. - soil_con.Wpwp[lidx]);
    #endif
  }
  cell->wetness /= options.Nlayer;
}
//______________________________________________________________________________
void rescale_soil_veg_fluxes(const double oldfrac,
                             const double newfrac,
                             cell_data_struct *cell,
                             veg_var_struct   *veg_var)
/**********************************************************************
  rescale_soil_veg_fluxes	Ted Bohn	2009

  Function to update wetland soil and veg moisture fluxes
  to account for changes in wetland area.

  Modifications:

**********************************************************************/
{
  extern option_struct   options;
  double use_new_frac = newfrac;
  if (use_new_frac < SMALL) use_new_frac = SMALL;
  if (oldfrac > 0.0) { // existed at beginning of time step
    double adj = oldfrac / use_new_frac;
    for (int lidx = 0; lidx < options.Nlayer; lidx++) {
      cell->layer[lidx].evap *= adj;
    }
    cell->baseflow *= adj;
    cell->inflow *= adj;
    cell->runoff *= adj;
    if (veg_var != NULL) {
      veg_var->canopyevap *= adj;
      veg_var->throughfall *= adj;
    }
  } else {                                                                       // didn't exist at beginning of time step; set fluxes to 0
    for (int lidx = 0; lidx < options.Nlayer; lidx++) {
      cell->layer[lidx].evap = 0.0;
    }
    cell->baseflow = 0.0;
    cell->inflow = 0.0;
    cell->runoff = 0.0;
    if (veg_var != NULL) {
      veg_var->canopyevap = 0.0;
      veg_var->throughfall = 0.0;
    }
  }
}
//______________________________________________________________________________
void advect_snow_storage(const double lakefrac,
                         const double max_newfraction,
                         const double newfraction,
                         snow_data_struct  *snow)
/**********************************************************************
  advect_snow_storage	Ted Bohn	2009

  Function to update moisture storage in the wetland snow pack
  to account for changes in wetland area.

  Modifications:
**********************************************************************/
{
  double usenewfraction = newfraction;
  if ((1. - usenewfraction) < SMALL) usenewfraction = 1. - SMALL;
  if (lakefrac < 1.0) {                                                          // wetland existed during this step
    if (max_newfraction > lakefrac) {
      double adj = (1. - max_newfraction) / (1. - usenewfraction);
      snow->depth *= adj;
      snow->pack_water *= adj;
      snow->snow_canopy *= adj;
      snow->surf_water *= adj;
      snow->swq *= adj;
    } else {
      double adj = (1. - lakefrac) / (1. - usenewfraction);
      snow->depth *= adj;
      snow->pack_water *= adj;
      snow->snow_canopy *= adj;
      snow->surf_water *= adj;
      snow->swq *= adj;
    }
  }  else {                                                                      // Wetland didn't exist until now; create new wetland
    snow->depth = 0.0;
    snow->pack_water = 0.0;
    snow->snow_canopy = 0.0;
    snow->surf_water = 0.0;
    snow->swq = 0.0;
  }
}
//______________________________________________________________________________
void rescale_snow_storage(const double oldfrac,
                          const double newfrac,
                          snow_data_struct  *snow)
/**********************************************************************
  rescale_snow_storage	Ted Bohn	2011

  Function to update moisture storage in the lake ice snowpack
  to account for changes in lake area.

  Modifications:
**********************************************************************/
{
  double use_new_frac = newfrac;
  if (use_new_frac < SMALL) use_new_frac = SMALL;
  double adj = oldfrac / use_new_frac;
  snow->depth *= adj;
  snow->pack_water *= adj;
  snow->snow_canopy *= adj;
  snow->surf_water *= adj;
  snow->swq *= adj;
}
//______________________________________________________________________________
void rescale_snow_energy_fluxes(const double oldfrac,
                                const double newfrac,
                                snow_data_struct  &snow,
                                energy_bal_struct &energy)
/**********************************************************************
  rescale_snow_energy_fluxes	Ted Bohn	2009

  Function to update the wetland snow and energy fluxes
  to account for changes in wetland area.

  Modifications:
  2009-Nov-09 Added rescaling of snow->swq.				TJB
  2009-Nov-30 Removed rescaling of snow->swq.				TJB
**********************************************************************/
{
  double use_new_frac = newfrac;
  if (use_new_frac < SMALL) use_new_frac = SMALL;
  if (oldfrac > 0.0) {                                                           // existed at beginning of time step
    double adj = oldfrac / use_new_frac;
    snow.blowing_flux *= adj;
    snow.melt *= adj;
    snow.surface_flux *= adj;
    snow.vapor_flux *= adj;
    energy.advected_sensible *= adj;
    energy.advection *= adj;
    energy.AtmosError *= adj;
    energy.AtmosLatent *= adj;
    energy.AtmosLatentSub *= adj;
    energy.AtmosSensible *= adj;
    energy.canopy_advection *= adj;
    energy.canopy_latent *= adj;
    energy.canopy_latent_sub *= adj;
    energy.canopy_refreeze *= adj;
    energy.canopy_sensible *= adj;
    energy.deltaCC *= adj;
    energy.deltaH *= adj;
    energy.error *= adj;
    energy.fusion *= adj;
    energy.grnd_flux *= adj;
    energy.latent *= adj;
    energy.latent_sub *= adj;
    energy.longwave *= adj;
    energy.LongOverIn *= adj;
    energy.LongUnderIn *= adj;
    energy.LongUnderOut *= adj;
    energy.melt_energy *= adj;
    energy.NetLongAtmos *= adj;
    energy.NetLongOver *= adj;
    energy.NetLongUnder *= adj;
    energy.NetShortAtmos *= adj;
    energy.NetShortGrnd *= adj;
    energy.NetShortOver *= adj;
    energy.NetShortUnder *= adj;
    energy.out_long_canopy *= adj;
    energy.out_long_surface *= adj;
    energy.refreeze_energy *= adj;
    energy.sensible *= adj;
    energy.shortwave *= adj;
    energy.ShortOverIn *= adj;
    energy.ShortUnderIn *= adj;
    energy.snow_flux *= adj;
  } else {                                                                       // didn't exist at beginning of time step; set fluxes to 0
    snow.blowing_flux = 0.0;
    snow.melt = 0.0;
    snow.surface_flux = 0.0;
    snow.vapor_flux = 0.0;
    energy.advected_sensible = 0.0;
    energy.advection = 0.0;
    energy.AtmosError = 0.0;
    energy.AtmosLatent = 0.0;
    energy.AtmosLatentSub = 0.0;
    energy.AtmosSensible = 0.0;
    energy.canopy_advection = 0.0;
    energy.canopy_latent = 0.0;
    energy.canopy_latent_sub = 0.0;
    energy.canopy_refreeze = 0.0;
    energy.canopy_sensible = 0.0;
    energy.deltaCC = 0.0;
    energy.deltaH = 0.0;
    energy.error = 0.0;
    energy.fusion = 0.0;
    energy.grnd_flux = 0.0;
    energy.latent = 0.0;
    energy.latent_sub = 0.0;
    energy.longwave = 0.0;
    energy.LongOverIn = 0.0;
    energy.LongUnderIn = 0.0;
    energy.LongUnderOut = 0.0;
    energy.melt_energy = 0.0;
    energy.NetLongAtmos = 0.0;
    energy.NetLongOver = 0.0;
    energy.NetLongUnder = 0.0;
    energy.NetShortAtmos = 0.0;
    energy.NetShortGrnd = 0.0;
    energy.NetShortOver = 0.0;
    energy.NetShortUnder = 0.0;
    energy.out_long_canopy = 0.0;
    energy.out_long_surface = 0.0;
    energy.refreeze_energy = 0.0;
    energy.sensible = 0.0;
    energy.shortwave = 0.0;
    energy.ShortOverIn = 0.0;
    energy.ShortUnderIn = 0.0;
    energy.snow_flux = 0.0;
  }
}

