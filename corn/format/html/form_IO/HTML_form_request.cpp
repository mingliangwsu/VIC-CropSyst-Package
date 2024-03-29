#include <corn/format/HTML/form_IO/HTML_form_request.h>
#include <corn/format/HTML/form_IO/content.h>
#include <corn/format/datarec.h>

namespace HTML
{
HTML_form_request::HTML_form_request(/*const char *i_content*/)
: Data_source()
, content(/*i_content*/)
{
}
//____________________________________________________________________________
#ifndef local_debug

bool HTML_form_request::get(Data_record &data_rec)
{
   bool got = Data_source::get(data_rec);
   {
	  char key[100];    int key_i = 0;
      char value[100];  int value_i = 0;
      //             Currently HTML forms don't have sections, but VV_File is setup that way.
      //             The default current section is []
       VV_Section *
       current_section = (VV_Section *)data_rec.get_current_section(); //010308

       bool scanning_key = true;
       for (int i = 0; i <= content.length(); i++)
       {
          char curr_char = content.c_str()[i];

       if (scanning_key)
       {
          if (curr_char == '=')
          {   scanning_key = false; key_i = 0; }
          else
          {  key[key_i++] = curr_char; key[key_i] = 0;
          }
       }
       else
       {
          if ((curr_char == '&') || (curr_char == 0))
          {  // End of variable=value
             CORN_string clean_value(value);
             unescape_url(clean_value);
             current_section->add_entry(key/*.c_str()*/,clean_value.c_str(),false/*,unexpected_only*/);  // I think unexpected_only false //030415
             scanning_key = true;
             key[0] = 0; key_i = 0;     // key.assign("");
             value[0] = 0; value_i = 0; // value.assign("");
          }
          else
          {  value[value_i++] = curr_char; value[value_i] = 0;
          }
       }
    }
   }
   return true;
}
#endif
//____________________________________________________________________________
bool HTML_form_request::set(Data_record &data_rec) //010308
{
   // NYI currently we only just read forms from the CGI
   return true;
}
//____________________________________________________________________________
/*//170424 replaced with label_wstring
const char *HTML_form_request::label_cstr(char *buffer) const
{   return buffer; // Currently not needed
}
*/
//____________________________________________________________________________
}//_namespace_HTML_____________________________________________________________/

