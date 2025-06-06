#ifndef recordH
#define recordH
#ifndef binary_recordH
#  include "corn/format/binary/binary_record.h"
#endif
#ifndef UED_typesH
#  include "UED/library/UED_types.h"
#endif
#include "UED/library/UED_options.h"
#include "UED/library/codes.h"
#include "corn/chronometry/time_types.hpp"
namespace UED {
//______________________________________________________________________________
class _UED_DLL Record_type_distinguisher_interface
{
 public:
      // These masks are for selecting bits, not interpretting them! 980904
   enum masks                                //  recrod code form:
   {                                         //  7654 3210
#define          standard_bit_mask                  0x80 //  1xxx xxxx  x indicates don't care
#define          standard_subcode                   0x00 /*  0___ ____ */
#define          non_standard_subcode               0x80 /*  1___ ____ */
//               The following are all standard masks
//               Non standard records may use partial standard codings so bit 8 is marked x
//duplicate #define          standard_subcode                   0x00 /*  0___ ____ */
#define          data_bit_mask                      0x40 /*  x1xx xxxx */
#define          data_subcode                       0x00 /*  _0__ ____ */
#define          data_scalar_vector_set_bitmask     0x70 /*  _111 ____ */
//     data_mask                         =0x40 //  x0xx xxxx
//     data_scalar_vector_mask           =0x60 //  x00x xxxx
//   , data_scalar_mask                  =0x70 //  x000 xxxx
#define          data_scalar_subcode                0x00 /*  _000 ____  */
//    , data_vector_mask                  =0x70 //  x001 xxxx
#define          data_vector_subcode                0x10 /*  _001 ____  */
//   , data_set_mask                     =0x60 //  x01x xxxx
#define          data_set_float32_subcode           0x20 /*  _010 ____  */
#define          data_set_float16_subcode           0x30 /*  _011 ____  reserved for future used */

//140129 added 16bit float

///*_______*/    , non_data_mask  =  0x40                  //  x1xx xxxx
#define           non_data_subcode                  0x40 /*  _1__ ____ */
   // If non_data
     control_mask = 0x60                     //  x10x xxxx
   , marker_mask = 0x78                      //  x100 0xxx                       //980904
   , global_control_mask = 0x70              //  x101 xxxx
   , commentary_mask =   0x78                //  x101 1xxx                       //980904
   , definition_mask = 0x60                  //  x11x xxxx
   // If data
   // If non_time_stamp
   // no bit patterns currently defined.

   // If time_stamp
#define          time_stamp_bit_mask                0x0C  /* xxxx 11xx */
#define          variable_code_bit_mask             0x02  /* xxxx xx1x */
#define          variable_code_subcode              0x00  /* ____ __0_ */
#define          no_variable_code_subcode           0x02  /* ____ __1_ */
#define          units_code_bit_mast                0x01  /* xxxx xxx1 */
#define          units_code_subcode                 0x00  /* ____ ___0 */
#define          no_units_code_subcode              0x01  /* ____ ___1 */
// ,        variable_code_mask= 0x02                      // xxxx xx0x
// ,        variable_units_code_max=0x01                  // xxxx xxx0
// ,        no_variable_code_mask= 0x02                   // xxxx xx1x
// ,        no_variable_units_code_max=0x01               // xxxx xxx1
   };

#define        timestamp_year_subcode               0x00  /* ____ 00__ */
#define        timestamp_time_subcode               0x04  /* ____ 01__ */
#define        timestamp_date_subcode               0x08  /* ____ 10__ */
#define        timestamp_date_time_subcode          0x0C  /* ____ 11__ */

   enum time_stamp_codes
   {  time_stamp_year      = 0x0   // 0 0
   ,  time_stamp_time      = 0x1   // 0 1
   ,  time_stamp_date      = 0x2   // 1 0
   ,  time_stamp_date_time = 0x3   // 1 1
   };
 public:
   inline bool             is_standard()        const { return ((get_code() & standard_bit_mask)== 0x00); }     // 0xxxxxxx
   inline bool             has_data      ()     const { return ((get_code() & data_bit_mask) == data_subcode /*0x00*/); }        // x0xxxxxx
   inline bool             is_data_vector()     const { return ((get_code() & data_scalar_vector_set_bitmask) == data_vector_subcode); } // x001xxxx
   inline bool             is_data_scalar()     const { return ((get_code() & data_scalar_vector_set_bitmask) == data_scalar_subcode); } // x001xxxx
   inline bool             is_control()         const { return ((get_code() & control_mask) == 0x40); }      // x10xxxxx
   inline bool             is_global_control()  const { return ((get_code() & global_control_mask) ==0x50);}// x101xxxx
   inline bool             is_data_set()        const
      { return ((get_code() & data_scalar_vector_set_bitmask) == data_set_float32_subcode)
            || ((get_code() & data_scalar_vector_set_bitmask) == data_set_float16_subcode); //140129
      }
   inline bool             is_data_set_float16()const                            //140129
      { return ((get_code() & data_scalar_vector_set_bitmask) == data_set_float16_subcode); //140129
      }

   inline time_stamp_codes get_time_stamp_code() const { return (time_stamp_codes)((get_code() &time_stamp_bit_mask ) >> 2); }
   inline bool             has_variable_code()   const { return ((get_code() & variable_code_bit_mask) == variable_code_subcode /*0x00*/); }
   inline bool             has_units_code()      const { return ((get_code() & units_code_bit_mast ) == units_code_subcode/*0x00*/); }
   inline bool has_year_time_stamp() const
   { time_stamp_codes time_stamp_code = get_time_stamp_code();
     return (((time_stamp_code == time_stamp_year) ||
             (time_stamp_code & 0x02)));
   }
   inline bool has_date_time_stamp()                                       const
   {  return (get_time_stamp_code() & 0x02) > 0;                                 //040909
   }
   inline bool has_time_time_stamp()                                       const
   {  return (get_time_stamp_code() & 0x01) > 0;                                 //040909
   }
   inline bool is_variable_definition() const
   { return ((get_code() == UED_variable_definition_format_0) ||
             (get_code() == UED_variable_definition_format_1) ||
             (get_code() == UED_variable_definition_format_2) ||
             (get_code() == UED_variable_definition_format_ontological));        //160619
   }
   inline bool is_units_definition() const
   { return ((get_code() == UED_units_definition_format_0) ||
             (get_code() == UED_units_definition_format_1) ||
             (get_code() == UED_units_definition_format_2) ||
             (get_code() == UED_units_definition_format_3)) ;
   }
 public:
   // Here are some standard data record type codes others are in codes.h
   enum standard_record_types
   { UED_data_value_year_var_units        // 00000000b
   , UED_data_value_year_var              // 00000001b
   , UED_data_value_year_units            // 00000010b
   , UED_data_value_year                  // 00000011b
   , UED_data_value_time_var_units        // 00000100b
   , UED_data_value_time_var              // 00000101b
   , UED_data_value_time_units            // 00000110b
   , UED_data_value_time                  // 00000111b
   , UED_data_value_date_var_units        // 00001000b
   , UED_data_value_date_var              // 00001001b
   , UED_data_value_date_units            // 00001010b
   , UED_data_value_date                  // 00001011b
   , UED_data_value_date_time_var_units   // 00001100b
   , UED_data_value_date_time_var         // 00001101b
   , UED_data_value_date_time_units       // 00001110b
   , UED_data_value_date_time             // 00001111b

   , UED_data_vector_year_var_units       // 00010000b
   , UED_data_vector_year_var             // 00010001b
   , UED_data_vector_year_units           // 00010010b
   , UED_data_vector_year                 // 00010011b
   , UED_data_vector_time_var_units       // 00010100b
   , UED_data_vector_time_var             // 00010101b
   , UED_data_vector_time_units           // 00010110b
   , UED_data_vector_time                 // 00010111b
   , UED_data_vector_date_var_units       // 00011000b
   , UED_data_vector_date_var             // 00011001b
   , UED_data_vector_date_units           // 00011010b
   , UED_data_vector_date                 // 00011011b
   , UED_data_vector_date_time_var_units  // 00011100b
   , UED_data_vector_date_time_var        // 00011101b
   , UED_data_vector_date_time_units      // 00011110b
   , UED_data_vector_date_time            // 00011111b

   , UED_data_set_year_var_units          // 00100000b
   , UED_data_set_year_var                // 00100001b
   , UED_data_set_year_units              // 00100010b
   , UED_data_set_year                    // 00100011b
   , UED_data_set_time_var_units          // 00100100b
   , UED_data_set_time_var                // 00100101b
   , UED_data_set_time_units              // 00100110b
   , UED_data_set_time                    // 00100111b
   , UED_data_set_date_var_units          // 00101000b
   , UED_data_set_date_var                // 00101001b
   , UED_data_set_date_units              // 00101010b
   , UED_data_set_date                    // 00101011b
   , UED_data_set_date_time_var_units     // 00101100b
   , UED_data_set_date_time_var           // 00101101b
   , UED_data_set_date_time_units         // 00101110b
   , UED_data_set_date_time               // 00101111b
   , ND48                                 // 00110000b
   , ND49                                 // 00110001b
   , ND50                                 // 00110010b
   , ND51                                 // 00110011b
   , ND52                                 // 00110100b
   , ND53                                 // 00110101b
   , ND54                                 // 00110110b
   , ND55                                 // 00110111b
   , ND56                                 // 00111000b
   , ND57                                 // 00111001b
   , ND58                                 // 00111010b
   , ND59                                 // 00111011b
   , ND60                                 // 00111100b
   , ND61                                 // 00111101b
   , ND62                                 // 00111110b
   , ND63                                 // 00111111b
   ,error             = 0xFF              // 11111111b
   };
 public:
    // Returns true if this is a known type code.
    virtual CORN::Record_code get_code()                                const=0; //990105
};
//______________________________________________________________________________
class _UED_DLL Record_code_clad //190916 was Smart_record_code
: public Record_type_distinguisher_interface
{
 public:
   nat16 code;      // Currently only the low 8 bits are used, the upper 8 bits are reserved for future use
 public:
   inline Record_code_clad(nat16 _code)
      : code(_code)
      {}
   inline virtual CORN::Record_code get_code()                             const //990511
      { return code; }
   inline virtual CORN::Record_code set_code(nat16 _code)          modification_ //990511
      { code = _code ; return code; }
   bool get_timestamp                                                            //990115
      (CORN::datetime64 target_date_time
      ,CORN::datetime64 &timestamp_date_time);
   // Given a date/tim this method deterines a record time stamp for the specified record_type
   // returns false if the time stamp cannot be determined or record type is not
   // for a time stamped record.
};
//______________________________________________________________________________
// This class is a base class for all UED_records which should be derived from it.
class _UED_DLL Record_base
: public Record_type_distinguisher_interface
, public CORN::Binary_record                                                     //990108
{ public: // These variables are used for in memory management and are not stored
   // This is the size of the record when the record is read from an existing file.
   // This will be set to 0 when creating the record.
   // At the time of writing, if the current size does not match,
   // a new record will be written and if the record is already located in the file,
   // the space of the original record will be cleared and the record space deleted for reallocation. 000517
 protected :
   std::string unknown_data;                                                     //990107
                        // Otherwise unformatted data will be buffered here
 public:  // The following methods should not be overridden
   inline virtual CORN::Record_code get_code()                             const //990105
      { return CORN::Binary_record::get_code();}
 public: // The following methods must be overridden
   virtual bool read_body(CORN::Binary_file_interface  &parent_database);        //971103
      ///< This method must be overridden it will be called by read() after reading the record type code
      ///< For unrecognized record types, the data will be read it to unknown data
   virtual void write_body(CORN::Binary_file_interface &parent_database);        //971103
      ///< This  method must be overridden it will be called by write() after writing the record type code
 public: // Constructors
   Record_base(CORN::Record_code rec_type_code);
   Record_base(const Record_base &from_copy);                                    //050128
      ///< This constructor is used when creating a record in a database
      ///< the record will be written to the end of the file if written.
   virtual ~Record_base();                                                       //971104
   virtual Record_base *clone()                                           const; //140116
#ifdef UED_DUMP
   virtual void dumpX(std::ostream &txt_strm);
#endif
   virtual void write_YAML(std::ostream &YAML_strm,nat16 indent_level);          //180108
};
//______________________________________________________________________________
}//_namespace UED______________________________________________________________/
#endif
//record

