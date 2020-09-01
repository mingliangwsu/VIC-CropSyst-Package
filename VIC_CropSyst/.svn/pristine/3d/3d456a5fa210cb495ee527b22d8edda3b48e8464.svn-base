#include "VIC_crop_common.h"
namespace VIC_crop
{
//______________________________________________________________________________
Crop_common::Crop_common
(const Crop_parameters &   _parameters
,CORN::Date &              _today
,bool                      _clipped_automatically                       //110426
,bool                      _mint_clipping                               //110427
)
: parameters               (_parameters)
, today                    (_today)
, clipped_automatically    (_clipped_automatically)
, mint_clipping            (_mint_clipping)
{
}
//______________________________________________________________________________
bool Crop_common::process_harvest()                                       modification_
{  bool harvested = false;                                                       //110620
   // Search for harvest of annual crops
   const CropSyst::Phenology_interface &phenology_actual = ref_phenology();      //140811
   if (!parameters.is_perennial())                                               //   If Not CropParameters.PerennialCrop And CropState.CropMature Then
   {
      if (phenology_actual.is_mature())
      {
         if (phenology_actual.get_days_since_start_of(NGS_MATURITY) == 10)       //       If TimeState.DaysAfterPhysiologicalMaturity = 10 Then
         {  // Harvest is assumed to occur 10 days after maturity.
            harvest_annual(true);
            harvested = true;                                                    //110620
            /* CropHarvested set by harvest_annual */                            //           CropState.CropHarvested = True
            /* DOYHarvestset by harvest_annual */                                //           CropState.DOYHarvest = TimeState.DayOfYear
            /* DaysAfterCropExistAtEndOfSeason either set by harvest_annual or not used in c++*/ //           CropState.DaysAfterCropExistAtEndOfSeason = TimeState.DaysAfterCropExist
            /* CropTerminated set by harvest_annual */                           //           SimControl.CropTerminated = True
            /* 110405 use function to check if season ended */                   //           SimControl.CropSeasonEnded = True 'XXX ROGER LATEST March 25 2011
            /* CropExist not appl to C++ */                                      //           SimControl.CropExist = False
            /* NewCropFound not appl to C++ */                                   //           SimControl.NewCropFound = False
            /* CropSeasonStartednot appl to C++ */                               //           SimControl.CropSeasonStarted = False
          }                                                                      //       End If
      }  /*End of search for harvest of annual crops*/                           //   End If 'End of search for harvest of annual crops

   }
   // calculate yield for clipped perennials (not fruit trees)
   else //                                                                       //If CropParameters.PerennialCrop Then
   {  // special case of perennial where it has been a year in pseudodormancy
      // we break the pseudodormancy and get a yield.
      // Search for harvest of tree fruit crops
      if (parameters.is_fruit_tree())                                            //   If CropParameters.TreeFruitCrop And CropState.CropMature Then
      {
         if (phenology_actual.is_mature() &&
             (phenology_actual.get_days_since_start_of(NGS_MATURITY) == 5))      //       If TimeState.DaysAfterPhysiologicalMaturity = 5 Then 'Harvest is assumed to occur 5 days after maturity
         {  // Harvest is assumed to occur 5 days after maturity
            harvest_fruit_tree();                                                //     Call DetermineTreeFruitYield(CropParameters, CropState)
            harvested = true;                                                    //110620
            /* moved to harvest_fruit_tree*/                                     //           CropState.CropHarvested = True
            /* moved to harvest_fruit_tree*/                                     //           CropState.DOYHarvest = TimeState.DayOfYear
         }                                                                       //       End If
      } /* End of search for harvest of tree fruit crops */                      //   End If 'End of search for harvest of tree fruit crops
      else
      if (clipped_automatically)                                                 //110426
      {  if (!mint_clipping)                                                     //110620
         {
            if (perform_clipping_and_ensure_end_of_pseudodormancy())
            {  if (today.get_DOY() != 1)
               {  respond_to_clipping();
                  clipped_yield();                                                  //            Call DetermineClippedYield(CropParameters, CropState, TimeState)
                  harvested = true;                                              //110620
         }   }  }
         if (is_ready_for_clipping())
         {
            respond_to_clipping();
            clipped_yield();                                                  //            Call DetermineClippedYield(CropParameters, CropState, TimeState)
            harvested = true;                                                    //110620
         }
       }
   }  /* End of write output for perennial crops at the onset of dormancy */     //End If
   return harvested;
}
//_2014-08-11__________________________________________________________________/
} //namespace VIC_crop
