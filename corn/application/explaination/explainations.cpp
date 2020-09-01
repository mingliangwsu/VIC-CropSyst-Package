#include "corn/application/explaination/explainations.h"
namespace CORN {
//______________________________________________________________________________
Explainations::Explainations
(bool  _find_unqualified_filenames
,const CORN::OS::Directory_name *_program_doc_dir_fully_qual
,const CORN::OS::Directory_name *_application_doc_dir_fully_qual
,const CORN::OS::Directory_name *_suite_doc_dir_fully_qual
,const CORN::OS::Uniform_resource_identifier *_Web_root_URI
,const CORN::OS::Directory_name *_local_root_path_fully_qual)
: Bidirectional_list()
, find_unqualified_filenames(_find_unqualified_filenames)
{
   program    =_program_doc_dir_fully_qual;
   application=_application_doc_dir_fully_qual;
   suite      =_suite_doc_dir_fully_qual;
   Web_root_URI =_Web_root_URI;
   local_root =_local_root_path_fully_qual;
}
//_Explainations:constructor___________________________________________________/
Item *Explainations::take(Item *new_element)                      appropriation_
{  Item *taken = 0;                                                              //160302LML
   if (!new_element) return taken;
   #pragma omp critical
   {
   Explaination *comparable_explaination = dynamic_cast<Explaination *>(find_comparable(*new_element,true));
   if (comparable_explaination)
   {  Explaination *new_explaination = dynamic_cast<Explaination *>(new_element);
      comparable_explaination->context_URIs.transfer_all_from(new_explaination->context_URIs);
      comparable_explaination->occurance_datetimes.append_array(new_explaination->occurance_datetimes);
      delete new_explaination;
      taken = 0;
   } else
      taken = Bidirectional_list::append(new_element);
   }
   return taken;
}
//_take_____________________________________________________________2011-06-22_/
/*151013 refactored to take()
Item *Explainations::append(Item *new_element)                    appropriation_
{  if (!new_element) return 0;
   Explaination *comparable_explaination = dynamic_cast<Explaination *>(find_comparable(*new_element,true));
   if (comparable_explaination)
   {  Explaination *new_explaination = dynamic_cast<Explaination *>(new_element);
      comparable_explaination->context_URIs.transfer_all_from(new_explaination->context_URIs);
      comparable_explaination->occurance_datetimes.append_array(new_explaination->occurance_datetimes);
      delete new_explaination;
      return 0;
   } else
      return Bidirectional_list::append(new_element);
}
//__________________________________________________________________2011-06-22_/
Item *Explainations::prepend(Item *new_element)                   appropriation_
{  // Just appeand because we don't care about order
   // eventually container will have take() method.
   return append(new_element);
}
*/
//__________________________________________________________________2011-06-22_/
nat32 Explainations::write
(std::ostream &explaination_stream)                                 performs_IO_
{
   FOR_EACH(explaination,Explaination,each_explaination)
   {  explaination->resolve_URL(*this,find_unqualified_filenames);
   } FOR_EACH_END(each_explaination)
   return Bidirectional_list::write(explaination_stream);
}
//_write____________________________________________________________2015-07-27_/
Explainations *explainations_global = 0;
Explainations &provide_explainations_global()
{  if (!explainations_global)
        explainations_global = new Explainations;
   return *explainations_global;
}
//_provide_explainations_global_____________________________________2015-01-08_/
}//_namespace_CORN_____________________________________________________________/
