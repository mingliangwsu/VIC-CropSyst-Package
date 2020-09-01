#include "mapunit_struct.h"

namespace USDA_NRCS
{
//______________________________________________________________________________
Mapunit_struct::Mapunit_struct()
{  clear();
}
//_Mapunit_struct:constructor_______________________________________2011-12-18_/
void Mapunit_struct::clear()
{
   musym.clear();
   muname.clear();
   muacres = 0.0;
   mapunitlfw.clear();
   mapunitpfa.clear();
   interpfocus.clear();
   iacornsr = 0;
   lkey.clear();
   mukey.clear();
}
//_clear____________________________________________________________2011-12-18_/
}//_namespace USDA_NRCS________________________________________________________/
/*
mapunit.txt
"s66"|"Leesburg-Gorgas-Allen (s66)"|"Association"||8285||||||||||||"Order 5"||||||"15970"|"657944"
"s67"|"Sullivan-State-Ketona-Decatur-Bodine (s67)"|"Association"||7619||||||||||||"Order 5"||||||"15970"|"657945"
"s68"|"Wynnville-Townley-Sipsey (s68)"|"Association"||14201||||||||||||"Order 5"||||||"15970"|"657946"
"s69"|"Townley-Stemley-Minvale-Bodine (s69)"|"Association"||6126||||||||||||"Order 5"||||||"15970"|"657947"
*/

