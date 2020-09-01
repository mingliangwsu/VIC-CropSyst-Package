#ifndef VIC_crop_commonH
#define VIC_crop_commonH
#include "VIC_crop_interface.h"
#include "VIC_crop_param.h"

namespace VIC_crop
{
//______________________________________________________________________________
class Crop_common
: public implements_ Crop_interface
{
 protected: // external references
   const Crop_parameters            &parameters;
   CORN::Date &today;
 protected:
   bool clipped_automatically;
   bool mint_clipping;
 public:
   Crop_common
      (const Crop_parameters& _parameters
      ,CORN::Date &           _today
      ,bool                   _clipped_automatically
      ,bool                   _mint_clipping
      );
 protected:
   virtual bool process_harvest()                                  modification_;
   virtual float64 clipped_yield()                                 modification_=0;
   virtual bool is_ready_for_clipping()                                    const=0;
   virtual bool respond_to_clipping()                              modification_=0;
   virtual bool perform_clipping_and_ensure_end_of_pseudodormancy()modification_=0;
};
//_2014-05-09___________________________________________________________________
} //namespace VIC_crop
#endif

