//Method implementations common to both the CropSyst V4 and V5 soil editor form
#include "form_soil_editor.h"
#include "CropSyst/source/soil/soil_param.h"
#include "corn/OS/file_system_engine.h"
#if (CS_VERSION==4)
#  include "../../help/soil.h"
#endif
#include "form_soil_texture.h"
#include "corn/math/moremath.h"
#include "corn/math/compare.hpp"
//______________________________________________________________________________
Soil_parameters::user_specified_fields texture_order_edited[MAX_soil_horizons_alloc][3];
//______________________________________________________________________________
void __fastcall Tsoil_editor_form::bind_to_V4_and_V5(Smart_soil_parameters *i_soil,Tparameter_file_form *i_parameter_form)
{
parameter_file_form = i_parameter_form;
   Application->HintHidePause = 8000;                                            //101014
   texture_grid->RowCount = MAX_soil_horizons + 1;                               //101014
   active_layer = 1;                                                             //101014
   soil = i_soil;
   parameter_file_form = i_parameter_form;
   for (int i = 1; i <= MAX_soil_horizons; i++)
   {  int order = 0;
      nat16 specified = (nat16)(soil->user[i]) & (nat16)Soil_parameters::specified_sand;  //981123
      if (specified)
      {  texture_order_edited[i][order] = specified;
         order ++;
      }
      specified = soil->user[i] & Soil_parameters::specified_clay;               //981123
      if (specified)
      {  texture_order_edited[i][order] = specified;
         order ++;
      }
      specified =soil->user[i] & Soil_parameters::specified_silt;                //981123
      if (specified)
      {  texture_order_edited[i][order] = specified;
         order ++;
      }
  }
   setup_hydr_prop_calc_button();                                                //071205
   edit_soil_layers           ->bind_to(&(soil->horizon_count) ,10);             //080207
   radiogroup_hydro_group     ->bind_to(&(soil->hydrologic_group_labeled) , 0);
   radiogroup_hydro_condition ->bind_to(&(soil->hydrologic_condition_labeled),0);
   checkbox_compute_surface_storage->bind_to (&(soil->compute_surface_storage) V4_HELP_URL(HELP_P_soil_evap_layer)); //070521_
}
//__________________________________________________________bind_to_V4_and_V5__/
void __fastcall Tsoil_editor_form::update_properties(int layer,bool clear_fixed_fields)
{
   if (!clear_fixed_fields && soil->layer_texture_hydraulics[layer])
   {
      if (soil->user[layer] & Soil_parameters::specified_bulk_density)        soil->layer_texture_hydraulics[layer]->bulk_density_status        = Soil_texture_hydraulics::fixed;
      if (soil->user[layer] & Soil_parameters::specified_PWP)                 soil->layer_texture_hydraulics[layer]->vol_WC_at_1500_status      = Soil_texture_hydraulics::fixed;
      if (soil->user[layer] & Soil_parameters::specified_FC)                  soil->layer_texture_hydraulics[layer]->vol_WC_at_33_status        = Soil_texture_hydraulics::fixed;

      if (soil->user[layer] & Soil_parameters::specified_water_pot_at_FC)           soil->layer_texture_hydraulics[layer]->water_pot_at_FC_status     = Soil_texture_hydraulics::fixed;
      if (soil->user[layer] & Soil_parameters::specified_sat_hydraul_cond)    soil->layer_texture_hydraulics[layer]->sat_hydraul_cond_m_d_status= Soil_texture_hydraulics::fixed;
      if (soil->user[layer] & Soil_parameters::specified_saturation)          soil->layer_texture_hydraulics[layer]->saturation_status          = Soil_texture_hydraulics::fixed; //080313_
      if (soil->user[layer] & Soil_parameters::specified_air_entry_pot)       soil->layer_texture_hydraulics[layer]->air_entry_pot_status       = Soil_texture_hydraulics::fixed;
      if (soil->user[layer] & Soil_parameters::specified_soil_B)              soil->layer_texture_hydraulics[layer]->Campbell_b_status          = Soil_texture_hydraulics::fixed;
   }
  if (soil && soil->layer_texture_hydraulics[layer])
  {
      soil->user[layer] &= ~(Soil_parameters::specified_sand | Soil_parameters::specified_clay | Soil_parameters::specified_silt);
      soil->user[layer] |= texture_order_edited[layer][0] | texture_order_edited[layer][1] ;
      if ((soil->user[layer]& soil->specified_clay) && (soil->user[layer]& soil->specified_silt))
      {
         float32 sand
            = 100.0 - (soil->properties.clay.get(layer) +soil->properties.silt.get(layer));
            soil->properties.sand.set(layer,sand);
      }
      if ((soil->user[layer]& soil->specified_sand) && (soil->user[layer]& soil->specified_silt))
      {
         float32 clay
            = 100.0
            - (  soil->properties.sand.get(layer)
               + soil->properties.silt.get(layer));
         soil->properties.clay.set(layer,clay);
      }
      if ((soil->user[layer]& soil->specified_sand) && (soil->user[layer]& soil->specified_clay))
      {
         float32 silt
            = 100 - (  soil->properties.sand.get(layer)
                     + soil->properties.silt.get(layer));
         soil->properties.silt.set(layer,silt);
      }
      if (clear_fixed_fields)
      {
         soil->fix_bulk_density         ( layer, false);
         soil->fix_perm_wilt_point      ( layer, false);
         soil->fix_field_capacity       ( layer, false);
         soil->fix_water_pot_at_FC      ( layer, false);
         #ifdef CHECK_OBSOLETE
         130128
         soil->fix_vol_WC_at_1500       ( layer, false);
         soil->fix_vol_WC_at_33         ( layer, false);
         #endif
         soil->fix_sat_hydraul_cond_m_d ( layer, false);
         soil->fix_saturation_WC        ( layer, false);                         //080313
         /*170717 now always calculated
         soil->fix_air_entry_pot        ( layer, false);                         //050401
         soil->fix_Campbell_b           ( layer, false);                         //050401
         */
      }
      if (   (soil->properties.sand.get(layer) > 0)
          && (soil->properties.clay.get(layer) > 0)
          && (soil->properties.silt.get(layer) > 0) &&
           ((  soil->properties.sand.get(layer)
             + soil->properties.clay.get(layer)
             + soil->properties.silt.get(layer)) <100.1))                        //170717_981123
        soil->layer_texture_hydraulics[layer]->recompute_every_thing();
  }
}
//__________________________________________________________update_properties__/
void __fastcall Tsoil_editor_form::Invalidate(void)
{
   if (soil)
   for (int active_layer = 1; active_layer <=12; active_layer++)
      soil_editor_form->update_properties(active_layer,false);
   update_grids();
}
//_________________________________________________________________Invalidate__/
void __fastcall Tsoil_editor_form::Update()
{
   edit_soil_layers           ->Update();                                        //080207
   radiogroup_hydro_group     ->Update();
   radiogroup_hydro_condition ->Update();
   edit_steepness             ->Update();
   edit_slope_length          ->Update();
   edit_fallow_curve          ->Update();
   edit_override_curve_number ->Update();                                        //080430
   checkbox_compute_surface_storage->Update();                                   //070521
   edit_surface_storage       ->Update();
   edit_albedo_dry            ->Update();                                        //041203
   edit_albedo_wet            ->Update();                                        //041203
   edit_water_vapor_conductance_atmosphere_adj->Update();                        //041206
   update_grids();                                                               //040912
   for (int i = 1; i <= MAX_soil_horizons; i++)
   {    int order = 0;
          texture_order_edited[i][order] = true;
          order ++;
          texture_order_edited[i][order] = true;
          order ++;
          texture_order_edited[i][order] = true;
          order ++;
   }
   /*
   edit_evaporative_layer_thickness_cascade->Update();                           //120804
   edit_evaporative_layer_thickness_finite_difference->Update();                 //120804
   */
   edit_SLPF->Update();
   TForm::Update();
   show_hide_controls();                                                         //070521
}
//_____________________________________________________________________Update__/
void Tsoil_editor_form::show_hide_controls()
{  edit_surface_storage->Visible = !(soil->compute_surface_storage);
}
//_________________________________________________________show_hide_controls__/
void Tsoil_editor_form::update_grid(int horizon)
{
   if (!soil) return;
   update_properties(horizon,false);

   texture_grid->Ints[0][horizon] =  horizon;

   texture_grid->Floats[1][horizon] = soil->properties.thickness.get(horizon);
   texture_grid->Floats[2][horizon] = soil->properties.sand.get(horizon);
   texture_grid->Floats[3][horizon] = soil->properties.clay.get(horizon);
   texture_grid->Floats[4][horizon] = soil->properties.silt.get(horizon);
   texture_grid->Floats[5][horizon] = soil->properties.bypass_coef.get(horizon);

   texture_grid->Floats[6][horizon] = soil->properties.permanent_wilt_point.get(horizon);
   texture_grid->Floats[7][horizon] = soil->properties.field_capacity.get(horizon);
   texture_grid->Floats[8][horizon] = soil->properties.bulk_density_g_cm3.get(horizon);
   texture_grid->Floats[9][horizon] = soil->properties.water_pot_at_FC.get(horizon);
   texture_grid->Floats[11][horizon] = soil->properties.sat_hydraul_cond_m_d.get(horizon);
   texture_grid->Floats[12][horizon] = soil->properties.air_entry_pot.get(horizon);
   texture_grid->Floats[13][horizon] = soil->properties.Campbell_b.get(horizon);

   texture_grid->Floats[14][horizon] = soil->properties.saturation_WC.get(horizon);
   texture_grid->Floats[15][horizon] = soil->properties.cation_exchange_capacity.get(horizon);
   texture_grid->Floats[16][horizon] = soil->properties.pH.get(horizon);
}
//________________________________________________________________update_grid__/
void __fastcall Tsoil_editor_form::texture_clicked(nat8 layer)
{
   if (soil)
   {
      float32 l_vol_WC_at_33        = soil->properties.field_capacity.get(layer);//130326
      float32 l_vol_WC_at_1500      = soil->properties.permanent_wilt_point.get(layer); //130326
      float32 l_sand                = soil->properties.sand.get(layer);
      float32 l_clay                = soil->properties.clay.get(layer);
      float32 l_silt                = soil->properties.silt.get(layer);
      float32 l_sat_hydraul_cond_m_d= soil->properties.sat_hydraul_cond_m_d.get(layer);
      float32 l_saturation_WC       = soil->properties.saturation_WC.get(layer);        //080313
      float32 l_bulk_density        = soil->properties.bulk_density_g_cm3.get(layer);
      float32 l_SFsoil_b            = soil->properties.Campbell_b.get(layer);
      float32 l_SFair_entry_pot     = soil->properties.air_entry_pot.get(layer);

      Tsoil_texture_form *td = new Tsoil_texture_form(this);
      td->bind_to
         (&l_vol_WC_at_33
         ,&l_vol_WC_at_1500
         ,&l_sand
         ,&l_clay
         ,&l_silt
         ,&l_sat_hydraul_cond_m_d
         ,&l_bulk_density
         ,&l_SFsoil_b
         ,&l_SFair_entry_pot
         ,parameter_file_form
         );

      if (td->ShowModal() == mrOk)
      {
         soil->properties.field_capacity        .set(layer,l_vol_WC_at_33);
         soil->properties.permanent_wilt_point  .set(layer,l_vol_WC_at_1500);
         soil->properties.sand                  .set(layer,l_sand);
         soil->properties.clay                  .set(layer,l_clay);
         soil->properties.silt                  .set(layer,l_silt);
         soil->properties.sat_hydraul_cond_m_d  .set(layer,l_sat_hydraul_cond_m_d);
         soil->properties.saturation_WC         .set(layer,l_saturation_WC);            //080313
         soil->properties.bulk_density_g_cm3    .set(layer,l_bulk_density);
         soil->properties.Campbell_b            .set(layer,l_SFsoil_b);
         soil->properties.air_entry_pot         .set(layer,l_SFair_entry_pot);
         soil->user[layer] |= Soil_parameters::specified_sand;
         soil->user[layer] |= Soil_parameters::specified_clay;
         soil->user[layer] |= Soil_parameters::specified_silt;
         if ( soil->layer_texture_hydraulics[layer])
         soil->layer_texture_hydraulics[layer]->texture_status = Soil_texture_hydraulics::fixed;
         update_properties(layer,true);                                          //040912
      }
      delete(td);
   }
   update_grid(layer);                                                           //040912
   update_grid_colors(layer);                                                    //080207
}
//____________________________________________________________texture_clicked__/
void __fastcall Tsoil_editor_form::calculate_clicked(nat8 layer)
{
   soil->user[layer] |= Soil_parameters::specified_sand;
   soil->user[layer] |= Soil_parameters::specified_clay;
   soil->user[layer] |= Soil_parameters::specified_silt;
};
//_calculate_clicked______________________________________________1998-05-29_/
void Tsoil_editor_form::update_properties_on_cell_change( int ACol, int ARow)
{
   if (!soil) return;
   int horizon= ARow;
   int prev_horizons = soil->number_horizons;
   soil->number_horizons = std::max<int>(soil->number_horizons,horizon);
   if (soil->number_horizons !=prev_horizons)
   {
      edit_soil_layers->Update();
   }
   switch (ACol)
   {
      case 1 : soil->horizon_thickness [horizon] = texture_grid->Floats[1][horizon]  ; break;
      case 2 : soil->horizon_sand      [horizon] = texture_grid->Floats[2][horizon]  ; soil->user[horizon] |= Soil_parameters::specified_sand; push_texture_order(horizon,Soil_parameters::specified_sand);  break;
      case 3 : soil->horizon_clay      [horizon] = texture_grid->Floats[3][horizon]  ; soil->user[horizon] |= Soil_parameters::specified_clay; push_texture_order(horizon,Soil_parameters::specified_clay);  break;
      case 4 : soil->horizon_silt      [horizon] = texture_grid->Floats[4][horizon]  ; soil->user[horizon] |= Soil_parameters::specified_silt; push_texture_order(horizon,Soil_parameters::specified_silt);  break;
      case 5 : soil->bypass_coef       [horizon] = texture_grid->Floats[5][horizon]  ; break;

      case 6 : soil->horizon_permanent_wilt_point  [horizon]= texture_grid->Floats[6][horizon]; soil->user[horizon] |= Soil_parameters::specified_PWP;                break;
      case 7 : soil->horizon_field_capacity        [horizon]= texture_grid->Floats[7][horizon]; soil->user[horizon] |= Soil_parameters::specified_FC;                 break;
      case 8 : soil->horizon_bulk_density_g_cm3    [horizon]= texture_grid->Floats[8][horizon]; soil->user[horizon] |= Soil_parameters::specified_bulk_density;       break;
      case 9 : soil->water_pot_at_FC               [horizon]= texture_grid->Floats[9][horizon]; soil->user[horizon] |= Soil_parameters::specified_water_pot_at_FC;        break; //130326
      case 11: soil->sat_hydraul_cond_m_d          [horizon]= texture_grid->Floats[11][horizon]; soil->user[horizon] |= Soil_parameters::specified_sat_hydraul_cond;  break;
      case 12: soil->horizon_air_entry_pot         [horizon]= texture_grid->Floats[12][horizon]; soil->user[horizon] |= Soil_parameters::specified_air_entry_pot;     break;
      case 13: soil->horizon_Campbell_b            [horizon]= texture_grid->Floats[13][horizon]; soil->user[horizon] |= Soil_parameters::specified_soil_B;            break;
      case 14: soil->horizon_saturation_WC         [horizon]= texture_grid->Floats[14][horizon]; soil->user[horizon] |= Soil_parameters::specified_saturation;        break;

//080313 now saturation is an input       //   14 : Saturation is shown only for output

      case 15 : soil->cation_exchange_capacity     [horizon]= texture_grid->Floats[15][horizon];   break;
      case 16 : soil->pH                           [horizon]= texture_grid->Floats[16][horizon];  break;
   }
   if ((ACol >= 2) && (ACol <= 3) &&  soil->layer_texture_hydraulics[horizon])
   {
      soil->layer_texture_hydraulics[horizon]->texture_status = Soil_texture_hydraulics::fixed;
      soil->user[horizon] = Soil_parameters::specified_sand | Soil_parameters::specified_clay | Soil_parameters::specified_silt;
   }
   update_properties(horizon,false);
   update_grid(horizon);
   update_grid_colors(horizon);
}
//_update_properties_on_cell_change____________________________________________/
void Tsoil_editor_form::update_grids()
{  if (!soil) return;
   for (int horizon = 1; horizon <= MAX_soil_horizons; horizon++)
   {
      update_grid(horizon);
      update_grid_colors(horizon);
   }
}
//_update_grids________________________________________________________________/
TColor Tsoil_editor_form::get_status_color(Soil_texture_hydraulics::Status status)
{  TColor status_color = clWhite;
   switch (status)
   {  case  Soil_texture_hydraulics::unknown             :  status_color = clFuchsia;   break;
      case  Soil_texture_hydraulics::estimated           :  status_color = clAqua;      break;
      case  Soil_texture_hydraulics::computed_from_fixed :  status_color = clLime;      break;
      case  Soil_texture_hydraulics::fixed               :  status_color = clYellow;    break;
      case  Soil_texture_hydraulics::other               :  status_color = clBlue;      break;
   }
   return status_color;
}
//_get_status_color____________________________________________________________/
void Tsoil_editor_form::update_grid_colors(int horizon)
{  if (!soil) return;
   TColor default_layer_color = (horizon > soil->number_horizons)
      ? clBtnShadow
      : clWhite;

   for (int c = 1; c <= 14; c++)
      texture_grid->Colors[c][horizon] = default_layer_color;
   if (horizon <= soil->number_horizons)
   {
   if (soil->user[horizon])
   {  texture_grid->Colors[2][horizon] =  soil->user[horizon]&Soil_parameters::specified_sand ? clYellow : clLime;
      texture_grid->Colors[3][horizon] =  soil->user[horizon]&Soil_parameters::specified_clay ? clYellow : clLime;
      texture_grid->Colors[4][horizon] =  soil->user[horizon]&Soil_parameters::specified_silt ? clYellow : clLime;
   }
   if (soil->layer_texture_hydraulics[horizon])
   {
   texture_grid->Colors[6][horizon] = get_status_color(soil->layer_texture_hydraulics[horizon]->vol_WC_at_1500_status);
   texture_grid->Colors[7][horizon] = get_status_color(soil->layer_texture_hydraulics[horizon]->vol_WC_at_33_status);
   texture_grid->Colors[8][horizon] = get_status_color(soil->layer_texture_hydraulics[horizon]->bulk_density_status);
   texture_grid->Colors[9][horizon] = get_status_color(soil->layer_texture_hydraulics[horizon]->water_pot_at_FC_status); //130326
//130326   texture_grid->Colors[9][horizon] = get_status_color(soil->layer_texture_hydraulics[horizon]->vol_WC_at_1500_status);
//130326   texture_grid->Colors[10][horizon] = get_status_color(soil->layer_texture_hydraulics[horizon]->vol_WC_at_33_status);
   texture_grid->Colors[11][horizon] = get_status_color(soil->layer_texture_hydraulics[horizon]->sat_hydraul_cond_m_d_status);
   texture_grid->Colors[12][horizon] = get_status_color(soil->layer_texture_hydraulics[horizon]->air_entry_pot_status);
   texture_grid->Colors[13][horizon] = get_status_color(soil->layer_texture_hydraulics[horizon]->Campbell_b_status);
   texture_grid->Colors[14][horizon] = get_status_color(soil->layer_texture_hydraulics[horizon]->saturation_status);
   }
   }
}
//______________________________________________________________________________
void  Tsoil_editor_form::push_texture_order(nat8 layer,nat16 latest_texture_edited)
{
   for (int i = 0; i< 2;i++)
   { if (texture_order_edited[layer][i] == latest_texture_edited)
         texture_order_edited[layer][i] = texture_order_edited[layer][i+1];
   }
   for (int i = 2; i> 0;i--)
      texture_order_edited[layer][i] = texture_order_edited[layer][i-1];
   texture_order_edited[layer][0] = latest_texture_edited;
}
//______________________________________________________________________________
void Tsoil_editor_form::setup_hydr_prop_calc_button()
{  // Enable button for Hydraulic properties calculator if installed
      bool show_hydr_prop_calc_button = false;
      {
         CORN::OS::Directory_name_logical program_files_SPAW_hydrology_dir(0x0026 /*CSIDL_PROGRAM_FILES*/,L"SPAW Hydrology",L"");   //120322_091030
         if (CORN::OS::file_system_engine.exists(program_files_SPAW_hydrology_dir))
         {
            CORN::OS::Directory_name_logical program_files_SPAW_hydrology_dir_Soil_Water_Characteristics(program_files_SPAW_hydrology_dir,L"Soil Water Characteristics"); //091030_
            {  std::string name_ext;                                             //120614
               if (CORN::OS::file_system_engine.exists(program_files_SPAW_hydrology_dir))
               CORN::OS::File_name_logical SWC(program_files_SPAW_hydrology_dir,SoilWaterCharacteristics_exe.append_name_with_ext_to(name_ext)); //120614
               show_hydr_prop_calc_button = true;
            }
         }
      }
      button_hydr_prop_calc->Visible =  show_hydr_prop_calc_button;
      label_install_hydr_prop_calc->Visible = ! show_hydr_prop_calc_button;
};
//_2070-12-05______________________________________________________________________________
void Tsoil_editor_form::clear_layer_status()
{
   int horizon = texture_grid->Row;
   if (!soil->layer_texture_hydraulics[horizon]) return;
   switch (texture_grid->Col)
   {
      case 6 : soil->layer_texture_hydraulics[horizon]->vol_WC_at_1500_status      = Soil_texture_hydraulics::unknown; break;
      case 7 : soil->layer_texture_hydraulics[horizon]->vol_WC_at_33_status        = Soil_texture_hydraulics::unknown; break;
      case 8 : soil->layer_texture_hydraulics[horizon]->bulk_density_status        = Soil_texture_hydraulics::unknown; break;
//130326      case 9 : soil->layer_texture_hydraulics[horizon]->vol_WC_at_1500_status      = Soil_texture_hydraulics::unknown; break;
//130326      case 10: soil->layer_texture_hydraulics[horizon]->vol_WC_at_33_status        = Soil_texture_hydraulics::unknown; break;
      case 11:soil->layer_texture_hydraulics[horizon]->sat_hydraul_cond_m_d_status = Soil_texture_hydraulics::unknown; break;
      case 12: soil->layer_texture_hydraulics[horizon]->air_entry_pot_status       = Soil_texture_hydraulics::unknown; break;
      case 13: soil->layer_texture_hydraulics[horizon]->Campbell_b_status          = Soil_texture_hydraulics::unknown; break;
      case 14: soil->layer_texture_hydraulics[horizon]->saturation_status          = Soil_texture_hydraulics::unknown; break;
   }
   switch (texture_grid->Col)
   {
      case 6 : soil->user[horizon] &= ~Soil_parameters::specified_PWP;             break;
      case 7 : soil->user[horizon] &= ~Soil_parameters::specified_FC;              break;
      case 8 : soil->user[horizon] &= ~Soil_parameters::specified_bulk_density;    break;
      case 9 : soil->user[horizon] &= ~Soil_parameters::specified_water_pot_at_FC; break;//130326
//131326      case 9 : soil->user[horizon] &= ~Soil_parameters::specified_vol_WC_1500;     break;
//131326      case 10: soil->user[horizon] &= ~Soil_parameters::specified_vol_WC_33;       break;
      case 11: soil->user[horizon] &= ~Soil_parameters::specified_sat_hydraul_cond;break;
      case 12: soil->user[horizon] &= ~Soil_parameters::specified_air_entry_pot;   break;
      case 13: soil->user[horizon] &= ~Soil_parameters::specified_soil_B;          break;
      case 14: soil->user[horizon] &= ~Soil_parameters::specified_saturation;      break;
   }
   update_properties(horizon,false); // clear_fixed_fields)
   update_grid(horizon);
   update_grid_colors(horizon);
}
//_____________________________________________________________________________/
void Tsoil_editor_form::texture_grid_autodeleterow(int ARow)
{  if (!soil) return;
   soil->delete_horizon(ARow);
   for (int layer = 1; layer <= MAX_soil_horizons; layer++)
         texture_grid->Ints[0][layer] =  layer;
   edit_soil_layers->Update();                                                   //080207
   update_grids();
   texture_grid->RowCount =  MAX_soil_horizons+1;
}
//_texture_grid_autodeleterow__________________________________________________/
void Tsoil_editor_form::texture_grid_autoinsertrow(int ARow)
{
   if (!soil) return;
   soil->insert_horizon(ARow);
   edit_soil_layers->Update();                                                   //080207
   update_grid(ARow);
   update_grid_colors(ARow);                                                     //080207
   for (int layer = 1; layer <= MAX_soil_horizons; layer++)
         texture_grid->Ints[0][layer] =  layer;
   texture_grid->RowCount =  MAX_soil_horizons+1;
}
//_texture_grid_autoinsertrow__________________________________________________/

