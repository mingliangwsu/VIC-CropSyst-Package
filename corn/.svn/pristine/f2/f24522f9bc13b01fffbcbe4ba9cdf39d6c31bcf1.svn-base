// actually I should have a math precomiled header
#include <math.h>
#ifndef compareHPP
#  include "corn/math/compare.hpp"
#endif
#include "corn/math/moremath.h"
#include "corn/math/statistical/stats.h"
#include <algorithm>
namespace CORN {
namespace statistical {
//______________________________________________________________________________
Descriptive_common::Descriptive_common()
: status(0)
, minimum( 999999999.9)
, maximum(-999999999.9)
, mean(0)
, summation(0)
, standard_deviation(0)
, coefficient_of_variation(0)                                                    //120311
, sum_of_squares(0)   // moved from Statistical_data_set                         //020701
, current(0.0)                                                                   //050604
{}
//_Descriptive_common:constructor______________________________________________/
bool Descriptive_common::reset()
{  status = STAT_NEEDS_RECALC;                                                   //010406
   mean = 0;
   summation = 0;
   standard_deviation = 0;
   coefficient_of_variation = 0;                                                 //120311
   sum_of_squares  = 0; // moved from Statistical_data_set                       //020701
   minimum = 999999999.9;
   maximum = -999999999.9;
   return true;
}
//_Descriptive_common::reset___________________________________________________/
float32 Descriptive_common::append(float32 value)                  modification_
{  current = value ;
   return current;
}
//_Descriptive_common::append_______________________________________2004-08-12_/
void Descriptive_common::append_dataset
(const Dynamic_array<float32> &data_append
,nat32 append_count)   // Not returned, but it may be limited locally
{  append_count = std::min(append_count,data_append.get_count());
   for (nat32 i = 0; i < append_count; i++)
      append(data_append.get(i));
}
//_Descriptive_common::append_dataset_______________________________2009-12-17_/
Descriptive_dataset::Descriptive_dataset()
: Dynamic_array<float32>(0,30,0)
,coefficient_of_variation(0)                                                     //070710
,median(0)                                                                       //070710
{ reset();}                                                                      //050705
//______________________________________________________________________________
Descriptive_dataset::Descriptive_dataset(const Dynamic_array<float32> &data_to_copy)
: Dynamic_array<float32>(data_to_copy)
,coefficient_of_variation(0)                                                     //070710
,median(0)                                                                       //070710
{ reset();}                                                                      //050705
//______________________________________________________________________________
bool Descriptive_dataset::reset()                                                //150825
{  coefficient_of_variation = 0.0;
   median = 0.0;
   return Descriptive_common::reset();                                           //200701
}
//_Descriptive_dataset::reset_______________________________________2004-01-23_/
float32 Descriptive_dataset::get_probability_value(float percentage) provision_
{ float d_prob;
  float d_value;
  float prob;
#define       zero_base_correction -1
   Descriptive_dataset sorted(*this);                                            //061020
   sorted.sort(DESCENDING);                                                      //061020
   prob = 0.0;
   for (nat32 i=  1; i < (sorted.get_count()) ; i++)
      // probability loop is 1 based, data set is 0 based
      // Need to check this loop is is i = 1 to n-1  or i to n
   {   float prob_i =  sorted.probability(i);
       float prob_1 = sorted.probability(i+1);
       if (  (prob_i < percentage) &&
             (prob_1 >= percentage))
       {
          d_prob  = sorted.probability(i+1) - sorted.probability(i);
          d_value = sorted.get(i+1 zero_base_correction) - sorted.get(i zero_base_correction);
          prob    = sorted.get(i zero_base_correction) + (percentage - sorted.probability(i)) * d_value / d_prob;
       }
   }
   //if (prob == 0.0)
   //   cerr << "probability not found\n";
   return prob;
}
//_Descriptive_dataset::get_probability_value__________________________________/
float64 Descriptive_dataset::get_weighted_value(int at_i)                 const
{  float64 sum = get_sum();
   float64 value = get(at_i);
   float64 weighted_value = CORN::is_approximately<float64>(sum,0.0,0.0000001) ? 0.0 : value/sum;
   return weighted_value;
}
//_Descriptive_dataset::get_weighted_value__________________________2008-10-24_/
float32 Descriptive_dataset::calc_median()                                const
{  Descriptive_dataset sorted(*this);                                            //200701_061020
   sorted.sort(DESCENDING);
   nat32 cnt = sorted.get_count();
   nat32 midpt = cnt/2;
   float32 return_median = (cnt & 1) // odd
   ? sorted.get(midpt)
   : (sorted.get(midpt) + sorted.get(midpt+1)) / 2.0;
   return return_median;
}
//_Descriptive_dataset::calc_median_________________________________2001-04-06_/
float32 Descriptive_dataset::get_median()                            provision_
{  if (!status_get(STAT_median_bit)) // If we haven't already calculate the median, calculate it now.
   {  median = calc_median();
      status_set(STAT_median_bit);
   };
   return median;
}
//_Descriptive_dataset::get_median__________________________________2001-04-06_/
float32 Descriptive_dataset::get_mean()                                   const
{  if (changed) { Descriptive_dataset *not_const_this = const_cast<Descriptive_dataset *>(this);
      not_const_this->recompute(); }
   return mean;
}
//_Descriptive_dataset::get_mean_______________________________________________/
float32 Descriptive_dataset::get_coefficient_of_variation()          provision_
{  if (changed) { Descriptive_dataset *not_const_this = const_cast<Descriptive_dataset *>(this);
      not_const_this->recompute(); }
   return coefficient_of_variation;
}
//_Descriptive_dataset::get_coefficient_of_variation___________________________/
float32 Descriptive_dataset::get_standard_deviation()                provision_
{  if (changed) { Descriptive_dataset *not_const_this = const_cast<Descriptive_dataset *>(this);
      not_const_this->recompute(); }
   return standard_deviation;
}
//_Descriptive_dataset::get_standard_deviation_________________________________/
void Descriptive_dataset::recompute()                             modification_
{
// shouldn't need to resort
   float64 summation64 = 0;                                                      //060118
   float64 sum_of_squares64 = 0;                                                 //060118
   nat32 local_count = get_count();
   minimum =( 999999999.9);                                                      //020701
   maximum =(-999999999.9);                                                      //020701
   bool any_negative_values=false;  // to check for coef.var. validity           //111215
   for (nat32 i = 0; i < local_count; i++)
   {  float32  Xi = get(i);                                                      //020702
      minimum = std::min<float32>(minimum,Xi);                                   //020702
      maximum = std::max<float32>(maximum,Xi);                                   //020702
      summation64 +=  Xi;
      sum_of_squares64 += CORN_sqr(Xi);
      if (Xi < 0) any_negative_values = true;
   }
    if (local_count == 0)
    { mean = 0;
      standard_deviation = 0;
    } else
    { mean = summation64 / ((float64)local_count);   // actually arithmetic_mean
    }
   //080930 Now using the same method as Excel
   float64 sum_squared_deviations = 0;                                           //080930
   for (nat32 i = 0; i < local_count; i++)                                       //080930
   {  float64  Xi = (float64)get(i);                                             //020702
      float64 deviation = (Xi - mean);                                           //080930
      float64 deviation_squared = CORN_sqr(deviation);                           //080930
      sum_squared_deviations += deviation_squared;                               //080930
   }                                                                             //080930
   if (   (local_count <= 1)                                                     //080930
       || CORN::is_approximately<float64>(sum_squared_deviations,0.0,0.000001) // would cause a div0 error
       || CORN::is_approximately<float64>(mean,0.0,0.000001)
       )
   {  // sample size is insufficient to calculate sample_standard_deviation
      standard_deviation = 0.0;
      coefficient_of_variation = 0.0;
   } else
   {  float64 degrees_of_freedom = (local_count - 1);                            //080930
      standard_deviation = sqrt(sum_squared_deviations / degrees_of_freedom);    //080930
      coefficient_of_variation =
         any_negative_values ? 0.0 :
         /* Coefficient of variation is valid only for data sets in a ratio scale (non negative values)
            It is not valid for data sets in interval scale.
         */
          standard_deviation / mean * 100.0;                                     //990307
   }                                                                             //080930
   status_set(STAT_sum_bit|STAT_sum_sqrs_bit|STAT_arithmetic_mean_bit|STAT_coef_var_bit|STAT_std_dev_bit);
   changed = false;
   summation = (float32)summation64;                                             //060118
   sum_of_squares = (float32)sum_of_squares64;                                   //060118
}
//_Descriptive_dataset::recompute______________________________________________/
// Weibull related functions,
// I alpha is a unitless shape parameter and beta is the scale
// parameter (same units as the generated value) 070403_
float32 Descriptive_dataset::bisection_method(float32 alpha_initial)      const
{  float32 alpha_mid = alpha_initial;
   float32 alpha_upper = alpha_mid * 1.05;
   float32 alpha_lower = alpha_mid * 0.95;
   float32 f_low;
   float32 f_high;
   do
   {  alpha_upper = alpha_upper * 1.05;
      alpha_lower = alpha_lower * 0.95;
       f_low = Weibull_f(alpha_lower);
       f_high = Weibull_f(alpha_upper);
   } while ((f_low *f_high > 0.0));//(sign(f_low) == sign(f_high));
   bool negative_end_is_alpha_lower = (f_low < 0);
   int iterations = 0; // to avoid pathologic cases
   bool found = false;
   float32   negative_end =  (negative_end_is_alpha_lower)
      ? alpha_lower
      : alpha_upper;
    float32  positive_end =  (negative_end_is_alpha_lower)
      ? alpha_upper
      : alpha_lower;
   do
   {  alpha_mid = (negative_end + positive_end)/2;
      float32 f_mid = Weibull_f(alpha_mid);
      if (f_mid <= 0)   negative_end = alpha_mid;
      else              positive_end = alpha_mid;
      if (CORN::is_approximately<float32>(f_mid,0.0,0.001))
         found = true;
      iterations++;
   } while(!found && (iterations <= 100));
   return alpha_mid;
}
//_Descriptive_dataset::bisection_method____________________________1998-03-18_/
float32 Descriptive_dataset::Weibull_f(float32 alpha)                     const
{  nat32 n = get_count();
   float64 sum_q = 0;
   float64 sum_r = 0;
   float64 sum_s = 0;
   for (nat32 i = 0; i < n; i++)
   {  float64 x_i = get(i);
      float64 ln_x_i = (x_i > 0) ?  CORN_ln(x_i) :  0.0;                         //030616
      float64 x_i_alpha  = pow(x_i,(float64)alpha);
      sum_q += ln_x_i;
      sum_r += x_i_alpha * ln_x_i;
      sum_s += x_i_alpha;
   }
   return
   (CORN::is_approximately<float32>(alpha,0.0,0.000001) ||                       //980404
    CORN::is_approximately<float32>(sum_s,0.0,0.000001))                         //980404
   ? 0.0                                                                         //980404
   : n / alpha + sum_q - (n * sum_r / sum_s);
}
//_Descriptive_dataset::Weibull_f___________________________________1998-03-18_/
void Descriptive_dataset::get_Weibull_alpha_beta
(float32 &alpha_shape,  float32 &beta_scale) const
{  /* First approximation of alpha:
   I think this is called Maximum Likelihood parameterization method - RLN
   see http://www.brighton-webs.co.uk/distributions/weibull2.htm
   */
   float32 stddev_ = get_standard_deviation();                                   //060118
   float32 mean_   = get_mean();                                                 //060118
   if ( CORN::is_approximately<float32>(mean_,0.0,0.0000001 )                    //061217
      ||CORN::is_approximately<float32>(stddev_,0.0,0.0001 ))                    //061217
   {  alpha_shape = 0.0;                                                         //061217
      beta_scale = 0.0;                                                          //061217
      // When Alpha is 0 (I.e. because StDev is 0, Beta cannot be computed either).
      return;                                                                    //061217
   };
   alpha_shape = mean_/stddev_;
   nat32 n = get_count();
   alpha_shape = bisection_method(alpha_shape);
   float64 sum_x_pow_alpha = 0;
   for (nat32 i = 0; i < n; i++)  // for each item in the dataset
      sum_x_pow_alpha += pow((float64)get(i),(float64)alpha_shape);
   float64 beta64 =  //  avoid div by 0
      (CORN::is_approximately<float32>(alpha_shape,0.0,0.000001) ||              //980404
       CORN::is_approximately<float32>( n,0.0,0.000001))                         //980404
   ? 0.0                                                                         //980404
   : pow(sum_x_pow_alpha / n, (float64)(1.0 / alpha_shape));                     //051025
   beta_scale = beta64;
}
//_Descriptive_dataset::get_Weibull_alpha_beta______________________1998-03-18_/
///////////////////////////////////////////////////////////////////////////////
float32 Descriptive_dataset::append(float32 value)
{  Dynamic_array<float32>::append(value);
   return Descriptive_common::append(value);
}
//_Descriptive_dataset::append______________________________________2004-08-12_/
Descriptive_progressive::Descriptive_progressive()                               //200701
:Descriptive_common()
,count(0)
{}
//_Descriptive_progressive:constructor_________________________________________/
bool Descriptive_progressive::reset()                                            //150825
{  count = 0;
   return Descriptive_common::reset();
}                                                                                //150825
//_Descriptive_progressive::reset______________________________________________/
float32 Descriptive_progressive::append(float32 value)
{  status_clear_all();
   Descriptive_common::append(value);                                            //040812
   count++;
   summation += value;
   mean = summation/(float32)count;
   minimum = std::min<float32>(minimum,value);
   maximum = std::max<float32>(maximum,value);
   sum_of_squares += CORN_sqr(value);                                            //100710
   status_set(STAT_sum_bit|STAT_sum_sqrs_bit|STAT_arithmetic_mean_bit|STAT_min_bit|STAT_max_bit);
   return value;
}
//_Descriptive_progressive::append_____________________________________________/
float32 Descriptive_progressive::calc_standard_deviation()                 const
{   float64 term = (count) ? sum_of_squares/count - CORN_sqr(mean) : 0.0;        //020701
   float32 standard_deviation = (term > 0.0)
   ? sqrt(term)
   : 0.0;
   status_set(STAT_std_dev_bit);
   return standard_deviation;
}
//_Descriptive_progressive::calc_standard_deviation____________________________/
float32 Descriptive_progressive::get_standard_deviation()                  const
{  if (!status_get(STAT_std_dev_bit))
      standard_deviation = calc_standard_deviation();
   return standard_deviation;
}
//_Descriptive_progressive::get_standard_deviation_____________________________/
float32 Descriptive_progressive::calc_coefficient_of_variation()           const
{  float32 coef_var = (mean != 0.0)
      ? coefficient_of_variation =(get_standard_deviation() / get_mean())*100.0
      : 0.0;
   return coef_var;
}
//_Descriptive_progressive::calc_coefficient_of_variation___________2009-10-02_/
float32 Descriptive_progressive::get_coefficient_of_variation()       provision_
{  if (!status_get(STAT_coef_var_bit))
      coefficient_of_variation = calc_coefficient_of_variation();
   return coefficient_of_variation;
}
//_Descriptive_progressive::get_coefficient_of_variation____________2009-10-02_/
Descriptive_progressive_index::Descriptive_progressive_index(nat16 _applicable)
: Sample_cumulative<float32,float32>(_applicable)                                //160803
, index(0.0)
{ current = 1.0; }                                                               //040812
//_Descriptive_progressive_index:constructor___________________________________/
float32 Descriptive_progressive_index::append(float32 value)
{
   Sample_cumulative<float32,float32>::append(value);                            //160803
   current = value;
   index = 1 - value;
   return value;
}
//_Descriptive_progressive_index::append_______________________________________/
bool Descriptive_progressive_index::reset()                                      //150825
{  current = 1.0;
   index = 0.0;
   return
      //160803 Statistical_running_tally
      Sample_cumulative<float32,float32>::reset();                               //160803_150825
}
//_Descriptive_progressive_index_______________________________________________/
}//_namespace_statistical___________________________________________2020-07-01_/
}//_namespace_CORN_____________________________________________________________/

