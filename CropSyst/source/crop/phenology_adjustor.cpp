#include "phenology_adjustor.h"
#include "corn/data_source/vv_file.h"
#include "rot_param.h"
#include "common/weather/weather.h"
namespace CropSyst {

std::ofstream qnd_thermal_time("qnd_thermal_time.dat");

//______________________________________________________________________________
const char *Phenology_adjustor::label_string(std::string &buffer)          const
{  CORN::wstring_to_string(crop_parameters_owned->data_source_URL,buffer);
   return buffer;
}
//_Phenology_adjustor::label_string_____________________________________________
/*190818 obs
const char *Phenology_adjustor::label_cstr_deprecated(char *buffer)        const //deprecated
{  CORN::wstring_to_ASCIIZ(crop_parameters_owned->data_source_URL,buffer);
   return buffer;
}
//______________________________________________________________________________
*/
Phenology_adjustor::Phenology_adjustor
(const std::wstring       & unique_crop_identifier_                              //150211
,const CORN::Date&          typical_planting_date_relative_
,const CORN::Date         * typical_relative_
,const CORN::Date&          typical_relative_senescence_                          //160211
,const CORN::Date&          typical_relative_senescence_full_                    //160211
,const Air_temperature_maximum         &air_temperature_max_                     //160201
,const Air_temperature_minimum         &air_temperature_min_                     //160201
,const Air_temperature_average         &air_temperature_avg_                     //160201
,const float64               &day_length_hours_                                  //160201
,const CORN::Date&            today_known_)
: CS::Simulation_element_abstract(today_known_)
, crop_parameters_owned (0)
, unique_crop_identifier(unique_crop_identifier_)                                //150211
, typical_planting_date_relative(typical_planting_date_relative_)
,typical_relative_senescence(typical_relative_senescence_)                       //160211
,typical_relative_senescence_full(typical_relative_senescence_full_)             //160211
,air_temperature_max(air_temperature_max_)                                       //160201
,air_temperature_min(air_temperature_min_)                                       //160201
,air_temperature_avg(air_temperature_avg_)                                       //160201
,day_length_hours       (day_length_hours_)                                      //160201
, crop_current          (0)
, adjust_mode           (false)                                                  //160211
{
   for (nat8 sequence = 0; sequence < NGS_COUNT; sequence++)
   {  typical_relative           [sequence].set_date32(typical_relative_[sequence].get_date32());
      start_day_in_season_stats  [sequence] = new CORN::statistical::Sample_cumulative<nat16,float64>(STAT_ALL_bits); //150929 actually dont need all stats
      start_deg_days_stats       [sequence] = new CORN::statistical::Sample_cumulative<nat16,float64>(STAT_ALL_bits); //150929 actually dont need all stats
   }
   start_day_in_season_stats_senescence = new CORN::statistical::Sample_cumulative<nat16,float64>(STAT_ALL_bits);  //160211
   start_day_in_season_stats_senescence_full  = new CORN::statistical::Sample_cumulative<nat16,float64>(STAT_ALL_bits);  //160211
   start_deg_days_stats_senescence             = new CORN::statistical::Sample_cumulative<nat16,float64>(STAT_ALL_bits); //160211
   start_deg_days_stats_senescence_full        = new CORN::statistical::Sample_cumulative<nat16,float64>(STAT_ALL_bits); //160211
}
//_Phenology_adjustor:constructor______________________________________________/
Phenology_adjustor::~Phenology_adjustor()
{  for (nat8 sequence = 0; sequence < NGS_COUNT; sequence++)
   {
      delete start_day_in_season_stats  [sequence];
      delete start_deg_days_stats       [sequence];
   }
   delete start_day_in_season_stats_senescence;                                  //160211
   delete start_day_in_season_stats_senescence_full;                             //160211
   delete start_deg_days_stats_senescence;                                       //160211
   delete start_deg_days_stats_senescence_full;                                  //160211
}
//__Phenology_adjustor:destructor___________________________________2014-11-93_/
bool Phenology_adjustor::start_day()
{  bool started_day = true;
//160201 now using know_day_length_hours   day_length_hours = weather.get_day_length_hours(0);                           //140812
   if (crop_current)
   {  bool record_season_stats = false;
      float64 current_cum_deg_days = crop_current->thermal_time->get_accum_degree_days(false,false);

      /*160205 now recording all sequences
      if (crop_current->date == typical_relative[NGS_END_CANOPY_GROWTH])         //150220
         crop_current->start_deg_days[NGS_END_CANOPY_GROWTH] = current_cum_deg_days; //150220
      */

      for (nat8 sequence = 0; sequence < NGS_COUNT; sequence++)                  //160205
         if (crop_current->date_relative.get_DOY() == typical_relative[sequence].get_DOY()) //160205_150220
         {
            crop_current->start_deg_days[sequence] = current_cum_deg_days;       //150220
            std::clog << '\t' << normal_crop_event_sequence_label_table[sequence];
            qnd_thermal_time << '\t' << normal_crop_event_sequence_label_table[sequence] << " (" << typical_relative[sequence].get_DOY() << ")";
         }

      if (crop_current->date_relative.get_DOY() == typical_relative_senescence.get_DOY()) //160211
         {
            crop_current->start_deg_days_senescence = current_cum_deg_days;
            std::clog << '\t' << "senescence";
            qnd_thermal_time << '\t' << "senescence" << " (" << typical_relative_senescence.get_DOY() << ")";
         }
      if (crop_current->date_relative.get_DOY() == typical_relative_senescence_full.get_DOY()) //160211
         {
            crop_current->start_deg_days_senescence_full = current_cum_deg_days;
            std::clog << '\t' << "full_senescence";
            qnd_thermal_time << '\t' << "full_senescence" << " (" << typical_relative_senescence_full.get_DOY() << ")";
         }


      if (crop_current->has_maturity())
      {  if (crop_current->date_relative.get_DOY() == typical_relative[NGS_MATURITY].get_DOY())
         {  crop_current->event_start(NGS_MATURITY);
            record_season_stats = true;
            qnd_thermal_time << '\t' << "maturity (targetDOY=" << typical_relative[NGS_MATURITY].get_DOY() << ") ";

         }
      }
      else
      {
         nat16 days_since_start_of_inactivity = crop_current->phenology.get_days_since_start_of(NGS_DORMANT_or_INACTIVE);
         if ((days_since_start_of_inactivity == 1)
            /*150223 Cant used full senescence to terminate the calibration period because it needs to be adjusted
            || (current_cum_deg_days > crop_parameters_owned->phenology.full_senescence_deg_day)
            */
            || (today.get_DOY() == typical_planting_date_relative.get_DOY() )    //150223
               // Stop if we run over to the start of the next season
         )
         {
            record_season_stats = true;
            qnd_thermal_time << '\t' << "start inactivity ";

         }
      }
      if (record_season_stats)
      {  for (nat8 sequence = 0; sequence < NGS_COUNT; sequence++)
         {  start_day_in_season_stats[sequence]->append(crop_current->phenology.start_day_in_season[sequence]);
            start_deg_days_stats[sequence]->append(crop_current->start_deg_days[sequence]);
         }
         start_day_in_season_stats_senescence->append(crop_current->phenology.start_day_in_season_senescence);
         start_deg_days_stats_senescence->append(crop_current->start_deg_days_senescence);
         start_day_in_season_stats_senescence_full->append(crop_current->phenology.start_day_in_season_senescence_full);
         start_deg_days_stats_senescence_full->append(crop_current->start_deg_days_senescence_full);

         delete crop_current;
         crop_current = 0;
      }
   } else
   if (today.get_DOY() == typical_planting_date_relative.get_DOY() )
   {
     if (crop_parameters_owned)
     {
      crop_current = new Crop
          (*crop_parameters_owned,today,typical_relative
            ,typical_relative_senescence
            ,typical_relative_senescence_full
            ,air_temperature_max
            ,air_temperature_min
            ,air_temperature_avg
            ,day_length_hours
            );
      started_day &= crop_current->start();                                       //160205
      qnd_thermal_time << '\t' << "start crop:" << crop_parameters_owned->description.brief;
                  //160205crop_current->CS::Simulation_element_abstract::start()
                  //160205&& crop_current->Phenology_sequencer::start();
     } else
     {  std::cerr << "error: Crop parameters not set." << std::endl;
        started_day = false;
     }
   }
   if (crop_current) // we may have just created crop today so second if is needed here.
      started_day &= crop_current->start_day();
   return started_day;
}
//_Phenology_adjustor::start_day________________________________________________
int Phenology_adjustor::compare(const Item &other)                         const
{
   int comparison = CS::Simulation_element_abstract::compare(other);
   if (comparison == 0)
   {
      const Phenology_adjustor &other_adj = dynamic_cast<const Phenology_adjustor &>(other);
      comparison = typical_planting_date_relative.get_DOY() - other_adj.typical_planting_date_relative.get_DOY();
      // comparison will be 0 if the same
   }
   return comparison;
}
//_Phenology_adjustor::compare______________________________________2015-02-11_/
Phenology_adjustor::Crop::Crop
(const Crop_parameters& crop_parameters_
,const CORN::Date&      today_
,const CORN::Date*      typical_relative_//[NGS_COUNT]
,const CORN::Date &     typical_relative_senescence_                             //160211
,const CORN::Date &     typical_relative_senescence_full_                        //160211
,const Air_temperature_maximum&     air_temperature_max_
,const Air_temperature_minimum&     air_temperature_min_
,const Air_temperature_average&     air_temperature_avg_
,const float64&                     ref_day_length_hours_)                       //140812
: Phenology_sequencer
            (today_,crop_parameters_,air_temperature_min_,ref_day_length_hours_)
, CS::Simulation_element_abstract                                       (today_) //150624
, air_temperature_max                                     (air_temperature_max_)
, air_temperature_avg                                     (air_temperature_avg_)
, crop_parameters                                             (crop_parameters_)
, typical_relative                                           (typical_relative_)
, typical_relative_senescence                     (typical_relative_senescence_) //160211
, typical_relative_senescence_full           (typical_relative_senescence_full_) //160211
, date_relative                                      ((Year) 0,today_.get_DOY())
{  for (nat8 sequence = 0; sequence < NGS_COUNT; sequence++)
      start_deg_days[sequence] = 0;
   start_deg_days_senescence = 0;                                                //160211
   start_deg_days_senescence_full = 0;                                           //160211
}
//_Phenology_adjustor::Crop:constructor________________________________________/
bool Phenology_adjustor::Crop::start()
{  bool started = true;
   started &= Phenology_sequencer::start();
   started &= Phenology_sequencer::start_season();
   event_start(NGS_PLANTING);                                                    //140620
   event_start(NGS_GERMINATION);                                                 //140620
   qnd_thermal_time
         << "date\t"
         << "tmax\t"
         << "tmin\t"
         << "tavg\t"
         << "degdays\t" << std::endl;
   return started && CS::Simulation_element_abstract::start();                   //160205
}
//_Phenology_adjustor::Crop::start__________________________________2016-02-05_/
bool Phenology_adjustor::Crop::start_day()                         modification_
{  date_relative.inc();
   bool day_started
      =  Phenology_sequencer::start_day()
      && CS::Simulation_element_abstract::start_day();
   for (nat8 gs = NGS_GERMINATION; gs < NGS_COUNT; gs++) //This could be moved to start day
   {
//std::clog <<  " TR:" << date_relative.get_DOY() << normal_crop_event_sequence_label_table[gs] << typical_relative[gs].get_DOY() << std::endl;
      if (date_relative.get_DOY() == typical_relative[gs].get_DOY())
      {  event_start(gs);
         start_deg_days[gs] = this->thermal_time->get_accum_degree_days(false,false);
      }
   }
   if (date_relative.get_DOY() == typical_relative_senescence.get_DOY())
      {  //WARNING CHECK NEEDED event_start(gs);
         start_deg_days_senescence = this->thermal_time->get_accum_degree_days(false,false);
      }
   if (date_relative.get_DOY() == typical_relative_senescence_full.get_DOY())
      {  //WARNING CHECK NEEDED event_start(gs);
         start_deg_days_senescence_full = this->thermal_time->get_accum_degree_days(false,false);
      }
   day_started &=   thermal_time_event();                                                      //160205
   return day_started;
}
//_Phenology_adjustor::Crop::start_day______________________________2014-07-17_/
bool Phenology_adjustor::Crop::process_day() /*day*/               modification_ //150624
{  bool processed = true;
   if (phenology.growth_stage != NGS_DORMANT_or_INACTIVE)
   {
      stress_adjusted_temperature.copy(air_temperature_max);
      temperature_with_est_night.copy(air_temperature_avg);
      thermal_time->accumulate_degree_days
         (0 // CORN::Dynamic_float_array *stress_adj_hourly_temperature_optional
         ,phenology.get_growth_stage_sequence());
         // Claudio say not to to apply vernalization and photo period
      float64 tmax = air_temperature_max.Celcius();
      float64 tmin = air_temperature_min.Celcius();
      float64 tavg = air_temperature_avg.Celcius();
      float64 accum_degday = thermal_time->get_accum_degree_days(false,false);
      qnd_thermal_time
         << std::endl
         << today.as_string() << "(" << today.get_DOY() << ")\t"
         << tmax << '\t'
         << tmin << '\t'
         << tavg << '\t'
         << accum_degday << '\t';

   }
   //190821 moved to start_day processed &= thermal_time->end_day();
   return processed;
}
//_Phenology_adjustor::Crop::process_day_______________________________________/
bool Phenology_adjustor::process_day() //day                                     //150624
{  bool processed = true;
   if (crop_current) processed &= crop_current->process_day();                   //150624
   return processed;
}
//_Phenology_adjustor::process_day_____________________________________________/
bool Phenology_adjustor::commit_computed_phenology_parameters
(modifiable_ CropSyst::Crop_parameters_struct::Phenology &phenology_parameters
//,Normal_crop_event_sequence   reference_point_growth_stage                       //160204
)  const
{  bool commited = true;
         // Initially the adjusted parameters will be the same as the source crop file.
         CropSyst::Crop_parameters_struct::Phenology unadjusted(phenology_parameters);                 //150209
         /* 160211 now simply copy constructor
         unadjusted.initiation.emergence     = phenology_parameters.initiation.emergence     ;
         unadjusted.initiation.tuber         = phenology_parameters.initiation.tuber         ;
         unadjusted.culmination.accrescence  = phenology_parameters.culmination.accrescence  ;
         unadjusted.culmination.root_depth   = phenology_parameters.culmination.root_depth   ;
         unadjusted.initiation.flowering     = phenology_parameters.initiation.flowering     ;
         unadjusted.initiation.filling       = phenology_parameters.initiation.filling       ;
         unadjusted.initiation.maturity      = phenology_parameters.initiation.maturity      ;
         unadjusted.initiation.senescence    = phenology_parameters.initiation.senescence    ;
         unadjusted.culmination.senescence   = phenology_parameters.culmination.senescence   ;
         unadjusted.initiation.rapid_fruit_growth = phenology_parameters.initiation.rapid_fruit_growth;
         #if (!defined(YAML_PARAM_VERSION) || (CROPSYST_VERSION==4))
         unadjusted.WUE_change       = phenology_parameters.WUE_change;
         #endif
         unadjusted.phenologic_sensitivity_water_stress = phenology_parameters.phenologic_sensitivity_water_stress;
         unadjusted.maturity_significant     = phenology_parameters.maturity_significant;
         */

         // The adjusted values will ultimately be stored back into the phenology_parameters structure;
         CropSyst::Crop_parameters_struct::Phenology &adjusted = phenology_parameters;
         CropSyst::Crop_parameters_struct::Phenology average;                    //150209

         // Initially the adjusted values are simply the averages
         average.initiation.emergence     = start_deg_days_stats[NGS_EMERGENCE]           ->get_mean(); //141031
         //average.culmination.tuber        = start_deg_days_stats[NGS_xx]   ->get_mean();
         average.culmination.accrescence  = start_deg_days_stats[NGS_END_CANOPY_GROWTH]   ->get_mean();
         //average.culmination.root_depth   = start_deg_days_stats[NGS_xxx]   ->get_mean();
         average.initiation.flowering     = start_deg_days_stats[NGS_ANTHESIS]            ->get_mean();
         average.initiation.filling       = start_deg_days_stats[NGS_FILLING]            ->get_mean();
         average.initiation.rapid_fruit_growth        = start_deg_days_stats[FGS_RAPID_FRUIT_DEVELOPMENT]            ->get_mean();
         average.initiation.veraison      = start_deg_days_stats[FGS_VERAISON]            ->get_mean();
         average.initiation.maturity      = start_deg_days_stats[NGS_MATURITY]            ->get_mean();
         //200125 obsolete average.initiation.senescence     = start_deg_days_stats_senescence  ->get_mean();     //160211
         //200125 obsolete average.culmination.senescence    = start_deg_days_stats_senescence_full ->get_mean(); //160211

         continue add duration.senescence


   commited = (adjust_mode)
      ? apply_adjustments  (unadjusted ,average, adjusted)
      : apply_averages     (average    , adjusted);


         std::clog << (adjusted.maturity_significant ? "maturity significant" :  "maturity not significant") << std::endl;
         std::clog << "emerged     GDD:" << unadjusted.initiation.emergence     << "->" << adjusted.initiation.emergence     << std::endl;
         if (/*adjustor->*/crop_parameters_owned->harvested_part_labeled.get() == tuber_part)
            std::clog<<"tuber init  GDD:"<< unadjusted.initiation.tuber         << "->" << adjusted.initiation.tuber         << std::endl;
         std::clog << "max root    GDD:" << unadjusted.culmination.root_depth   << "->" << adjusted.culmination.root_depth   << std::endl;
         std::clog << "flowering   GDD:" << unadjusted.initiation.flowering     << "->" << adjusted.initiation.flowering     << std::endl;
         std::clog << "filling     GDD:" << unadjusted.initiation.filling       << "->" << adjusted.initiation.filling       << std::endl;
         std::clog << "canopy max  GDD:" << unadjusted.culmination.accrescence  << "->" << adjusted.culmination.accrescence  << std::endl;
         //200125 obsolete std::clog << "senesc beg  GDD:" << unadjusted.initiation.senescence    << "->" << adjusted.initiation.senescence    << std::endl;

         continue here add duration.senescence


         if (adjusted.maturity_significant)
            std::clog<<"maturity    GDD:"<< unadjusted.initiation.maturity      << "->" << adjusted.initiation.maturity      << std::endl;
         //200125 obsolete std::clog << "senesc full GDD:" << unadjusted.culmination.senescence   << "->" << adjusted.culmination.senescence   << std::endl;
         // NYI
         // std::clog << "fruit rapidGDD:" << unadjusted.begin_rapid_fruit_growth_deg_day;
         // std::clog << "WUE change GDD:" << unadjusted.WUE_change_deg_day;

   return commited;
}
//_Phenology_adjustor::commit_computed_phenology_parameters_________2016-02-03_/
bool Phenology_adjustor::apply_adjustments
(const         CropSyst::Crop_parameters_struct::Phenology &unadjusted
,const      CropSyst::Crop_parameters_struct::Phenology &  average
,modifiable_   CropSyst::Crop_parameters_struct::Phenology &  adjusted)       const
{  Normal_crop_event_sequence reference_point_growth_stage = NGS_END_CANOPY_GROWTH; //160204
   std::clog << "info:Phenology adjusting:" << std::endl;
   if (average.initiation.emergence    )
   {  adjusted.initiation.emergence = average.initiation.emergence  ;
   } else
   {  std::clog << "warning:emergence not adjusted (typical emergence date not specified)" << std::endl;
   }
   if (average.initiation.flowering  )
   {  adjusted.initiation.flowering           = average.initiation.flowering  ;
      reference_point_growth_stage = NGS_ANTHESIS;
   }
   else
   {  std::clog << "warning:flowering not adjusted" << std::endl;
   }
   if (average.culmination.accrescence  )
   {  adjusted.culmination.accrescence           = average.culmination.accrescence  ;
      reference_point_growth_stage = NGS_END_CANOPY_GROWTH;
   }
   else
   {  std::clog << "warning:max canopy not adjusted" << std::endl;
   }
   bool maturity_significant = unadjusted.maturity_significant;
   float32 avg_degdays_maturity = 0.0;
   if (maturity_significant)
   {
      if (average.initiation.maturity      > 0.0000001)
      {  adjusted.initiation.maturity      = average.initiation.maturity;
      }
   }
   if (reference_point_growth_stage == NGS_ANTHESIS)
   {
      /*200125 obsolete
      adjusted.culmination.senescence         =
               adjust_full_senescence
               (unadjusted.culmination.senescence
               ,unadjusted.initiation.flowering
               ,unadjusted.initiation.maturity
               ,average.initiation.flowering
               ,average.initiation.maturity
               ,maturity_significant);
      */

      // average Emergence, max canopy cover, and maturity  already calculated
      adjusted.initiation.tuber        = adjust_given_planting_flowering_maturity(unadjusted.initiation.tuber      ,unadjusted,average,adjusted,maturity_significant);
      adjusted.culmination.root_depth  = adjust_given_planting_flowering_maturity(unadjusted.culmination.root_depth,unadjusted,average,adjusted,maturity_significant);
      adjusted.culmination.accrescence  = adjust_given_planting_flowering_maturity(unadjusted.culmination.accrescence,unadjusted,average,adjusted,maturity_significant);
      adjusted.initiation.filling      = adjust_given_planting_flowering_maturity(unadjusted.initiation.filling    ,unadjusted,average,adjusted,maturity_significant);
      //200125 obsolete adjusted.initiation.senescence   = adjust_given_planting_flowering_maturity(unadjusted.initiation.senescence ,unadjusted,average,adjusted,maturity_significant);

      continue here add duration senescence

      // adjusted.full_senescence_deg_day is calculated above
      adjusted.WUE_change              = adjust_given_planting_flowering_maturity(unadjusted.WUE_change,unadjusted,average,adjusted,maturity_significant);
      adjusted.initiation.rapid_fruit_growth = adjust_given_planting_flowering_maturity(unadjusted.initiation.rapid_fruit_growth,unadjusted,average,adjusted,maturity_significant);
   }
   else
   {
      /*200125 obsolete
      adjusted.culmination.senescence         =
               adjust_full_senescence
                  (unadjusted.culmination.senescence
                  ,unadjusted.culmination.accrescence
                  ,unadjusted.initiation.maturity
                  ,average.culmination.accrescence
                  ,average.initiation.maturity
                  ,maturity_significant);
      */
            // average Emergence, max canopy cover, and maturity  already calculated
            adjusted.initiation.tuber        = adjust_given_emergence_accrescence_maturity(unadjusted.initiation.tuber      ,unadjusted,average,adjusted,maturity_significant);
            adjusted.culmination.root_depth  = adjust_given_emergence_accrescence_maturity(unadjusted.culmination.root_depth,unadjusted,average,adjusted,maturity_significant);
            adjusted.initiation.flowering    = adjust_given_emergence_accrescence_maturity(unadjusted.initiation.flowering  ,unadjusted,average,adjusted,maturity_significant);
            adjusted.initiation.filling      = adjust_given_emergence_accrescence_maturity(unadjusted.initiation.filling    ,unadjusted,average,adjusted,maturity_significant);
            //200125 adjusted.initiation.senescence   = adjust_given_emergence_accrescence_maturity(unadjusted.initiation.senescence ,unadjusted,average,adjusted,maturity_significant);

            continue here add duration.senesnce

            // adjusted.full_senescence_deg_day is calculated above
            adjusted.WUE_change              = adjust_given_emergence_accrescence_maturity(unadjusted.WUE_change,unadjusted,average,adjusted,maturity_significant);
            adjusted.initiation.rapid_fruit_growth = adjust_given_emergence_accrescence_maturity(unadjusted.initiation.rapid_fruit_growth,unadjusted,average,adjusted,maturity_significant);
   }
   return true;
}
//_Phenology_adjustor::apply_adjustments____________________________2016-02-11_/
bool Phenology_adjustor::apply_averages
(const      CropSyst::Crop_parameters_struct::Phenology &  average
,modifiable_  CropSyst::Crop_parameters_struct::Phenology &  adjusted)       const
{
   // continue here
   if (CORN::is_zero<float32>(average.initiation.emergence/*200127 ,0.0,0.00001*/))
         std::clog << "warning:emergence not specified" << std::endl;
   else  adjusted.initiation.emergence = average.initiation.emergence;
   if (CORN::is_zero<float32>(average.culmination.accrescence/*200127 ,0.0,0.00001*/))
         std::clog << "warning:canopy_growth_end not specified" << std::endl;
   else  adjusted.culmination.accrescence = average.culmination.accrescence;
   if (CORN::is_zero<float32>(average.initiation.flowering/*200127 ,0.0,0.00001*/))
         std::clog << "warning:flowering not specified" << std::endl;
   else  adjusted.initiation.flowering = average.initiation.flowering;
   if (CORN::is_zero<float32>(average.initiation.filling/*200127 ,0.0,0.00001*/))
         std::clog << "warning:filling not specified" << std::endl;
   else  adjusted.initiation.filling = average.initiation.filling;
   if (CORN::is_zero<float32>(average.initiation.rapid_fruit_growth/*200127 ,0.0,0.00001*/))
         std::clog << "warning:rapid_fruit_growth not specified" << std::endl;
   else  adjusted.initiation.rapid_fruit_growth = average.initiation.rapid_fruit_growth;
   if (CORN::is_zero<float32>(average.initiation.veraison/*200127 ,0.0,0.00001*/))
         std::clog << "warning:veraison not specified" << std::endl;
   else  adjusted.initiation.veraison = average.initiation.veraison;
   if (CORN::is_zero<float32>(average.initiation.maturity/*200127,0.0,0.00001*/))
         std::clog << "warning:maturity not specified" << std::endl;
   else  adjusted.initiation.maturity = average.initiation.maturity;

   continue here add duration.senescence

   /*200125 obsolete
   if (CORN::is_zero<float32>(average.initiation.senescence,0.0,0.00001))
         std::clog << "warning:senescence not specified" << std::endl;
   else  adjusted.initiation.senescence = average.initiation.senescence;
   if (CORN::is_zero<float32>(average.culmination.senescence,0.0,0.00001))
         std::clog << "warning:full_senescence not specified" << std::endl;
   else  adjusted.culmination.senescence = average.culmination.senescence;
   */

   /*NYN
   if (CORN::is_zero<float32>(average.initiation.tuber,0.0,0.00001))
         std::clog << "warning:tuber_initiation  not specified" << std::endl; // warning check label
   else  adjusted.initiation.tuber = average.initiation.tuber;
   */
   /*NYN
   if (CORN::is_zero<float32>(average.culmination.root_depth,0.0,0.00001))
         std::clog << "warning:root_depth_max not specified" << std::endl;  warning check label
   else  adjusted.culmination.root_depth = average.culmination.root_depth;
   */
   return true;
}
//_Phenology_adjustor::apply_averages_______________________________2016-02-11_/
#ifdef OFOOT_VERSION
Phenology_adjustors::Phenology_adjustors
(Sun &sun_                                                                       //160201
,::Weather &weather_
,modifiable_ CORN::Date  &today_)                                                //150211
: CS::Simulation_element_composite(today_)
, sun                   (sun_)                                                   //160201
, weather               (weather_)
, today_modifiable      (today_)                                                 //150624_150211
, adjustments_calculated(false)
{}
//_Phenology_adjustors:constructor_____________________________________________/
Phenology_adjustor* Phenology_adjustors::add_crop
(const CORN::OS::File_name  & crop_filename
,const CORN::Date           & typical_planting_date_relative
,const CORN::Date           * typical_relative) //[NGS_COUNT]; //owned by caller
{
   nat8 relative_adjustments = 0;
   for (nat8 ngs = 0; ngs < NGS_COUNT; ngs++)
      if (typical_relative[ngs].get_DOY() != 0)
         relative_adjustments +=1;
   if (!relative_adjustments) return 0;
   Phenology_adjustor *new_phenology_adjustor =
    new Phenology_adjustor
         (crop_filename.w_str() //get_name() // only to uniquely identify        //150211
         ,today
         ,typical_planting_date_relative
         ,typical_relative

         continue here need weather and weather provider

         ,weather.weather_data.ref


         //160201,weather
         );
   Phenology_adjustor *existing_phenology_adjuster
      = dynamic_cast<Phenology_adjustor *>(
       sub_elements.find_comparable(*new_phenology_adjustor));
   if (existing_phenology_adjuster)
      delete new_phenology_adjustor;
   else
   {  Crop_parameters *crop_parameters_reliquished = new Crop_parameters;
      CORN::VV_File crop_file(crop_filename.c_str());
      crop_file.get(*crop_parameters_reliquished);
      crop_filename.append_to_wstring(crop_parameters_reliquished->data_source_URL);//140731
      new_phenology_adjustor->take_crop_parameters(crop_parameters_reliquished);
      // Adds the specified crop to the list of Phenology_adjustor
      sub_elements.append(new_phenology_adjustor);
      existing_phenology_adjuster = new_phenology_adjustor;
   }
   return existing_phenology_adjuster;
}
//_Phenology_adjustors::add_crop_______________________________________________/
#endif
float32 adjust_full_senescence
(float32 full_senescence_unadjusted
,float32 reference_point_unadjusted // max_canopy or flowering unadjusted        // 160204
,float32 mature_unadjusted
,float32 reference_point_average    // max_canopy or flowering_average           //160204
,float32 maturity_average
,bool    maturity_significant)                                                   //160102 const
{
   float32 overshoot_ratio
      = full_senescence_unadjusted
       / ( maturity_significant
            ? mature_unadjusted
            : reference_point_unadjusted  // max_canopy_unadjusted               //160204
            );
   float32 full_senescence_adjusted
      = overshoot_ratio *
       (maturity_significant
         ? maturity_average
         : reference_point_average        // max_canopy_average                  //160224
         );
   return full_senescence_adjusted;
}
//_adjust_full_senescence___________________________________________2015-02-09_/
float32 adjust_generalized
( float32 calibrated  // specific phenology parameter to adjust
, float32 early_unadjusted
, float32 late_unadjusted
, float32 early_average
, float32 late_average)                                                    // const
{
   return ((calibrated      - early_unadjusted)
          /(late_unadjusted - early_unadjusted))
          *(late_average - early_average) + early_average;
}
//_adjust_generalized_______________________________________________2015-02-25_/
float32 adjust_given_emergence_accrescence_maturity
(float32 calibrated  // specific calibrated phenology parameter to adjust
,const CropSyst::Crop_parameters_struct::Phenology &unadjusted
,const CropSyst::Crop_parameters_struct::Phenology &average
,const CropSyst::Crop_parameters_struct::Phenology &adjusted
,bool  maturity_significant)                                              // const
{
   return
      (calibrated > unadjusted.culmination.accrescence)
         ? maturity_significant
            ? (calibrated > unadjusted.initiation.maturity)
               // Note that in the case where we adjust based on full senescence
               // we use the adjusted full senescence
               // not the average (because is is not computed)
               ?  /* case C */ adjust_generalized(calibrated,unadjusted.initiation.maturity     ,unadjusted.culmination.senescence  ,average.initiation.maturity      ,adjusted.culmination.senescence  )
               :  /* case D */ adjust_generalized(calibrated,unadjusted.culmination.accrescence ,unadjusted.initiation.maturity     ,average.culmination.accrescence  , average.initiation.maturity     )
            :     /* case B */ adjust_generalized(calibrated,unadjusted.culmination.accrescence ,unadjusted.culmination.senescence  ,average.culmination.accrescence  , adjusted.culmination.senescence )
         :        /* case A */ adjust_generalized(calibrated,unadjusted.initiation.emergence    ,unadjusted.culmination.accrescence ,average.initiation.emergence     , average.culmination.accrescence )   //  between emerge and max canopy
      ;
}
//_adjust_given_emergence_accrescence_maturity______________________2015-02-25_/
float32  adjust_given_planting_flowering_maturity
(float32 calibrated  // specific calibrated phenology parameter to adjust
,const CropSyst::Crop_parameters_struct::Phenology &unadjusted
,const CropSyst::Crop_parameters_struct::Phenology &average
,const CropSyst::Crop_parameters_struct::Phenology &adjusted
,bool  maturity_significant)                                              // const
{
   return
      (calibrated > unadjusted.initiation.flowering)
         ? maturity_significant
            ? (calibrated > unadjusted.initiation.maturity)
               // Note that in the case where we adjust based on full senescence
               // we use the adjusted full senescence
               // not the average (because is is not computed)
               ?  /* case C */ adjust_generalized(calibrated,unadjusted.initiation.maturity  ,unadjusted.culmination.senescence  ,average.initiation.maturity  , adjusted.culmination.senescence  )
               :  /* case D */ adjust_generalized(calibrated,unadjusted.initiation.flowering ,unadjusted.initiation .maturity    ,average.initiation.flowering , average.initiation.maturity     )
            :     /* case B */ adjust_generalized(calibrated,unadjusted.initiation.flowering ,unadjusted.culmination.senescence  ,average.initiation.flowering , adjusted.culmination.senescence )
         :        /* case A */ adjust_generalized(calibrated,unadjusted.initiation.emergence ,unadjusted.initiation .flowering   ,average.initiation.emergence , average.initiation.flowering )   //  between emerge and max canopy
      ;
}
//_adjust_given_planting_flowering_maturity_________________________2015-02-25_/

#ifdef OFOOT_VERSION
bool Phenology_adjustors::adjust_phenology_parameters
(const std::wstring &data_source_URL                                             //140731
,modifiable_ CropSyst::Crop_parameters_struct::Phenology &phenology_parameters)
{

   std::string data_source_URL_str; CORN::wstring_to_string(data_source_URL,data_source_URL_str);
   std::clog << "Adjust phenology for crop:" << data_source_URL_str << std::endl;

   bool adjusted = adjustments_calculated;
   if (!adjustments_calculated)
      adjusted = determine_adjustments();
      // The filename is used to uniquely identify the crop adjustments.
      // The adjustments must have been previously calculated with a call to
      // determine_adjustments().
      // This method does not open or modify the file.
      // We pass the phenology parameters to be modified.

   if (adjusted)
   {
      const Phenology_adjustor *adjustor = dynamic_cast<const Phenology_adjustor *>
         (sub_elements.find_wstring(data_source_URL));
      if (adjustor)
      {

         adjustor->commit_adjusted_phenology_parameters                          //160203
            (phenology_parameters

            );
         /*160203 moved to commit_adjusted_phenology_parameters
         CropSyst::Crop_parameters_struct::Phenology unadjusted;                 //150209
         unadjusted.initiation.emergence     = phenology_parameters.initiation.emergence     ;
         unadjusted.initiation.tuber         = phenology_parameters.initiation.tuber         ;
         unadjusted.culmination.accrescence  = phenology_parameters.culmination.accrescence  ;
         unadjusted.culmination.root_depth   = phenology_parameters.culmination.root_depth   ;
         unadjusted.initiation.flowering     = phenology_parameters.initiation.flowering     ;
         unadjusted.initiation.filling       = phenology_parameters.initiation.filling       ;
         unadjusted.initiation.maturity      = phenology_parameters.initiation.maturity      ;
         unadjusted.initiation.senescence    = phenology_parameters.initiation.senescence    ;
         unadjusted.culmination.senescence   = phenology_parameters.culmination.senescence   ;
         unadjusted.initiation.rapid_fruit_growth = phenology_parameters.initiation.rapid_fruit_growth;
         #if (!defined(YAML_PARAM_VERSION) || (CROPSYST_VERSION==4))
         unadjusted.WUE_change       = phenology_parameters.WUE_change;
         #endif
         unadjusted.phenologic_sensitivity_water_stress = phenology_parameters.phenologic_sensitivity_water_stress;
         unadjusted.maturity_significant     = phenology_parameters.maturity_significant;

         // The adjusted values will ultimately be stored back into the phenology_parameters structure;
         CropSyst::Crop_parameters_struct::Phenology &adjusted = phenology_parameters;
         CropSyst::Crop_parameters_struct::Phenology average;                    //150209

         std::clog << "info:Phenology adjusting:" << std::endl;
         average.initiation.emergence     = adjustor->start_deg_days_stats[NGS_EMERGENCE]->get_mean(); //141031
         if (average.initiation.emergence    )
         {
            adjusted.initiation.emergence = average.initiation.emergence  ;
         } else
         {  std::clog << "error:emergence not adjusted" << std::endl;
            return false;
         }

         average.culmination.accrescence   =adjustor->start_deg_days_stats[NGS_END_CANOPY_GROWTH]->get_mean();
         if (average.culmination.accrescence  )
         {  adjusted.culmination.accrescence           = average.culmination.accrescence  ;
         }
         else
         {  std::clog << "error:max canopy not adjusted" << std::endl;
            return false;
         }

         bool maturity_significant = phenology_parameters.maturity_significant;
         float32 avg_degdays_maturity = 0.0;
         if (maturity_significant)
         {
            average.initiation.maturity      =  adjustor->start_deg_days_stats[NGS_MATURITY]->get_mean();
            if (average.initiation.maturity      > 0.0000001)
            {  phenology_parameters.initiation.maturity      = average.initiation.maturity;
            }
         }

         // Must adjusted full_senescence_deg_day first because
         // it may be needed to determine values relative to maturity or max canopy
         adjusted.culmination.senescence         = adjust_full_senescence
            (unadjusted.culmination.senescence
            ,unadjusted.culmination.accrescence
            ,unadjusted.initiation.maturity
            ,average.culmination.accrescence
            ,average.initiation.maturity
            ,maturity_significant);


         // average Emergence, max canopy cover, and maturity  already calculated
         adjusted.initiation.tuber        = adjust_given_emergence_accrescence_maturity(unadjusted.initiation.tuber      ,unadjusted,average,adjusted,maturity_significant);
         adjusted.culmination.root_depth  = adjust_given_emergence_accrescence_maturity(unadjusted.culmination.root_depth,unadjusted,average,adjusted,maturity_significant);
         adjusted.initiation.flowering    = adjust_given_emergence_accrescence_maturity(unadjusted.initiation.flowering  ,unadjusted,average,adjusted,maturity_significant);
         adjusted.initiation.filling      = adjust_given_emergence_accrescence_maturity(unadjusted.initiation.filling    ,unadjusted,average,adjusted,maturity_significant);
         adjusted.initiation.senescence   = adjust_given_emergence_accrescence_maturity(unadjusted.initiation.senescence ,unadjusted,average,adjusted,maturity_significant);
         // adjusted.full_senescence_deg_day is calculated above
         adjusted.WUE_change              = adjust_given_emergence_accrescence_maturity(unadjusted.WUE_change,unadjusted,average,adjusted,maturity_significant);
         adjusted.initiation.rapid_fruit_growth = adjust_given_emergence_accrescence_maturity(unadjusted.initiation.rapid_fruit_growth,unadjusted,average,adjusted,maturity_significant);

         std::clog << (maturity_significant ? "maturity significant" :  "maturity not significant") << std::endl;
         std::clog << "emerged     GDD:" << unadjusted.initiation.emergence     << "->" << adjusted.initiation.emergence     << std::endl;
         if (adjustor->crop_parameters_owned->harvested_part_labeled.get() == tuber_part)
            std::clog<<"tuber init  GDD:"<< unadjusted.initiation.tuber         << "->" << adjusted.initiation.tuber         << std::endl;
         std::clog << "max root    GDD:" << unadjusted.culmination.root_depth   << "->" << adjusted.culmination.root_depth   << std::endl;
         std::clog << "flowering   GDD:" << unadjusted.initiation.flowering     << "->" << adjusted.initiation.flowering     << std::endl;
         std::clog << "filling     GDD:" << unadjusted.initiation.filling       << "->" << adjusted.initiation.filling       << std::endl;
         std::clog << "canopy max  GDD:" << unadjusted.culmination.accrescence  << "->" << adjusted.culmination.accrescence  << std::endl;
         std::clog << "senesc beg  GDD:" << unadjusted.initiation.senescence    << "->" << adjusted.initiation.senescence    << std::endl;
         if (maturity_significant)
            std::clog<<"maturity    GDD:"<< unadjusted.initiation.maturity      << "->" << adjusted.initiation.maturity      << std::endl;
         std::clog << "senesc full GDD:" << unadjusted.culmination.senescence   << "->" << adjusted.culmination.senescence   << std::endl;
         // NYI
         // std::clog << "fruit rapidGDD:" << unadjusted.begin_rapid_fruit_growth_deg_day;
         // std::clog << "WUE change GDD:" << unadjusted.WUE_change_deg_day;
         */
      }
      else
      {  // should output explaination
         std::clog << "warn:Phenology not adjusted:" << std::endl;
         adjusted = false;
      }
   }
   return adjusted;
}
//_Phenology_adjustors::adjust_phenology_parameters____________________________/
#endif
#ifdef OFOOT_VERSION
bool Phenology_adjustors::determine_adjustments()
{
/*
Before running the simulation the program (CropSyst OFoot) will calculate for the simulation period (30 year)
 the thermal time from the planting date and record the GDD for

 both maxCC and maturity date for each 'season' of all the years to get the average GDD for each occurrence of the crop started on the
 When the simulation runs, the respective crop phenology parameters will be set 'in memory'.
 Claudio says that we dont need to adjust the other phenology parameters. (meeting 13-09-24)
When Cropsyst reads the rotation file, calculate average growing degree days over a 30 year span to optimize the crop template file for the simulated farm (meeting 13-09-24)
Maximum canopy cover will change in the crop file based on the subroutine, but date of harvest will not.
It is necessary to 'calibrate' maximum canopy cover and maturity thermal time accumulation crop parameters to 'typical' year dates (day of year) provided by the user. (meeting 13-09-24)

*/
      // Calculateds adjustments for all crops previously added
      // either with add_crop or add_crops_in_rotation.

   bool continue_processing = start();
   while(continue_processing)
   {
      std::clog << "date:" << today.as_string() << "     \r";
      continue_processing &= start_day();
      know_day_length_hours(day_length_hours);                                   //160201
      continue_processing &= process_day();
      continue_processing &= end_day();
      today_modifiable.inc();
   }
   adjustments_calculated = stop();
   return adjustments_calculated;
}
//_Phenology_adjustors::determine_adjustments(_________________________________/
bool Phenology_adjustors::start()
{

   #ifdef OFOOT_VERSION
   // This probably should now be done by the user of this class
   today_modifiable.set_date32(weather.get_earliest_available_date() );
   #endif
   bool
      started =
      #ifdef OFOOT_VERSION
         weather.start() &&
         sub_elements.count() > 0 &&
      #endif
      CS::Simulation_element_composite::start();
   return started;
}
//_Phenology_adjustors::start__________________________________________________/
bool Phenology_adjustors::start_day()                              modification_
{
   bool started =
      #ifdef OFOOT_VERSION
      weather.start_day() &&
      #endif
      CS::Simulation_element_composite::start_day();                             //150624
   return started;
}
//_Phenology_adjustors::start_day___________________________________2015-02-11_/
#endif
}//_namespace CropSyst_________________________________________________________/
/* todo

When Cropsyst reads the rotation file, calculate average growing degree days over a 30 year span
to optimize the crop template file for the simulated farm (meeting 13-09-24)
Maximum canopy cover will change in the crop file based on the subroutine, but date of harvest will not.
It is necessary to 'calibrate' maximum canopy cover and maturity thermal time accumulation crop parameters to 'typical' year dates (day of year) provided by the user. (meeting 13-09-24)
*/

