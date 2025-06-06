#include "cs_mgmt.h"
#include "mgmtevnt.h"
#include "cs_events.h"

// This adds events from the current management file to
//  the management event lists.
//
//  When sync is an actual date, the actual date is entered.
//     (This will pick up all management entries with real dates).
//  When sync is a month, the current date's year is extracted and
//  appended to
namespace CropSyst {
//______________________________________________________________________________
void Management::transfer_all                  // rename to transfer_all_to
( Common_event_list &to_event_list                                               //991122
, bool run_nitrogen
, Synchronization_mode relevant_sync                                             //151004
, const char  *management_filename)                                              //041015
   // May be 0 if not to be associated with a mangement filename in schedule
{
// The following set filenames, identify the management file containing the
// original parameters (this is used only by the schedule viewer for cross referencing)
   if (management_filename)                                                      //041015
   {  irrigation_operations            .set_event_filename(management_filename);
      inorganic_nitrogen_operations    .set_event_filename(management_filename);
#if ((CROPSYST_VERSION > 0) && (CROPSYST_VERSION < 5))
      organic_nitrogen_operations      .set_event_filename(management_filename);
#endif
      biomatter_application_operations .set_event_filename(management_filename);
      tillage_operations               .set_event_filename(management_filename);
      residue_operations               .set_event_filename(management_filename);
      chemical_operations              .set_event_filename(management_filename);
      harvest_or_clipping_operations   .set_event_filename(management_filename);
      auto_N_applications              .set_event_filename(management_filename);
      auto_irrigation_mode_operations  .set_event_filename(management_filename);
      auto_clip_biomass_mode_operations.set_event_filename(management_filename);
      #ifdef LADSS_MODE
      LADSS_operations                 .set_event_filename(management_filename);
      #endif
   }
   #ifdef LADSS_MODE
   to_event_list.transfer_from(LADSS_operations                 ,relevant_sync); //151004_030810
   #endif
   to_event_list.transfer_from(tillage_operations               ,relevant_sync); //151004_991122
      // Used only in LADSS mode, but may put all operations here in one list
   to_event_list.transfer_from(residue_operations               ,relevant_sync); //151004_991122
   to_event_list.transfer_from(harvest_or_clipping_operations   ,relevant_sync); //031014
   to_event_list.transfer_from(irrigation_operations            ,relevant_sync); //151004_991122
   to_event_list.transfer_from(chemical_operations              ,relevant_sync); //151004_991122
   to_event_list.transfer_from(auto_clip_biomass_mode_operations,relevant_sync); //011208
   to_event_list.transfer_from(auto_irrigation_mode_operations  ,relevant_sync); //151004_020313
#ifdef NITROGEN
   if (run_nitrogen)
   {  to_event_list.transfer_from(inorganic_nitrogen_operations ,relevant_sync); //151004
#if ((CROPSYST_VERSION > 0) && (CROPSYST_VERSION < 5))
      to_event_list.transfer_from(organic_nitrogen_operations   ,relevant_sync); //151004
#endif
      to_event_list.transfer_from
         (biomatter_application_operations                      ,relevant_sync); //151004
      if (N_application_soil_observation_event)                                  //060804
      {  if (auto_N_applications.count())                                        //160328
            N_application_soil_observation_event->split_applications = true;     //160328
         if ((N_application_soil_observation_event->begin_sync.get_sync_mode()   //160326
               == relevant_sync) || (relevant_sync == ANY_SYNC_MODE))            //160326
         {  to_event_list.append(N_application_soil_observation_event);          //060804
             N_application_soil_observation_event = 0;
      }  }
      to_event_list.transfer_from(auto_N_applications,relevant_sync);            //151004_020314
   }
#endif
}
//_transfer_all________________________________________________________________/
}//_namespace CropSyst_________________________________________________________/

