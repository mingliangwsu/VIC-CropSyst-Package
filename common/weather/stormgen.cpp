#include <math.h>
#include "corn/math/compare.hpp"
#include "corn/math/moremath.h"
#include "corn/math/random.hpp"
#include "common/weather/stormgen.h"
#include <assert.h>
namespace CS {
namespace meteorology {
//______________________________________________________________________________
Storm_generator::Storm_generator
(const CORN::date32  &today_
,const Precipitation &precipitation_                                             //151015
,const float32        mean_peak_rainfall_[])                                     //151015
: CS::Simulation_element_abstract(today_)                                        //151015
,alpha0_5                        (0.0)
,peak_precip_intensity_mm        (0.0)
,mean_peak_rainfall_fract        (mean_peak_rainfall_)
,precipitation                   (precipitation_)                                //151015
{}
//_Storm_generator:constructor____________________________________________1999_/
bool Storm_generator::start_day()                                  modification_ //151015
{  bool started = true;
   // Note: This function is a daily value, it may eventually be used also for
   // soil erosion and soil compaction, it could be in its own class.
   assert (mean_peak_rainfall_fract);                                            //151015_120529
   CORN::Month month = simdate.get_month();
   float64 mean_peak_precip_month_fract = mean_peak_rainfall_fract[month];       //000612
   float64 daily_precip_mm = precipitation.get_mm();                             //151015
   if ((daily_precip_mm > 0.0)
      && (mean_peak_precip_month_fract > 0.0))                                   //000612
         // Can only generate if mean peak rainfall specified.
   {  if (daily_precip_mm > 1.0)                                                 //990504
      {  peak_precip_intensity_mm = daily_precip_mm;
         // Incase triangular random sample fails to give while condition break
         nat16  limit_count = 0;                                                 //000926
         do                                                                      //990504
         {  alpha0_5 = triangular_random_sample(mean_peak_precip_month_fract);   //990504
            peak_precip_intensity_mm = calc_event_peak_precip_intensity();
            limit_count++;                                                       //000926
         } while ((peak_precip_intensity_mm < CORN_min(daily_precip_mm,0.25))    //990504
            && (limit_count < 100)); // <- Avoid infinite loop condition         //000926
      } else                                                                     //990504
      {  alpha0_5 = 0.9999999;                                                   //990504
         peak_precip_intensity_mm = daily_precip_mm;                             //990504
      }                                                                          //990504
   } else
   {  alpha0_5 = 0.0;
      peak_precip_intensity_mm = 0.0;
   }
   return started;
}
//_start_day___________________________________________________________________/
float64 Storm_generator::get_precipitation_mm_by_interval
(bool generate_intensity
,CORN::Minutes interval_minutes
,CORN::Time &event_start_time   // time the event starts is generated.                 //990505
,CORN::Dynamic_array<float32> &precip_X_minute // <- returned
)                                                 const     //0000615
{
   // I think the reference is: Arnold, J.G and Williams, J.R. 1989.
   // Stochastic generation of internal storm structure at a point.
   // Transactions of the ASAE, Vol 32, No.1, 161-167

   float64 daily_precip_mm = precipitation.get_mm();                             //160503
   if (daily_precip_mm > 0.0)
   {
      static nat16 intervals =  20;
      if (generate_intensity)
      {  precip_X_minute.clear();                                                //990504
         CORN::Minutes duration = calc_duration(/*alpha0_5*/);
         intervals = (daily_precip_mm < 1.0) // Must have at least 1mm of precip to split into intervals
             ? 1 : nat16(duration / interval_minutes);                           //990424
         if (intervals > 1)
         {  nat16 peak_interval = get_peak_interval(intervals);
            nat16 max_intervals_per_day = nat16(1440 / interval_minutes);        //990424
            nat16 start_at_30min_interval = CORN_uniform_random_between          //990424
            (0,  (max_intervals_per_day - intervals));                           //990424
            #define start_at_interval  0
            event_start_time.set_S((CORN::Seconds)(start_at_30min_interval * 60 * interval_minutes));   //990505
            float64 mean_interval_precip_mm
               = -peak_precip_intensity_mm / CORN_ln(1.0/(2.0 * intervals));
            float64 remaining_precip_to_distribute_mm
               = daily_precip_mm - peak_precip_intensity_mm;
           for (nat16 interval = 0; interval < intervals-1; interval++)
           {
              if (interval != peak_interval)
              {
                 float64 precip_i_mm = 0.0;
                 // Don't bother distributing minisule amounts.
                 if (remaining_precip_to_distribute_mm < 0.25)
                 {   precip_i_mm = remaining_precip_to_distribute_mm;            //990424
                     if (peak_interval > interval)
                     { // make sure there are no empty intervals between last full interval and peak.
                       peak_interval = (nat16)(interval + 1);
                       intervals =     (nat16)(peak_interval+1);
                     } else intervals = (nat16)(interval+2);
                 } else                                                          //990504
                 { nat16 limit_count = 0;                                        //000926
                    do
                    {  float64 ran_0_1 = CORN_uniform_random_between(0.000000001,1.0);
                       precip_i_mm = - mean_interval_precip_mm * CORN_ln(ran_0_1);
                       limit_count ++;                                           //000926
                    } while ((precip_i_mm > peak_precip_intensity_mm) ||
                             (precip_i_mm > remaining_precip_to_distribute_mm)
                              || (precip_i_mm < 0.25) // <- This could lead to an unsatisfied condition, added limit_count
                              && (limit_count < 100)) ;
                 }
                 remaining_precip_to_distribute_mm -= precip_i_mm;
                 precip_X_minute.set(start_at_interval + interval,precip_i_mm);  //990424
              }
           }
           precip_X_minute.set(start_at_interval + peak_interval, peak_precip_intensity_mm );
           // Rarely, the randomly distributed precip may be distributed
           // such that the last remaining amount still exceeds the peak value.
            if (remaining_precip_to_distribute_mm > 0.0)
            {  float64 addition_per_interval = remaining_precip_to_distribute_mm/intervals;
               for (nat16 interval = 0; interval < intervals;interval++)
               {  float64 adjusted = precip_X_minute.get(start_at_interval+ interval) + addition_per_interval;
                  precip_X_minute.set(start_at_interval+ interval,adjusted);
               }
            }
            /*130520 I am not sure if this orignal if statement
               is correct, it doesn't look right.
           if (remaining_precip_to_distribute_mm > peak_precip_intensity_mm)
           {  // we used to simply evenly distribute the difference from the peak to the other intervals (not including the last interval) //990429
              float64 overage = remaining_precip_to_distribute_mm - peak_precip_intensity_mm;
              remaining_precip_to_distribute_mm = peak_precip_intensity_mm;
              float64 addition_per_interval = overage/(intervals - 1);
              for (nat16 interval = 0; interval < intervals-1;interval++)
              {  float64 adjusted = precip_X_minute.get(start_at_interval+ interval) + addition_per_interval;
                 precip_X_minute.set(start_at_interval+ interval,adjusted);
              }
           }
           if (remaining_precip_to_distribute_mm > 0.0)                          //990505
              precip_X_minute.set(start_at_interval+intervals-1
                  ,std::min<float64>(remaining_precip_to_distribute_mm , peak_precip_intensity_mm ));   //990424
           //if any one of the intervals has a value < 0.25, reject the set

         */
//990504               for (nat16 interval = 0; interval < intervals-1;interval++)
//990504                  if (precip_X_minute.get(start_at_interval+interval) < 0.25)
//990504                     acceptable = false;
//990504               if (!acceptable) rejected_interval_distribution ++;
         } else
            precip_X_minute.set((nat32)0,daily_precip_mm);
      }
      else
      { // just evenly distribute the precip
         for (int interval = 0; interval < intervals; interval++) // WARNING eventually this array will be provided by weather   //990303
             precip_X_minute.set(interval,(float32)(daily_precip_mm /(float32)intervals));   //990303
      }
   } else
      precip_X_minute.clear();
  return daily_precip_mm;
}
//_get_precipitation_mm_by_interval____________________________________________/
nat16 Storm_generator::get_peak_interval(nat16 intervals)                  const
{  // Of the possible intervals, this selects one of the intervals for the as the peak interval
   float64 random_Y = 0.0;
   float64 random_X = 0.0;
   float64 g;
   do
   {  random_Y = CORN_uniform_random_between(0.0,0.175);
      random_X = CORN_uniform_random();
      g = (random_X <= 0.5)
      ? -0.18 * random_X + 0.175
      : 0.08;
   } while (g <= random_Y);
   return (nat16)(random_X * (intervals - 1));
}
//_get_peak_interval___________________________________________________________/
float64 Storm_generator::calc_duration()                            calculation_
{  float64 duration_min = 0.0;
   float64 duration_upper =
    CORN::is_approximately<float64>(alpha0_5,0.0,0.00001) ? 0.0 :                //020227
      1440.0 * (1.0 - exp(-0.3 / alpha0_5));
   float64 duration_peek  = 0.4 * duration_upper;
   float64 duration = CORN::triangle_random(duration_min,duration_upper,duration_peek);
   return duration;
}
//_calc_duration_______________________________________________________________/
float64 Storm_generator
::triangular_random_sample(float64 mean_peak_precip_month_fract)           const
{  // Note: This function is a daily value, it may eventually be used also for
   // soil erosion and soil compaction, it could be in its own class
   float64 up = (mean_peak_precip_month_fract <= 0.5)
      ? mean_peak_precip_month_fract * 2.0
      : 1.0;
   float64 low = (mean_peak_precip_month_fract <= 0.5)
      ? 0.0208
      : mean_peak_precip_month_fract * 2.0 - 1.00001;                            //020227
     // Cannot be -1.0 because a value of 1 will give low=high causing div0
   return CORN::triangle_random(low,up,mean_peak_precip_month_fract);
}
//_triangular_random_sample_____________________________1999-03-17__1999-02-19_/
float64 Storm_generator::calc_event_peak_precip_intensity()         calculation_ //140602
{  // Note: This function is a timestep value, it may eventually be used also
   // for soil erosion and soil compaction, it could be in its own class
  return alpha0_5 * precipitation.get_mm(); //160503 daily_precip_mm;        // amount mm
}
//_calc_event_peak_precip_intensity____________________________________________/
float64 Storm_generator::calc_storm_energy()                        calculation_ //140602
{ // Note: This function is a daily value, it may eventually be used also for soil erosion and soil compaction, it could be in its own class
   float64 daily_precip_mm = precipitation.get_mm();                             //160503
   float64 Rp          // (r_p)  peak_rainfall_rate mm/h
   = -2.0 * daily_precip_mm * CORN_ln(1.0 - alpha0_5);
   //    The equation for e is from apparently  Brown & Foster, 1987. Trans. ASAE, 30, 379-386
   //    (Note that the original equation units were J/(m2 . mm)
   //    The limit to 76 appears to be from Renard et al., 1997 RUSLE. USDA
   float64 e         // units are MJ/(ha.mm)
   = (Rp > 76.0 /*mm/hr*/) // energy
   ? 0.283
   : 0.29 * (1.0 - 0.72 * exp(-0.05 * Rp));
      //  from RUSLE chapter 2 pg 25  equation 2-6

//The equation for e can be use for each interval.
//Here we are only applying to the peak interval.
//where in a general case it would be applied to an interval.

// Luca thinks e is here peak interval's energy value
// So this value is being used for the whole day
// So this is the peak intensity is applied to the daily precip
// He thinks we should generate the perciptation intervals
// and calculate Rp and e for each interval then sum(precip_i * e_i)
// to get the daily storm energy.
   float64 storm_energy = daily_precip_mm * e;

// Do this for each interval and add to get daily valu

   return storm_energy;
}
//_calc_storm_energy________________________________________________1999-03-18_/
float64 Storm_generator::get_storm_energy()                                const
{  return calc_storm_energy();
}
//_get_storm_energy_______________________________________________________1999_/
float64 Storm_generator::get_peak_precip_intensity_mm()                    const //140602_000615
{  // Note: This function is a daily value, it may eventually be used also for soil erosion and soil compaction, it could be in its own class
   return peak_precip_intensity_mm;
}
//_get_peak_precip_intensity_mm___________________________________________1999_/
}}//_namespace CS______________________________________________________________/
