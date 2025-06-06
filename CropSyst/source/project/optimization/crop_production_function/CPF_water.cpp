#include "project/optimization/crop_production_function/CPF_water.h"
#include "corn/data_source/vv_file.h"
#include "UED/library/database_file.h"
#include "UED/library/timeqry.h"
#include "CS_suite/application/CS_suite_directory.h"
#include "csvc.h"
#include "management/management_param_V4.h"

namespace CropSyst {
//______________________________________________________________________________
Crop_production_function_simulation::Crop_production_function_simulation
(const CORN::OS::Directory_name  &primary_scenario_dirname_
,const CORN::OS::File_name       *format_filename_
,nat32                            CPF_water_percents_)
: irrig_event_list()
, primary_scenario_directory                   (primary_scenario_dirname_,false)
   // should already exist
, format_filename                                             (format_filename_)
, CPF_water_percents                                       (CPF_water_percents_)
, irrigation_adjustment_filename                                             (0)
{}
//_Crop_production_function_simulation:consturctor_____________________________/
Crop_production_function_simulation::Crop_production_function_simulation
(const CORN::OS::Directory_name  &primary_scenario_dirname_
,const CORN::OS::File_name       *format_filename_
,const CORN::OS::File_name       &irrigation_adjustment_filename_)
: irrig_event_list()
, primary_scenario_directory                   (primary_scenario_dirname_,false)
   // should already exist
, format_filename                                             (format_filename_)
, CPF_water_percents                                                         (0)
, irrigation_adjustment_filename
             (new CORN::OS::File_name_concrete(irrigation_adjustment_filename_)) //150417
{}
//_Crop_production_function_simulation:constructor__________________2015-04-17_/
void Crop_production_function_simulation::collect_automatic_irrigation
( const char *ued_filename)
{  UED::Database_file_indexed ued_database(ued_filename,std::ios_base::in,false);//150417
   ued_database.initialize();
   Date_time_64 earliest_date(ued_database.get_earliest_date_time());// all records all variable
   Date_time_64 latest_date(ued_database.get_latest_date_time());    // all records all variable
   int irrig_count = 0;
   for (Date_time_64 date(earliest_date)
       ; date.get_datetime64() <= latest_date.get_datetime64(); date.inc())      //031218
   {  UED::Time_query_single_date          query_date(date.get_datetime64());    //040217
      Units_code          requested_units = UC_mm;                               //040716
      CORN::Quality_clad attribute(UED_generated);                               //150417_110725
       float32 irrigation_mm = ued_database.get_no_create
         (UED::Record_type_distinguisher_interface::UED_data_set_year_var_units
         ,CSVC_mgmt_irrig_applied_time_step_sum,query_date,requested_units,attribute);
      if (! CORN::is_zero<float32>(irrigation_mm /*,0.0,0.00001*/))
      {  CPF_Irrigation_event *irrig_event = new CPF_Irrigation_event
            (date.get_datetime64(),irrigation_mm);                               //081118
         irrig_event_list.append(irrig_event);
         irrig_count++;
}  }  }
//_Crop_production_function_simulation::collect_automatic_irrigation___________/
CORN::OS::Directory_name *Crop_production_function_simulation::create_scenario
(nat8 percent_of_optimal
,const CORN::Dynamic_array<float32>*irrig_adj_fraction                           //081117
   // Each element in the array represents the amount to adjust
   // irrigation
   // If 0 the percent of optimal value is used
,const CORN::Dynamic_array<nat32>*irrig_adj_dates                                //081117
   // The corresponding dates as date32 values
) rendition_
{   // Note that this method has two modes depending on if irrig_adj_fraction is provided 081117
   bool irrig_adj_mode = (irrig_adj_fraction && irrig_adj_dates);                //081117
   CORN::OS::Directory_name *rendered_CPF_scenario =                             //150417
      irrigation_adjustment_filename
      ? primary_scenario_directory.render_crop_production_function_adjustment_schedule
         (irrigation_adjustment_filename->get_name())
      : primary_scenario_directory.render_crop_production_function_adjustment_percent
         (percent_of_optimal);

   std::clog << "creating scenario:"<< std::endl << rendered_CPF_scenario->c_str() << std::endl;
   CORN::OS::file_system_engine.provide_directory(*rendered_CPF_scenario);

   // Create a copy of the primary simulation and rotation files: named 10.* respectively.
   if (percent_of_optimal || irrig_adj_mode)
   {  // We don't want to have any irrigations for 0% (otherwise cropsyst will refill on amount=0). //000401

      CORN::OS::File_name_concrete CPF_irrigation_filename((*rendered_CPF_scenario),"irrigation",CS_management_LEXT);
      CropSyst::Management_parameters irrig_mgmt;                                //131007_010112
      irrig_mgmt.description.brief.assign("crop production function simulation ");
      CORN::append_wstring_to_string(rendered_CPF_scenario->get_name(),irrig_mgmt.description.brief); //150417
      irrig_mgmt.description.brief.append("% of optimal irrigation amount");
      FOR_EACH_IN(irrig_event,CPF_Irrigation_event,irrig_event_list,each_irrig)  //031218
       {
         float32 irrig_adj_fract = percent_of_optimal / 100.0;                   //081117
         if (irrig_adj_mode)                                                     //081117
         {  irrig_adj_fract = 1.0;   // Default to 1 if no adjustments dates     //081117
            Date32 irrig_event_date = irrig_event->date.get_date32();            //120316_081117
            if (!irrig_event_date) // it may be setup in begin_sync              //081117
               irrig_event_date = irrig_event->begin_sync.get_date32();          //081117
            nat32 date_index;                                                    //081117
            irrig_adj_dates->occurs(irrig_event_date, date_index);
            irrig_adj_fract = irrig_adj_fraction->get(date_index);               //081117
         }
         CropSyst::Irrigation_operation *irrig_op = new CropSyst::Irrigation_operation(); //131007_021203
         irrig_op->amount_mm = irrig_event->amount * irrig_adj_fract;            //081117
         //NYI irrig_op->salinity_32 = 0.0;
         //NYI irrig_op->chemical_concentration_32 = 0.0;
         //NYI irrig_op->net_irrigation_mult_32 = 0.0;
         //NYI irrig_op->chemical_name = "";
         Common_event *irrig_op_record
            = new Common_event(irrig_event->begin_sync,irrig_op,true);
          irrig_mgmt.irrigation_operations.append(irrig_op_record);
       } FOR_EACH_END(each_irrig)
      CORN::VV_File irrig_mgmt_file(CPF_irrigation_filename.c_str());
      irrig_mgmt_file.set(irrig_mgmt);
   }
   return rendered_CPF_scenario;                                                 //150417
}
//_Crop_production_function_simulation::create_scenario________________________/
void Crop_production_function_simulation::run(bool &CPF_abort)
{
   // The simulation must have automatic irrigation management
   // and daily irrigation output

   // Run the primary simulation (should be with auto irrigation)
   // Delete the crop production function directories from previous run
   // Note we want to make sure all the directories are deleted
   // so we can tell that all the simulations were completely run.

   std::clog << "running primary scenario:" << primary_scenario_directory << std::endl; //150417
   run_scenario(primary_scenario_directory);                                     //150417
   // Current directory will be set by sim_rerun and should be for the primary simulation.
   // Collect the irrigation into a list of irrigation events
   std::clog << "collecting automatic irrigation "<< std::endl;
   collect_automatic_irrigation(primary_scenario_directory.CS_output_daily_UED_cstr);
   bool specifically_scheduled_adjustments =
      irrigation_adjustment_filename &&
      CORN::OS::file_system_engine.exists(*irrigation_adjustment_filename);      //150417
   if (specifically_scheduled_adjustments)                                       //081117
   {  std::clog << "running with specifically scheduled adjustments" << std::endl;
      CORN::Dynamic_array<float32>  irrig_adj_fraction(20000,366);               //101018
      CORN::Dynamic_array<nat32>    irrig_adj_dates   (20000,366,0);             //101018
      // Each element in the array represents the amount to adjust               //081117
      // irrigation                                                              //081117
      ifstream irrig_adj_file(irrigation_adjustment_filename->c_str());
         //open the irrigation_adjustments file                                  //081117
      int      year;
      int      DOY;
      float32  irrig_adj;
      int adj_index = 0;
      while (!irrig_adj_file.eof())                                              //081117
      {  // read the year DOY and irrig_adj_fraction
         irrig_adj_file >> year >> DOY >> irrig_adj;
         nat32 irrig_adj_date = year * 1000 + DOY;
         irrig_adj_fraction.set(adj_index,irrig_adj);
         irrig_adj_dates   .set(adj_index,irrig_adj_date);
         adj_index++;
      }
      // Make sure that we start in the correct directory for each run. 081117_
      std::clog << "creating scenario:" << irrigation_adjustment_filename->c_str() << std::endl;
      CORN::OS::Directory_name *adj_scenario_dirname_rendered
         = create_scenario(0,&irrig_adj_fraction,&irrig_adj_dates);              //150417
      std::clog << "running simulation:" << adj_scenario_dirname_rendered->c_str() << std::endl;
      run_scenario(*adj_scenario_dirname_rendered);
      // std::clog << "recycling database:" << scenario_database_dirname << std::endl;
      // scenario_database_dirname.recycle(false);  // might want to delete forever but for now just recycle incase user want's to look at the processing
   } else
   {  // CPF_water_percents is a bit array selecting the percentages to run //081117
      std::clog << "running with selected adjustments" << std::endl;
      //Used to go to just 90 now need up to 150%
      for (int i = 0; (i < 16) && (!CPF_abort) ; i++)                            //030530_
      {  bool run_percent = CPF_water_percents & (1 << i);                       //030530
         if (run_percent)                                                        //030530
         {  nat8 percent_of_optimal =  i * 10;                                   //030530_
            CORN::OS::Directory_name *adj_scenario_dirname_rendered              //150417
               = create_scenario(percent_of_optimal,NULL,NULL);                  //150417_030720
            std::clog << "running scenario:"<< std::endl << (*adj_scenario_dirname_rendered) << std::endl; //150417
            CORN::OS::Directory_name_concrete adj_scenario_dirname(adj_scenario_dirname_rendered->c_str());
            run_scenario(adj_scenario_dirname);                                  //150417
            delete adj_scenario_dirname_rendered;                                //150417
            // std::clog << "recycling database:"<< scenario_database_dirname << std::endl;
            // scenario_database_dirname.recycle(false);  // might want to delete forever but for now just recycle incase user want's to look at the processing   //030720_
}  }  }  }
//______________________________________________________________________________
void Crop_production_function_simulation::run_scenario(CORN::OS::Directory_name &scenario_dirname)
{  std::string command("\"");
   CS::Suite_directory->CropSyst().cropsyst_model_executable().append_to_string(command);  //150418
   command.append("\" ");  command.append(" \"");                                //030516
   command.append(scenario_dirname.c_str());
   command.append("\" ");
   if (format_filename)
   {  command.append("\"");
      command.append(format_filename->c_str());
      command.append("\" ");
   }
   PROCESS_INFORMATION process_info;
   STARTUPINFO  startup_info;
   GetStartupInfo(&startup_info);
   startup_info.dwFlags |= STARTF_USESHOWWINDOW;
   startup_info.wShowWindow = SW_NORMAL; // SW_HIDE; // SW_NORMAL;
   bool result = true;
      if (CreateProcess
            (0
            ,(char *)command.c_str()
            ,0 // default security
            ,0 // default security
            ,true
            ,CREATE_SEPARATE_WOW_VDM|NORMAL_PRIORITY_CLASS
            ,0  // current environment block
            ,0  // pointer to current directory name  (should be current)
            ,&startup_info // pointer to STARTUPINFO
            ,&process_info // pointer to PROCESS_INFORMATION
            ))
         WaitForSingleObject(process_info.hProcess,INFINITE);
      else
         result =  false;
      // Must close process and thread handles or you get a memory leak.
   std::clog << "closing handles" << std::endl;
      CloseHandle(process_info.hProcess);
      CloseHandle(process_info.hThread);
}
//_Crop_production_function_simulation::run_scenario___________________________/
}//_namespace CropSyst__________________________________________________________


