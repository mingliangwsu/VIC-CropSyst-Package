#ifndef seclusionH
#define seclusionH

/*
   This class implements rules for filtering strings by
   including/excluding strings/patterns.

   It is analagous to the Unix/Linx rsync include/exclude pattern rules.

   Patterns rules can be loaded from command line argc/argv

        -f, --filter=RULE           add a file-filtering RULE
        -F                          same as --filter=�dir-merge /.rsync-filter�
                                    repeated: --filter=�- .rsync-filter�
            --exclude=PATTERN       exclude  matching PATTERN
            --exclude-from=FILE     read exclude patterns from FILE
            --include=PATTERN       don�t exclude  matching PATTERN
            --include-from=FILE     read include patterns from FILE
            --files-from=FILE       read list of source-file names from FILE
        -0, --from0                 all *from file lists are delimited by nulls

http://linuxcommand.org/man_pages/rsync1.html
http://linux.about.com/library/cmd/blcmdl1_rsync.htm

   or injected with functions.


    NYI
     Seclusion is an ordered list of include/exclude options
   enum Rule
      { exclude // exclude, - specifies an exclude pattern.
      , include // include, + specifies an include pattern.
              merge, . specifies a merge-file to read for more rules.
              dir-merge, : specifies a per-directory merge-file.
              hide, H specifies a pattern for hiding files from the  transfer.
              show, S files that match the pattern are not hidden.
              protect,  P  specifies a pattern for protecting files from dele-
              tion.
              risk, R files that match the pattern are not protected.
              clear, ! clears the current include/exclude list (takes no arg)
      };

*/
#include "corn/container/unilist.h"
#include "corn/container/text_list.h"
#include "corn/OS/directory_entry_name.h"
#include "corn/OS/file_system.h"
//#include "corn/application/arguments.h"
//---------------------------------------------------------------------------
namespace CORN
{
//______________________________________________________________________________
class Seclusion
: public Unidirectional_list // list of Clusion
{public:
   #define RULE_DEN_CONDITION_DIRECTORY 1
   #define RULE_DEN_CONDITION_ROOTED    2
   //___________________________________________________________________________
   class Clusion
   : public implements_ Item
   { public:
      bool        include;  // true is include false is exclude
      std::string pattern;
      nat8        DEN_conditions;
     public:
      Clusion
         (bool        include_
         ,std::string pattern_);
      inline bool is_rooted()
         { return DEN_conditions & RULE_DEN_CONDITION_ROOTED; }
      inline bool is_directory()
         { return DEN_conditions & RULE_DEN_CONDITION_DIRECTORY; }
      inline virtual const std::string &append_to_string(std::string &buffer) const;
     public: // item implemenations
        inline virtual bool is_key_string(const std::string &key) affirmation_  //180820
      { return pattern == key; }

/*180820  was probably only used for find_cstr now using is_key
         inline virtual const char *get_key()                              const
            { return pattern.c_str(); }
         inline virtual const char *key_string (std::string &buffer)       const //170904
         { buffer = pattern;
            return buffer.c_str();}
*/            
         /*NYN
         inline virtual const char *label_string(std::string &buffer)      const //170904
         { buffer = pattern; return buffer.c_str();}
         */
   };
   //__________________________________________________________________Clusion_/

   class Arguments

   /*200801  now simply use  global arguments
   : public extends_ CORN::Arguments
   */
   {
    public:
       Seclusion &seclusion;
    public: // structors
       inline Arguments(Seclusion &seclusion_)
         //: CORN::Arguments()
         : seclusion(seclusion_)                                              {}

    public: // implements CORN::Arguments
      virtual bool take_assignment
         (const std::string &variable
         ,const std::string &value)                               modification_;
      //200216 virtual bool recognize_option(const std::string &paramstr)  modification_;
      bool take(Item_string *arg)                                 modification_;
      virtual bool find_and_read_seclusion_files()                modification_;
   };
   //_Arguments_________________________________________________________________
   OS::File_system::Recursion recursion;
      // recursive is provided for functions/commands such as rsync
      // where recursion is optional
      // (I.e. only sync one directory level or recurse from directory and below)
   bool case_insensitive;
   bool DEN_matching;
      // user may set this false if seclusion is not used to to match file names.
 public:
   bool      include_all_others;                                                 //170404
 public:
/*161020 never implemented
   Seclusion(int argc, const char *argv[],bool case_insensitive_=false);         //160924
   // actually might want to always use new Arguments class
*/

      /** Constructor used to include,exclude patterns from command line
      argument (I.e. linux rsync command)
       (see options documented above)
      **/
   Seclusion(bool case_insensitive_=false);                                      //160924
      // When no patterns are specified, everything is included.

 public: // constructors used to create with a single pattern
   Seclusion(const char          *pattern,bool case_insensitive_,bool include);
   Seclusion(const wchar_t       *pattern,bool case_insensitive_,bool include);
   Seclusion(const std::string   &pattern,bool case_insensitive_,bool include);
   Seclusion(const std::wstring  &pattern,bool case_insensitive_,bool include);
 public: // inclusion/exclusion of specific patterns
   bool clude_pattern(const char *pattern,bool include, bool prepend_it);        //200805

   // the following  \return true indicating the pattern is valid

   bool include_pattern(const char *pattern        ,bool prepend = false);       //200805
   bool include_pattern(const wchar_t *pattern     ,bool prepend = false);       //200805
   bool include_pattern(const std::string &pattern ,bool prepend = false);       //200805
   bool include_pattern(const std::wstring &pattern,bool prepend = false);       //200805

   bool exclude_pattern(const char *pattern        ,bool prepend = false);       //200805
   bool exclude_pattern(const wchar_t *pattern     ,bool prepend = false);       //200805
   bool exclude_pattern(const std::string &pattern ,bool prepend = false);       //200805
   bool exclude_pattern(const std::wstring &pattern,bool prepend = false);       //200805
/*200805

   bool include_pattern_cstr     (const char *pattern        );
   bool include_pattern_wstr     (const wchar_t *pattern     );
   bool include_pattern_string   (const std::string &pattern );
   bool include_pattern_wstring  (const std::wstring &pattern);

   bool exclude_pattern_cstr     (const char *pattern        );
   bool exclude_pattern_wstr     (const wchar_t *pattern     );
   bool exclude_pattern_string   (const std::string &pattern );
   bool exclude_pattern_wstring  (const std::wstring &pattern);
*/
 public: // loading patterns from rule files
   nat32 clude_from(const char *rule_file,bool include);
   inline nat32 include_from(const char *rule_file) { return clude_from(rule_file,true); }
   inline nat32 exclude_from(const char *rule_file) { return clude_from(rule_file,false); }

      // strings on lines with no rule indicator are
      // included when include_from
      // excluded when exclude_from

      // Lines with +/- rules are always inclusions/exclusion respectively.

      // \returns number of rules encountered.
 public: // inclustion matching
   bool is_included_string
      (const std::string   &string
      ,bool is_directory   =false
      ,bool must_be_rooted =false)                                 affirmation_;
   bool is_included_wstring
      (const std::wstring  &string
      ,bool is_directory   =false
      ,bool must_be_rooted =false )                            affirmation_;
   inline bool is_included_cstr
      (const char          *string
      ,bool is_directory   /*=false*/
      ,bool must_be_rooted /*=false*/)                             affirmation_
      { return is_included_string(std::string(string),is_directory,must_be_rooted); }
   bool are_any_included(const CORN::Text_list &items)             affirmation_;
      // This matches each item in the text list.
      // Return true if any of the items match
   bool are_all_included(const CORN::Text_list &items)             affirmation_;
      // This matches each item in the text list.
      // Return true if all of the items match

   bool is_included_DEN(const CORN::OS::Directory_entry_name &DEN)  affirmation_;
   inline bool is_excluded_DEN(const CORN::OS::Directory_entry_name &DEN)  affirmation_
      { return ! is_included_DEN(DEN); }
      // DEN may be unqualified, fully qualified or partially qualified
      // may be relatively qualied, but relative paths are ignored.
      // caller should resolve paths.

   inline bool is_excluded_string
      (const std::string   &str
      ,bool is_directory   /*=false*/
      ,bool must_be_rooted /*=false*/)                              affirmation_
      { return !is_included_string(str,is_directory,must_be_rooted); }
   inline bool is_excluded_wstring
      (const std::wstring  &str
      ,bool is_directory   /*=false*/
      ,bool must_be_rooted /*=false*/)                              affirmation_
      { return !is_included_wstring(str,is_directory,must_be_rooted); }
   inline bool is_excluded_cstr
      (const char          *str
      ,bool is_directory   /*=false*/
      ,bool must_be_rooted /*=false*/)                              affirmation_
      { return !is_included_cstr(str,is_directory,must_be_rooted); }
   /*NYI
   bool is_included_wstr      (const wchar_t       *string);
   bool is_included_wstring   (const std::wstring  *string);
   */

   inline bool are_all_excluded(const CORN::Text_list &items)      affirmation_
      { return !are_all_included(items); }
   inline bool are_any_excluded(const CORN::Text_list &items)      affirmation_
      { return !are_any_included(items); }
 public:
   bool is_included_text_list
      (const CORN::Text_list &text_list)                           affirmation_;
   inline bool is_excluded_text_list
      (const CORN::Text_list &text_list)                           affirmation_
      { return !is_included_text_list(text_list); }
 private:
   bool is_included_recurse_text_list
      (CORN::Text_list &text_list
      ,bool is_root)                                               affirmation_;
   inline bool is_excluded_recurse_text_list
      (CORN::Text_list &text_list
      ,bool is_root)                                               affirmation_
      { return !is_included_recurse_text_list(text_list,is_root); }
 protected:
   bool match_wildcard
      (const char * tame_text // A string without wildcards
      ,const char * wild_text // A (potentially) corresponding string with wildcards
      ,bool case_insensitive = false  // By default, match on 'X' vs 'x'
      ,char alt_terminator = '\0'    // For function names, for example, you can stop at the first '('
      ) const;
};
//_Seclusion___________________________________________________________________/
class Inclusion
: public extends_ Seclusion
{public:
   inline Inclusion
      (const char *pattern,bool case_insensitive)
     : Seclusion(pattern,case_insensitive,true)                               {}
   inline Inclusion
      (const wchar_t *pattern,bool case_insensitive)
     : Seclusion(pattern,case_insensitive,true)                               {}
   inline Inclusion
      (const std::string &pattern,bool case_insensitive)
     : Seclusion(pattern,case_insensitive,true)                               {}
   inline Inclusion
      (const std::wstring &pattern,bool case_insensitive)
     : Seclusion(pattern,case_insensitive,true)                               {}
   inline Inclusion()  // Include every thing
     : Seclusion()                               { include_all_others = false; }
};
//_Inclusion___________________________________________________________________/
class Exclusion
: public extends_ Seclusion
{public:
   inline Exclusion(const char *pattern,bool case_insensitive)
     : Seclusion(pattern,case_insensitive,false)                              {}
   inline Exclusion(const wchar_t *pattern,bool case_insensitive)
     : Seclusion(pattern,case_insensitive,false)                              {}
   inline Exclusion(const std::wstring &pattern,bool case_insensitive)
     : Seclusion(pattern,case_insensitive,false)                              {}
   inline Exclusion(const std::string &pattern,bool case_insensitive)
     : Seclusion(pattern,case_insensitive,false)                              {}
   inline Exclusion()                                                            //170404
      : Seclusion()                               { include_all_others = true; }
};
//_Exclusion___________________________________________________________________/
}//_namespace_CORN_____________________________________________________________/
#endif

