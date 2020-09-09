#ifdef VIC_CROPSYST_VERSION
#ifdef VCS_V5
#include "vic_def.h"
#else
#include "vicNl_def.h"
#include "vicNl.h"
#endif
#include "VCS_Nl_def.h"
#include "VCS_Nl.h"

/*170413LML moved to VCS_Nl.h
int get_cropping_system_numbers(const veg_con_struct *veg_con_array);          //150928LML
int set_output_for_crop_numbers(out_data_struct *out_data,const int numcrops); //150928LML
int copy_veg_lib_record(const veg_lib_struct &from_veg, veg_lib_struct &to_veg);      //151001LML
int make_veg_lib_for_crops(veg_con_struct *veg_con,veg_lib_struct *veg_lib);     //151001LML
*/
#if (FULL_IRRIGATION==FALSE)
irrigation_pattern_struct *irrig_patt;
#endif
#if (VIC_CROPSYST_VERSION>=3)
#include "corn/OS/file_system_logical.h"
namespace CORN { namespace OS {
File_system &file_system() { return CORN::OS::file_system_logical; }
}}
#endif
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
  return 1;
}
//______________________________________________________________________________
bool find_irrigation_type(const Irrigation_Types_In_Each_Cell &irrig_lib,
                          const int crop_code, Irrigation_Type &irrigation_type,
                          bool &full_irrigation)
{
  //160609LML may need optimize later!
  for (int i = 0; i < MAX_NUM_CROP_TYPES_FOR_IRRIGATION_DEFINE; i++) {
    if (crop_code == irrig_lib.crop_code[i]) {
      irrigation_type = irrig_lib.irrigation_type[i];
      full_irrigation = irrig_lib.full_irrigation[i];
      return true;
    }
  }
  return false;
}
//150929LML_____________________________________________________________________
int set_average_veglib_for_crop(veg_lib_struct &veglib,
                                const veg_var_struct * const * const *veg_var,
                                const dist_prcp_struct *prcp,
                                const soil_con_struct *soil_con,
                                const int veg_index,
                                const int current_month)
{
    //set the average over dist and snow bands
    int Ndist = options.NDIST;
    veglib.LAI[current_month]            = 0;
    veglib.albedo[current_month]         = 0;
    veglib.displacement[current_month]   = 0;
    veglib.roughness[current_month]      = 0;
    veglib.Wdmax[current_month]          = 0;
    veglib.emissivity[current_month]     = 0;
    veglib.rad_atten                     = 0;
    veglib.rmin                          = 0;
    veglib.rarc                          = 0;
    veglib.trunk_ratio                   = 0;
    veglib.wind_atten                    = 0;
    veglib.wind_h                        = 0;
    veglib.RGL                           = 0;
    for (int dist = 0; dist < Ndist; dist++ ) {                                  //150929LML
      for (int band = 0; band < options.SNOW_BAND; band++) {                                //150929LML
        crop_data_struct *current_crop = veg_var[dist][veg_index][band].crop_state;
        if (current_crop) {
          VIC_land_unit_activate(current_crop->CropSystHandle);                  //151001LML time consuming!!!
          double mu = (dist == WET ? prcp->mu[veg_index] : (1.0 - prcp->mu[veg_index]));
          double snow_zone_frac = soil_con->AreaFract[band];
          veglib.LAI[current_month]            += mu * snow_zone_frac * VIC_land_unit_get(VIC::GAI); //180619LML replaced from LAI
          veglib.albedo[current_month]         += mu * snow_zone_frac * VIC_land_unit_get(VIC::CANOPY_ALBEDO);
          veglib.displacement[current_month]   += mu * snow_zone_frac * VIC_land_unit_get(VIC::DSPLACEMENT);
          veglib.roughness[current_month]      += mu * snow_zone_frac * VIC_land_unit_get(VIC::ROUGHNESS);
          veglib.Wdmax[current_month]          += mu * snow_zone_frac * LAI_WATER_FACTOR * veglib.LAI[current_month];
          veglib.emissivity[current_month]     += mu * snow_zone_frac * VIC_land_unit_get(VIC::EMISSIVITY); //LML 150505 seemd never used
          veglib.rad_atten                     += mu * snow_zone_frac * VIC_land_unit_get(VIC::RAD_ATTEN);
          veglib.rmin                          += mu * snow_zone_frac * VIC_land_unit_get(VIC::MIN_STOMATA_CONDUCTANCE);
          veglib.rarc                          += mu * snow_zone_frac * VIC_land_unit_get(VIC::RA_ARCHITECTURAL);
          veglib.trunk_ratio                   += mu * snow_zone_frac * VIC_land_unit_get(VIC::TRUNK_RATIO);
          veglib.wind_atten                    += mu * snow_zone_frac * VIC_land_unit_get(VIC::WIND_ATTEN);
          veglib.wind_h                        += mu * snow_zone_frac * VIC_land_unit_get(VIC::WIND_H);
          veglib.RGL                           += mu * snow_zone_frac * VIC_land_unit_get(VIC::RGL);
        }
      } //band
    } //dist
    return 1;
}
/*LML 150427__________________________________________________________________*/
void copy_and_split_veg_con_element(const veg_con_struct &from,
                                    veg_con_struct &to,
                                    int split_num,
                                    int rotation_index,
                                    int set_zone)
{
    //Copy or split the veg_con element according to crop rotation types
    //Split_num is the total sub-elemets that will be splited
    int j;
    if (split_num > 1) {
      to.Cv             = from.Cv / (double)split_num;
      to.VCS.veg_class_code = from.VCS.veg_class_code * MULTIPLIER_FOR_SOWING_CODE + rotation_index;
    } else {
      to.Cv             = from.Cv;
      to.VCS.veg_class_code = from.VCS.veg_class_code;
    }
    to.Cv_sum           = from.Cv_sum;
    to.veg_class        = from.veg_class;
    to.LAKE             = from.LAKE;
    if (options.BLOWING) {
      to.sigma_slope    = from.sigma_slope;
      to.lag_one        = from.lag_one;
      to.fetch          = from.fetch;
    }
    if (set_zone) {
      for (j = 0; j < options.ROOT_ZONES; j++) {
          to.zone_depth[j] = from.zone_depth[j];
          to.zone_fract[j] = from.zone_fract[j];
      }
    }
}
#endif //VIC_CROPSYST_VERSION>=3
/*170901LML
#ifndef USE_SIMPLIFIED_IRRIGATION_TYPES
double evaporation_from_irrigation_systems(
                                           double ET0,
                                           double ET0_open_water,
                                           int irrigation_index,
                                           double crop_h = 0.5
                                           )
{
    double Ed = 0;
    VIC_Irrigation_library *current_irrigation = &Irrigation_library[irrigation_index];
    int irrigation_code = current_irrigation->IrrigationType_code;
    //  double irrigation_runoff = Irrigation_library[irrigation_index].irrigation_efficiency[0];
    #if (VIC_CROPSYST_VERSION>=3)
    double cd               = current_irrigation->irrigation_efficiency.cd;                      //1.1; //nuzzle coeficient
    double h                = current_irrigation->irrigation_efficiency.h_op*100;                   //207.0; //irrigation system operating pressure kpa
    double g                = GRAVITY;//9.81;

    double D                = current_irrigation->irrigation_efficiency.D;                          //3.0; //average droplet size mm
    double teta             = current_irrigation->irrigation_efficiency.q_nuzzle*0.0174532925;   //35.0*0.0174532925; //angle of the nuzzle it should be converted to radian by a function
    double x0               = current_irrigation->irrigation_efficiency.x_dm_spk;                  //10.0; //diameter of sprinkler
    double y0_0             = current_irrigation->irrigation_efficiency.y0_spk;                  //2.0; //height of the irrigation sprinkler from ground m
    double time_of_irrig    = current_irrigation->irrigation_efficiency.tirrig;
    double actual_drop_size = current_irrigation->irrigation_efficiency.actual_droplet_size;
    double Ap               = current_irrigation->irrigation_efficiency.A_Ap;                      //0.07;//percentage of area which is covered by irrigation system at a time
    #else
    double cd               = current_irrigation->irrigation_efficiency[4];                      //1.1; //nuzzle coeficient
    double h                = current_irrigation->irrigation_efficiency[2]*100;                     //207.0; //irrigation system operating pressure kpa
    double g                = 9.81;

    double D                = current_irrigation->irrigation_efficiency[1];                         //3.0; //average droplet size mm
    double teta             = current_irrigation->irrigation_efficiency[8]*0.0174532925;         //35.0*0.0174532925; //angle of the nuzzle it should be converted to radian by a function
    double x0               = current_irrigation->irrigation_efficiency[6];                        //10.0; //diameter of sprinkler
    double y0_0             = current_irrigation->irrigation_efficiency[5];                      //2.0; //height of the irrigation sprinkler from ground m
    double time_of_irrig    = current_irrigation->irrigation_efficiency[11];
    double actual_drop_size = current_irrigation->irrigation_efficiency[12];
    double Ap               = current_irrigation->irrigation_efficiency[10];                       //0.07;//percentage of area which is covered by irrigation system at a time
    #endif

    const double K_t = 0.6425;
    const double k_D = -0.0108;
    // static double crop_h=0.5; //crop height m it should be impelemented from CropSyst
    // static double K_t=-0.07527; //empirical coefficient of the t term
    // static double k_D=0.31866;
    if (irrigation_code < IrrigTP_drip_0_0) {
        double v0 = cd*pow((2.0*g*h/100.0),0.5); //initial velocity
        //
        //calcualte the evaporation from
        //irrigation system   keyvan 121130
        //
        double t_term = pow((v0*sin(teta)/g+pow(pow(v0,2.0)*pow(sin(teta),2)*g*y0_0,0.5)/g),K_t);//pow(((tan(teta)-(y0_0-crop_h)/x0)*v0*cos(teta)/g),K_t);
        double D_term = pow(h/(actual_drop_size*10.), k_D);
        Ed = ET0 * D_term * t_term;
    }
    else{
        Ed = ET0_open_water; //it's just a fixed number and later we can define it for diferent irrigation systems
    }
    //printf("Ed(%.5f)\tAp(%.5f)\ttime_of_irrig(%.5f)\n",Ed,Ap,time_of_irrig);
    return Ed * Ap * time_of_irrig / 24.0 / 3.0; //LML 150501 I don't know where 3.0 come from.

};
#endif
*/

double solve_penman_for_T(double Ts, va_list ap){
    double init_T;
    double vpd;
    //double vp;
    double Ec;
    double Ed;
    double ET_pot;
    double rc;
    double rarc;
    double ra;
    double Rn;
    double elev;
    //double cp;
    //LML 140812 added CROPSYST_ON
    //150702LML#ifdef VIC_CROPSYST_VERSION
    //150702LML crop_data_struct *current_crop;                                              //150702LML crops;
    //150702LML int current_crop_counter;
    //150702LML#endif


    ////////////////// Passing arguments through va_list/////////

    init_T              = (double) va_arg(ap,double ); /// has to be in C and will be converted to K
    vpd                 = (double) va_arg(ap,double ); /// has to be in Pascal
    //vp                  = (double) va_arg(ap,double );
    Ec                  = (double) va_arg(ap, double); /// mm
    Ed                  = (double) va_arg(ap, double); /// mm
    ET_pot              = (double) va_arg(ap, double); /// mm
    rc                  = (double) va_arg(ap, double);
    rarc                = (double) va_arg(ap, double);
    ra                  = (double) va_arg(ap, double);
    Rn                  = (double) va_arg(ap, double);
    elev                = (double) va_arg(ap, double);

    //LML 140812 added CROPSYST_ON
    #ifdef VIC_CROPSYST_VERSION
    crop_data_struct *current_crop = (crop_data_struct *) va_arg(ap, crop_data_struct *); ///crop structure
    //150702 current_crop_counter = (int) va_arg(ap, int);
    //150702 crop_data_struct *current_crop = &crops[current_crop_counter];
    #endif
    //cp                  = (double) va_arg(ap, double);
    double init_ea = 1000* 0.618*exp(17.27*init_T/(init_T+237.3))-vpd;

    //double P=101.3*pow((293.0-0.0065*elev)/293.0 ,5.26);
    //Ts=init_T;
    double C_gas_law        = 2.16679;
    double e                = 0.622; ///water_to_air_molecular_weight_ratio
    double cp               = 0.001013; ///specific heat of air
    double init_A           = C_gas_law*init_ea/(init_T+273.15);
    double resid_vertical_dist_coeff=0.8756; ///for 30 minutes of residence time and 10 cm above crop for 10 m log profile starting from 10cm below canopy level
    double new_A            = init_A +resid_vertical_dist_coeff*(Ed +Ec);

    double new_ea           = new_A*(Ts+273.15)/C_gas_law;
    double new_es           = 0.618*exp(17.27*Ts/(Ts+237.3))*1000.;
    double new_vpd          = new_es-new_ea;

    //LML 140812 added CROPSYST_ON
    #ifdef VIC_CROPSYST_VERSION
    current_crop->new_vpd  = new_vpd;
    current_crop->new_Temp = Ts;
    current_crop->new_ea   = new_ea;
    #endif

    //  penman(air_temp, elevation, rad, vpd, ra, rc, rarc); *********
    double ET = penman(Ts, elev, Rn, new_vpd, ra, rc, rarc);
    double kc =
        #if VIC_CROPSYST_VERSION >= 3
        VIC_land_unit_get(VIC::ET_COEF_FULL_SHADING);
        #else
        crop_coefficient();
        #endif
    double target_ET = (ET_pot - (Ed + Ec)) / kc;
    //Ts=init_T;
    //double check_exp=0.6108*exp(17.27*Ts/(Ts+237.3));
    return ET - target_ET;
}
//______________________________________________________________________________
double solve_wet_bulb(double Ts, va_list ap)
{
    double air_T;
    double vpd;
    double elev;
    //////////////// Passing arguments through va_list/

    air_T               = (double) va_arg(ap,double ); /// has to be in C and will be converted to K
    vpd                 = (double) va_arg(ap,double ); /// has to be in Pascal
    elev                = (double) va_arg(ap, double);

    double cp = 0.001013; ///specific heat of air
    double calc_air_T;
    double latent_heat_vaporization = 2.501-0.002361*air_T;
    double P = 101.3*pow((293.0-0.0065*elev)/293.0 ,5.26);
    double psychrometric_constant = cp*P/latent_heat_vaporization; ////
    double ea = (0.618*exp(17.27*air_T/(air_T+237.3)))- vpd/1000.0;

    double es_TW = 0.618*exp(17.27*Ts/(Ts+237.3));

    calc_air_T = Ts + (es_TW-ea)/psychrometric_constant;


    double error = air_T-calc_air_T;
    return error;
}
/*LML 150501__________________________________________________________________*/
void clear_cell_irrigation_water_fluxes(cell_data_struct *current_cell)
{
  //Initialize water fluxes from current crop in current time step
    current_cell->VCS.evap_from_irrigation_syst      = 0;
    current_cell->VCS.irrigation_netdemand = 0.0;
    current_cell->VCS.irrigation_water         = 0;
    current_cell->VCS.actual_irrigation_reach_ground_amount = 0;                     //150714LML
    current_cell->VCS.deep_percolation_from_irrigation = 0;                      //180531LML
    current_cell->VCS.irrigation_runoff              = 0;
    current_cell->VCS.intercepted_irrigation         = 0;
    current_cell->VCS.evap_from_irrigation_intercept = 0;
    current_cell->VCS.potential_transpir             = 0;
}
//150928LML_____________________________________________________________________
int set_output_for_crop_numbers(out_data_struct *out_data,const int numcrops)
{
    out_data[OUT_CROP_BIOM_CUR].nelem       = numcrops;
    out_data[OUT_CROP_TRANSPIR].nelem       = numcrops;
    out_data[OUT_CROP_IRRI_WAT].nelem       = numcrops;
    out_data[OUT_CROP_BIOM_AHA].nelem       = numcrops;
    out_data[OUT_CROP_BIOMYELD].nelem       = numcrops;
    out_data[OUT_CROP_WSI].nelem            = numcrops;
    out_data[OUT_CROP_LAI].nelem            = numcrops;
    out_data[OUT_CROP_GROWTHST].nelem       = numcrops;
    out_data[OUT_LYR0_VW ].nelem            = numcrops;
    out_data[OUT_LYR1_VW ].nelem            = numcrops;
    out_data[OUT_LYR2_VW ].nelem            = numcrops;
    out_data[OUT_CROP_TRANS0 ].nelem        = numcrops;
    out_data[OUT_CROP_TRANS1 ].nelem        = numcrops;
    out_data[OUT_CROP_TRANS2 ].nelem        = numcrops;
    out_data[OUT_SWD0 ].nelem               = numcrops;
    out_data[OUT_SWD1 ].nelem               = numcrops;
    out_data[OUT_SWD2 ].nelem               = numcrops;
    out_data[OUT_CROP_EVAP ].nelem          = numcrops;
    out_data[OUT_INFILT ].nelem             = numcrops;//this need not be based on the crops and should actually be for each land cover
    ///130503 Keyvan added
    out_data[OUT_EVAP_CANOP_IRRIG ].nelem   = numcrops;
    out_data[OUT_EVAP_FROM_IRRIG ].nelem    = numcrops;
    out_data[OUT_EVAP_SOIL ].nelem          = numcrops;
    out_data[OUT_RUNOFF_IRRIG ].nelem       = numcrops;
    out_data[OUT_ET_POT_SHORT ].nelem       = numcrops;
    out_data[OUT_CROP_COEF_KC ].nelem       = numcrops;
    out_data[OUT_CROP_SOIL_MOIST ].nelem    = numcrops;
    out_data[OUT_CROP_SOIL_POTEN ].nelem    = numcrops;
    out_data[OUT_CROP_TRANS_POT].nelem      = numcrops;
    out_data[OUT_CANOPY_FRACT].nelem        = numcrops; //130709 keyvan
    out_data[OUT_DEL_T].nelem               = numcrops;
    out_data[OUT_DEL_VPD].nelem             = numcrops;
    //170924LML out_data[OUT_IRRIGATION].nelem          = numcrops;                          //150930LML
    return numcrops;
}
/*150710LML                                                                   */
#ifdef DEBUG_SPECIFIC_VEG_BAND
void print_state_flux_specific_veg_band(int dist,
                                        double mu,
                                        int band,
                                        #ifdef CHECK_WATER_BALANCE
                                        double init_total_water_mm,
                                        #endif
                                        const veg_lib_struct   &veglib_rec,
                                        const cell_data_struct *cell,
                                        const snow_data_struct *snow,
                                        const veg_var_struct *veg_var,
                                        const veg_con_struct *veg_con,
                                        //150929LML const crop_data_struct *crop,
                                        const soil_con_struct *soil,
                                        const atmos_data_struct *atmos)
{
    int month_index = (int)global_today.get_month() - 1;
    std::ofstream debugout;
    static bool newed(false);
    std::string filename = "c:\\temp\\vic_veg_band.csv";
    if (!newed) {
        debugout.open(filename.c_str(),std::ofstream::out);
        debugout <<"Year,"
                 <<"Month,"
                 <<"Day,"
                 <<"DOY,"
                 <<"rec,"
                 <<"CELL-ID,"
                 <<"Veg_Code,"
                 <<"dist,"
                 <<"band,"
                 <<"Active_crop,"
                 <<"Accum_DD,"
                 <<"Grow_Stage,"
                 <<"LAI,"
                 <<"GAI,"
                 <<"Total_Canopy_Cover,"
                 <<"Biomass_kg_m2,"
                 <<"Yield_kg_m2,"
                 <<"Root_depth_mm,"
                 <<"PET_SHORT_mm,"
                 <<"Transp_VIC_mm,"
                 <<"Transp_CS_mm,"
                 <<"irrig_total_mm,"
                 <<"irrig_evap_mm,"
                 <<"irrig_runoff_mm,"
                 <<"irrig_intcpt_mm,"
                 <<"irrig_intcpt_evap_mm,"
                 <<"Soil_E_mm,"
                 <<"Canopy_E_mm,"
                 <<"Snow_sub_mm,"
                 <<"Canopy_sub_mm,"
                 <<"Runoff,"
                 <<"Baseflow,"
                 <<"PPT,"
                 <<"SWE_mm,"
                 <<"Profile_moisture_mm,"
                 <<"Profile_thickness_mm,"
                 <<"Profile_VWC,"
                 #ifdef CHECK_WATER_BALANCE
                 <<"init_total_water_mm,"
                 <<"final_total_water,"
                 <<"inflow,"
                 <<"outflow,"
                 <<"balance,"
                 #endif
                 ;
        #ifdef PRINT_VIC_SOIL_LAYERS_FOR_DEBUG
        for (int i = 0; i < options.Nlayer; i++) debugout<<"VWC["<<i<<"],";
        for (int i = 0; i < options.Nlayer; i++) debugout<<"Moist_mm["<<i<<"],";
        for (int i = 0; i < options.Nlayer; i++) debugout<<"Depth_mm["<<i<<"],";
        for (int i = 0; i < options.Nlayer; i++) debugout<<"FC_VWC["<<i<<"],";
        for (int i = 0; i < options.Nlayer; i++) debugout<<"SAT_VWC["<<i<<"],";
        for (int i = 0; i < options.Nlayer; i++) debugout<<"T["<<i<<"],";
        #endif
        debugout<<std::endl;
        debugout.close();
        newed = true;
    }
    debugout.open(filename.c_str(),std::ofstream::out | std::ofstream::app);
    crop_data_struct *crop = veg_var->crop_state;
    if (crop)
      VIC_land_unit_activate(crop->CropSystHandle);

    //if (veg_con->veg_class_code == 300200) {
      CropSyst::Crop_interfaced *CropSyst_crop                = VIC::active_land_unit ? VIC::active_land_unit->crop_active_or_intercrop : 0;
      //150722 Organic_matter_residues_profile_abstract *CropSyst_soil = VIC::active_land_unit ? VIC::active_land_unit->organic_matter_residues_profile_kg_m2 : 0;
      bool active_crop = CropSyst_crop ? true : false;
      double total_transpiration = 0.0;
      double total_transpiration_daily_from_CropSyst = 0.0;
      double profile_moisture   = get_total_soil_moisture(*cell.layer);
      double profile_thickness  = get_total_soil_thickness_mm(*soil);
      double profile_vwc        = profile_moisture / profile_thickness;
      for (int i = 0; i < options.Nlayer; i++) {
          total_transpiration                     += cell->layer[i].evap;
          total_transpiration_daily_from_CropSyst += CropSyst_crop ? VIC::active_land_unit->ref_VIC_cell().layer[i].transpiration_daily : 0.0;
      }
      #ifdef CHECK_WATER_BALANCE
      double final_total_water = profile_moisture
                                 + m_to_mm(snow->swq)
                                 + m_to_mm(snow->snow_canopy)
                                 + veg_var->Wdew;
      double inflow            = (mu > 1e-6 ? (atmos->prec[options.NR] / mu * soil->Pfactor[band]) : 0.0)
                                 + (crop ? crop->irrigation_water : 0);
      double outflow           = cell->runoff
                                 + cell->baseflow
                                 + cell->layer[0].evap_bare_soil
                                 + veg_var->canopyevap
                                 + total_transpiration
                                 + (crop ? crop->irrigation_evap                : 0)
                                 + (crop ? crop->irrigation_runoff              : 0)
                                 + (crop ? crop->evap_intercepted_irrig_water   : 0)
                                 + m_to_mm(snow->vapor_flux + snow->canopy_vapor_flux);
      double balance           = (final_total_water - init_total_water_mm)
                                 - (inflow - outflow);
      #endif
      debugout<<(int)global_today.get_year()
              <<","<<(int)global_today.get_month()
              <<","<<(int)global_today.get_DOM()
              <<","<<(int)global_today.get_DOY()
              <<","<<global_rec
              <<","<<(int)soil->gridcel
              <<","<<veg_con->veg_class_code
              <<","<<dist
              <<","<<band
              <<","<<active_crop
              <<","<<(active_crop ? CropSyst_crop->get_accum_degree_days(false)         : 0)
              <<","<<(active_crop ? CropSyst_crop->describe_growth_stage()              : "N/A")
              <<","<<(active_crop ? CropSyst_crop->get_LAI(true)                        : 0)
              <<","<<(active_crop ? CropSyst_crop->get_GAI(true)                        : 0)
              <<","<<(active_crop ? CropSyst_crop->get_fract_canopy_cover()             : 0)
              <<","<<(active_crop ? CropSyst_crop->get_canopy_biomass_kg_m2()           : 0)
              <<","<<(active_crop ? CropSyst_crop->get_latest_yield_kg_m2()             : 0)
              <<","<<(active_crop ? m_to_mm(CropSyst_crop->get_recorded_root_depth_m()) : 0)
              <<","<<cell->pot_evap[PET_SHORT]//(active_crop ? m_to_mm(CropSyst_crop->get_pot_transpiration_m())   : 0)
              <<","<<total_transpiration
              <<","<<(active_crop ? m_to_mm(CropSyst_crop->get_act_transpiration_m())   : 0)
              <<","<<(crop ? crop->irrigation_water               : 0)
              <<","<<(crop ? crop->irrigation_evap                : 0)
              <<","<<(crop ? crop->irrigation_runoff              : 0)
              <<","<<(crop ? crop->intercepted_irrigation         : 0)
              <<","<<(crop ? crop->evap_intercepted_irrig_water   : 0)
              <<","<<cell->layer[0].evap_bare_soil
              <<","<<veg_var->canopyevap
              <<","<<m_to_mm(snow->vapor_flux)
              <<","<<m_to_mm(snow->canopy_vapor_flux)
              <<","<<cell->runoff
              <<","<<cell->baseflow
              <<","<<(mu > 1e-6 ? (atmos->prec[options.NR] / mu * soil->Pfactor[band]) : 0.0) //150930LML atmos->prec[options.NR]
              <<","<<m_to_mm(snow->swq + snow->snow_canopy)
              <<","<<profile_moisture
              <<","<<profile_thickness
              <<","<<profile_vwc
              #ifdef CHECK_WATER_BALANCE
              <<","<<init_total_water_mm
              <<","<<final_total_water
              <<","<<inflow
              <<","<<outflow
              <<","<<balance
              #endif
              ;
        #ifdef PRINT_VIC_SOIL_LAYERS_FOR_DEBUG
        for (int i = 0; i < options.Nlayer; i++)
            debugout<<","<<cell->layer[i].moist / m_to_mm(soil->depth[i]);
        for (int i = 0; i < options.Nlayer; i++)
            debugout<<","<<cell->layer[i].moist;
        for (int i = 0; i < options.Nlayer; i++)
            debugout<<","<<m_to_mm(soil->depth[i]);
        for (int i = 0; i < options.Nlayer; i++)
            debugout<<","<<soil->Wcr[i] / m_to_mm(soil->depth[i]);
        for (int i = 0; i < options.Nlayer; i++)
            debugout<<","<<soil->max_moist[i] / m_to_mm(soil->depth[i]);
        for (int i = 0; i < options.Nlayer; i++)
            debugout<<","<<cell->layer[i].T;
        #endif
        debugout<<std::endl;
    //}
    debugout.close();
}
#endif  //DEBUG_SPECIFIC_VEG_BAND
#endif  //VIC_CROPSYST_VERSION
