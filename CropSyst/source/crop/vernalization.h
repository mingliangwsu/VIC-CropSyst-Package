#error abandoned, delete this

#ifndef vernalizationH
#define vernalizationH
namespace CropSyst
{
//____________________________________________________________________________
class Vernalization
{
 private:
   const CropSyst::Crop_parameters_struct::Vernalization &parameters;         //080605
   contribute_ float64 vern_days;
   contribute_ float64 Vfactor;
 public:
   enum Status {VERNALIZATION_INACTIVE,VERNALIZATION_START,VERNALIZATION_ACTIVE,VERNALIZATION_END};
   contribute_ Status status;
 public:
   Vernalization(const Crop_parameters_struct::Vernalization &_parameters);
   inline bool is_satisfied()                                          const
      { return status == VERNALIZATION_END; }
   float64 update_factor(float64 temperature)                 rectification_;
};
//_class Vernalization_______________________________________________________/
//____________________________________________________________________________
}//_namespace CropSyst_________________________________________________________/
#endif
