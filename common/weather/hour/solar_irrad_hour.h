#ifndef solar_irrad_hourH
#define solar_irrad_hourH

class Solar_radiation;                                                           //140715
namespace CS
{
   class Solar_radiation_extraterrestrial_hourly;
}
//______________________________________________________________________________
//200505 I had this named irradiance but actually this is solar radiation
// (because it is the integrated value)

class Solar_radiation_hour
{
   const Solar_radiation                        &solar_radiation_daily;          //140715
   const CS::Solar_radiation_extraterrestrial_hourly &extraterrestrial_solar_radiation;//150627
 private:  // The following may be moved to actual solar radiation
   float64 diffuse_solar_radiation[24]; // the units are specified by the passed units code
   float64 direct_solar_radiation[24]; // the units are specified by the passed units code
   float64 solar_radiation[24];
 public: // constructor
   Solar_radiation_hour
   (const Solar_radiation &solar_radiation_daily                                 //140715
   ,const CS::Solar_radiation_extraterrestrial_hourly
      &extraterrestrial_solar_radiation);                                        //150627

 public: // processing
   virtual bool update();                                                        //140715
 public: // accessors
   inline float64 get_diffuse_solar_radiation   (CORN::Hour hour)          const
      { return diffuse_solar_radiation[hour];}
   inline float64 get_direct_solar_radiation    (CORN::Hour hour)          const
      { return direct_solar_radiation[hour];}
   inline float64 get_solar_irradiation         (CORN::Hour hour)          const
      { return solar_radiation[hour];}
};
//_Solar_radiation_hour________________________________________________________/
#endif

