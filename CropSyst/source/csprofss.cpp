#if (CROPSYST_VERSION==4)

// This will not be used in V5 because report generation will be done after run.
/*
  This unit is soil profile spreadsheet.
*/
#include "options.h"
#include "corn/measure/measures.h"
#include "static_phrases.h"
#include "csprofss.h"
#include "crop/crop_interfaced.h"
#include "common/residue/residues_interface.h"
#include "soil.h"
#include "soil/chemicals_profile.h"
#include "soil/chemical_mass_profile.h"
#include "soil/temperature_hourly.h"
#include "soil/nitrogen_profile.h"
#include "soil/salinity_profile.h"
#include "organic_matter/OM_residues_profile_abstract.h"
#include "corn/OS/directory_entry_name_concrete.h"
#include "corn/chronometry/date_const.hpp"
using namespace CORN;
//______________________________________________________________________________
Soil_spreadsheet::Soil_spreadsheet
(const CORN::OS::File_name &filename_                                            //161008
,const CropSyst::Soil_components &soil_                                          //161008
,const char                *simulation_description
,const char                *spreadsheet_description)
:BIFF234_File(filename_.c_str(),0,1000,0,256)
,P_ROW(0)
,P_COL(0)
,soil(soil_)
{  write_label_cell(0,P_ROW++,Spreadsheet_abstract::CSS_justify_left,simulation_description);
   write_label_cell(0,P_ROW++,Spreadsheet_abstract::CSS_justify_left,soil.parameters_ownable->description.brief.c_str());//150112
   write_label_cell(0,P_ROW++,Spreadsheet_abstract::CSS_justify_left,spreadsheet_description);
}
//_Soil_spreadsheet:constructor_____________________________________2001-02-23_/
Soil_horizon_spreadsheet::Soil_horizon_spreadsheet
(const CORN::OS::File_name &filename_
,const CropSyst::Soil_components &soil_                                          //161008
,const char                *sim_description)
:Soil_spreadsheet(filename_,soil_,sim_description,TL_Soil_profile)
{
   const Soil::Hydraulic_properties_interface *hydraulic_props = soil.ref_hydraulic_properties();   //160412_061108
   const Soil::Structure *soil_structure = soil.ref_structure();                 //160412_070725
   write_label_cell(0,3,Spreadsheet_abstract::CSS_justify_center,TL_Soil_layer);
   write_label_cell(0,3,Spreadsheet_abstract::CSS_justify_center,"Sublayer");
   write_label_cell(2,3,Spreadsheet_abstract::CSS_justify_center,TL_Thickness);
   write_label_cell(3,3,Spreadsheet_abstract::CSS_justify_center,TL_Vol_PWP);
   write_label_cell(4,3,Spreadsheet_abstract::CSS_justify_center,TL_Vol_FC);
   write_label_cell(5,3,Spreadsheet_abstract::CSS_justify_center,TL_Bulk_density);
   freeze_pane_row = 5;                                                          //010223
   for (nat8 sublayer = 1; sublayer <= soil.layers->count(); sublayer++)
   {  write_integer_cell(0,(int16)(sublayer+4),soil.layers->get_horizon_at_layer(sublayer) ); //051116
      write_integer_cell(1,(int16)(sublayer+4),sublayer);
      write_number_cell (2,(int16)(sublayer+4),soil.layers->get_thickness_m(sublayer),3);
      write_number_cell (3,(int16)(sublayer+4),
          hydraulic_props->get_permanent_wilt_point_volumetric(sublayer),3);     //051204
      write_number_cell (4,(int16)(sublayer+4),
          hydraulic_props->get_field_capacity_volumetric(sublayer),3);           //051204
      write_number_cell (5,(int16)(sublayer+4),
          soil_structure->get_bulk_density_g_cm3(sublayer),3);                   //070725
   }
}
//_Soil_horizon_spreadsheet:constructor________________________________________/
Soil_horizon_spreadsheet::~Soil_horizon_spreadsheet()
{  freeze_pane(2, freeze_pane_row); }
//_Soil_horizon_spreadsheet:destructor__________________2001-02-23__2000-09-26_/
Soil_profile_spreadsheet::~Soil_profile_spreadsheet()
{  freeze_pane(5, freeze_pane_row); }
//_Soil_profile_spreadsheet:destructor__________________2001-02-23__2000-09-26_/
Soil_profile_spreadsheet::Soil_profile_spreadsheet
(const CORN::OS::File_name &filename
,const CropSyst::Soil_components &soil_                                          //161009
,const char                *sim_description
,const char                *profile_description
,const char                *profile_units)
:Soil_spreadsheet(filename,soil_,sim_description,profile_description)
{
   const Soil::Hydraulic_properties_interface *hydraulic_props = soil.ref_hydraulic_properties(); //160412
   const Soil::Structure *soil_structure = soil.ref_structure();                 //160412
   nat8 number_layers = soil.layers->count();
   write_label_cell(0,P_ROW++,Spreadsheet_abstract::CSS_justify_left,profile_units);
   // write layer info
   write_label_cell(0,P_ROW,Spreadsheet_abstract::CSS_justify_left,TL_Soil_layer);
   P_COL = 4;
   for (nat8 sublayer = 1; sublayer <= number_layers; sublayer++)
      write_integer_cell((int16)(P_COL+sublayer),(int16)P_ROW,soil.layers->get_horizon_at_layer(sublayer));
   P_ROW++;
   // write sublayer info
   write_label_cell(0,P_ROW,Spreadsheet_abstract::CSS_justify_left,"Sublayer");
   P_COL = 4;
   for (nat8 sublayer = 1; sublayer <= number_layers; sublayer++)
      write_integer_cell((int16)(P_COL+sublayer),(int16)P_ROW,sublayer);
   P_ROW++;
   // write thickness
   write_label_cell(0,P_ROW,Spreadsheet_abstract::CSS_justify_left,TL_Thickness);
   P_COL = 4;
   write_label_cell(P_COL,P_ROW,Spreadsheet_abstract::CSS_justify_left,TU_m);
   for (nat8 sublayer = 1; sublayer <= number_layers; sublayer++)
      write_number_cell((int16)(P_COL+sublayer),(int16)P_ROW,soil.layers->get_thickness_m(sublayer),2);
   P_ROW++;
   // write depth
   write_label_cell(0,P_ROW,Spreadsheet_abstract::CSS_justify_left,"Depth");
   P_COL = 4;
   write_label_cell(P_COL,P_ROW,Spreadsheet_abstract::CSS_justify_left,TU_m);
   float32 depth = 0.0;
   for (nat8 sublayer = 1; sublayer <= number_layers; sublayer++)
   {  depth += soil.layers->get_thickness_m(sublayer);
      write_number_cell((int16)(P_COL+sublayer),(int16)P_ROW,depth,2);
   }
   P_ROW++;
   // write PWP
   write_label_cell(0,P_ROW,Spreadsheet_abstract::CSS_justify_left,TL_Vol_PWP);
   P_COL = 4;
   write_label_cell(P_COL,P_ROW,Spreadsheet_abstract::CSS_justify_left,TU_m3_m3);
   for (nat8 sublayer = 1; sublayer <=number_layers; sublayer++)
      write_number_cell((int16)(P_COL+sublayer),(int16)P_ROW,
          hydraulic_props->get_permanent_wilt_point_volumetric(sublayer),2);
   P_ROW++;
   // write FC
   write_label_cell(0,P_ROW,Spreadsheet_abstract::CSS_justify_left,TL_Vol_FC);
   P_COL = 4;
   write_label_cell(P_COL,P_ROW,Spreadsheet_abstract::CSS_justify_left,TU_m3_m3);
   for (nat8 sublayer = 1; sublayer <= number_layers; sublayer++)
      write_number_cell((int16)(P_COL+sublayer),(int16)P_ROW,
         hydraulic_props->get_field_capacity_volumetric(sublayer),2);
   P_ROW++;
   // write bulk density
   write_label_cell(0,P_ROW,Spreadsheet_abstract::CSS_justify_left,TL_Bulk_density);
   P_COL = 4;
   write_label_cell(P_COL,P_ROW,Spreadsheet_abstract::CSS_justify_left,TU_g_cm3);
   for (nat8 sublayer = 1; sublayer <= number_layers; sublayer++)
      write_number_cell((int16)(P_COL+sublayer),(int16)P_ROW,
         soil_structure ->get_bulk_density_g_cm3(sublayer),2);
   P_ROW++;
   P_ROW++;
   write_label_cell(0,(int16)P_ROW,Spreadsheet_abstract::CSS_justify_center,T_date_Year);
   write_label_cell(1,(int16)P_ROW,Spreadsheet_abstract::CSS_justify_center,T_date_Month);
   write_label_cell(2,(int16)P_ROW,Spreadsheet_abstract::CSS_justify_center,T_date_Day);
   write_label_cell(3,(int16)P_ROW,Spreadsheet_abstract::CSS_justify_center,T_date_Day_of_year);
   write_label_cell(4,(int16)P_ROW,Spreadsheet_abstract::CSS_justify_center,TL_Root_depth);
   P_COL = 4;
   for (nat8 sublayer = 1; sublayer <= number_layers; sublayer++)
       write_integer_cell((int16)(P_COL+sublayer),(int16)P_ROW,sublayer);
   P_COL += number_layers;
   P_ROW++;
   freeze_pane_row = P_ROW;
}
//_constructor___________________________________________2007-03-14_2001-02-23_/
void Soil_profile_spreadsheet::write_day
(const CORN::Date_const &today
,float64   root_depth
,soil_sublayer_array_64(profile))
{  write_integer_cell (0,P_ROW,today.get_year());
   write_integer_cell (1,P_ROW,today.get_month());
   write_integer_cell (2,P_ROW,today.get_DOM());
   write_integer_cell (3,P_ROW,today.get_DOY());
   write_number_cell  (4,P_ROW,root_depth,2);
   for (nat8 layer = 1; layer <= soil.layers->count(); layer++)
      write_number_cell(layer+4,P_ROW,profile[layer],2);
   P_ROW++;
}
//_write_day___________________________________________________________________/
Soil_profile_spreadsheet_set::Soil_profile_spreadsheet_set
(const CORN::OS::Directory_name  &output_soil_dir
,const CropSyst::Soil_components &soil_                                          //161008
,const char                      *sim_description
#ifdef NITROGEN
,bool                       nitrogen_sim_
#endif
#ifdef SALINITY
,bool                       salinity_sim_
#endif
,bool SS_hydraulic_properties
,bool SS_water_content
,bool SS_water_potential
,bool SS_root_fraction
,bool SS_root_biomass
#ifdef NITROGEN
,bool SS_ammonium
,bool SS_nitrate
,bool SS_denitrification
,bool SS_nitrification
,bool SS_NH4_mineralization
,bool SS_organic_matter
#endif
#ifdef RESIDUES
,bool SS_plant_residue
,bool SS_manure_residue
#endif
#ifdef SOIL_TEMPERATURE
,bool SS_temperature
#endif
#ifdef SALINITY
,bool SS_salinity
,bool SS_salt
#endif
)
: soil(soil_)
, soil_horizon_SS(0)
, water_depth_SS(0)
, cum_water_depth_SS(0)
, water_content_SS(0)
, water_potential_SS(0)
, root_fraction_SS(0)
, root_biomass_SS(0)
#ifdef NITROGEN
, nitrogen_sim(nitrogen_sim_)
, nitrate_SS(0)
, ammonium_SS(0)
, NH4_mineralization_SS(0)
, nitrification_SS(0)
, denitrification_SS(0)
, percent_organic_matter_SS(0)
#endif
#ifdef RESIDUES
, plant_residue_biomass_SS(0)
, manure_residue_biomass_SS(0)
#endif
#ifdef SALINITY
, salinity_sim(salinity_sim_)
, salinity_SS(0)
, salt_SS(0)
#endif
#ifdef SOIL_TEMPERATURE
, temperature_SS(0)
#endif
{
   if (SS_hydraulic_properties)  soil_horizon_SS            = new Soil_horizon_spreadsheet(CORN::OS::File_name_concrete(output_soil_dir,L"hydraulic_properties.xls"),soil_,sim_description);
   //051128 NYI /*000911_*/    if (SS_water_depth)
   water_depth_SS               = new Soil_profile_spreadsheet(CORN::OS::File_name_concrete(output_soil_dir,L"water_depth.xls")       ,soil_,sim_description,"Water depth",TU_mm);
   cum_water_depth_SS           = new Soil_profile_spreadsheet(CORN::OS::File_name_concrete(output_soil_dir,L"cum_water_depth.xls")       ,soil_,sim_description,"Cumulative water depth",TU_mm);
   if (SS_water_content)         water_content_SS           = new Soil_profile_spreadsheet(CORN::OS::File_name_concrete(output_soil_dir,L"water_content.xls")       ,soil_,sim_description,TL_Water_content           ,TU_m3_m3);
   if (SS_water_potential)       water_potential_SS         = new Soil_profile_spreadsheet(CORN::OS::File_name_concrete(output_soil_dir,L"water_potential.xls")     ,soil_,sim_description,TL_Water_potential         ,"J/kg"); // Warning check units
   if (SS_root_fraction)         root_fraction_SS           = new Soil_profile_spreadsheet(CORN::OS::File_name_concrete(output_soil_dir,L"Root_fraction.xls")       ,soil_,sim_description,"Root fraction"            ,"0-1");
   if (SS_root_biomass)          root_biomass_SS            = new Soil_profile_spreadsheet(CORN::OS::File_name_concrete(output_soil_dir,L"Root_biomass.xls")        ,soil_,sim_description,"Root biomass"            ,"kg/ha");
   #ifdef SOIL_TEMPERATURE
   if (SS_temperature)           temperature_SS             = new Soil_profile_spreadsheet(CORN::OS::File_name_concrete(output_soil_dir,L"temperature.xls")         ,soil_,sim_description,TL_Temperature             ,TU_degrees_C);
   #endif
   #ifdef RESIDUES
   if (SS_plant_residue)         plant_residue_biomass_SS   = new Soil_profile_spreadsheet(CORN::OS::File_name_concrete(output_soil_dir,L"plant_residue.xls")       ,soil_,sim_description,"Plant residue biomass"    ,TU_kg_ha);
   if (SS_manure_residue)        manure_residue_biomass_SS  = new Soil_profile_spreadsheet(CORN::OS::File_name_concrete(output_soil_dir,L"manure_residue.xls")      ,soil_,sim_description,"Manure residue biomass"   ,TU_kg_ha);
   #endif
   #ifdef NITROGEN
   if (nitrogen_sim)
   {
      if (SS_organic_matter)     percent_organic_matter_SS  = new Soil_profile_spreadsheet(CORN::OS::File_name_concrete(output_soil_dir,L"organic_matter.xls")   ,soil_,sim_description,TL_Percent_organic_matter  ,TU_percent);
      if (SS_nitrate)            nitrate_SS                 = new Soil_profile_spreadsheet(CORN::OS::File_name_concrete(output_soil_dir,L"nitrate.xls")          ,soil_,sim_description,TL_Nitrate                  ,TU_kgN_ha);
      if (SS_ammonium)           ammonium_SS                = new Soil_profile_spreadsheet(CORN::OS::File_name_concrete(output_soil_dir,L"ammonium.xls")         ,soil_,sim_description,TL_Ammonium                 ,TU_kgN_ha);
      if (SS_NH4_mineralization) NH4_mineralization_SS      = new Soil_profile_spreadsheet(CORN::OS::File_name_concrete(output_soil_dir,L"NH4_mineralization.xls") ,soil_,sim_description,TL_NH4_mineralization_ammonium,TU_kgN_ha);
      if (SS_nitrification)      nitrification_SS           = new Soil_profile_spreadsheet(CORN::OS::File_name_concrete(output_soil_dir,L"nitrification.xls")  ,soil_,sim_description,TL_Nitrification            ,TU_kgN_ha);
      if (SS_denitrification)    denitrification_SS         = new Soil_profile_spreadsheet(CORN::OS::File_name_concrete(output_soil_dir,L"denitrification.xls"),soil_,sim_description,TL_Denitrification          ,TU_kgN_ha);
   }
   #endif
   #ifdef SALINITY
   if (salinity_sim)
   {  if (SS_salinity)           salinity_SS                = new Soil_profile_spreadsheet(CORN::OS::File_name_concrete(output_soil_dir,L"salinity.xls")          ,soil_,sim_description,TL_Salinity                 ,TU_dS_m);
      if (SS_salt)               salt_SS                    = new Soil_profile_spreadsheet(CORN::OS::File_name_concrete(output_soil_dir,L"salt.xls")              ,soil_,sim_description,"Salt"                  ,TU_kg_ha);
   }
   #endif
}
//_Soil_profile_spreadsheet_set:constructor____________________________________/
void Soil_profile_spreadsheet_set::write_day
(const CORN::Date_const &today
, float64 root_depth
,const CropSyst::Crop_model_interface *crop                                      //050209
,Organic_matter_residues_profile_abstract *organic_matter_residue)               //050726
{
#define GET_WATER_CONTENT_VOLUMETRIC   soil.hydrology->get_liquid_water_content_volumetric
   // Not sure why were were reporting liquid water content and not total water content
   nat8 number_layers = soil.layers->count();                                    //061005
   if (water_content_SS)                                                         //000911
   {  soil_sublayer_array_64(liquid_water_content);soil.clear_sublayer_array(liquid_water_content);
      for (nat8 sublayer = 1; sublayer <= number_layers; sublayer++)
        liquid_water_content[sublayer] = GET_WATER_CONTENT_VOLUMETRIC(sublayer);
      water_content_SS           ->write_day(today,root_depth,liquid_water_content);
   }
   if (water_depth_SS)                                                           //051128
   {  soil_sublayer_array_64(water_depth);soil.clear_sublayer_array(water_depth);
      for (nat8 sublayer = 1; sublayer <= number_layers; sublayer++)
        water_depth[sublayer] = GET_WATER_CONTENT_VOLUMETRIC(sublayer)
         * m_to_mm(soil.layers->get_thickness_m(sublayer));
      water_depth_SS           ->write_day(today,root_depth,water_depth);
   }
   if (cum_water_depth_SS)                                                       //051128
   {  soil_sublayer_array_64(cum_water_depth);soil.clear_sublayer_array(cum_water_depth);
      cum_water_depth[0] = 0;
      for (nat8 sublayer = 1; sublayer <= number_layers; sublayer++)
        cum_water_depth[sublayer] = GET_WATER_CONTENT_VOLUMETRIC(sublayer)
         * m_to_mm(soil.layers->get_thickness_m(sublayer)) + cum_water_depth[sublayer-1];
      cum_water_depth_SS           ->write_day(today,root_depth,cum_water_depth);
   }
   if (water_potential_SS)                                                       //000911
   {  soil_layer_array64(water_pot);clear_layer_array64(water_pot);
      for (nat8 layer = 1; layer <= number_layers; layer++)
        water_pot[layer] = soil.hydrology->
            calc_water_potential(layer,true,false);                              //080125
      water_potential_SS         ->write_day(today,root_depth,water_pot);
   }
   if (root_fraction_SS && crop)                                                 //010315
   {  soil_sublayer_array_64(root_fraction);soil.clear_sublayer_array(root_fraction);
      for (nat8 sublayer = 1; sublayer <= number_layers; sublayer++)
        root_fraction[sublayer] = crop->get_active_fract_root_length_m(sublayer);//010322
      root_fraction_SS         ->write_day(today,root_depth,root_fraction);
   }
   if (root_biomass_SS && crop)                                                  //071214
   {  soil_sublayer_array_64(root_biomass);soil.clear_sublayer_array(root_biomass);
      crop->get_root_biomass(root_biomass); // was calc_root_biomass
      for (nat8 sublayer = 1; sublayer <= number_layers; sublayer++)
        root_biomass[sublayer] = per_m2_to_per_ha(root_biomass[sublayer]);
            // active_fract_root_length(sublayer)[sublayer];
      root_biomass_SS         ->write_day(today,root_depth,root_biomass);
   }
   #ifdef SOIL_TEMPERATURE
   if (temperature_SS)
   {  soil.provide_temperature_profile(); // This will force the soil temperature profile to be created.
      soil_sublayer_array_64(temperature);soil.clear_sublayer_array(temperature);
      for (nat8 sublayer = 1; sublayer <= number_layers; sublayer++)
      {  temperature[sublayer] = soil.get_temperature(sublayer);
         if (soil.temperature_profile_hourly)
          temperature[sublayer] = soil.temperature_profile_hourly->get_avg_temperature(sublayer);
      }
      temperature_SS->write_day(today,root_depth,temperature);
   }
   #endif
   if (organic_matter_residue)                                                   //080125
   {
      #ifdef RESIDUES
      if (plant_residue_biomass_SS)
      {  soil_sublayer_array_64(plant_residue); soil.clear_sublayer_array(plant_residue);
                 organic_matter_residue->mod_residues()->
            get_subsurface_plant_biomass(plant_residue);
         for (nat8 sublayer = 1; sublayer <= number_layers; sublayer++)
            plant_residue[sublayer] = per_m2_to_per_ha(plant_residue[sublayer]);
         plant_residue_biomass_SS   ->write_day(today,root_depth,plant_residue);
      }
      if (manure_residue_biomass_SS)
      {  soil_sublayer_array_64(manure_residue);soil.clear_sublayer_array(manure_residue);
         if (organic_matter_residue)                                                //050726
            organic_matter_residue->mod_residues()->get_subsurface_manure_biomass(manure_residue);
         for (nat8 sublayer = 1; sublayer <= number_layers; sublayer++)
            manure_residue[sublayer] = per_m2_to_per_ha(manure_residue[sublayer]);
         manure_residue_biomass_SS ->write_day(today,root_depth,manure_residue);
      }
      #endif
   }
   #ifdef NITROGEN
   if (nitrogen_sim && soil.chemicals)
   {  if (nitrate_SS)
      {  soil_sublayer_array_64(NO3_kg_ha);
         for (nat8 sublayer = 1; sublayer <= number_layers; sublayer++)
            NO3_kg_ha[sublayer] = per_m2_to_per_ha(soil.chemicals->get_elemental_N_from_NO3(sublayer));
         nitrate_SS              ->write_day(today,root_depth,NO3_kg_ha);
      }
      if (ammonium_SS)
      {  soil_sublayer_array_64(NH4_kg_ha);
         for (nat8 sublayer = 1; sublayer <= number_layers; sublayer++)
            NH4_kg_ha[sublayer] = per_m2_to_per_ha(soil.chemicals->get_elemental_N_from_NH4(sublayer));
         ammonium_SS             ->write_day(today,root_depth,NH4_kg_ha);
      }
      if (organic_matter_residue)
      {
         if (NH4_mineralization_SS)
         {  soil_sublayer_array_64(mineralized_kg_ha);
            for (nat8 sublayer = 1; sublayer <= number_layers; sublayer++)
               mineralized_kg_ha[sublayer] = per_m2_to_per_ha
                  ( organic_matter_residue->get_soil_organic_matter_mineralization_N(sublayer));
            NH4_mineralization_SS   ->write_day(today,root_depth,mineralized_kg_ha);
         }
         if (percent_organic_matter_SS && organic_matter_residue)
         {  soil_sublayer_array_64(percent_organic_matter);
            for (nat8 sublayer = 1; sublayer <= number_layers; sublayer++)
            {  percent_organic_matter[sublayer] =
                  organic_matter_residue ? organic_matter_residue->get_percent_organic_matter(sublayer) : 0.0;
            }
            percent_organic_matter_SS->write_day(today,root_depth,percent_organic_matter);
         }
      }
      if (nitrification_SS)
      {  soil_sublayer_array_64(nitrified_NH4_N_kg_ha);
         for (nat8 sublayer = 1; sublayer <= number_layers; sublayer++)
         {  float64 nitrified_kg_m2 =
                  #ifdef OLD_N_XFER
                  #error reached
                   soil.chemicals->NO3->molecular_to_elemental*
                  (soil.chemicals->NO3->transformed_from_M[sublayer]);
                  #else
                  soil.chemicals->NH4->get_nitrified_kgN_m2(sublayer);           //170502
                  #endif
            nitrified_NH4_N_kg_ha[sublayer] = per_m2_to_per_ha(nitrified_kg_m2); //170502
         }
         nitrification_SS        ->write_day(today,root_depth,nitrified_NH4_N_kg_ha);
      }
      if (denitrification_SS)
      {  soil_sublayer_array_64(denitrified_NO3_kg_ha);
         for (nat8 sublayer = 1; sublayer <= number_layers; sublayer++)
         {
            float64 denitrified_kg_m2 =
                  #ifdef OLD_N_XFER
                  #error reached
                   denitrification_N_E_output[sublayer]
                  #else
                  soil.chemicals->NO3->get_denitrified_kgN_m2(sublayer);         //170502
                  #endif
            denitrified_NO3_kg_ha[sublayer]=per_m2_to_per_ha(denitrified_kg_m2); //170502

/*170503
            denitrified_NO3_kg_ha[sublayer] =
               per_m2_to_per_ha(soil.chemicals->
               denitrification_N_E_output[sublayer]);                             //130913
*/
         // This needs to be done as for nitrification   170503

         }
         denitrification_SS      ->write_day(today,root_depth,denitrified_NO3_kg_ha);
      }
   }
   #endif
   #ifdef SALINITY
   if (salinity_SS)
   {
      //041002 Note this is now caculated also in soilchem (I am not sure if I am doing it in the right place)
      //       Because this is also output for new UED I have this here just to make sure it is calculated (eventually these spreadsheet outputs will be obsolete anyway)
      for (nat8 sublayer = 1;sublayer <= number_layers; sublayer++)
         soil.chemicals->salinity_dSm->salinity_profile->salinity_ECw_dSm_input_output[sublayer]
            = soil.chemicals->salinity_dSm->get_concentration_dSm(sublayer);
      salinity_SS->write_day(today,root_depth,soil.chemicals->salinity_dSm->salinity_profile->salinity_ECw_dSm_input_output);
   }
   if (salt_SS)
   {  soil_layer_array64(salt_kg_ha);
      for (nat8 layer = 1;layer <= number_layers; layer++)
         salt_kg_ha[layer] = 10000.0 * soil.chemicals->salinity_dSm->salinity_profile->
            mass_M->get_layer(layer);
      salt_SS->write_day(today,root_depth,salt_kg_ha);
   }
   #endif
}
//_write_day___________________________________________________________________/
Soil_profile_spreadsheet_set::~Soil_profile_spreadsheet_set()
{  if (soil_horizon_SS)             delete soil_horizon_SS;
   if (water_content_SS)            delete water_content_SS;
   if (water_depth_SS)              delete water_depth_SS;
   if (cum_water_depth_SS)          delete cum_water_depth_SS;
   if (water_potential_SS)          delete water_potential_SS;
   if (root_fraction_SS)            delete root_fraction_SS;
   if (root_biomass_SS)             delete root_biomass_SS;
   #ifdef SOIL_TEMPERATURE
   if (temperature_SS)              delete temperature_SS;
   #endif
   #ifdef RESIDUES
   if (plant_residue_biomass_SS)    delete plant_residue_biomass_SS;
   if (manure_residue_biomass_SS)   delete manure_residue_biomass_SS;
   #endif
   #ifdef NITROGEN
   if (nitrate_SS)                  delete nitrate_SS;
   if (ammonium_SS)                 delete ammonium_SS;
   if (NH4_mineralization_SS)       delete NH4_mineralization_SS;
   if (nitrification_SS)            delete nitrification_SS;
   if (denitrification_SS)          delete denitrification_SS;
   if (percent_organic_matter_SS)   delete percent_organic_matter_SS;
   #endif
   #ifdef SALINITY
   if (salinity_SS)                 delete salinity_SS;
   if (salt_SS)                     delete salt_SS;
   #endif
}
//_Soil_profile_spreadsheet_set:destructor_____________________________________/
#endif

