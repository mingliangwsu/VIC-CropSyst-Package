#include "soil_database_to_VIC_engine.h"
#include "corn/OS/file_system_engine.h"
#include "corn/OS/file_system_logical.h"
#include "corn/data_source/command_options_datasrc.h"
#include "global.h"

// int debug_received_component_count = 0; // debug only

void nrerror(char error_text[]) {}

option_struct options;
// Note options is normally instantiated in global.c for VIC

//______________________________________________________________________________
namespace CORN { namespace OS {
File_system &file_system() { return CORN::OS::file_system_logical; }
}}
//______________________________________________________________________________


int main(int argc, const char *argv[])
{  bool processed = true;
   CORN::Command_options_data_source command_line_options(argc,(const char **) argv); //170831
   Soil_database_to_VIC::Arguments arguments;                                    //170831
   command_line_options.get(arguments);                                          //170831

   geocoordinate_approximation = 0.001;                                          //190218
//   #if (VIC_VERSION==407)
//   options.Nlayer=3;
//   options.SNOW_BAND = 1;
//   options.JULY_TAVG_SUPPLIED = 1;    // may need to be 5
//   #else
   options.Nlayer = 3;
   options.SNOW_BAND = 1;
   options.JULY_TAVG_SUPPLIED = 0;
   options.INIT_STATE = 0;
//   #endif

   std::clog << "Target output directory: " << arguments.destination_directory << std::endl;
   /*190217 now command argument VIC=
   CORN::OS::File_name_concrete original_VIC_soil_grid_filename(argv[1]);        //150204
      //"C:\Projects\CRB\Database\Soil\soil_param.0625.hb2860.20090813.vsd.txt"  // This was CRB
      //
   */
   Soil_database_to_VIC soil_database_to_VIC
      (arguments/*190217 ,original_VIC_soil_grid_filename*/ );                   //150204

   soil_database_to_VIC.set_scope(41.0,-125.0,53,-110.0);                        //151010

   // Note that scope could be replaced by extent

   bool success
    =  soil_database_to_VIC.initialize()
     && soil_database_to_VIC.process();
         //190217       (original_VIC_soil_grid_filename);


//std::clog << "component count" << debug_received_component_count << std::endl; // debug

   exit(0); // something is crashing                                             //190228
   //19022 return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
//______________________________________________________________________________
/*
// command line options latest for Keyvan with 17 layers:
"C:\Projects\MSLSH\CRB\Database\Soil\soil_param.0625.hb2860.20090813.vsd.txt"
 calibrated grid file for CRB  (This had been prepared by Keyvan and includes his specializations/
"C:\Projects\MSLSH\CRB\gridcell_lat_long_STATSGO2_mapunit.dbf"
"C:\Projects\MSLSH\CRB\gridcell_lat_long_CanSIS_NSDB_SLC_polyID.dbf"
C:\Simulation\Database\STATSGO2\gsmsoil_us\tabular

// Command line options
C:\Projects\MSLSH\Database\Soil\vic.soil.cal.us
 Calibrated grid file for the entire US 3 layers proported to be compatible with VIC 4.0.7

 C:\Projects\BPA


For STATSGO the following fields are needed to identify the cells to import
MUKEY,C,30
CELLNO,N,10,0
LAT,N,19,11
LON,N,19,11

CELLNO or CELL
--WSU

C:\Projects\MSLSH\Database\Soil

"C:\Projects\MSLSH\Database\Soil\vic.soil.cal.us"

--CanSIS_NSDB_SLC="S:\Database\CanSIS\NSDB\SLC_3.2"
--STATSGO2-tabular="C:\Simulation\Database\STATSGO2\gsmsoil_us\tabular"

layers= number of layers to put in VIC file (includes 'deep' layer

The VIC files  option specifies the points of interest the be selected from the STATSGO database
This option must be specified when using STATSGO

"cell_MUKEY_Kalamath.dbf"
There may be any number of dbf file (eventually also txt or dat)
which specify the points of interest and gecoordinate and MUKEY or xxxx association.


*/
/*

The --WSU generates a VIC file format for use with the WSU
version of VIC cropSyst which has 17 layers

The program (currently) does not generate a VIC file from scratch because
the VIC soil file has properties that I don't know how to derive from the available data.
Consequently default column are loaded from an existing VIC file.
vic.soil.cal.us is provided listing grid cell properties for the entire USA.

The VIC file is required, it drives what will be loaded (attempted) from the
the STATSGO and/or CanSIS databases.
(Conceptual: allow the data to be read from STDIN)

The grid cell values are the points of interest.
The grid cell values are simply echoed from this file if
superior soil properties cannot be derived from STATSGO2 or CanSIS databases.
So the generated file will have the same grid cell found in the specified VIC file.
(I.e. if no MUKEY or xxxx is not found in the respective database, the
gridcell output is simply taken from the VIC file.)




The program itself as I currently have it was specifically written to 'enhance' an existing source/template
VIC soil file by replacing values with those from the STATSGO
(and EnvironCanada Canadian soils) databases and/or
calculated with pedotransfer functions.
It also extends layers and provides additional properties added for the VIC-CropSyst model
(although I could add an option to generated the original VIC format).

To use the program as-is one provides a source/template VIC file preset with the values the exporter program
doesn't know and with dummy values in the other column.

I can easily modify the program to generate a VIC soil file from scratch
(where a source/template VIC file would be optional).
A simple table with grid cell ID and geocoordinates (latitude and longitude, elevation) would still be needed.
(Elevation could be omitted as I can get it from NKN).
We would need to know how to derive the properties that are unavailable from the soil databases.  
I could output default values in these columns to be provided later if needed.

bi  Infiltration parameter 
 appears to be either 1 or 3, 
presuming will/may be calibrated? So a default value is used???

ds   
Subsurface flow rate  
 (fraction range 0 to 1)

dsMax mm/day
Maximum subsurface flow rate
(fraction range 0 to 1)

soil c  
exponent in ARNO baseflow curve
appears to be always 2


elevation
(is this available?)

avg_temp
average soil temperature? Or is it Air temperature?
This could be derived with soil temperature model, which I do have, but would need to be
added to the program.  It would be time consuming, I don't think this is very critical.
One could probably simply use a value based on average air temperature.
In either case daily weather data would be needed.

dp
I don't know what this is.

rough , snow_rough
Soil and snow surface roughness
Perhaps default values can be used?

offGMT
hour offset from Greenwich Mean Time Mean Time?
(could be calculated from longitude)

avgJulyAirTemp
This is optional if JULY_TAVG_SUPPLIED is TRUE.
annual_prec
could get this from NKN/REACCH weather database.

resid_moist
resid_moist is 0 because 0 in Campbell formula
If using Brooks-Corey Model would need a non-zero value.


C:\Projects\BPA\Database\Soil\vic.soil.cal.us_test

--verbose=127 VIC="C:\Projects\MSLSH\Database\Soil\vic.soil.cal.us" "cell_MUKEY_Kalamath.dbf" --CanSIS_NSDB_SLC="S:\Database\CanSIS\NSDB\SLC_3.2" --STATSGO2-tabular="C:\Simulation\Database\STATSGO2\gsmsoil_us\tabular"

*/
