#include "corn/data_source/command_options_datasrc.h"
#include "corn/data_source/vv_file.h"
#include "corn/OS/file_system_engine.h"
#include "corn/OS/directory_entry_name_concrete.h"

#ifdef __BCPLUSPLUS__
#ifndef CROPSYST_VERSION
#include <vcl.h>
#define VCL_ARGS
#endif
#endif
#include <cstring>

namespace CORN {
//______________________________________________________________________________
Command_options_data_source::Command_options_data_source
(nat16       argc_
,const char *argv_[]
,const char *response_file_extension_)
: VV_Data_source()
, argc(argc_)
, argv(argv_)
, args()
{  for (int a = 0; a < 1000; a++)
      if ((a >= 0) && (a < argc))                                                //171110_161016
         args.add_cstr(argv[a]);
}
//_constructor_________________________________________________________________/
Command_options_data_source::Command_options_data_source
(nat16  argc_
,char  *argv_[]
,char  *response_file_extension_)
: VV_Data_source()
, argc(argc_)
, argv(const_cast<const char **>(argv_))
, args()
{  for (int a = 0; a < 1000; a++)
      if ((a >= 0) && (a < argc))
         args.add_cstr(argv[a]);
}
//_constructor______________________________________________________2019-04-14_/
Command_options_data_source::Command_options_data_source(const std::string &response_line)
: VV_Data_source()
, argc(0)
, argv(0)
, args(response_line.c_str(),' ')
{}
//_constructor______________________________________________________2016-10-15_/
Command_options_data_source::Command_options_data_source
(const CORN::OS::File_name &response_filename)
: VV_Data_source()
, argc(0)
, argv(0)
, args()
{  nat32 line_count = 0;
   std::ifstream response_stream(response_filename.c_str());
   std::string response_line;
   while (response_stream.good() && !response_stream.eof())
   {  getline(response_stream,response_line);
      if (!response_line.empty())
      {  CORN::Text_list line_args(response_line.c_str(),' ');
         args.transfer_all_from(line_args);
         line_count++;
      }
   }
}
//_constructor______________________________________________________2016-10-15_/
#ifdef VCL_ARGS
Command_options_data_source::Command_options_data_source()
{  for (nat8 arg_i = 0; arg_i <= ParamCount(); arg_i++)
   {  Item_string *arg_raw = new Item_string(ParamStr(arg_i).c_str());
      args.take(arg_raw);
   }
}
//_constructor:VCL__________________________________________________2020-03-01_/
#endif
bool Command_options_data_source::get(modifiable_ Data_record &options_record)
{  bool got = VV_Data_source::get(options_record);
   Arguments *as_arguments = dynamic_cast<Arguments *>(&options_record);         //190925
   { // The first argument arg0 is the (fully qualified) program name
   Item_string *arg0 = dynamic_cast<Item_string *>(args.pop_first());            //190925
   if (arg0)                                                                     //190925
   {  if (as_arguments)                                                          //191010
          as_arguments->know_program(arg0->c_str());                             //191220_190925
      delete arg0;
   }
   }
   for (Item_string *arg = dynamic_cast<Item_string *>(args.pop_first())
          ;arg
          ;arg = dynamic_cast<Item_string *>(args.pop_first()))
   {  Submission_status status = submit(options_record,*arg,false);
      if ((status == unrecognized) || (status==entry_unknown))                   //191103
         if (as_arguments)                                                       //191103
         {  bool taken = as_arguments->take(arg);                                //191103
               // Note that arg may have been relinquished at this point
            if (!taken)
               as_arguments->take_unrecognized(arg);
            status = taken ? entry_known : unrecognized;                         //200223
         }
      // At this point arg has been given                                        //191103
   } // for arg
   options_record.get_end();
   return true; // expected_entried_found ;
}
//_get_________________________________________________________________________/
VV_Data_source::Submission_status Command_options_data_source::submit
(modifiable_ Data_record &data_rec
,const std::string &text
,bool unexpected_only)                                               submission_
{  VV_Data_source::Submission_status status
   = VV_Data_source::submit(data_rec,text,unexpected_only);                      //190224
   return status;
}
//_submit___________________________________________________________2016-10-15_/
const char *Command_options_data_source::label_string(std::string &buffer) const
{  buffer = "command line options";
   return buffer.c_str();
}
//_label_string_____________________________________________________2017-04-23_/
}//_namespace_CORN_____________________________________________________________/

