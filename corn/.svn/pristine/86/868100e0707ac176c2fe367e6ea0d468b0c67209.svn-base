#  include "corn/parameters/parameters.h"
#  include "corn/parameters/parameters_directory_OS_FS.h"
#  include "corn/data_source/datarec.h"
#include "corn/OS/OS.h"
//______________________________________________________________________________
Common_parameters::Common_parameters
(Associated_directory_mode associated_directory_mode_                            //060620
,nat32                     version_code_                                        //180626
)
: associated_directory_mode(associated_directory_mode_)                          //060627
{
   CORN::Date_format BCC_compiler_DATE_format(D_MDY,D_YYYY|D_Mmm,' ');           //180626
   version_program_date_curr.set_c_str(__DATE__,&BCC_compiler_DATE_format);      //180626
}
//______________________________________________________________________________
#ifdef REDO
060627 I want to redo this
void Common_parameters::write_designation(const char *parameters_filename)
{
#if (__BCPLUSPLUS__ >= 0x0550)
   // The following will create an icon for the project directory
   {
      const Common_parameters_directory *associated_directory = provide_associated_directory(parameters_filename,false);
      if (associated_directory && associated_directory->exists())
      {CORN::File_name qualfied_designation_filename("designation.htm");
       CORN::File_name image_file_name;                                          //090414
      qualfied_designation_filename.set_path(associated_directory->c_str());
      ofstream notes_file(qualfied_designation_filename.c_str());
      notes_file << "<HTML>\n<CENTER>\n<IMG SRC=\"";
      get_image_filename(image_file_name);
      notes_file << image_file_name;
      notes_file << "\">" ;
      notes_file << "</CENTER><BR><B>Project</B><BR>Description: ";
      notes_file << description << endl;
      notes_file << endl << "<BR>Version: ";
      notes_file << current.major << '.' << current.minor << '.' << current.release << endl;
      notes_file << get_designation_notes() << endl;
     notes_file  << "</HTML>" << endl;
      };
   };
#endif
}
//______________________________________________________________________________
#endif
#ifdef REDO
090627 I may redo this

/*050203_*/ bool Common_parameters::get_image_filename
/*090315_*/ (CORN::File_name &filename_return_buffer
/*090315_*/ ,bool check_existance)                                         const
//090511/*050203_*/ const CORN::File_name &Common_parameters::get_image_filename(modifiable CORN::File_name &filename_return_buffer) const
///*050203_*/ const CORN::File_name &Common_parameters::get_image_filename(modifiable CORN::File_name &filename_return_buffer) const
/*_______*/ {  // Default, we don't have an image _filename
/*050207_*/    filename_return_buffer.set("");
/*090315_*/    return filename_return_buffer.exists();
/*_______*/ }
//_get_image_filename_2009-03-15__2005-02-03________________get_image_filename_/
#endif
bool Common_parameters::is_associated_directory_required()                 const
{  return
      ( (associated_directory_mode == REQUIRED_ASSOCIATED_DIRECTORY)
      ||(associated_directory_mode == PARENT_ASSOCIATED_DIRECTORY));
}
//_is_associated_directory_required_________________________________2006-06-14_/

