#include "UED_adjustor_engine.h"

using namespace CORN;
using namespace UED;
//______________________________________________________________________________
int main(int argc, char *argv[])
{
   UED::Adjustor_engine adjustor;

   // Either
   CORN::OS::File_name_concrete *source_filename=0; // optional
   CORN::OS::File_name_concrete *target_filename=0; // optional

   // Or
   CORN::OS::Directory_name_concrete *source_dirname=0; // optional
   CORN::OS::Directory_name_concrete *target_dirname=0; // optional

   Interpolation_curve_type interpolation_mode = step_curve;
   nat8 adjustment_file_count = 0;
   for (nat8 a = 1; a < argc; a++)
   {
      if      (strcmp(argv[a],"step") == 0)    interpolation_mode = step_curve;
      else if (strcmp(argv[a],"linear") == 0)  interpolation_mode = linear_curve;
      else
      {
         CORN::OS::File_name_concrete *unidentified_filename
            = new CORN::OS::File_name_concrete(argv[a]);

         if (unidentified_filename->has_extension_ASCII("UED",false))
         {
            if (source_filename)
            {  target_filename = unidentified_filename; unidentified_filename = 0; }
            else
            {  source_filename = unidentified_filename; unidentified_filename = 0; }
         } else
         {  // may be adjustment file
            Adjustment_mode adjustment_mode =
                unidentified_filename->has_extension_ASCII("delta",false)  ? UED::DELTA_ADJUSTMENT
              : unidentified_filename->has_extension_ASCII("scale",false)  ? UED::SCALE_ADJUSTMENT
              : unidentified_filename->has_extension_ASCII("csv",false)  ? UED::SCALE_ADJUSTMENT //180808 special for CAMO project
              : UED::UNKNOWN_ADJUSTMENT;
            if (adjustment_mode)
            {  adjustor.load_adjustments
                  (*unidentified_filename,interpolation_mode,adjustment_mode);
               delete unidentified_filename; unidentified_filename = 0;
               adjustment_file_count++;
            } else // assume argument is a directory
            {  delete unidentified_filename;
               if (source_dirname)
                    target_dirname = new CORN::OS::Directory_name_concrete(argv[a],CORN::OS::directory_entry);
               else source_dirname = new CORN::OS::Directory_name_concrete(argv[a],CORN::OS::directory_entry);
            }
         }
      }
   }
   bool args_specified = true;
   if (adjustment_file_count == 0)
   {
      std::cerr << "No adjustment file specified"                             << std::endl;
      args_specified = false;
   }
   nat32 file_count = 0;
   if        (source_filename && target_filename)
   {
      file_count = adjustor.adjust_file(source_filename,target_filename);
   } else if (source_dirname && target_dirname)
   {
      file_count = adjustor.adjust_directory(source_dirname,target_dirname);
   } else
      args_specified = false;

   if (!args_specified)
   {
      std::clog
      << "Command arguments:"                                                 << std::endl
      << "Either source and target file name"                                 << std::endl
      << "or     source and target directory name"                            << std::endl
      << "The source name must precede the target name."                      << std::endl
      << "File and Directory names can be qualified or relative."             << std::endl
      << "Targets will be overwritten/replaced."                              << std::endl << std::endl

      << "Adjustment values are stored in text files."                        << std::endl
      << "One file for each element to be adjusted so the command line names" << std::endl
      << "one or more adjustment files with file extension .delta or .scale." << std::endl
      << "The filename corresponds to the UED variable code (or name) to be adjusted." << std::endl
      << "The first column of the file is the date in YYYYDDD format"         << std::endl
      << "and the second column is the adjustment delta or scale value."      << std::endl << std::endl

      << "Dates specify the when the adjustment or scaling value becomes in affect." << std::endl
      << "Values can be specified every day, or any any arbitrary periodic"   << std::endl
      << "interval, for example annual adjustments one might have values every January 1." << std::endl
      << "or one could have value monthly, weekly, or at any arbitrary date." << std::endl << std::endl

      << "The adjustment can either be applied in steps on the specified dates (default)" << std::endl
      << "or linearly interpolated."                                          << std::endl << std::endl

      << "Interpolation type  step or linear (specified anywhere on the command line)" << std::endl
      << "Adjustment in the files specified on the command line after this keyword will be interpolated accordingly." << std::endl
      << "(The default is step)" << std::endl;
   }
   return args_specified && file_count ? EXIT_SUCCESS : EXIT_FAILURE;
}
//_2015-03-08_____________________________________________________________main_/
/* Example
historical historical+4  step tmax.delta step tmin.delta
C:\Simulation\Projects\REACCH\PNW Insect\Database\Weather

Special for CAMO
G:\obsolete\Database\Weather\future_high_Srad\RCP8.5\GFDL-ESM2M\26.39N098.23W.UED  G:\Database\Weather\future\RCP8.5\GFDL-ESM2M\26.39N098.23W.UED  "G:\temp\GCMs_K adjustment factors_modified\GFDL-ESM2M\26.39N098.23W.csv"



*/
