#ifndef date_32H
#define date_32H
#include "corn/chronometry/date_T.h"
namespace CORN
{
//200220 class Common__tabular_file_format;
//______________________________________________________________________________
class Date_cowl_const_32
: public extends_ Date_cowl_const<date32>
{public:
   inline Date_cowl_const_32(const date32 &date_ref_)
      : Date_cowl_const<date32>(date_ref_)
      {}
 public: // accessors
   inline const date32 &ref_date32()                  const { return date_ref; } //171115
};
//_Date_cowl_const_32_______________________________________________2017-05-24_/
class Date_cowl_32
: public extends_ Date_cowl<date32>
{public:
   inline Date_cowl_32(date32 &date_ref_)
      : Date_cowl<date32>(date_ref_)
      {}
 public: // assignations
   inline const date32 &ref_date32()                  const { return date_ref; } //171115
   inline modifiable_ date32 &mod_date32()                  { return date_ref; } //171115
};
//_Date_cowl_32_____________________________________________________2017-04-26_/
class Date_clad_32
: public extends_ Date_clad<date32>
{public:
   Date_clad_32();
   Date_clad_32(const date32 date_raw);
   Date_clad_32(const Date_clad_32 &other);
   Date_clad_32(const Temporal &other);
   Date_clad_32(Year year, DOY doy);
   Date_clad_32(Year year, Month month, DOM dom);                                //170712
 public:
   inline const date32 &ref_date32()                  const { return date_num; } //171115_170530
   inline modifiable_ date32 &mod_date32()                  { return date_num; } //171115_
 private:
   Date_clad_32 *check_abstract() { return new Date_clad_32(); }
};
//_Date_clad_32________________________________________________________________/
}//_namespace CORN_____________________________________________________________/
#endif
