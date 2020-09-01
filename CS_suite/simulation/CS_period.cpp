#include "CS_suite/simulation/CS_period.h"
#include "corn/container/item.h"
namespace CS {
//______________________________________________________________________________
int Period_clad::compare(const CORN::Item &other_item)                          const
{
   const Period_clad &other_period = dynamic_cast<const Period_clad &>(other_item);
   int compare_result
   = (this->level_or_period != other_period.level_or_period)
     ? -1
     : (this->initiation_date!= other_period.initiation_date)
       ? -1
       : 0;
   // conceptual:  may also need to compare context
   return compare_result;
}
//_compare_____________________________________________________________________/
void Period_clad::write_YAML(std::ostream &YAML_strm,nat16 indent_level) stream_IO_
{
   std::string indent(indent_level,' ');
   YAML_strm

      // WARNING this wouldnt be proper YAML because period: here
      // would be an key/index and it would not be unique.
      // If it want to indicate the item is a period, use type tag

      << indent << "- period:" << std::endl
      << indent << "  name: " << name << std::endl
      << indent << "  initiation: " << /*std::setprecision(7) << */initiation_date << std::endl
      << indent << "  completion: " << /*std::setprecision(7) << */completion_date << std::endl
      << std::endl;
}
//_write_YAML_______________________________________________________2019-01-24_/

}//_namespace_CS_______________________________________________________________/
