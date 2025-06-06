#ifndef locrecH
#define locrecH
/* This class implements the location record */
#include "UED/library/datasetrec.h"
#include "common/geodesy/geolocation.h"
namespace UED {
//______________________________________________________________________________
class Geolocation_record
: public Record_base
, public Geolocation                                                             //031217
{
 public: // Constructors
   Geolocation_record();
   Geolocation_record(const Geolocation& copy_from);                             //050503
   virtual Geolocation_record* clone()                                    const; //140116
 public: // UED_record_base overrides
   inline virtual nat32 compute_record_body_size()                        const; //001008
      // Returns the size of the record body (not including the header).
   virtual bool read_body (Binary_file_interface& parent_database);
   virtual void write_body(Binary_file_interface& parent_database);
   bool is_more_complete_than                                                    //140608
      (const Geolocation_record &other_geolocation_record)         affirmation_;
#ifdef UED_DUMP
   virtual void dumpX(std::ostream &txt_strm);
#endif
   virtual void write_YAML(std::ostream &YAML_strm,nat16 indent_level);          //180108
};
//_Geolocation_record_______________________________________________2007-07-18_/
}//_namespace UED______________________________________________________________/
#endif

