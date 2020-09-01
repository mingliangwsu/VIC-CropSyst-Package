#ifndef geocoordinateH
#define geocoordinateH

// https://en.wikipedia.org/wiki/Geographic_coordinate_system

#ifndef primitiveH
#  include "corn/primitive.h"
#endif
#include "corn/const.h"
#include "corn/validate/validtyp.h"
#include "corn/data_source/datarec.h"

#if (!__BCPLUSPLUS__ || (__BCPLUSPLUS__ > 0x340))
#include "corn/parameters/parameter.h"
#endif
#include "corn/measure/measures.h"

namespace CORN {
class Data_record;
}
// The defaults are a not 0.0 to avoid any potential div/zero errors
#define DEF_latitude_dec_deg           0.00001
#define DEF_longitude_dec_deg          0.00001
//151117 #define DEF_elevation                  0.00001
#define DEF_screening_height        2.0

#define   LABEL_latitude                  "latitude"
#define   LABEL_longitude                 "longitude"
#define   LABEL_elevation                 "elevation"

#define global_elevation_mean_meter 840.0
// The mean height of land above sea level is 840 m
//https://en.wikipedia.org/wiki/Earth


// This should be in a namespace (CS)
extern float32 geocoordinate_approximation;                                      //180205

//______________________________________________________________________________
struct Geocoordinate_struct
{
   float32     latitude_dec_deg;    // Input in degrees, minutes decimal
   float32     longitude_dec_deg;   // Input in degrees, minutes decimal         //991004
   float32     elevation;              // meters                                 //000731
   //32bit floating point with 6 to 7 digits of percision is quite adequate
   // for geocoordinates 6 digits of precision is about 0.11m of accuracy
   // measured at the equator.
   // see https://en.wikipedia.org/wiki/ISO_6709
 public:
    inline Geocoordinate_struct()
      : latitude_dec_deg   (0)
      , longitude_dec_deg  (0)
      , elevation          (0)
      {}
};
//_Geocoordinate_struct_____________________________________________2012-03-25_/
enum Coordinate_decimal
{  decimal_degrees
,  decimal_minutes
,  decimal_seconds
};
//_Coordinate_decimal_______________________________________________2015-10-26_/
struct Geocoordinate_format
{
 nat8 order[3];
   // order values are
   // 0 latitude
   // 1 longitude
   // 2 elevation
 Coordinate_decimal decimal;                                                     //151026
 bool NSEW;  // use N S E W  otherwise + -
   // N S E W always trails
   // + - always leads
 nat8 precision_latitude;
 nat8 precision_longitude;
 char zero_fill_char;                /*optional*/
 CORN::Units_code elevation_units;   /*optional*/
 char  separator;                    /*optional*/
};
// According to ISO 6709
// latitude degrees always has 2 leading digits
// longitude degrees always has 3 leading digits
// minutes and seconds have 2 digits
// and the precision is whatever
//_Geocoordinate_format_____________________________________________2014-01-28_/
class Geocoordinate

// more accurate name would be GCS_Coordinate
// (Geodetic Coordinate System coordinate)
// or Coordinate_geodetic
// https://en.wikipedia.org/wiki/Geographic_coordinate_system

: public extends_ Geocoordinate_struct
{
   enum Coordinate_value { unknown_value,latitude_value,longitude_value};
 public:
   std::string coordinate; // as string                                          //190923
 protected:
   cognate_ const Geocoordinate_format *ref_preferred_format;                    //140128
      // multiple coordinates may share the same format
 public:
   Geocoordinate();
   Geocoordinate(float32 latitude_dec_deg_
                ,float32 longitude_dec_deg_
                ,float32 elevation_=0);
   Geocoordinate(const Geocoordinate &copy_source);
   Geocoordinate                                                                 //151026
      (const std::string &from_string);
      // Will surmise the format
   Geocoordinate                                                                 //151026
      (const std::string          &from_string
      ,const Geocoordinate_format &known_format);
   virtual ~Geocoordinate();                                                     //130515
 public:
   virtual void copy_from_geocoordinate(const Geocoordinate &copy_source);
   virtual bool compares_exactly_to_geocoordinate(const Geocoordinate &other) affirmation_;
   virtual int compare(const Geocoordinate &other)                 affirmation_; //160902
   float32  DMS_to_decimal_degrees(float32 degrees,float32 minutes, float32 seconds) const; //050401
   float64  decimal_degrees_to_DMS(float64 decimal_degrees,bool longitude, int16 &degrees,int16 &minutes, float64 &seconds,char &direction) const; //060106
   char *   decimal_degrees_to_DMS_cstr(float64 decimal_degrees,bool longitude,int decimals, char *buffer) const; //06010
//                DMS will be formated D°M'S"d and will be appended to the buffer
   std::string &decimal_degrees_to_DMS_string(float64 decimal_degrees,bool longitude,int decimals,std::string &append_buffer) const;   //100107
               // converts the value in decimal degress to degreesomintues"seconds' string
               // the string is appended to the append buffer
   inline float32  get_latitude_dec_deg()     const  { return latitude_dec_deg;}
   inline float32  get_longitude_dec_deg()    const { return longitude_dec_deg;} //991004
          float64  get_latitude_radians()                                 const; //050601
          float64  get_longitude_radians()                                const; //050601
   inline float32 get_elevation()                     const { return elevation;} //150627

   inline bool is_in_northern_hemisphere()                         affirmation_
                                               { return latitude_dec_deg >= 0; } //081031
   inline bool is_in_southern_hemisphere()                         affirmation_
                                                { return latitude_dec_deg < 0; } //151118
   virtual bool is_valid()                                         affirmation_; //151208
   virtual float32 set_latitude_dec_deg_f32   (float32 latitude);
   virtual float32 set_longitude_dec_deg_f32  (float32 longitude);
   virtual float64 set_latitude_dec_deg_f64   (float64 latitude);
   virtual float64 set_longitude_dec_deg_f64  (float64 longitude);
           float64 set_screening_height      (float32 screening_height);        //060729
   float32 set_latitude_dec_deg_cstr   (const char *latitude);
   float32 set_longitude_dec_deg_cstr  (const char *longitude);
   float32 set_elevation_meter_cstr    (const char *elevation_meter);
   float32 set_elevation_feet_cstr     (const char *elevation_feet);
   float64 set_latitude_DMS_cstr        (const char *latitude) ;
   float64 set_longitude_DMS_cstr       (const char *longitude);
              ///< the string must be in the form: 99°99"99.99'D where
              //  D is compass direction
              // 99 is any number of digits
              // .99 is fraction of a second.;
              ///< \return longitude in dec deg
              // NYI add get_latitude_DMS_str
              // NYI add get_longitude_DMS_str
   bool set_latitude_and_longitude
      (const std::string &lat_long_str, bool coordinate_order_lat_long=true);
      // The string can be i one of te following forms:
      // +99.9999 +999.999999 latitude and longitude is in decimal degrees
      // 99.9999N 999.999999E latitude and longitude is in decimal degrees
      //    North south east and west are specified.
      // 99°99"99.99'N 99°99"99.99'W
      // The delimiter can be either a space or a comma or a tab or none
      // (if delimiter, there must be +or_ sign or direction indicator).
      //  if not direction indicator is specified the first coordinate is assumed to be latitude followed by longitude
   virtual float32 set_latitude_DMS (float32 degs,float32 mins, float32 secs);   //050526
   virtual float32 set_longitude_DMS(float32 degs,float32 mins, float32 secs);   //050526
   virtual inline float32 set_elevation_meter(float32 elevation_m_)assignation_
      { elevation =elevation_m_; return elevation; }                             //100117
   virtual inline float32 set_elevation_feet(float32 elevation_f_) assignation_
      { return set_elevation_meter(foot_to_m(elevation_f_));  }
   bool has_valid_geocoordinates()                                 affirmation_; //100204
 public: // set_from_ISO_6709 must not be virtual because used by constructor
   bool set_from_ISO_6709_specific_format                                        //151026
      (const std::string &from_string
      ,const Geocoordinate_format &set_from_ISO_6709_with_format) modification_;
   bool set_from_ISO_6709                                                        //151026
      (const std::string &from_string
      ,Geocoordinate_format &surmised_format)                     modification_; //161122
         // returned identified format

      // Currently recognizes annex H
      // (could easily implement annex D)
   /*190923 now using data record
   virtual bool setup_structure(CORN::Data_record &data_rec,bool for_write); //contribution_  //080326
*/
 public: // Data record and Data source setup
   bool surmise_ISO_6709
      (Geocoordinate_format &format
      ,const std::string          &from_string);
      ///< /return true if the format could be surmised.

 //  void parse_DMS(const char *str, float32 DMS[3])                         const;
   bool parse_DMS_or_decimal_deg
      (const char *str, Coordinate_value &value, float32 DMS[3])          const; //120326
      // The parser will attempt to determine the coordinate value
      // Indicator indicator of north south east or west is required.
      // If + - is used the indicator must always be leading.
      // If N W S E is used the indicator must always be trailing.
      // also if the degrees is beyond 90 degrees.
      // The caller should otherwise specify the value.
      // Latitude must always have 2 leading digits
      // lontigude must always have 3 leading digits
   //170423 virtual const char *label_cstr(char *buffer)                  const;
   virtual const char *label_string(std::string &buffer)                  const;

   inline bool know_format(const Geocoordinate_format &_preferred_format) cognition_
      {  ref_preferred_format = &_preferred_format; return true; }

   std::string &label_decimal_degrees_with_format
      (std::string &append_to_buffer
      ,bool NSEW  // use N S E W  otherwise + -
      ,nat8 precision_latitude
      ,nat8 precision_longitude
      ,char zero_fill_char             = 0
      ,CORN::Units_code elevation_units= 0
      ,char separator                  = 0)                               const;
      // by default there is no leading zero_fill_char
      // if non zero output elevation in the specified units
/* NYI
   std::string &label_with_format
      (std::string &append_to_buffer
      ,Coordinate_decimals decimals                                                    //151026
      ,bool NSEW  // use N S E W  otherwise + -
      ,nat8 precision
      ,char zero_fill_char             = 0
      ,char *separator                 = 0
      ,CORN::Units_code elevation_units= 0)                                const;
      // by default there is no leading zero_fill_char
      // if non zero output elevation in the specified units
*/
};
//_Geocoordinate____________________________________________________2012-03-25_/
struct Scope
{  Geocoordinate minimum;
   Geocoordinate maximum;
};
// Scope appears to be replacable by Extent
//_Scope_______________________________________________________________________/
#endif

