#ifndef recursionH
#define recursionH

namespace CORN {
namespace OS {
namespace FS {
//____________________________________________________________________________
const nat8 descendent   = 0x1;
const nat8 ascendent    = 0x2;
const nat8 exclusive_deprecated = 0x4;
   // Exclusive is only provided for compatibility
const nat8 depth_first  = 0x8;
   // This is actually more generally extreme_first
const nat8 breadth_first  = 0x0;

const nat8 to_extreme      = 0xFF;
const nat8 from_CWD        = 0x00;
/* Conceptual     Have direction option

top_down  top_first (extreme first)
   in asendent mode recurse to the top (stop) directory
   then search/gather files coming back down
   (Top files are listed first)
bottom_up
   start in the current directory add files then
   recuse up adding files.
*/

struct Recursion
{
   nat8 direction;   // flags
      // Bit mask bits
      // 0 descendent (subdirectory)
      // 1 ascendent  (superdirectory)
      // 2 exclusive    (abandoned/obsolete)
      // 3 depth first  (when subdirectory) 
      //    Applicable when searching down into subdirectory/
      //    true indicates that subdirectory recursion
      //    first recurses down into the directory structure, then
      //    searchs for matching files as the recursion comes out,
      //    otherwise the search occurs as directories are entered.
      // The remaining bits super directory search stop conditions (conceptual, may be CS specific)
      // Although may want to specify path name (or pattern)
      // 4 home_stop    (dont go above home)
      // 5 project_stop (dont go above project)
      // 6 suite_stop   (don't go above application suite directory)
      // 7 application_stop (dont go above application directory)
   struct Depth
   {  nat8 max;
      // Descend at most levels (a non-negative integer)
      // levels of directories below the starting-points.
      // -maxdepth 0 means only apply the tests and actions to the starting-points themselves.
      nat8 min;
      // Do not apply any tests or actions at levels less than levels
      // (a non-negative integer).
      // -mindepth 1 means process all files except the starting-points.
   } depth;
};
//____________________________________________________________________________2020-08-02_/
/* conceptual
class Recursion_extended
: public extends_ Recursion
{
   Seclusion children;
   Seclusion genitors;
};   
//____________________________________________________________________________2020-08-02_/
*/
}}}//_namespace_CORN::OS::FS___________________________________________________/
#endif
