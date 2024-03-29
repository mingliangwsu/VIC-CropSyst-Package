#ifndef date_formatH
#define date_formatH
#ifndef constH
#  include "corn/const.h"
#endif
#ifndef primitiveH
#  include "corn/primitive.h"
#endif
#include <string>

#define D_omit    (nat8)0x0    /* Omit the date or date field (used with time formatting in date time class */

#define DO_Year   (nat8)0x1
#define DO_Month  (nat8)0x2
#define DO_DOM    (nat8)0x3
#define DO_DOY    (nat8)0x4
#define DO_rsrvd1 (nat8)0x5      /* reserved for future use */
#define DO_rsrvd2 (nat8)0x6
#define DO_rsrvd3 (nat8)0x7

//050412 ordering is now an unsigned 16bit word composed of 3 3 bit fields
// defining the order of the year month day and/or day of year
// These are ordered left to right with the list significant field denoting the right most date values
// The most significant bit is not used at this time
// (In the case of datetime ordering the upper 16bit word denotes the date order and the
//  least significant 16bit word denotes the time ordering.)

/* In the following gregorian date orderings, D is the day of month
   if month is present otherwise it is the day of year.
*/

#define D_YMD  (nat16)((DO_Year  << 6)|(DO_Month  << 3)|(DO_DOM))
#define D_YDM  (nat16)((DO_Year  << 6)|(DO_DOM    << 3)|(DO_Month))
#define D_MYD  (nat16)((DO_Month << 6)|(DO_Year   << 3)|(DO_DOM))
#define D_MDY  (nat16)((DO_Month << 6)|(DO_DOM    << 3)|(DO_Year))
#define D_DYM  (nat16)((DO_DOM   << 6)|(DO_Year   << 3)|(DO_Month))
#define D_DMY  (nat16)((DO_DOM   << 6)|(DO_Month  << 3)|(DO_Year))
// The following two formats are calandar date format
#define D_MD   (nat16)((DO_Month << 3)|(DO_DOM))  /* The year is excluded */
#define D_DM   (nat16)((DO_DOM   << 3)|(DO_Month))  /* The year is excluded */
/* In the following Date date orderings, D is the day of year */
// The following three are ordinal formats (US military calls these 'Julian' formats but they are not truely Julian).
#define D_YD   (nat16)((DO_Year << 3)|(DO_DOY))
#define D_DY   (nat16)((DO_DOY  << 3)|(DO_Year))
#define D_D    (nat16)((DO_DOY))
#define D_Y    (nat16)((DO_Year))

#define D_unknown_ordering 0

//#define D_x    (nat8)0xD  reserved for future used
//#define D_x    (nat8)0xE
//#define D_x    (nat8)0xF
// The upper 4bits are reserved for date/time formatting (the upper bits being time format)

/* The year format, numeric formats and month representations are OR'ed together: */
/* Year formats: */
#define OLD_STYLE_CODE
// the new style code is not yet implemented
#ifdef OLD_STYLE_CODE
// Year styles  (first 2 bits )
#define D_unknown_style 0
#define D_YYYY        (nat16)0x0000   /* Year with 4 digits:  1993 */
#define D_YY          (nat16)0x0001   /* Year with 2 digits:  93   */
#define D_YYY         (nat16)0x0002   /* Year with apostraphe and 2 digits:  '93  */
//                    (nat16)0x0003   // availabe for future use
/* Numeric formats: */
#define D_lead_none   (nat16)0x0004   /* No leading spaces or zeros I.e.: Jan=1, Dec=12 */
#define D_lead_zero   (nat16)0x0008   /* Use leading zeros I.e.: Jan=01*/
#define D_lead_space  (nat16)0x0010   /* Use leading spaces I.e.: Jan=_1 */
/* Month representations: */
#define D_Mmm         (nat16)0x0020   /* Month abbreviation I.e.: Jan */
#define D_Mmmm        (nat16)0x0040   /* Month name unabbreviated I.e.: January */
#define D_M           (nat16)0x0080   /* Month number Jan=1 ... Dec=12 */
/* Date kinds */
#define D_relative    (nat16)0x0100   /* Year is relative */
#define D_relative_1  (nat16)0x0200   /* Relative year is 1 based */
/* Day of week representations: */
#define D_Www         (nat16)0x0400
#define D_Wwww        (nat16)0x0800
#define D_W           (nat16)0x1000
#else
//Note  I want to change the bitmap of the style to the following
/*
   Two bits are used to represent 8 substyles with up to 4 sub style values
\1\1\1\1\1\1\
 \5\4\3\2\1\0\9\8\7\6\5\4\3\2\1\0   <- bit number
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |x x x x x|k k k|n n|w w|m m|y y|
*/
#define D_unknown_style 0
// When all bits are 0 the style is unknown or don't care

In order to use these new style codes, it will be necessary to mask then 0r

// Year style field (y) bits 0 and 1
#define D_YYYY        (nat16)0x0000   // xxxxxxxxxxxxxx00 Year with 4 digits:  1993. default is to always show all digits (normally 4)
// not yet used       (nat16)0x0001   // xxxxxxxxxxxxxx01 available for future use
#define D_YY          (nat16)0x0002   // xxxxxxxxxxxxxx10 Year with 2 digits:  93   */
#define D_YYY         (nat16)0x0003   // xxxxxxxxxxxxxx11 Year with apostraphe and 2 digits:  '93  */
#define D_Y_mask      (nat16)0x0003

// Month style field (y) bits 2 and 3
#define D_M           (nat16)0x0000   // xxxxxxxxxxxx00xx Month number Jan=1 ... Dec=12 */
#define D_Mmm         (nat16)0x0004   // xxxxxxxxxxxx01xx Month abbreviation I.e.: Jan */
#define D_Mmmm        (nat16)0x0008   // xxxxxxxxxxxx10xx Month name unabbreviated I.e.: January */
// not yet used       (nat16)0x000C   // xxxxxxxxxxxx11xx available for future use
#define D_M_mask      (nat16)0x000C

// Day of week representations (w) bits 4 an 5
#define D_W           (nat16)0x0000   // xxxxxxxxxx00xxxx Day of week number (default) Sun = 1 ... Sat = 7
#define D_Www         (nat16)0x0100   // xxxxxxxxxx01xxxx Day of week abbreviation (Sun,Mon,Tue,Wed,Thu,Fri,Sat)
#define D_Wwww        (nat16)0x0200   // xxxxxxxxxx10xxxx Day of week name Sunday, Monday etc..
// not yet used       (nat16)0x0300   // xxxxxxxxxx11xxxx available for future use
#define D_W_mask      (nat16)0x0300

// Numeric formats (n) bit 6 and 7
#define D_lead_none   (nat16)0x0000   // xxxxxxxx00xxxxxx No leading spaces or zeros I.e.: Jan=1, Dec=12 */
#define D_lead_zero   (nat16)0x0040   // xxxxxxxx01xxxxxx Use leading zeros I.e.: Jan=01*/
#define D_lead_space  (nat16)0x0080   // xxxxxxxx10xxxxxx Use leading spaces I.e.: Jan=_1 */
// not yet used       (nat16)0x00C0   // xxxxxxxx11xxxxxx available for future use
#define D_N_mask      (nat16)0x00C0

// Date kinds  (k)  bit 8,9 and 10
#define D_calendar    (unit16)0x0000   // xxxxx000xxxxxxxx Year is relative to the common era
// not yet used       (nat16)0x0100   // xxxxx001xxxxxxxx available for future use
#define D_relative_0  (nat16)0x0200   // xxxxx010xxxxxxxx Year is relative (0 based)
#define D_relative_1  (nat16)0x0300   // xxxxx011xxxxxxxx Year is relative (1 based)
#define D_K_mask      (nat16)0x0300
// Bits 11 to 15 are not yet assigned
#endif

#define CORN_date_DEFAULT_ORDERING  D_YMD
#define CORN_date_DEFAULT_STYLES    D_YYYY|D_M|D_lead_zero
#define CORN_date_DEFAULT_SEPARATOR '-'
#define CORN_date_NO_SEPARATOR      0
#define CORN_ORACLE_FORMAT D_DMY,D_YYYY|D_Mmm|D_lead_zero,'-'
#define CORN_SORTABLE_DATE_FORMAT D_YMD,D_YYYY|D_M|D_lead_zero,'-'

//170428 These DF_ macros may be obsolete

#define        DF_YYYY_M_D       "YYYY/M/D"     /* 2005/1/2 */
#define        DF_YYYY__M__D     "YYYY/ M/ D"   /* 2005/ 1/ 2 */
#define        DF_YYYY_MM_DD     "YYYY/MM/DD"   /* 2005/01/02 */
#define        DF_YYYYMMDD       "YYYYMMDD"     /* 20050102 */
#define        DF_YYYYDDD        "YYYYDDD"      /* 2005002 */
#define        DF_YYYY_Mmm_DD    "YYYY/Mmm/DD"  /* 2005/Jan/02 */
#define        DF_Mmm_D_YYYY     "Mmm D,YYYY"   /* Jan 2,2005 */
// Example relative 0 and 1 based other ordering applied
#define        DF_0000_M_D       "0000/M/D"     /* 0/1/2 0 based relative year*/
#define        DF_0001_M_D       "0001/M/D"     /* 1/1/2 1 based relative year*/
#define        DF_0DDD        "0DDD"      /* 0000002 0 based relative year*/
#define        DF_1DDD        "1DDD"      /* 0001002 1 based relative year*/


// It is recommended to use ISO 8601 format
// See http://www.w3.org/TR/NOTE-datetime
#define CORN_ISO_8601_DATE_FORMAT D_YMD,D_YYYY|D_M|D_lead_zero,'-'

namespace CORN
{
//170525   class Date; // forward decl
//______________________________________________________________________________
class Date_format
{
protected:
   friend class Date_const_abstract;
   friend class Date_abstract;
  // The following are for formatted output only they
  // have nothing to  do with storage
   mutable nat16 ordering_BS;   //                                               //050412
   mutable nat16 styles;                                                         //990225
   mutable nat8  separator;                                                      //990225
   mutable bool   DOY_indicator; // append (999) day of year to format           //020507
public:
   Date_format
      (nat16 ordering_      = CORN_date_DEFAULT_ORDERING                         //050412
      ,nat16 styles_        = CORN_date_DEFAULT_STYLES                           //990225
      ,nat8 separator_      = CORN_date_DEFAULT_SEPARATOR                        //990225
      ,bool DOY_indicator_  = false);
   Date_format                                                                   //030715
      (const Date_format &from_copy);
      // Copy Constructor
   Date_format(const char *format_string);                                       //050412
   virtual ~Date_format();                                                       //130515
   inline nat16 get_ordering_BS()  const  { return ordering_BS; }                //950412
   inline nat16 get_styles()       const  { return styles; }                     //980206
   inline nat8  get_separator()    const  { return separator; }                  //980206
   bool set_ordering_styles_separator
      (nat16 _ordering       = CORN_date_DEFAULT_ORDERING                        //050412
      ,nat16 _styles         = CORN_date_DEFAULT_STYLES                          //990225
      ,nat8  _separator      = CORN_date_DEFAULT_SEPARATOR                       //990225
      ,bool  _DOY_indicator  = false)                             contribution_;
   bool copy_format(const Date_format &from_copy)                 contribution_; //050412
   virtual bool set_cstr(const char *format_string)               contribution_; //141120_050412
   virtual bool set_string(const std::string &format_string)      contribution_; //141120
 public:                                                                         //980206
   inline virtual bool is_style_relative()                                 const //990225
      { return (styles & D_relative)>0; }
   inline virtual bool is_style_1_based()                                  const //990225
      { return (styles & D_relative_1)>0;}
   inline virtual bool set_style_relative(bool _relative = true)                 //990225
   {  if (_relative)                                                             //990225
      styles ^= D_relative ;                                                     //040909
      return is_style_relative();                                                //990225
   }
};
//______________________________________________________________________________
extern Date_format ISO_8601_date_format;                                         //170428
extern Date_format ISO_8601_with_DOY_date_format;                                //200410
extern Date_format year_only_date_format;                                        //180108
}//_namespace CORN_____________________________________________________________/
#endif

