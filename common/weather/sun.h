#ifndef sunH
#define sunH

#define DATE_TIME Date_time_64

/*
   This class computes the sun position and day duration solar radiation etc..
   Given a date or a time.
   If only date is specified, the noon time is used.
*/

#ifndef timeH
#  include "corn/chronometry/time_conversion.h"
#endif
#include "corn/chronometry/date_32.h"
#ifndef constH
#  include "corn/const.h"
#endif
class Geocoordinate;                                                             //150629
using namespace CORN;                                                            //020126
// NYI this should be in CS namespace  150921
//______________________________________________________________________________
class Sun

// Conceptual: should be derived from Simulation_element

{
 public:
   const Temporal &now;                                                          //150120
 public: // location
   const Geocoordinate &geocoordinate;                                           //150629
 protected:
   friend class Sky;                                                             //110414
 private:    // Current calculation values
   float64  azimuth_angle_deg;       float64 azimuth_angle_rad;
   float64  elevation_angle_deg;     float64 elevation_angle_rad;
   float64  declination_deg;         float64 declination_rad;     // for the day
   float64  eot_adj;
   Minutes  daylight_adj;
   float64  local_standard_time_meridian;
 private: // Optional data computed as necessary
   mutable CORN::Date_clad_32 sun_rise_set_date;
      // The latest date sunrise and set were computed for
   mutable Local_time  sun_rise_time;
   mutable Local_time  sun_set_time;
   bool    provide_sun_rise_set_times()                              provision_; //991008
      //130629 This should be renamed to update_sun_rise_set_times
 public: // These are optional and provided because they can be easily computed
   Local_time &get_sun_rise_time()                                        const; //991007
   Local_time &get_sun_set_time()                                         const; //991007
   inline const Local_time &ref_sun_rise_time()  const { return sun_rise_time; } //130628
   inline const Local_time &ref_sun_set_time()   const { return sun_set_time;  } //130628
 private: // The following remain constant for every year
   mutable bool day_length_hours_good;                                           //011127
   mutable float64 day_length_hours[367];                                        //011127
 protected:
   float64 day_length_hours_today;                                               //150627
 public: // recalculate
   virtual bool update()                                          modification_;
   inline virtual void invalidate(bool /*absolutely*/)           modification_{} // needed for derived classes virtuality //091216
 public:  // constructors
   Sun(const Temporal/*170430 _base*/  &today                                    //150125
      ,const Geocoordinate &geocoordinate);                                      //150629
   virtual ~Sun();                                                               //130515
 protected:
   void calc_angles
      (DOY      doy
      ,float    solar_minutes_after_midnight
      ,float64 &declination_rad
      ,float64 &elevation_angle_rad
      ,float64 &azimuth_angle_rad
      ,float64 &hour_angle_rad);                           // may be const ?
   // Derived classes (I.e. weather base should override commit when the date changes to save any modified values)
 public:
   inline virtual void commit( const Date_const &)               modification_{} //050616
public:  // accessors
   inline float64 get_elevation_angle_deg() const { return elevation_angle_deg;}
   float get_elevation_angle_rad()                                        const;
   inline float64 get_azimuth_angle_rad()    const { return azimuth_angle_rad; } //071115
   // the latitude and longitude must be provided by derived classes
public:  // Used by derived classes
   float64 get_day_length_hours(DOY doy = 0)                              const;
   float64 calc_day_length_hours(DOY doy)                                 const; //011129
   float64 get_solar_noon()                                               const;
   inline const float64 &ref_day_length_hours_today()                     const  //150627
      { return day_length_hours_today; }
private:
   bool setup_day_length_hours()                                  contribution_; //110414_011129
};
//_Sun______________________________________________________________1999-10-01_/
#endif

