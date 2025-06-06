#ifndef filtered_filenameH
#define filtered_filenameH

// Filtered_file_name is deprecated
// I have this only until I replace everything with filters
// (It is needed for my old AutoControls)

#  include "corn/OS/directory_entry_name_concrete.h"
#  include "corn/OS/file_type_description.h"

/*
   Filtered_filename extends the Smart_filename adding the ability
   to recognize different filename extension and type filters.
*/
namespace CORN {
//______________________________________________________________________________
class Filtered_file_name
:public OS::Directory_name_concrete  //180315  Not sure why Directory_name_concrete should be filename
{
   Bidirectional_list file_type_descriptions;                                    //990806
 public: // Constructors:
   Filtered_file_name
      (const char *fname = 0
      ,const char *_preferred_extension = 0 // The preferred extension (without DOT)
      ,const char *_filter_label = 0        // Short description of the file type
      ,const char *_wildcard_filter = 0);   // I.e.  *.DAT if (0), *. and preferred extension will be used
   Filtered_file_name(const Filtered_file_name &from_copy);
 public:
   virtual const  std::string &append_preferred_extension_to(std::string &result_preferred_extension) const;
   const          std::string &append_filter_label_to
                 (std::string &result_filter_label)                       const; //080325
   const          std::string &append_wildcard_filter_to
                 (std::string &wild_card_filter)                          const; //080325
   const          std::string &append_file_type_pattern_for_Windows_to           //990806
                 (std::string &file_type_pattern)                         const; //080325
   void set_filter_label
           (const std::string &new_filter_label)                  modification_;//080325
   Filtered_file_name & set_with_filters(const Filtered_file_name &i_filename);
 public:
   void add_file_type                                                            //990806
      (const char *i_preferred_extension = 0 // The preferred extension (without DOT)
      ,const char *i_filter_label = 0        // Short description of the file type
      ,const char *i_wildcard_filter = 0);   // I.e.  *.DAT if (0), *. and preferred extension will be used
   void add_file_type(const OS::File_type_description &type_desc);               //990806
   const Bidirectional_list &get_file_type_descriptions()                 const; //990806
 private:
   //180813 NYN inline virtual const std::string &append_to_string(std::string &buffer) const { return buffer; }
            //180813 not currently used  may want to write in a preferred/specified format
};
//_Filtered_file_name__________________________________________________________/
}//_namespace_CORN_____________________________________________________________/
#endif
// filtered_filename.h

