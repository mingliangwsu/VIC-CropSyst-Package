#ifndef distribution_WeibullH
#define distribution_WeibullH
#include "corn/math/statistical/sample_dataset_T.h"
namespace CORN {
//______________________________________________________________________________
// http://en.wikipedia.org/wiki/Weibull_distribution
class Distribution_Weibull
{
   //200322 const Statistical_sample_dataset_cowl<float64,float64,float64> &statistical_dataset;
   const statistical::Sample &statistical_dataset;                               //200701_200322
 public:
   inline Distribution_Weibull
      (const statistical::Sample //200701 Statistical_sample_interface
      //200322  const Statistical_sample_dataset_cowl<float64,float64,float64>
         &statistical_dataset_)
      : statistical_dataset(statistical_dataset_)
      {}
   void get_shape_scale(float64 &alpha_shape, float64 &beta_scale)        const;
   inline void get_alpha_beta(float64 &alpha_shape, float64 &beta_scale)   const
      { get_shape_scale(alpha_shape,beta_scale); }
   inline void get_alpha_beta(float32 &alpha_shape32,float32 &beta_scale32)const
      {
         float64 alpha_shape64;
         float64 beta_scale64;
         get_shape_scale(alpha_shape64,beta_scale64);
         alpha_shape32 = alpha_shape64;
         beta_scale32 = beta_scale64;
      }
 public:
   /* NYI
   probability_density_function(x)
   cumulative_distribution_function(x)
   calc_mean
   calc_median
   calc_mode
   calc_variance
   calc_skewness
   calc_kurtosis
   calc_entropy
   calc_moment_generating_function
   calc_characteristic_function
   calc_quantile(probability?(0 to 1)

   */
 private:
   float64 bisection_method(float64 alpha_initial)                        const;
   float64 f(float64 alpha)                                               const;
};
//_Distribution_Weibull_____________________________________________2013-08-01_/
}//_namespace_CORN_____________________________________________________________/
#endif
