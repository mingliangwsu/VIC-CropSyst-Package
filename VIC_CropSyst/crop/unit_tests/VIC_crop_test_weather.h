//---------------------------------------------------------------------------

#ifndef VIC_crop_test_weatherH
#define VIC_crop_test_weatherH
//---------------------------------------------------------------------------
struct Weather_elements
{
   int year;
   int doy;
   float precip;
   float Tmax;
   float Tmin;
   float SRad;
   float RHmax;
   float RHmin;
   float windspeed;
};

extern struct Weather_elements weather_elements[];
#ifdef obsolete
//11032
extern float VPD_daytime[];
extern float ET_ref_mm[];
// For potato test Othello 1975
extern Weather_elements weather_elements_othello[];
#endif



#endif
