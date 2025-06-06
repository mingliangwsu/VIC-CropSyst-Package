#ifndef management_param_classH
#define management_param_classH
#include "management/management_param_struct.h"
#include "cs_operation.h"
#include "corn/enum.h"
#include "common/simulation/event.h"
#include "cs_operation.h"
#include "N_sources.h"
#include "common/residue/residue_const.h"
#include "cs_filenames.h"
#include "corn/OS/file_system_engine.h"
//______________________________________________________________________________
#define NO_adj_relative_growth_rate 1.0
#define NO_LAI_forces_clipping      0.0
#define NO_biomass_forces_clipping  0.0
#define NO_flowering_forces_clipping 0
//______________________________________________________________________________
#define LABEL_management            "management"
//_Forward declarations_________________________________________________________
namespace CORN
{  class Item;
}
class Biomass_decomposition_parameters;
namespace CropSyst {
class N_application_soil_observation_mode_event;
}
//______________________________________________________________________________
#define VIRTUAL_VOID_SETUP_PARAMETERS \
   virtual void setup_parameters(CORN::Data_record &record,bool for_write)    modification_
//______________________________________________________________________________
namespace CropSyst {                                                             //120419
//______________________________________________________________________________
struct Tillage_operation_class
: public CropSyst_field_operation_VX                                             //011207
, public Tillage_operation_struct
{
 public: // constructor
   Tillage_operation_class();
   VIRTUAL_VOID_SETUP_PARAMETERS;
 public: // virtual methods
   //170426 virtual const char *label_cstr(char *buffer)                            const;//030123
   virtual const char *label_string(std::string &buffer)                  const; //170423
   virtual void log(std::ostream &log_file)                               const; //020810
   inline virtual int compare(const CORN::Item &/*other*/)    const { return 0;} // At this point nothing really to compare, eventually compare phen sync;
   inline virtual int32 /*enumXX */ get_type()                            const  //011207
      { return op_clad.get();/*170605 CS_OP_MGMT_TILLAGE;*/ }                 //170605
};
//_Tillage_operation_class__________________________________________2011-01-13_/
DECLARE_ENUM_COWL
   (Org_N_appl_method_cowl
   ,Org_N_appl_method
   ,org_N_appl_method_label_table);
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
class Organic_nitrogen_operation_abstract_class
: public CropSyst_field_operation_VX                                             //011207
   // CropSyst_field_operation may be sufficient
, public Organic_nitrogen_operation_abstract_struct
{
 public:
   Organic_matter_source_cowl organic_matter_source_cowl;                        //110831
   Org_N_appl_method_cowl     org_N_appl_method_cowl;                            //020322
 public: // constructor
   Organic_nitrogen_operation_abstract_class(CropSyst_Op cs_op);
 public: // virtual methods
   VIRTUAL_VOID_SETUP_PARAMETERS;
   inline  virtual int compare(const CORN::Item &other)                   const
                                                { UNUSED_arg(other); return 0;}
      // At this point nothing really to compare eventually compare phen sync;
   virtual void log(std::ostream &log_file)                               const;
      // temporarily abstract so I make sure to implement  //020810_
 public: //accessors
          virtual bool is_liquid()                                        const; //020322
   inline float64 get_org_N_kg_ha()                 const { return org_N_kg_ha;} //060717
   inline float64 get_NH3_N_kg_ha()                 const { return NH3_N_kg_ha;} //060717
   inline float64 param_long_term_org_N_volatilization_loss_percent()     const
                         { return long_term_org_N_volatilization_loss_percent; } //020525
 public: //special
   virtual bool multiply_amount_by(float32 multiplier)            modification_; //070716
};
//_Organic_nitrogen_operation_abstract_class________________________2011-01-13_/
#define DECLARE_Biomatter_application_operation                                \
class Biomatter_application_operation                                          \
: public Organic_nitrogen_operation_abstract                                   \
{                                                                              \
 public:                                                                       \
   Biomass_decomposition_parameters_filename      biomatter_param_filename;    \
 public:                                                                       \
   provided_ Biomass_decomposition_parameters *decomposition_parameters;       \
 public:                                                                       \
   Biomatter_application_operation();                                          \
   virtual ~Biomatter_application_operation();                                 \
 public:                                                                       \
   virtual void setup_parameters(CORN::Data_record &data_rec,bool for_write) modification_;  \
   virtual const char *label_string(std::string &buffer)                const; \
   virtual void log(std::ostream &log_file)                             const; \
   inline  virtual int compare(const CORN::Item &other)    const { UNUSED_arg(other); return 0; } \
   inline  virtual int32 get_type() const { return CS_OP_MGMT_BIOMATTER_APPLICATION;} \
   Biomass_decomposition_parameters &provide_decomposition_parameters(Organic_matter_source source = default_source) provision_; \
};
/* The parameter are stored in the biomatter parameter file and loaded as needed 100216 */
//______________________________________________________________________________
#define DEFINE_Biomatter_application_operation                                 \
Biomatter_application_operation::Biomatter_application_operation()             \
:Organic_nitrogen_operation_abstract(CS_OP_MGMT_BIOMATTER_APPLICATION)         \
,biomatter_param_filename("\\Simulation\\Database\\biomatter\\swine.CS_manure")\
,decomposition_parameters()                                                    \
{ /* Probably should set the path to the simulation directory */ }             \
void Biomatter_application_operation::setup_parameters                         \
(Data_record &data_rec,bool for_write)                                         \
{                                                                              \
   Organic_nitrogen_operation_abstract::setup_parameters(data_rec,for_write);  \
   data_rec.expect_file_name("biomatter",biomatter_param_filename);            \
}                                                                              \
const char *Biomatter_application_operation::label_string(std::string &result) const  \
{  CropSyst_field_operation_VX::label_string(result);                          \
   biomatter_param_filename.append_to_string(result);                          \
   result += ":";                                                              \
   CORN::append_float32_to_string(org_N_kg_ha,3,result);                       \
   result += " kg N/ha";                                                       \
   return result.c_str();                                                      \
}                                                                              \
void Biomatter_application_operation::log(std::ostream &log_file) const        \
{                                                                              \
   Organic_nitrogen_operation_abstract::log(log_file);                         \
   log_file << "biomatter="<<biomatter_param_filename.c_str()<< std::endl;     \
}                                                                              \
Biomass_decomposition_parameters &Biomatter_application_operation              \
::provide_decomposition_parameters(Organic_matter_source source) provision_    \
{  if (!decomposition_parameters)                                              \
   {  decomposition_parameters = new Biomass_decomposition_parameters(source); \
      if (CORN::OS::file_system_engine.exists(biomatter_param_filename))       \
      {  CORN::VV_File decomp_param_file(biomatter_param_filename.c_str());    \
         decomp_param_file.get(*decomposition_parameters);                     \
      } else                                                                   \
      {                                                                        \
         std::clog << "Biomatter file not found, using default decomposition parameters" << biomatter_param_filename.c_str() << std::endl; \
         provide_decomposition_parameters(source).setup_default(source);       \
      }                                                                        \
   }                                                                           \
   return *decomposition_parameters;                                           \
}                                                                              \
Biomatter_application_operation::~Biomatter_application_operation()            \
{  if (decomposition_parameters) delete decomposition_parameters;              \
}

/* The biomatter_param_filename does not have to exist. */
/* In such case, just used the default parameters of the source. 110104*/
//______________________________________________________________________________
DECLARE_ENUM_COWL
   (Ammonium_source_cowl,Ammonium_source,ammonium_source_label_table);
DECLARE_ENUM_COWL
   (NH4_Appl_method_cowl,NH4_Appl_method,NH4_appl_method_label_table);
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
class Inorganic_nitrogen_operation_class
: public CropSyst_field_operation_VX
, public Inorganic_nitrogen_operation_struct   // CropSyst_field_operation may be sufficient
{
 public:
   Ammonium_source_cowl NH4_source_cowl;
   NH4_Appl_method_cowl NH4_appl_method_cowl;
 public: //accessors
   inline float64 get_NO3_N_kg_ha()                       const { return NO3_N;} //060717
   inline float64 get_NH4_N_kg_ha()                       const { return NH4_N;} //060717
      // Warning for derived Split_application parameters the NO3_N and NH4_N
      // values are percent until the actual amounts are determined.             120710
   inline float64 param_NH4_volatilization() const { return NH4_volatilization;} //981212
 public:
   Inorganic_nitrogen_operation_class() ;
   virtual void setup_parameters(CORN::Data_record &data_rec,bool for_write) modification_;   //970709
   //170426 virtual const char *label_cstr(char *buffer)                            const;//030123
   virtual const char *label_string(std::string &buffer)                  const; //170423
   virtual void log(std::ostream &log_file)                               const; //020810
   inline virtual int compare(const Item &other)                          const
                                                { UNUSED_arg(other); return 0;}
      // At this point nothing really to compare eventually compare phen sync;
   inline virtual int32 /*enumXX */ get_type()                            const
                                        { return CS_OP_MGMT_INORGANIC_NITROGEN;} //011207
   bool multiply_amount_by(float32 multiplier)                    modification_; //070716
};
//______________________________________________________________________________
DECLARE_ENUM_COWL(Harvest_fate_mode_cowl
   ,Harvest_fate_mode,harvest_mode_and_biomass_fated_cause_label_table);
DECLARE_ENUM_COWL(Harvest_amount_mode_cowl
   ,Harvest_amount_mode,harvest_amount_mode_label_table);
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
class Harvest_or_clipping_operation_class
: public virtual CropSyst_field_operation_VX   // CropSyst_field_operation may be sufficient   //011207_
, public virtual Harvest_or_clipping_operation_struct
{
 public:
   Harvest_amount_mode_cowl harvest_amount_mode_cowl;                            // recorded only for parameter editor
   Harvest_fate_mode_cowl   harvest_fate_mode_cowl;                              // was Harvest_fate_mode_labeled  harvest_fate_mode_labeled
 public: // constructors
   Harvest_or_clipping_operation_class(); // section setup constructor
 public: // CropSyst operation virtual overrides
   VIRTUAL_VOID_SETUP_PARAMETERS;
   //170426 virtual const char *label_cstr(char *buffer)                            const;//030123
   virtual const char *label_string(std::string &buffer)                  const; //170423
   virtual void log(std::ostream &log_file)                               const; //020810
   inline virtual int compare(const CORN::Item &other)                    const
      { UNUSED_arg(other); return 0; }
      // At this point nothing really to compare eventuall compare phen sync;
   inline virtual int32 /*enumXX */ get_type()                            const
                                       { return CS_OP_MGMT_HARVEST_OR_CLIPPING;} //011207
};
//_Harvest_or_clipping_operation_class______________________________2011-01-XX_/
class Auto_clip_biomass_mode_class
: public virtual Harvest_or_clipping_operation_class                             //150404_040204
, public Auto_clip_biomass_mode_struct
{
 public: // constructors
   Auto_clip_biomass_mode_class();
   // copy constructor
   Auto_clip_biomass_mode_class(const Auto_clip_biomass_mode_class& to_be_copied); // <- Might not be needed
 public: // virtual methods
   virtual void setup_parameters
      (CORN::Data_record &data_rec,bool for_write)                modification_; //970709
   virtual int32  get_type()       const { return CS_OP_MGMT_AUTO_CLIP_BIOMASS;} //011207
 public:
   virtual inline bool is_mode()                    affirmation_ { return true;} //020322
   void update_considerations()                                   modification_; //040831
   void set_considerations()                                      modification_; //040831
};
//_Auto_clip_biomass_mode_class_____________________________________2001-12-07_/
DECLARE_ENUM_COWL
   (Irrigation_application_mode_cowl
   ,Irrigation_application_mode
   ,irrigation_application_mode_label_table
   );
DECLARE_ENUM_COWL
   (Irrigation_consideration_mode_cowl
   ,Irrigation_consideration_mode
   ,irrigation_consideration_mode_label_table
   );
DECLARE_ENUM_COWL
   (Depletion_observation_depth_mode_cowl
   ,Depletion_observation_depth_mode
   ,depletion_observation_depth_mode_label_table
   );
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
 class Irrigation_operation_class
: public /*virtual*/ CropSyst_field_operation_VX
// CropSyst_field_operation may be sufficient 011207
, public /*virtual*/ Irrigation_operation_struct
{
 public:
   Irrigation_application_mode_cowl   application_mode_cowl;                     //041028
   Irrigation_consideration_mode_cowl consideration_mode_cowl;                   //091201
   Depletion_observation_depth_mode_cowl depletion_observation_depth_mode_cowl;
 public: // accessors
   inline float64 get_amount_mm()                     const { return amount_mm;} //020313
   inline float64 get_salinity()                       const { return salinity;} //020313
   inline float64 get_ECw_to_total_dissolved_solids()const { return ECw_to_TDS;} //080402
   inline float64 get_NO3_N_concentration() const { return NO3_N_concentration;} //080213
   inline float64 get_NH4_N_concentration() const { return NH4_N_concentration;} //080213
   inline float64 get_net_irrigation_mult() const { return net_irrigation_mult;} //981212x
   inline float64 get_refill_point()               const { return refill_point;} //051228
public:
   bool multiply_amount_by(float32 multiplier)                    modification_; //070716
      // used by crop production function
   virtual void setup_parameters(CORN::Data_record &data_rec,bool for_write) modification_; //970709
   //170426 virtual const char *label_cstr(char *buffer)                            const; //030123
   virtual const char *label_string(std::string &buffer)                  const; //170423
   virtual void log(std::ostream &log_file)                               const; // temporarily abstract so I make sure to implement 020810_
   inline virtual int compare(const Item &other)                          const
                                                { UNUSED_arg(other); return 0;}
   // At this point nothing really to compare eventuall compare phen sync;
   inline virtual int32 /*enumXX */ get_type()                            const
                                               { return CS_OP_MGMT_IRRIGATION;}  //011207
 public: // constructor
   Irrigation_operation_class(CropSyst_Op irrig_or_auto_irrig_code = CS_OP_MGMT_IRRIGATION);
 public: // accessors
   inline Depletion_observation_depth_mode get_depletion_observation_depth_mode() const
                          { return depletion_observation_depth_mode_cowl.get();} //070606
   inline float64 get_max_allowable_depletion()                           const
                                              { return max_allowable_depletion;} //981212
   inline float64 get_depletion_observe_depth()                           const
                                            { return depletion_observe_depth_m;} //981212
   inline float64 get_depletion_observe_root_zone_fract()                 const
                                    { return depletion_observe_root_zone_fract;} //070606
   float64 get_min_application_m()                                        const; //060719
   float64 get_max_application_m()                                        const; //981212
};
//_Irrigation_operation_class_______________________________________1998-12-12_/
class Automatic_irrigation_mode_class
: public /*virtual*/ Irrigation_operation_class                                              //170706_020313
{  // derived from Irrigation operation so we can get the salinity and chemical values
   // When simulating salinity, the automatic irrigation with have a default concentration.
public: // constructor
  Automatic_irrigation_mode_class();                                             //990224
public: // Common operation virtual methods
   virtual int32 get_type()           const {return CS_OP_MGMT_AUTO_IRRIGATION;} //011207
   virtual const char *label_string(std::string &buffer)                  const; //170423
   virtual inline bool is_mode()                          const  { return true;} //020322
   virtual void setup_parameters
      (CORN::Data_record &data_rec,bool for_write)                modification_; //970709
};
//_Automatic_irrigation_mode_class__________________________________2002-03-13_/
DECLARE_ENUM_COWL
   (Automatic_N_mode_cowl,Automatic_N_mode,automatic_N_mode_label_table);
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
class N_application_soil_observation_mode_class  // was Automatic_NO3_mode
: public CropSyst_field_operation_VX   // CropSyst_field_operation may be sufficient 011207_
, public N_application_soil_observation_mode_struct
{  // The event date is the sampling date.
 public:
   class Split_application // Automatic_NO3_application                          //991122
   : public Inorganic_nitrogen_operation_class                                   //120710
   {  // In auto NO3 mode we can optionally specify when the applications are split.
      // If no split application are specify, a single application will be generated at runtime.
   public: // public for parameter editor, we could use friendship
      int16    percent_of_application; // Could be uint8 but dont have edit class for it yet.
   public: // values computed during run when ready to apply
      float32  actual_NO3_kg_m2; // kg/m2 (used only for output?)                //161222LML note: is elemental N
      float32  actual_NH4_kg_m2; // kg/m2 (used only for output?)                //120710 161222LML note: is elemental N
   public: // Constructors
      inline Split_application(float32 _actual_amount_N_kg_m2 = 0.0)
         : Inorganic_nitrogen_operation_class()
         , percent_of_application(100)                                           //020314
         , actual_NO3_kg_m2(_actual_amount_N_kg_m2 * NO3_N/100.0) // computed at planting
         , actual_NH4_kg_m2(_actual_amount_N_kg_m2 * NH4_N/100.0) // computed at planting
         {  op_clad.set(CS_OP_MGMT_AUTO_N_APPL); }                            //120710
   public: // Accessors
      inline uint8 get_percent_of_application()                           const
                                               { return percent_of_application;} //030103
   public: // Overrides
      virtual void setup_parameters
         (CORN::Data_record &data_rec,bool for_write)            modification_;  //970709
      virtual const char *label_string(std::string &buffer)               const; //170423
      virtual void log(std::ostream &log_file)                            const; //020810
      inline virtual int compare(const Item &other)                       const
                                                { UNUSED_arg(other); return 0; }
         // At this point nothing really to compare eventually compare phen sync;
      inline virtual int32 get_type()    const { return CS_OP_MGMT_AUTO_N_APPL;} //011207
      //170720 inline virtual int16 get_SCS_code()                                 const  { return  111;} // Auto NO3 application is always Fertilization Injected. //990516
      inline bool is_mode()                        affirmation_ { return false;} //020318
         // Note that this is not actually a mode because the event date is the soil sampling date.
         // and there is no period.
   };
   //_Split_application________________________________________________________/
 public:
   Automatic_N_mode_cowl automatic_N_mode_cowl;
 public: // computed at runtime when the Auto_N_mode start event is reached
   float32 total_N_application_kg_m2;  // will be convered to kg/ha for output   //020314_120710 was total_NO3_application_kg_m2
 public:
   N_application_soil_observation_mode_class();
   float64 determine_total_automatic_N_application //  kg/m2                     //980824_120710 was determine_total_automatic_NO3_application
      (float64 sum_N_to_sampling_depth // kg/m2                                  //120710 was sum_NO3_to_sampling_depth
      ,float64 estimated_mineralization_kg_ha);                                  //120710
   inline virtual int32 get_type()                                        const
      { return CS_OP_MGMT_SOIL_N_SAMPLING; }                                     //120625
   virtual inline bool is_mode()                         const  { return false;} //020322
   virtual void setup_parameters
      (CORN::Data_record &data_rec,bool for_write)                modification_; //970709
   virtual const char *label_string(std::string &buffer)                  const; //170423
   virtual void log(std::ostream &log_file)                               const; //020810
};
//_N_application_soil_observation_mode_class________________________2011-01-XX_/
class Chemical_operation_class
: public CropSyst_field_operation_VX   // CropSyst_field_operation may be sufficient
, public Chemical_operation_struct
{
 public: // constructor
   Chemical_operation_class() ;
 public: // CropSyst operation virtual methods
   virtual void setup_parameters(CORN::Data_record &data_rec,bool for_write) modification_; //970709
   virtual const char *label_string(std::string &buffer)                  const; //170423
   virtual void log(std::ostream &log_file)                               const; //020810
   inline virtual int compare(const Item &other) const {UNUSED_arg(other);return 0;}
      // At this point nothing really to compare eventuall compare phen sync;
   inline virtual int32 get_type()          const { return CS_OP_MGMT_CHEMICAL;} //011207
};
//_Chemical_operation_class_________________________________________2011-01-XX_/
class Management_parameters_class
: public Common_parameters_data_record                                           //020611
{
 public:
   Land_treatment_clad  land_treatment_clad;                                     //020322
   bool                    crop_management;                                      //030606
      /** Management file events may either be associated with a crop
          in a rotation or associated with a simulation run.
          When associated with a crop, events may be syncronized
          relative to the Crop planting date and phenologic events.
          When assicated with a simulation, only those events
          that are not relevent to crop management will be displayed.
      **/
   bool                    CANMS_support;    // Used only by the management editor, enables CANMS operation options 060103_
 public:  // planting_parameters
   float32 soil_conservation_factor;    // 0-1  for RUSLE
   float32 seasonal_carbon_footprint_kgCO2e_ha;                                  //120430
   float32 seasonal_irrigation_carbon_footprint_kgCO2e_ha;                       //120430
   float32 irrigation_application_carbon_footprint_kgCO2e_ha_mm;                 //120430
 public: // event lists
   //011207  The following event list should be renamed to XXXX_events since they hold
   //       events not operations.
   Common_event_list irrigation_operations;                                      //981007
   Common_event_list auto_irrigation_mode_operations;                            //020313
   Common_event_list inorganic_nitrogen_operations;                              //981007
   Common_event_list biomatter_application_operations;                           //080902
   Common_event_list tillage_operations;                                         //981007
   Common_event_list residue_operations;                                         //981007
   Common_event_list chemical_operations;                                        //981007
   Common_event_list harvest_or_clipping_operations;                             //031014
   N_application_soil_observation_mode_event   *N_application_soil_observation_event;  // allocated on creation but transferred to the event list during run 060804
   Common_event_list auto_N_applications;  // Currently only one automatic_NO3_application_mode can have split   060804 need to rename this to split_N_applications 991122
   bool automatic_nitrogen;                                                      //060804
   Common_event_list auto_clip_biomass_mode_operations;                          //011207
   #ifdef LADSS_MODE
   Common_event_list LADSS_operations; // In LADSS, all operations are simply put on LADSS_operations list 030810
   #endif
 public: // accessors
   inline float64 get_soil_conservation_factor()                          const
                                            { return soil_conservation_factor;}  //981212
   inline Land_treatment param_land_treatment()                           const
                         { return (Land_treatment)(land_treatment_clad.get());}  //990105
          int16 total_automatic_N_applications()                          const; //020403
 public:
   Management_parameters_class();  // default management file
   inline virtual ~Management_parameters_class() { delete N_application_soil_observation_event;} //201128LML
   virtual bool expect_structure(bool for_write = false);                        //161025_981007
   inline virtual const char *get_primary_section_name()                  const
                                                      {return LABEL_management;} //020911
   inline virtual bool preserve_unexpected_entries()      const { return false;} //020108
   inline virtual const char *get_type_label()                            const
                                                { return "CropSyst management";} //051011
   bool delete_event(Common_event *)                              modification_; //170701
      // used by parameter editor
 public:
   uint32 irrigation_multiply_by(float32 multiplier)              modification_; //070716
   virtual nat32 fertilization_nitrogen_multiply_by
      (float multiplier)                                          modification_; //070716
       // These multipliers are used by Management generation when
       // generating adjusted irrigation and nitrogen fertilization amounts
};
//_Management_parameters_class______________________________________2011-01-15_/
}//_namespace CropSyst_________________________________________________________/
#endif

