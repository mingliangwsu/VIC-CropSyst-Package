//---------------------------------------------------------------------------

#ifndef crop_editor_frmH
#define crop_editor_frmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Dialogs.hpp>
#include <jpeg.hpp>
#include <Graphics.hpp>
#include "HTMLText.hpp"
#include "rtflabel.hpp"
#include "AdvGroupBox.hpp"
#include "AdvPanel.hpp"
#include "AdvOfficePager.hpp"
//---------------------------------------------------------------------------
#include "CS_Suite/CropSyst/crop_editor/form_crop_editor_inc.h"
#include <CheckLst.hpp>
#include "RNAutoCheckBox.h"
#include "RNAutoContainerCheckListBox.h"
#include "RNAutoContainerCheckListBox.h"
#include "RNAutoDateButton.h"
#include "RNAutoFloatEdit.h"
#include "RNAutoIntegerEdit.h"
#include "RNAutoParameterEditBar.h"
#include "RNAutoRadioGroup.h"
#include "RNAutoContainerListBox.h"
#include "AdvEdBtn.hpp"
#include "AdvEdit.hpp"
#include "AdvFileNameEdit.hpp"
//CROP_EDITOR_INCLUDES_AND_FORWARD_DECLS
/*
   class Tparameter_file_form;                                                 \
   namespace CORN                                                              \
   {  namespace OS {class File_name; }                                         \
   }
*/   
//---------------------------------------------------------------------------
#ifdef CSTRNGEM_H
#  include "corn/cstrngem.h"
#endif
#  include "corn/OS/directory_entry_name_concrete.h"
#include "common/simulation/synchronization.h"
class Crop_parameters;
namespace CORN
{
   class Bidirectional_list;
};
class Tparameter_file_form;

class TCrop_editor_form : public TForm
{
__published:	// IDE-managed Components
   TPageControl *crop_pagecontrol;
   TTabSheet *classification_sheet;
   TTabSheet *layout_sheet;
   TTabSheet *tabsheet_attainable_growth;
   TTabSheet *sheet_canopy_leaf;
   TTabSheet *transpiration_sheet;
   TTabSheet *phenology_sheet;
   TTabSheet *vernalization_sheet;
   TTabSheet *photoperiod_sheet;
   TTabSheet *harvest_sheet;
   TTabSheet *dormancy_sheet;
   TTabSheet *hardiness_sheet;
   TRNAutoParameterEditBar *edit_ET_crop_coef;
   TRNAutoParameterEditBar *edit_max_water_uptake_mm;
   TRNAutoParameterEditBar *edit_leaf_water_pot_stomatal_closure;
   TRNAutoParameterEditBar *edit_wilt_leaf_water_pot;
   TRNAutoParameterEditBar *edit_deg_day_emerge;
   TRNAutoParameterEditBar *edit_deg_day_tuber_initiation;
   TRNAutoParameterEditBar *edit_deg_day_begin_flower;
   TRNAutoParameterEditBar *edit_phenologic_sensitivity_water_stress;
   TRNAutoParameterEditBar *edit_HI_unstressed;
   TTabSheet *residue_sheet;
   TTabSheet *nitrogen_sheet;
   TRNAutoParameterEditBar *N_min_concentration_harvest_edit_obsolete;
   TTabSheet *salinity_sheet;
   TRNAutoParameterEditBar *edit_osmotic_pot_50;
   TRNAutoParameterEditBar *edit_salt_tolerance_P;
   TTabSheet *CO2_sheet;
   TImage *standard_row_image;
   TImage *hedge_row_image;
   TTabSheet *emergence_tabsheet;
   TRNAutoRadioGroup *radiogroup_emergence_model;
   TGroupBox *groupbox_hydrothermal_time;
   TRNAutoParameterEditBar *edit_seedling_leaf_area;
   TRNAutoParameterEditBar *edit_germination_base_temperature;
   TRNAutoParameterEditBar *edit_mean_base_soil_water_pot;
   TRNAutoParameterEditBar *edit_stdev_base_soil_water_pot;
   TRNAutoParameterEditBar *edit_a;
   TRNAutoParameterEditBar *edit_b;
   TRNAutoParameterEditBar *edit_deg_day_begin_maturity;
   TLabel *label_top_biomass;
   TLabel *Label4;
   TRNAutoFloatEdit *edit_fast_cycling_top_fraction;
   TRNAutoFloatEdit *edit_slow_cycling_top_fraction;
   TRNAutoFloatEdit *edit_lignified_top_fraction;
   TRNAutoFloatEdit *edit_fast_cycling_root_fraction;
   TRNAutoFloatEdit *edit_slow_cycling_root_fraction;
   TRNAutoFloatEdit *edit_lignified_root_fraction;
   TRNAutoFloatEdit *edit_fast_cycling_C_N_ratio;
   TRNAutoFloatEdit *edit_slow_cycling_C_N_ratio;
   TRNAutoFloatEdit *edit_lignified_C_N_ratio;
   TLabel *Label13;
   TLabel *Label14;
   TLabel *Label15;
   TLabel *Label16;
   TRNAutoIntegerEdit *edit_decomposition_time_50;
   TLabel *Label17;
   TLabel *Label18;
   TLabel *Label19;
   TLabel *Label20;
   TRNAutoFloatEdit *top_biomass_checksum;
   TRNAutoFloatEdit *root_biomass_checksum;
   TLabel *Label21;
   TTabSheet *root_tabsheet;
   TRNAutoParameterEditBar *edit_max_root_depth;
   TRNAutoParameterEditBar *edit_root_length_per_unit_mass;
   TRNAutoParameterEditBar *edit_surface_root_density;
   TRNAutoParameterEditBar *edit_root_density_distribution_curvature;
   TRNAutoRadioGroup *radiogroup_RULSE2_residue_type;
   TGroupBox *groupbox_residue_advanced;
   TLabel *Label7;
   TRNAutoFloatEdit *edit_fast_cycling_halflife;
   TRNAutoFloatEdit *edit_slow_cycling_halflife;
   TRNAutoFloatEdit *edit_lignified_halflife;
   TTabSheet *tabsheet_senescence;
   TLabel *Label22;
   TLabel *Label23;
   TRNAutoRadioGroup *pot_N_uptake_model_radio_obsolete;
   TGroupBox *pot_N_uptake_parameters_groupbox_obsolete;
   TRNAutoParameterEditBar *N_availability_adjust_edit_obsolete;
   TRNAutoParameterEditBar *N_residual_edit_obsolete;
   TRNAutoParameterEditBar *edit_max_root_depth_dd_obsolete;
   TTabSheet *tabsheet_thermal_time_accumulation;
   TTabSheet *tabsheet_defaults;
   TGroupBox *crop_class_groupbox;
   TLabel *label_select_default_crop;
   TGroupBox *legend_group_box;
   TLabel *critical_label;
   TLabel *adequate_label;
   TRNAutoCheckBox *checkbox_advanced_moved_to_param_file_form;
   TPageControl *pagecontrol_nitrogen;
   TTabSheet *tabsheet_N_demand;
   TTabSheet *tabsheet_potential_uptake;
   TRNAutoParameterEditBar *edit_N_max_stubble;
   TRNAutoParameterEditBar *edit_root_N_conc;
   TRNAutoParameterEditBar *edit_N_max_daily_uptake_rate;
   TRNAutoParameterEditBar *edit_residual_N_not_available_for_uptake;
   TRNAutoParameterEditBar *edit_soil_N_conc_where_N_uptake_decreases;
   TRNAutoParameterEditBar *edit_PAW_where_N_uptake_rate_decreases;
   TRNAutoParameterEditBar *N_uptake_adjust_edit_obsolete;
   TTabSheet *tabsheet_fruit;
   TRNAutoParameterEditBar *edit_fruit_load_max;
   TRNAutoParameterEditBar *edit_fruit_fract_total_solids;
   TRNAutoParameterEditBar *development_fract_biomass_edit;
   TRNAutoParameterEditBar *veraison_fract_biomass_edit;
   TLabel *Label28;
   TLabel *Label29;
   TPanel *Panel1;
   TRNAutoCheckBox *checkbox_clipping_resets;
   TPanel *panel_maturity_significant;
   TRNAutoCheckBox *checkbox_maturity_significant;
   TPageControl *pagecontrol_inactivity;
   TTabSheet *tabsheet_inactivity_none;
   TTabSheet *tabsheet_dormancy1;
   TTabSheet *tabsheet_quiescence;
   TLabel *label_dormancy_mode;
   TRNAutoParameterEditBar *edit_dormancy_senesce;
   TEdit *note_select_one2;
   TLabel *label_single_season_mode_2;
   TLabel *label_single_season_mode_1;
   TLabel *label_multiple_season_mode_2;
   TLabel *label_multiple_season_mode_1;
   TGroupBox *groupbox_inactive_period;
   TRNAutoParameterEditBar *edit_inducement_temperature;
   TRNAutoParameterEditBar *edit_consider_inactive_days;
   TLabel *label_inactivity_days_note;
   TLabel *label_default_parameters;
   TGroupBox *groupbox_chill_requirements;
   TRNAutoParameterEditBar *edit_deg_day_bud_break_chill_req_sat;
   TRNAutoParameterEditBar *edit_deg_day_bud_break_chill_req_not_sat;
   TRNAutoParameterEditBar *edit_chill_requirement;
   TPanel *panel_deciduous_senescence;
   TRNAutoCheckBox *checkbox_senesces;
   TGroupBox *groupbox_cold_temperature_sensitivity;
   TRNAutoParameterEditBar *edit_damage_cold_temperature;
   TRNAutoParameterEditBar *edit_lethal_cold_temperature;
   TPanel *panel_nitrogen_fixation_and_note;
   TRNAutoCheckBox *button_nitrogen_fixation;
   TLabel *Label30;
   TPanel *panel_thermal_time_parameters;
   TRNAutoParameterEditBar *edit_base_temp;
   TRNAutoParameterEditBar *edit_opt_temp;
   TRNAutoParameterEditBar *edit_max_temp;
   TRNAutoParameterEditBar *edit_cutoff_temp;
   TPanel *panel_vernalization_enabled;
   TRNAutoCheckBox *checkbox_vernalization;
   TPanel *panel_vernalization_parameters;
   TRNAutoParameterEditBar *edit_vern_low_temp;
   TRNAutoParameterEditBar *edit_vern_high_temp;
   TRNAutoParameterEditBar *edit_vern_start;
   TRNAutoParameterEditBar *edit_vern_end;
   TRNAutoParameterEditBar *edit_vern_min_factor;
   TPanel *panel_photoperiod_enabled;
   TPanel *panel_hardiness;
   TRNAutoCheckBox *checkbox_sensitive_to_cold;
   TButton *button_calibrate;
   TLabel *Label25;
   TScrollBox *scrollbox_classification;
   TRNAutoRadioGroup *radiogroup_crop_model_obsolete;
   TRNAutoRadioGroup *radiogroup_photosynthetic;
   TLabel *label_advanced_mode;
   TPageControl *page_canopy_growth;
   TTabSheet *tabsheet_canopy_LAI;
   TTabSheet *tabsheet_canopy_cover;
   TEdit *note_select_one_3;
   TRNAutoParameterEditBar *edit_max_LAI;
   TRNAutoParameterEditBar *edit_specific_leaf_area;
   TRNAutoParameterEditBar *edit_fract_LAI_mature;
   TRNAutoParameterEditBar *edit_stem_leaf_partition;
   TRNAutoParameterEditBar *edit_at_pt_root_limit;
   TRNAutoParameterEditBar *edit_LWP_that_reduces_canopy_expansion;
   TRNAutoParameterEditBar *edit_LWP_that_stops_canopy_expansion;
   TRNAutoParameterEditBar *edit_canopy_cover_initial;
   TRNAutoParameterEditBar *edit_canopy_cover_maximum;
   TRNAutoParameterEditBar *edit_thermal_time_season_end_obsolete;
   TPanel *panel_filling_and_fruit_growth;
   TRNAutoParameterEditBar *edit_deg_day_begin_filling_grain_or_fruit;
   TRNAutoParameterEditBar *edit_deg_day_begin_rapid_fruit_growth;
   TRNAutoParameterEditBar *edit_linger;
   TLabel *label_hardiness_leaf;
   TGroupBox *groupbox_hardiness_fruit;
   TRNAutoFloatEdit *edit_hardiness_fruit_lethal;
   TRNAutoFloatEdit *edit_hardiness_fruit_damage;
   TRNAutoFloatEdit *edit_detrition_time;
   TLabel *label_detrition;
   TGroupBox *groupbox_max_water_uptake;
   TRNAutoParameterEditBar *edit_max_water_uptake_after_maturity;
   TRNAutoParameterEditBar *edit_max_water_uptake_during_veraison;
   TRNAutoParameterEditBar *edit_max_water_uptake_during_rapid_fruit;
   TRNAutoParameterEditBar *edit_max_water_uptake_during_intial_fruit;
   TRNAutoParameterEditBar *edit_max_water_upake_before_fruit;
   TRNAutoRadioGroup *radiogroup_photoperiod_consideration;
   TGroupBox *groupbox_photoperiod_daylength;
   TRNAutoParameterEditBar *edit_stop_daylength;
   TRNAutoParameterEditBar *edit_constrained_daylength;
   TLabel *caption_at_which_stopped;
   TLabel *caption_at_which_constrained;
   TPanel *Panel3;
   TRNAutoParameterEditBar *edit_carbon_fraction;
   TRNAutoParameterEditBar *edit_stubble_area_covered_to_mass_ratio;
   TRNAutoParameterEditBar *edit_surface_area_covered_to_mass_ratio;
   TRNAutoParameterEditBar *edit_N_mature_max_conc;
   TRNAutoParameterEditBar *edit_conc_slope;
   TEdit *note_select_one_1;
   TRNAutoParameterEditBar *edit_end_dilution_curve;
   TRadioGroup *radiogroup_preset_end_dilution;
   TTabSheet *tabsheet_adjustments;
   TRNAutoContainerCheckListBox *checklistbox_adjustments;
   TPanel *panel_adjustment_edit;
   TBitBtn *button_adjustment_add;
   TBitBtn *button_adjustment_replace;
   TBitBtn *button_adjustment_delete;
   TComboBox *combobox_adjustment_variable;
   TPageControl *pagecontrol_adjustment_sync_mode;
   TTabSheet *tabsheet_adjustment_growth_stage;
   TTabSheet *tabsheet_adjustment_thermal_time;
   TRNAutoIntegerEdit *edit_adjustment_offset;
   TRNAutoIntegerEdit *edit_adjustment_thermal_time;
   TLabel *Label31;
   TRNAutoFloatEdit *edit_adjustment_value;
   TComboBox *combobox_adjustment_crop_event;
   TRNAutoParameterEditBar *edit_salvageable_deg_day;
   TGroupBox *groupbox_GAI;
   TRNAutoParameterEditBar *edit_initial_GAI;
   TRNAutoParameterEditBar *edit_regrowth_GAI;
   TRNAutoParameterEditBar *edit_root_shoot_full_ratio;
   TRNAutoParameterEditBar *edit_root_shoot_emergence_ratio;
   TRNAutoParameterEditBar *edit_root_sensitivity_to_water_stress;
   TGroupBox *groupbox_CO2_non_rectangular_hyperbola;
   TRNAutoParameterEditBar *edit_CO2_theta;
   TRNAutoParameterEditBar *edit_CO2_alpha;
   TGroupBox *groupbox_ref_CO2;
   TPanel *panel_ref_CO2_edits;
   TRNAutoParameterEditBar *edit_CO2_baseline_reference_conc;
   TRNAutoParameterEditBar *edit_CO2_elevated_reference_conc;
   TRNAutoParameterEditBar *edit_TUE_reference_conc;
   TRNAutoParameterEditBar *edit_RUE_reference_conc;
   TRNAutoParameterEditBar *edit_CO2_growth_ratio_asymptotic;
   TRNAutoParameterEditBar *edit_CO2_growth_ratio_elevated_to_baseline;
   TPanel *panel_canopy_model;
   TRNAutoRadioGroup *radiogroup_canopy_model;
   TGroupBox *groupbox_historic_CO2;
   TListBox *ListBox1;
   TGroupBox *groupbox_LAI_duration;
   TRNAutoParameterEditBar *edit_deg_day_leaf_duration;
   TRNAutoParameterEditBar *edit_leaf_duration_sensitivity_stress;
   TRNAutoRadioGroup *radiogroup_abscission;
   TGroupBox *GroupBox2;
   TButton *button_defaults_C3;
   TButton *button_defaults_C4;
   TPanel *Panel5;
   TRNAutoCheckBox *checkbox_HI_general_adjustments;
   TGroupBox *groupbox_adjustments_enabled;
   TPageControl *page_control_harvest;
   TTabSheet *tabsheet_harvest_grain;
   TGroupBox *groupbox_harvest_index_grain_temperature_stress;
   TGroupBox *groupbox_flowering_stress;
   TLabel *Label10;
   TLabel *Label27;
   TLabel *Label35;
   TRNAutoParameterEditBar *label_heat_stress_no_stress_threshold_temperature;
   TRNAutoParameterEditBar *label_heat_stress_max_stress_threshold_temperature;
   TRNAutoParameterEditBar *label_heat_stress_sensitivity;
   TRNAutoParameterEditBar *label_heat_stress_exposure_damaging_hours;
   TRNAutoFloatEdit *edit_cold_stress_no_stress_threshold_temperature;
   TRNAutoFloatEdit *edit_cold_stress_max_stress_threshold_temperature;
   TRNAutoFloatEdit *edit_cold_stress_sensitivity;
   TRNAutoFloatEdit *edit_cold_stress_exposure_damaging_hours;
   TRNAutoFloatEdit *edit_heat_stress_no_stress_threshold_temperature;
   TRNAutoFloatEdit *edit_heat_stress_max_stress_threshold_temperature;
   TRNAutoFloatEdit *edit_heat_stress_sensitivity;
   TRNAutoFloatEdit *edit_heat_stress_exposure_damaging_hours;
   TRNAutoIntegerEdit *edit_cold_stress_exposure_initial_hour;
   TPanel *panel_heat_exposure_hour;
   TRNAutoIntegerEdit *edit_heat_stress_exposure_initial_hour;
   TPanel *panel_sunrise_arbitrary;
   TButton *button_sunrise;
   TButton *button_arbitrary;
   TPanel *listbox_panel_temperature_stress_events;
   TLabel *Label34;
   TRNAutoContainerListBox *listbox_harvest_temperature_stress_events;
   TPanel *panel_harvest_stress_edit_buttons;
   TBitBtn *button_add_temperature_stress_period;
   TBitBtn *button_replace_temperature_stress_period;
   TBitBtn *button_delete_temperature_stress_period;
   TPageControl *pagecontrol_grain_stress_period;
   TTabSheet *tabsheet_relative_to_growthstage;
   TRNAutoIntegerEdit *edit_grain_stress_sensitivity_offset;
   TComboBox *combobox_stress_period;
   TTabSheet *tabsheet_thermal_time;
   TLabel *Label32;
   TRNAutoIntegerEdit *edit_grain_stress_sensitivity_thermal_time;
   TTabSheet *tabsheet_harvest_tuber;
   TLabel *Label6;
   TRNAutoParameterEditBar *edit_HI_tuber_init_sensitivity;
   TRNAutoParameterEditBar *edit_HI_tuber_growth_sensitivity;
   TTabSheet *tabsheet_harvest_fruit;
   TPanel *panel_fruit_harvest_condition;
   TRNAutoParameterEditBar *edit_harvest_temperature;
   TRNAutoParameterEditBar *edit_harvest_temperature_days;
   TRNAutoParameterEditBar *edit_harvest_DOY_earliest;
   TRNAutoParameterEditBar *edit_harvest_DOY_latest;
   TRNAutoRadioGroup *radiogroup_fruit_harvest_condition;
   TTabSheet *tabsheet_harvest_other;
   TRNAutoParameterEditBar *edit_HI_growth_sensitivity;
   TRNAutoParameterEditBar *edit_grain_translocation_fraction;
   TGroupBox *GroupBox1;
   TRNAutoParameterEditBar *edit_BM_start_dilution_crit_conc;
   TRNAutoParameterEditBar *edit_N_emerge_crit_conc;
   TRNAutoParameterEditBar *edit_N_emerge_max_conc;
   TGroupBox *groupbox_orientation;
   TGroupBox *groupbox_canopy_canopy_architecture;
   TLabel *row_width_label;
   TLabel *tree_breadth_label;
   TLabel *row_height_label;
   TRNAutoFloatEdit *edit_initial_row_height;
   TRNAutoFloatEdit *edit_initial_tree_breadth;
   TRNAutoFloatEdit *edit_initial_row_width;
   TLabel *Label11;
   TLabel *Label12;
   TRNAutoFloatEdit *edit_final_row_width;
   TRNAutoFloatEdit *edit_final_tree_breadth;
   TRNAutoFloatEdit *edit_final_row_height;
   TRNAutoParameterEditBar *edit_branch_insertion_height;
   TRNAutoParameterEditBar *edit_row_azimuth;
   TRNAutoParameterEditBar *edit_row_spacing;
   TRNAutoParameterEditBar *edit_tree_spacing;
   TRNAutoParameterEditBar *edit_canopy_cover_total_season_end;
   TRNAutoParameterEditBar *edit_canopy_cover_green_season_end;
   TRNAutoParameterEditBar *edit_shape_senescent;
   TRNAutoParameterEditBar *edit_shape_accrescent;
   TGroupBox *groupbox_deprecated;
   TRNAutoParameterEditBar *edit_quiescence_end_DOY;
   TRNAutoParameterEditBar *edit_quiescence_start_DOY;
   TGroupBox *GroupBox3;
   TLabel *Label33;
   TRNAutoParameterEditBar *edit_initial_fruit_mass;
   TRNAutoParameterEditBar *edit_initial_reserves_release_rate;
   TGroupBox *groupbox_fruit_other;
   TRNAutoParameterEditBar *edit_fruits_per_tree;
   TGroupBox *groupbox_fruit_grape;
   TRNAutoParameterEditBar *edit_clusters_per_vine;
   TRNAutoParameterEditBar *edit_berries_per_cluster;
   TComboBox *crop_class_combobox;
   TPanel *panel_images;
   TImage *general_crop_image;
   TImage *fruit_image;
   TImage *cropgro_image;
   TImage *grape_crop_image;
   TRNAutoRadioGroup *radiogroup_thermal_response;
   TRNAutoRadioGroup *radiogroup_stem_type;
   TImage *image_fruit_bush;
   TPageControl *pagecontrol_crop_model;
   TTabSheet *tabsheet_model_general;
   TTabSheet *tabsheet_model_fruit;
   TRNAutoRadioGroup *radiogroup_harvested_biomass;
   TRNAutoRadioGroup *radiogroup_landuse;
   TRNAutoCheckBox *checkbox_grape;
   TRNAutoRadioGroup *radiogroup_life_cycle;
   TLabel *label_fruit_canopy_cover;
   TRNAutoCheckBox *checkbox_canopy_architecture;
   TScrollBox *scrollbox_attainable_growth;
   TRadioGroup *radiogroup_efficiency_model;
   TGroupBox *groupbox_TUE;
   TRNAutoCheckBox *checkbox_WUE_changes;
   TRNAutoParameterEditBar *edit_WUE_change_deg_day;
   TGroupBox *groupbox_TUE_curve;
   TRNAutoParameterEditBar *edit_TUE_at_1kPa_VPD;
   TRNAutoFloatEdit *edit_TUE_at_1kPa_VPD_vegetative;
   TRNAutoParameterEditBar *edit_TUE_scaling_coef;
   TRNAutoFloatEdit *edit_TUE_scaling_coef_vegetative;
   TGroupBox *groupbox_RUE;
   TGroupBox *groupbbox_RUE_low_total_solar_rad;
   TRNAutoParameterEditBar *edit_RUE_rate_decrease;
   TRNAutoParameterEditBar *edit_RUE_at_low_total_solar_rad;
   TGroupBox *groupbox_RUE_low_VPD;
   TRNAutoParameterEditBar *edit_RUE_at_low_VPD;
   TGroupBox *groupbox_temperature_limit_new;
   TLabel *label_must_not_be_less_than_base_temperature;
   TRNAutoParameterEditBar *edit_temp_limit_max;
   TRNAutoParameterEditBar *edit_temp_limit_min;
   TGroupBox *groupbox_optimum_range;
   TRNAutoParameterEditBar *edit_temp_limit_high;
   TRNAutoParameterEditBar *edit_temp_limit_low;
   TGroupBox *groupbox_seasonal_adj;
   TRNAutoParameterEditBar *edit_seasonal_adjustment_1;
   TRNAutoParameterEditBar *edit_seasonal_adjustment_2;
   TRNAutoParameterEditBar *edit_seasonal_adjustment_3;
   TRNAutoParameterEditBar *edit_seasonal_adjustment_4;
   TRNAutoParameterEditBar *edit_seasonal_adjustment_5;
   TLabel *label_seasonal_adjustment;
   TRNAutoRadioGroup *radiogroup_WUE_submodel_obsolete;
   TRNAutoParameterEditBar *edit_root_elongation_duration;
   TRNAutoParameterEditBar *edit_light_extinction_coef;
   TGroupBox *group_box_duration;
   TRNAutoParameterEditBar *edit_accrescence_duration;
   TRNAutoParameterEditBar *edit_culminescence_duration;
   TRNAutoParameterEditBar *edit_senescence_duration;
   TRNAutoParameterEditBar *edit_senescence_initiation_obsolete;
   TRNAutoCheckBox *checkbox_HI_extreme_adjustments;
   TRNAutoCheckBox *checkbox_yield_water_response;
   TRNAutoCheckBox *checkbox_yield_heat_response;
   TGroupBox *groupbox_response_heat;
   TLabel *label_temperature_extreme_note;
   TPanel *panel_water_stress_sensitivity;
   TRNAutoFloatEdit *edit_water_stress_sensitivity;
   TPanel *panel_temperature_stress_sensitivity;
   TPanel *panel_temperature_optimum;
   TPanel *panel_temperature_damaging;
   TRNAutoFloatEdit *edit_temperature_stress_sensitivity;
   TRNAutoFloatEdit *edit_temperature_optimum;
   TRNAutoFloatEdit *edit_temperature_damaging;
   TPanel *panel_nitrogen_leaf_stress_mode;
   TLabel *label_leaf_expansion_sensitivity;
   TRNAutoFloatEdit *edit_nitrogen_leaf_stress_mode;
   //141106   TAutoFloat32EditBar *edit_senesce_LAI_unit_per_deg_day;
   void __fastcall photoperiod_checkbox_onclick(TObject *Sender);
   void __fastcall vernalization_checkbox_onclick(TObject *Sender);
   void __fastcall harvested_biomass_radiogroup_onclick(TObject *Sender);
   void __fastcall sensitive_to_cold_edit_onclick(TObject *Sender);
   void __fastcall perennial_checkbox_onclick(TObject *Sender);
   void __fastcall crop_class_combobox_onchange(TObject *Sender);
   void __fastcall Crop_editor_form_onactivate(TObject *Sender);
   void __fastcall calibrate_button_onclick(TObject *Sender);
   void __fastcall resolution_radiogroup_onclick(TObject *Sender);
   void __fastcall grape_checkbox_onclick(TObject *Sender);
   void __fastcall FormKeyPress(TObject *Sender, char &Key);
   void __fastcall life_cycle_radiogroup_onclick(TObject *Sender);
   void __fastcall senesces_checkbox_onclick(TObject *Sender);
   void __fastcall emergence_model_radiogroupon_onclick(TObject *Sender);
   void __fastcall update_top_biomass_checksum(TObject *Sender);
   void __fastcall update_root_biomass_checksum(TObject *Sender);
   void __fastcall checkbox_advanced_moved_to_param_file_formClick(TObject *Sender);
   void __fastcall pot_N_uptake_model_radio_onclick(TObject *Sender);
   void __fastcall checkbox_maturity_significantClick(TObject *Sender);
   void __fastcall pagecontrol_season_dormancy_mode_onchange(TObject *Sender);
   void __fastcall checkbox_WUE_changesClick(TObject *Sender);
   void __fastcall radiogroup_WUE_submodel_obsoleteClick(TObject *Sender);
   void __fastcall radiogroup_RUE_basis_mode_obsoleteClick(TObject *Sender);
//190816    void __fastcall radiogroup_senesced_biomass_fate_obsoleteClick(TObject *Sender);
   void __fastcall radiogroup_photoperiod_considerationClick(TObject *Sender);
   void __fastcall checkbox_HI_extreme_adjustmentsClick(TObject *Sender);
   //170524 void __fastcall button_harvest_index_upgrade_obsoleteClick(TObject *Sender);
   void __fastcall radiogroup_photosyntheticClick(TObject *Sender);
   void __fastcall pagecontrol_dilution_curve_basisChange(TObject *Sender);
   void __fastcall radiogroup_preset_end_dilutionClick(TObject *Sender);
   void __fastcall checklistbox_adjustmentsClick(TObject *Sender);
   void __fastcall button_adjustment_addClick(TObject *Sender);
   void __fastcall button_adjustment_replaceClick(TObject *Sender);
   void __fastcall button_adjustment_deleteClick(TObject *Sender);
   void __fastcall button_delete_temperature_stress_periodClick(TObject *Sender);
   void __fastcall button_add_temperature_stress_periodClick(
          TObject *Sender);
   void __fastcall button_replace_temperature_stress_periodClick(
          TObject *Sender);
   void __fastcall listbox_harvest_temperature_stress_eventsClick(
          TObject *Sender);
   void __fastcall button_upgrade_limitation_temperaturesClick(TObject *Sender);
   void __fastcall radiogroup_canopy_modelClick(TObject *Sender);
   void __fastcall button_sunriseClick(TObject *Sender);
   void __fastcall button_arbitraryClick(TObject *Sender);
   void __fastcall button_defaults_C3Click(TObject *Sender);
   void __fastcall button_defaults_C4Click(TObject *Sender);
   void __fastcall checkbox_HI_general_adjustmentsClick(TObject *Sender);
   void __fastcall radiogroup_abscissionClick(TObject *Sender);
   void __fastcall fileedit_default_parametersChange(TObject *Sender);
//190627   void __fastcall checkbox_canopy_architecture_deletemeClick(TObject *Sender);
   void __fastcall checkbox_canopy_architectureClick(TObject *Sender);
   void __fastcall pagecontrol_crop_modelChange(TObject *Sender);
   void __fastcall radiogroup_efficiency_modelClick(TObject *Sender);
   void __fastcall checkbox_yield_water_responseClick(TObject *Sender);
   void __fastcall checkbox_yield_heat_responseClick(TObject *Sender);
private:	// User declarations
   /*now using alignment
   int designed_deg_day_tuber_initiation_edit_top            ;
   int designed_max_root_depth_dd_edit_top                   ;
   int designed_deg_day_end_vegetative_growth_edit_top       ;
   int designed_deg_day_begin_flower_edit_top                ;
   int designed_deg_day_begin_filling_grain_or_fruit_edit_top;
   int designed_deg_day_begin_rapid_fruit_growth_edit_top    ;
   int designed_deg_day_begin_maturity_edit_top              ;
   int designed_phenologic_sensitivity_water_stress_edit_top ;
   */
   int16 adjustment_offset;
   int16 adjustment_thermal_time;
//   int16 grain_stress_sensitivity_offset;
//   int16 grain_stress_sensitivity_thermal_time;

 public:		// User declarations
   __fastcall TCrop_editor_form(TComponent* Owner);
   virtual __fastcall ~TCrop_editor_form();
   Tcrop_editor_form_USER_DECLARATIONS
 public:
   inline virtual TPageControl *get_main_page_control() const { return crop_pagecontrol;};
 protected:
  //150826 void __fastcall bind_to_harvest_index_V4();
   void translate_page_controls();
   void translate_tabsheets(TPageControl *page_control);

   void add_adjustment
   (  Synchronization_mode       sync_mode
   ,  Normal_crop_event_sequence event_sequence
   ,  nat32 offset
   ,  nat32 thermal_time
   ,  const char * /*const std::string &*/ parameter
   ,  float32 value);
      // Currently this is only in V4.
      // In V5 there will be a more sophisticated method
      // as most parameters will be synchronizable/adjustable

   void replace_harvest_grain_stress_sensitivity(int last_item_index);
   int last_item_index;
};
//---------------------------------------------------------------------------
extern PACKAGE TCrop_editor_form *Crop_editor_form;
//---------------------------------------------------------------------------
#endif

