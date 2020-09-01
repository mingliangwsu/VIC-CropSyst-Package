#ifndef sample_TH
#define sample_TH
#ifdef max
#undef max
#endif
#include "corn/math/statistical/statistics_descriptive.h"
#include "corn/dynamic_array/dynamic_array_T.h"

#include "corn/container/item.h"
// item is included only to get the format
namespace CORN {
namespace statistical {
//______________________________________________________________________________
interface_ Sample // was Statistical_sample_interface
: public extends_interface_ Descriptive // was Statistics_descriptive_interface
{
 public:
   virtual bool commit()                                        modification_=0;
   virtual STAT_mask get_applicable()                                  const =0;
      /// \returns applicable bitmask
   inline virtual bool is_applicable(Statistic statistic)           affirmation_ //160617
      { return get_applicable() & (1 << statistic); }
   virtual STAT_mask know_applicable(STAT_mask applicable_)        cognition_=0; //181225
   virtual bool reset()                                                      =0;
   virtual bool clear(float32 initial_value)                                 =0; //200529
      // Unlike reset, clear sets values to an expected initial value (for outputs)

//   virtual Observation_type ref_current()                                        =0;
   virtual float64 get(nat32 index,bool allow_resize=false)         mutation_=0; //200322
 public: // I/O
   virtual nat32 write(std::ostream &strm, const CORN::Item::Format &format,nat16 indent_level)   stream_IO_
      {  UNUSED_arg(strm); UNUSED_arg(format); UNUSED_arg(indent_level);
         return 0; }
      // Conceptual should have an option to specify (mask) which
      // elements to write (this is and with applicable)
};
//_Sample___________________________________________________________2015-11-11_/
// Observation type is the type of the value being observed any float or integer
// Element_type is the type to store calculated element values.
// Generally use double, but if precision is not significant and memory space is at a premium float can be used

template
   <typename Observation_type                                                    //140624
   ,typename Element_type>
class Sample_abstract //200701 was Statistical_sample
: public extends_ Descriptive_data<Element_type>                                 //200701
//200701 was Statistics_descriptive_data<Element_type>
, public implements_ Sample //200701 Statistical_sample_interface
{protected:
   mutable Observation_type current;
   // value element (reference to element values array)
   // The more recent value appended to the dataset.   //040812
 protected:
   const Observation_type &observation;
      // This is a reference to a variable that we are following
      // The reference is set with the observe() method.
      // If not using observation mode, simply reference the current element.
 protected:
   contribute_ STAT_mask   valid;
   contribute_ STAT_mask   applicable;                                           //150820
      // Applicable identifies which descriptive elements should be tallied.
   //___________________________________________________________________________
 public:
   // 181224 It was necessary to remove the optional applicable bit because
   // there is potential ambiguity between the constructors.
   // Not use know_applicable method to set the applicable element bitmask
   Sample_abstract()                                                             //200701_150820
      : Descriptive_data<Element_type>()
      , current                       (std::numeric_limits<double>::quiet_NaN()) //181224
      , observation                                                    (current)
      , applicable                                               (STAT_ALL_bits) //181224_150820
      {  ensure_applicable();                                                    //150820
         reset();
      }
   Sample_abstract                                                               //200701
      (const Observation_type &observation_)                                     //150820
      : Descriptive_data<Element_type>()                                         //200701
      , current                      (std::numeric_limits<double>::quiet_NaN() ) //181224

      , observation                                               (observation_) //140624
      , applicable                                               (STAT_ALL_bits) //181224 applicable_ //150820
      {   ensure_applicable();                                                   //150820
          reset(); }
   //___________________________________________________________________________
   virtual Observation_type &ref_current()
      { return current;}
   //_ref_current___________________________________________________2015-11-11_/
   virtual nat16 get_applicable()                                          const
   {  return applicable;
   }
   //_get_applicable________________________________________________2015-11-11_/
   nat16 ensure_applicable()                                       contribution_
   {  // Inorder to calculate some elements other elements may be required
      if (applicable & STAT_coef_var_bit)applicable |= STAT_std_dev_bit;
      if (applicable & STAT_std_dev_bit) applicable |= STAT_arithmetic_mean_bit;
      if (applicable & STAT_mean_bit) applicable|=(STAT_sum_bit|STAT_count_bit);
      return applicable;
   }
   //_ensure_applicable_____________________________________________2015-08-20_/
   virtual STAT_mask know_applicable(STAT_mask applicable_)           cognition_
   {  applicable = applicable_;
      return ensure_applicable();
   }
   //_know_applicable_______________________________________________2018-12-25_/
   nat16 set_valid(STAT_mask validity_to_set)                      contribution_
      { valid |= validity_to_set; return valid; }
   //___________________________________________________________________________
   nat16 get_valid(STAT_mask validity_to_get)                              const
      { return valid & validity_to_get; }
   //___________________________________________________________________________
   nat16 invalidate(STAT_mask validity_to_clear = STAT_ALL_bits)   contribution_
      { valid &= (STAT_mask)(~validity_to_clear); return valid; }
   //___________________________________________________________________________
   virtual float64 get_current()                        const { return current;}
   virtual const Observation_type &ref_current()        const { return current;}
   virtual       Observation_type &mod_current()        const { return current;}
   //___________________________________________________________________________
   virtual bool reset()
   // my want to return bool
      {  Descriptive_data<Element_type>::reset();
         valid = 0;
         current = std::numeric_limits<Observation_type>::quiet_NaN();
         return true;
      }
   //___________________________________________________________________________
   virtual bool clear(float32 initial_value)
   {  Descriptive_data<Element_type>::clear(initial_value);
      current = initial_value;
      return true;
   }
   //_clear_________________________________________________________2020-05-29_/
   virtual Observation_type append(Observation_type value)         modification_
      {  invalidate();
         this->element[value_statistic] = value;                                 //181224
         current = value ;
         set_valid(STAT_value_bit);
         return current;
      }
      // Normally the append and commit methods are not used at the same time.
      // That is you don't append values of variables that are observed
      // and will be commited.
      // However, one might use the append method to load the data sample
      // with some values that are not part of an observation.
   //___________________________________________________________________________
   // The append dataset methods are used to load the sample with values
   // from data sets.
   // Normally one would not append dataset that have (or will be) also observed.
   // As with append() one might use append_dataset methods to
   // load the sample with values that are not part of an observation.
   void append_dataset_float32
      (const Dynamic_array<float32> &data_append
      ,nat32 append_limit_count =
         #ifdef __BCPLUSPLUS__
         std::numeric_limits<nat32>::max()
         #else
         0xFFFFFFFF
         #endif
         // By default append everything
      )
      {  nat32 append_first_count = std::min<nat32>(append_limit_count,data_append.get_count());
         for (nat32 i = 0; i < append_first_count; i++)
            append((Observation_type)data_append.get(i));
      }
   //_append_dataset_float32________________________________________2009-12-17_/
   void append_dataset_float64
      (const Dynamic_array<float64> &data_append
      ,nat32 append_limit_count =
         #ifdef __BCPLUSPLUS__
         std::numeric_limits<nat32>::max()
         #else
         0xFFFFFFFF
         #endif
         // By default append everything
      )
      {  nat32 append_first_count = std::min<nat32>(append_limit_count,data_append.get_count());
         for (nat32 i = 0; i < append_first_count; i++)
            append((Observation_type)data_append.get(i));
      }
   //_append_dataset_float64________________________________________2009-12-17_/
   void append_dataset_spanning_float32
      (const Dynamic_array<float32> &data_append
      ,const Span &span)
      {  nat32 from = span.get_from();
         nat32 to   = span.get_to();
         for (nat32 i = from; i <= to; i++)
            append((Observation_type)data_append.get(i));
      }
   //_append_dataset_spanning_float32_______________________________2020-04-26_/
   void append_dataset_spanning_float64
      (const Dynamic_array<float64> &data_append
      ,const Span &span)         // By default append everything
      {  nat32 from = span.get_from();
         nat32 to   = span.get_to();
         for (nat32 i = from; i <= to; i++)
            append((Observation_type)data_append.get(i));
      }
   //_append_dataset_spanning_float64_______________________________2020-04-26_/
   float64 /*Real*/ calc_coefficient_of_variation()                        const
      {  float64 /*Real*/ mean = get_mean();
         float64 /*Real*/ std_dev = get_standard_deviation();
         float64 /*Real*/ coef_var = (mean != 0.0)
            ? (std_dev / mean)*100.0
            : 0.0;
         return coef_var;
      }
   //_calc_coefficient_of_variation_________________________________2009-10-02_/
   virtual float64 /*Real*/ provide_standard_deviation()              provision_{ return (float64)Sample_abstract<Observation_type,Element_type>::standard_deviation; }
   virtual float64 /*Real*/ provide_coefficient_of_variation()        provision_=0;
//   virtual float64 /*Real*/ provide_initial()                                      provision_=0;//130918

   virtual float64 /*Element_type*/ provide_min()                     provision_{ return (float64)Sample_abstract<Observation_type,Element_type>::minimum; }
   virtual float64 /*Element_type*/ provide_max()                     provision_{ return (float64)Sample_abstract<Observation_type,Element_type>::maximum; }
   virtual float64 /*Element_type*/ provide_sum()                     provision_{ return (float64)Sample_abstract<Observation_type,Element_type>::summation; }
   virtual float64 /*Element_type*/ provide_arithmetic_mean()         provision_{ return (float64)Sample_abstract<Observation_type,Element_type>::arithmetic_mean; }
   virtual float64 /*Element_type*/ provide_sum_of_squares()          provision_{ return (float64)Sample_abstract<Observation_type,Element_type>::sum_of_squares; }
   virtual float64 /*Element_type*/ provide_initial()                 provision_=0; // { return (float64)initial; }
   virtual float64 /*Element_type*/ provide_final()                   provision_=0; // { return (float64)final; }
   //   virtual Real provide_median()                                           const=0;
   //___________________________________________________________________________
   virtual float64 get_min()                                               const { return (float64)provide_min(); }
   virtual float64 get_max()                                               const { return (float64)provide_max(); }
   virtual float64 get_sum()                                               const { return (float64)provide_sum(); }
   virtual float64 get_arithmetic_mean()                                   const { return (float64)provide_arithmetic_mean(); }
   virtual float64 get_mean() /*alias */                                   const { return (float64)provide_arithmetic_mean(); }
   virtual float64 get_sum_of_squares()                                    const { return (float64)provide_sum_of_squares(); }
   virtual float64 get_standard_deviation()                                const { return (float64)provide_standard_deviation(); }
   virtual float64 get_coefficient_of_variation()                          const { return (float64)provide_coefficient_of_variation(); }
   virtual float64 get_initial()                                           const { return (float64)provide_initial(); }
   virtual float64 get_final()                                             const { return (float64)provide_final(); }
   //_get_xxxxx________________________________________________________________/
 public: // I/O
   virtual nat32 write(std::ostream &strm, const CORN::Item::Format &format,nat16 indent_level)   stream_IO_
      // Actually format and indent level could be optional.
   {  nat32 written_count = 0;
      //   std::string key_separator(format.key.separator ? format.key.separator : " ");
      //abandoned std::string key_element_separator(format.key.separator ? format.key.separator : " ");

      std::string indentation(indent_level,format.indentor);

      strm << indentation << format.initializer;

      for (nat8 element_index = STAT_count_index
          ; element_index < statistic_COUNT
          ; element_index++)
      {  nat32 element_bit = 1 << element_index;
         // Currently assuming always putting values on separate lines
         if (applicable & element_bit)
         {  written_count++;
            strm << indentation << statistic_label_table[element_index]
            << format.key.separator << get_statistic(element_index) <<std::endl;
         }
      } // for
      /* nyi PROBABILITY STATES
      if (applicable & xxxx)                    strm << indentation << "xxx" << key_element_separator << get_xxx() << endl;
      if (applicable & xxxx)       strm << indentation << "xxx" << key_element_separator << get_xxx() << endl;
      if (applicable & xxxx)       strm << indentation << "xxx" << key_element_separator << get_xxx() << endl;
      */
      strm << format.separator;
      strm << format.finalizer;
      return written_count;
   }
//_write_______________________________________________________________________/
 public:
/* Not sure how to setup observer after instanciation
   bool observe(const Observation_type *& _observer)             contribution_ //140624
      { observer = _observer; return observer!=nul;}
      // The observe method is optional it is used in conjunction with the
      // commit() method
*/
   virtual bool commit()                                           modification_
      {
         this->element[value_statistic] = observation;                           //181224
         append(((Element_type)(Sample_abstract<Observation_type,Element_type>::observation))); //140624
         return true;
      }
   //_commit________________________________________________________2015-11-11_/

   // virtual float64 get_median()   const { return (float64)provide_median(); }
};
//_Statistical_sample_______________________________________________2013-08-01_/
}//_namespace_statistical___________________________________________2007-07-01_/
}//_namespace_CORN_____________________________________________________________/
#endif
