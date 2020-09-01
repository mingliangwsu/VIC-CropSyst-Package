#ifndef collate_engineH
#define collate_engineH
#if (COLLATOR_VERSION==2014)
#include "output_collator_2014.h"
#else
#include "output_collator_2019.h"
#endif
namespace UED
{
//______________________________________________________________________________
class Collate_engine
{
 protected:
   UED::Arguments_collator &arguments;
 public:
   Collate_engine
      (UED::Arguments_collator &arguments);                                      //170328
   bool process();
   nat32 process_UED(const std::wstring &UED_name);                              //180119
};
//_class_Collate_engine_____________________________________________2014-09-03_/
}//_namespace UED______________________________________________________________/
#endif

