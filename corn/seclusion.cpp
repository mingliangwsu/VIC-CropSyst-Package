#include "corn/seclusion.h"
#include "corn/string/strconv.hpp"
#include "corn/OS/directory_entry_name_concrete.h"
#include "corn/OS/file_system_engine.h"
#include <assert.h>
#include <algorithm>
#include <string>
namespace CORN
{
//______________________________________________________________________________
Seclusion::Seclusion
(bool case_insensitive_)
: Unidirectional_list()
, case_insensitive(case_insensitive_)                                            //160924
, recursion(OS::File_system::subdirectory_recursion_inclusive)                   //161020
// not sure which recursion to default to
, DEN_matching(true)
, include_all_others(false)                                                      //170404
{}
//_Seclusion:constructor____________________________________________2016-10-20_/
Seclusion::Seclusion(const char *pattern,bool case_insensitive_,bool include)
: case_insensitive(case_insensitive_)                                            //160924
, DEN_matching(true)                                                             //161105
, include_all_others(!include)                                                   //170404
{  if (pattern)                                                                  //200806
   if (include)   include_pattern/*_cstr*/(pattern);
   else           exclude_pattern/*_cstr*/(pattern);
}
//______________________________________________________________________________
Seclusion::Seclusion(const wchar_t *pattern,bool case_insensitive_,bool include/* = true*/)
: case_insensitive(case_insensitive_)                                            //160924
, DEN_matching(true)                                                             //161105
, include_all_others(!include)                                                   //170404
{  if (pattern)                                                                  //200806
   if (include)   include_pattern/*_wstr*/(pattern);
   else           exclude_pattern/*_wstr*/(pattern);
}
//______________________________________________________________________________
Seclusion::Seclusion(const std::string &pattern,bool case_insensitive_,bool include/* = true*/)
: case_insensitive(case_insensitive_)                                            //160924
, DEN_matching(true)                                                             //161105
, include_all_others(!include)                                                   //170404
{  if (include)   include_pattern/*_string*/(pattern);
   else           exclude_pattern/*_string*/(pattern);
}
//______________________________________________________________________________
Seclusion::Seclusion(const std::wstring &pattern,bool case_insensitive_,bool include/* = true*/)
: case_insensitive(case_insensitive_)                                            //160924
, DEN_matching(true)                                                             //161105
, include_all_others(!include)                                                   //170404
{  if (include)   include_pattern/*_wstring*/(pattern);
   else           exclude_pattern/*_wstring*/(pattern);
}
//______________________________________________________________________________

/* interpret command line arguments ala rsync
        -a, --archive               archive mode; same as -rlptgoD (no -H)
        -r, --recursive             recurse into directories
   recursion = subdirectory_recursion_inclusive;


*/
#ifdef __GNUC__
#define TO_LOWER ::tolower
#define TO_UPPER ::toupper
#else
#define TO_LOWER std::tolower
#define TO_UPPER std::toupper
#endif

bool Seclusion::clude_pattern(const char *pattern,bool include,bool prepend_it)
{  Clusion *clusion = new Clusion(include,pattern);                              //200805
   if (prepend_it)   prepend(clusion);                                           //200805
   else              append (clusion);
   return true;
}
//______________________________________________________________________________
bool Seclusion::include_pattern/*_cstr*/(const char *pattern,bool prepend)
{  return clude_pattern(pattern,true,prepend); }
//______________________________________________________________________________
bool Seclusion::include_pattern/*_wstr*/(const wchar_t *pattern,bool prepend)
{  // currently seclusion only recognize ASCII filenames
   // so convert to ASCII
   std::string pattern_cstr; CORN::UnicodeZ_to_string(pattern,pattern_cstr);
   return clude_pattern(pattern_cstr.c_str(),true,prepend);
}
//______________________________________________________________________________
bool Seclusion::include_pattern/*_string*/(const std::string &pattern,bool prepend)
{ return include_pattern/*_cstr*/(pattern.c_str(),prepend); }
//______________________________________________________________________________
bool Seclusion::include_pattern/*_wstring*/(const std::wstring &pattern,bool prepend)
{ return include_pattern/*_wstr*/(pattern.c_str(),prepend); }
//______________________________________________________________________________
bool Seclusion::exclude_pattern/*_cstr*/(const char *pattern,bool prepend)
{  return clude_pattern(pattern,false,prepend); }
//______________________________________________________________________________
bool Seclusion::exclude_pattern/*_wstr*/(const wchar_t *pattern,bool prepend)
{  // currently seclusion only recognize ASCII filenames
   // so convert to ASCII
   std::string pattern_cstr; CORN::UnicodeZ_to_string(pattern,pattern_cstr);
   return clude_pattern(pattern_cstr.c_str(),false,prepend);
}
//______________________________________________________________________________
bool Seclusion::exclude_pattern/*_string*/(const std::string &pattern,bool prepend)
{ return exclude_pattern/*_cstr*/(pattern.c_str(),prepend); }
//______________________________________________________________________________
bool Seclusion::exclude_pattern/*_wstring*/(const std::wstring &pattern,bool prepend)
{ return exclude_pattern/*_wstr*/(pattern.c_str(),prepend); }
//______________________________________________________________________________

nat32 Seclusion::clude_from(const char *rule_filename,bool include)
{  nat32 rules_encountered;
   std::ifstream rule_strm(rule_filename);

//std::clog << "clude-from" << rule_filename << std::endl;

   while (rule_strm.good() &&!rule_strm.eof())
   {
      std::string rule_raw;
      getline(rule_strm,rule_raw);
      CORN::strip_string(rule_raw, CORN::Both,' ');
      CORN::strip_string(rule_raw, CORN::Both,'\t');
      if (rule_raw.length())  // ignore blank lines
      {

//std::clog << "rule:" << rule_raw << std::endl;


         char first_char = rule_raw.c_str()[0];
         std::string *pattern = 0;
         switch (first_char)
         {  case ';' : case '#' : break; // ignore comment lines
            case '+' : case '-' :
            {
               pattern = new std::string(rule_raw,2);

               // if the rule starts with "+ " (a plus followed by a space) then it is always considered an include pattern, even if specified as part of an exclude option. The "+ " part is discarded before matching.
               // if the rule starts with "- " (a minus followed by a space) then it is always considered an exclude pattern, even if specified as part of an include option. The "- " part is discarded before matching.

            } break;
            default :
               pattern = new std::string(rule_raw);
            break;
         } // seitch
         if (pattern)
         {
            CORN::strip_string(*pattern,CORN::Leading,' ');
            CORN::strip_string(*pattern,CORN::Leading,'\t');
            bool specifically_include = include;
            if (first_char == '-') specifically_include = false;
            append(new Clusion(specifically_include,*pattern));
            delete pattern;
            rules_encountered ++;
         }
      }
   }
   return rules_encountered;
}
//______________________________________________________________________________
bool Seclusion::is_included_text_list
(const CORN::Text_list &items)                                      affirmation_
{
   bool matches = false;
   FOR_EACH_IN(item,Item_string /*180115 Text_list::Item*/,items,each_item)
   {  if (item)
         matches &= is_included_string(*item,false,false);
   } FOR_EACH_END(each_item)
   return matches;
}
//______________________________________________________________________________
bool Seclusion::is_included_recurse_text_list
(CORN::Text_list &remaining_path
,bool is_root)                                                      affirmation_
{
   bool matches = false;
   Item_string /*180115 /*Text_list::Item*/ *item = dynamic_cast<Item_string /*180115 Text_list::Item*/ *>(remaining_path.pop_first());
   if (item)
   {
      matches = is_included_string(*item,true,is_root);
      if (matches) is_included_recurse_text_list(remaining_path,false);
         // matches so far so continue recursion
   }
   return matches;
}
//______________________________________________________________________________
/*
bool Seclusion::is_included_DEN(const CORN::OS::Directory_entry_name &DEN) affirmation_
{
   bool included = false; // If no matching include/exclude pattern
   CORN::Text_list components;
   bool path_matches = true;
   if (recursion)
   {

      // in rsync when used with -r (which is implied by -a), every subcomponent of every path is visited from top down, so include/exclude patterns get applied recursively to each subcomponent.

      // If you end an exclude list with --exclude '*', since the algorithm is applied recursively that unless you explicitly include parent directories of files you want to include then the algorithm will stop at the parent directories and never see the files below them. To include all directories, use --include asterics slash before the --exclude '*'.


      DEN.append_path_list_to(components,OS::Directory_entry_name::include_qualification_designated);
      path_matches = is_included_recurse_text_list(components,true);
   }
   if (path_matches)
   {  // now check the filename and extension
      std::wstring name_and_ext;
      DEN.get_name_with_extension(name_and_ext);
      included =is_included_wstring(name_and_ext,DEN.is_directory(),false);
   }
   return included;
}
//______________________________________________________________________________
*/


/* rules for matching filenames:

// Need to check if these conditions are handled

# if the pattern starts with a / then it is matched against the start of the filename, otherwise it is matched against the end of the filename.
Thus "/foo" would match a file called "foo" at the base of the tree. On the other hand, "foo" would match any file called "foo" anywhere in the tree
because the algorithm is applied recursively from top down; it behaves as if each path component gets a turn at being the end of the file name.

# if the pattern ends with a / then it will only match a directory, not a file, link or device.

# if the pattern contains a wildcard character from the set *?[ then
expression matching is applied using the shell filename matching rules.
Otherwise a simple string match is used.

# if the pattern includes a double asterisk "**" then all wildcards in the pattern will match slashes, otherwise they will stop at slashes.

# if the pattern contains a / (not counting a trailing /) then it is matched against the full filename, including any leading directory.
If the pattern doesn't contain a / then it is matched only against the final component of the filename. Again, remember that the algorithm is applied recursively so "full filename" can actually be any portion of a path.


//conceptual (I.e. rsync)  if the pattern is a single exclamation mark ! then the current include/exclude list is reset, removing all previously defined patterns.

*/
//______________________________________________________________________________
#ifdef example
Some exclude/include examples:

# --exclude "*.o"   would exclude all filenames matching *.o
# --exclude "/foo"  would exclude a file in the base directory called foo
# --exclude "foo/"  would exclude any directory called foo.
# --exclude "/foo/*/bar"  would exclude any file called bar two levels below a
                          base directory called foo.
# --exclude "/foo/**/bar" would exclude any file called bar two or more levels below
                          a base directory called foo.
# --include "*/" --include "*.c" --exclude "*"
                          would include all directories
                          and C source files
# --include "foo/" --include "foo/bar.c" --exclude "*"
                    would include only foo/bar.c (the foo/ directory must be
                    explicitly included or it would be excluded by the "*")
#endif

//______________________________________________________________________________
bool Seclusion::is_included_DEN(const CORN::OS::Directory_entry_name &DEN) affirmation_
{
   bool is_rooted = DEN.is_qualified();
   std::string DEN_str(DEN.c_str());

   std::string DEN_no_root(DEN_str);
   //DEN_no_root.replace(DEN_no_root.begin,DEN_no_root.end,'\\','/');
   for (nat32 i = 0; i <DEN_str.length(); ++i)
      if (DEN_str[i] == '\\')
          DEN_str[i] = '/';
   if (DEN_str[1] == ':') // appears to be
      DEN_no_root.assign(DEN_str.substr(3));
   else if (DEN_str[0] == '/')
         DEN_no_root.assign(DEN_str.substr(1));
   bool included = is_included_cstr
      (DEN_no_root.c_str() // DEN.c_str()
      ,DEN.is_directory() //bool is_directory
      ,is_rooted);
   //Warning not all filename conditions may be implemented by this
   return included;
}
//______________________________________________________________________________
bool Seclusion::is_included_string
(const std::string   &string
,bool is_directory
,bool is_root  //161024 must_be_rooted
)   affirmation_
{
   bool included = false; // If no matching include/exclude pattern
   FOR_EACH(clusion,Clusion,each_clusion)
   {  // match the string name each exclude/include pattern in turn.
/*
if (clusion->pattern == "Database")
std::clog << "Reached" << std::endl;
*/
/*
if (string == "Simulation/Projects/REACCH/GHG_SOC/Scenarios/test6/future/Pullman/RT_WS-SP-WW_high/Output")
{
std::clog << "pattern:" << clusion->pattern.c_str() <<std::endl;
std::clog << "string:" << string.c_str() <<std::endl;
}
*/
      bool matches = true;
      size_t string_match_start_pos = 0;
      if (DEN_matching)
      {  if (clusion->is_directory())  matches &= is_directory;
         if (clusion->is_rooted())     matches &= is_root;
         if (matches)
         {
            nat32 significant_pattern_slashs = 1;
            for (nat32 c = 0; c < clusion->pattern.length(); c++)
            {  char potential_slash = clusion->pattern[c];
               significant_pattern_slashs += potential_slash == '/';
               // leading and trailing slashes in DEN string and patterns have already been accomodated
            }
            string_match_start_pos = std::string::npos;
            int32 pos;
            for (pos = string.length()
                ;(pos >= 0 )
                 && significant_pattern_slashs
                ;pos--)
            {
               // WARNING this doesnt handle rsync rules
               // where ** matches/ignores multiple slashes
               if (string[pos] == '/')
               {
                  if (significant_pattern_slashs)
                      significant_pattern_slashs -= 1;
                  string_match_start_pos = pos + 1;
               }
            }
            if (pos <= 0) string_match_start_pos = 0;
         }
      }
      matches &=
            match_wildcard
            (&string.c_str()[string_match_start_pos]
            ,clusion->pattern.c_str()
            ,case_insensitive);
      if (matches)
      {  // The first matching pattern is acted on.
         if (! clusion->include)
            return included = false; // matches exclude pattern so not included
         if (  clusion->include)
            return included = true;  // matches include pattern.
      }
   } FOR_EACH_END(each_clusion)
   included = include_all_others;                                                //170404
   return included;
}
//______________________________________________________________________________
bool Seclusion::is_included_wstring
(const std::wstring   &string
,bool is_directory
,bool must_be_rooted)                                               affirmation_
{  std::string string_ASCII;
   CORN::wstring_to_string(string,string_ASCII);
   return is_included_string(string_ASCII,is_directory,must_be_rooted);
}
//______________________________________________________________________________
bool Seclusion::are_all_included(const CORN::Text_list &items)      affirmation_
{  bool all_included = true;
   FOR_EACH_IN(item, Item_string /*180115 CORN::Text_list::Item*/,items,each_item)
   {  all_included &= is_included_string(*item);
   } FOR_EACH_END(each_item)
   return all_included;
}
//______________________________________________________________________________
bool Seclusion::are_any_included(const CORN::Text_list &items)      affirmation_
{  bool any_included = true;
   FOR_EACH_IN(item, Item_string /*180115 CORN::Text_list::Item*/,items,each_item)
   {  any_included |= is_included_string(*item);
   } FOR_EACH_END(each_item)
   return any_included;
}
//______________________________________________________________________________

//http://www.drdobbs.com/architecture-and-design/matching-wildcards-an-algorithm/210200888

//This function compares text strings, one of which can have wildcards ('*').

// WARNING this does not recognize character classes [] as described in
// https://www.gnu.org/software/findutils/manual/html_node/find_html/Shell-Pattern-Matching.html

// Warning this does not recognizes the rsync command the double asterisk pattern "**" will match slashes while a single asterisk pattern "*" will stop at slashes.
// see http://linuxcommand.org/man_pages/rsync1.html

bool Seclusion::match_wildcard
(const char *tame_text_             // A string without wildcards
,const char *wild_text_             // A (potentially) corresponding string with wildcards
,bool case_insensitive  // By default, match on 'X' vs 'x'
,char alt_terminator    // For function names, for example, you can stop at the first '('
) const
{
   char *tame_text = const_cast<char *>(tame_text_);
   char *wild_text = const_cast<char *>(wild_text_);
   bool matches = true;
   char * after_last_wild = NULL; // The location after the last '*', if we�ve encountered one
   char * after_last_tame = NULL; // The location in the tame string, from which we started after last wildcard
   char t, w;

   // Walk the text strings one character at a time.
   while (1)
   {  t = *tame_text;
      w = *wild_text;
      // How do you match a unique text string?
      if (!t || t == alt_terminator)
      {  // Easy: unique up on it!
         if (!w || w == alt_terminator) { break; } // "x" matches "x"
         else if (w == '*')
         {  wild_text++;
            continue;                           // "x*" matches "x" or "xy"
         }
         else if (after_last_tame)
         {  if (!(*after_last_tame) || *after_last_tame == alt_terminator)
            {  matches = false;
               break;
            }
            tame_text = after_last_tame++;
            wild_text = after_last_wild;
            continue;
         }
         matches = false;
         break; // "x" doesn't match "xy"
      }
      else
      {  if (case_insensitive)
         {  // Lowercase the characters to be compared.
            if (t >= 'A' && t <= 'Z')
               t += ('a' - 'A');
            if (w >= 'A' && w <= 'Z')
               w += ('a' - 'A');
         }
         // How do you match a tame text string?
         if (t != w)
         {  // The tame way: unique up on it!
            if (w == '*')
            {  after_last_wild = ++wild_text;
               after_last_tame = tame_text;
               w = *wild_text;
               if (!w || w == alt_terminator)
               {  break;                           // "*" matches "x"
               }
               continue;                           // "*y" matches "xy"
            }
            else if (after_last_wild)
            {  if (after_last_wild != wild_text)
               {  wild_text = after_last_wild;
                  w = *wild_text;
                  if (case_insensitive && w >= 'A' && w <= 'Z')
                     w += ('a' - 'A');
                  if (t == w) wild_text++;
               }
               tame_text++;
               continue;           // "*sip*" matches "mississippi"
            }
            else
            {  matches = false;
               break;  // "x" doesn't match "y"
            }
         }
      }
      tame_text++;
      wild_text++;
   }
   return matches;
}
//______________________________________________________________________________
bool Seclusion::Arguments::take_assignment
(const std::string &variable
,const std::string &value)                                         modification_
{  bool taken = false;
   // These are not normal/simple assignments,
   // There may be multiple occurances of these on the command line
   // in order to build pattern matching precedence.

   // include-from and exclude-from are provided for consistency with
   // existing Unix commands
   if ((variable == "--exclusions") || (variable == "--exclude-from"))
   {  seclusion.exclude_from(value.c_str());
      taken = true;
   }
   else if ((variable == "--inclusions") || (variable == "--include-from"))
   {  seclusion.include_from(value.c_str());
      taken = true;
   }
   else if (variable == "--seclusions")
   {  seclusion.include_from(value.c_str());
      taken = true;
   }
   else if (variable == "--exclude")
   {  seclusion.exclude_pattern/*_string*/(value);
      taken = true;
   }
   else if (variable == "--include")
   {  seclusion.include_pattern/*_string*/(value);
      taken = true;
   }
   return taken;
}
//_take_assignment_____________________________________________________________/
bool Seclusion::Arguments::take(Item_string *arg)
{  if (arg->find("-seclusions") != std::string::npos)
   {  find_and_read_seclusion_files();
      delete arg; return true;
   } 
   return false;                                                                 //200801
//200801   return CORN::Arguments::take(arg);                                            //200311
}
//_take_____________________________________________________________2016-10-23_/
/*obsolete use take
bool Seclusion::Arguments::recognize_option(const std::string &paramstring) modification_
{  bool recognized = false;
   if (paramstring == "seclusions")
   {  find_and_read_seclusion_files();
      recognized = true;
   }
   return recognized;
}
//_recognize_option_________________________________________________2016-10-23_/
*/
bool Seclusion::Arguments::find_and_read_seclusion_files()         modification_
{
   CORN::Bidirectional_list clusions_accessible;
   CORN::Inclusion clusion_file_pattern("*.*clusion",false); // *.inclusion *.exclusion *.seclusion

   OS::Directory_name_concrete CWD;
   /*if (verbose) */std::clog << "info: Searching for accessible seclusion files." << std::endl;
   nat32 clusion_file_count = OS::file_system_engine.list_DENs
      (CWD,&clusions_accessible,0,clusion_file_pattern
      ,OS::File_system::superdirectory_recursion_inclusive);
   //161021 warning should have a virtual method to get the seclusion filename pattern from derived classes
   clusions_accessible.sort(Container::DESCENDING);
   // In CS Suite the lower directorys have precedence
   // since seclusions have precedence according to the order read
   // we need to load the lower directories first.

   // while reading the response files disable find, otherwise infinite loop.
   std::clog << "info: loading accessible seclusion files." << std::endl;
   FOR_EACH_IN(clusion_file,CORN::OS::File_name
              ,clusions_accessible,each_response_file)
   {
      /*if (verbose)*/ std::clog << clusion_file->c_str() << std::endl;
      seclusion.include_from(clusion_file->c_str());
   } FOR_EACH_END(each_response_file)
   return clusion_file_count;
}
//_find_and_read_seclusion_files____________________________________2016-10-23_/
Seclusion::Clusion::Clusion
(bool        include_
,std::string pattern_)
: Item()
, include(include_)
, pattern(pattern_)
, DEN_conditions(0)
{
   char first_char = pattern.c_str()[0];
   if ((pattern.c_str()[0] == '/')  // unix style filename
    || (pattern.c_str()[0] == '\\')) // windows style filename with no drive designator (on any drive)
   {  DEN_conditions |= RULE_DEN_CONDITION_ROOTED;
      strip_string(pattern, Leading , first_char);
   }
   if ((pattern.c_str()[1] == ':') // windows style filename with no drive designator (on any drive)
    && (pattern.c_str()[2] == '\\')) // windows style filename with no drive designator (on any drive)
     DEN_conditions |= RULE_DEN_CONDITION_ROOTED;
   // Warning I am not sure exactly how to represent root condition
   // when the user wants to match drive paths with drive designator
   // for windows
   char final_char = pattern.c_str()[pattern.length()-1];
   if ((final_char == '/')
     ||(final_char == '\\'))
   {  DEN_conditions |= RULE_DEN_CONDITION_DIRECTORY;
      strip_string(pattern, Trailing , final_char);
   }
}
//_Clusion:constructor______________________________________________2016-10-23_/
const std::string &Seclusion::Clusion::append_to_string(std::string &buffer) const
{
   buffer += (include) ? "+ " : "- ";
   if (DEN_conditions & RULE_DEN_CONDITION_ROOTED)    buffer += '/';
   buffer += pattern;
   if (DEN_conditions & RULE_DEN_CONDITION_DIRECTORY)    buffer += '/';
   return buffer;
}
//_append_to_string_________________________________________________2016-10-29_/
/* conceptual

Add unary option
--seclusion (with out =filename)
which enables
search for any files accessible file with extensions
seclusion
inclusion
exclusion

and if encountered read them accordingly as if
specified with the
--exclusion= --inclusion= or --seclusion=
*/

}//_namespace_CORN_____________________________________________________________/

