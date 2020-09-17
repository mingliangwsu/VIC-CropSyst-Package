#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>     //LML 140815
#include "vicNl.h"
#include "VCS_user_def.h"
#ifdef VIC_CROPSYST_VERSION
static char vcid[] = "$Id: read_croplib.c,v 4.1.1.1 2011/02/13 Kiran Chinnayakanahalli, kiran.c@wsu.edu $";
#if (VIC_CROPSYST_VERSION==2)
crop_lib_struct *read_croplib(FILE *croplib, int *No_ofcrops_in_cell)
/**********************************************************************
  read_croplib.c   Kiran Chinnayakanahalli, kiran.c@wsu.edu    2011

  This routine reads in a library of crop parameters for all
  crop classes used in the model.  This will be used only when the crop
  model is on (CROPSYST_ON == TRUE).  For the starter, it has only the crop
  code (WSDA cropland data layer derived) and crop names.  Crop speficific
  apramters can be potentially added.

**********************************************************************/
{
    extern option_struct options;
#if LINK_DEBUG
  extern debug_struct debug;
#endif
  crop_lib_struct *temp;
  int    i, j, temp_cropcode;
  int    tmpflag;
  int    Ncrop_type;
  char   str[MAXSTRING];
  char   ErrStr[MAXSTRING];
  double maxd;
  float MAD;


//rewind(croplib);
  fgets(str,MAXSTRING,croplib);
  Ncrop_type = 0;
  while(!feof(croplib)) {
    if(str[0]<=57 && str[0]>=48) Ncrop_type++; /** when read as a string, the str[0] has the ascii decimal equvivalent of the the first character.
                                              ascii characters 0-9 have ascii decimals from 48 to 57...KJC(commented)**/
    fgets(str,MAXSTRING,croplib);
  }
  *No_ofcrops_in_cell = Ncrop_type;
  rewind(croplib);

//  temp = (int *)calloc(Ncrop_type,sizeof(crop_lib_struct));
/*unsigned long cls_size =  sizeof(crop_lib_struct);
printf("%uld",cls_size);*/ //KM to check how it works

  temp = (crop_lib_struct *)calloc(Ncrop_type,sizeof(crop_lib_struct));
  fscanf(croplib, "%d %s %f", &temp_cropcode, str, &MAD);
  i=0;
  while (!feof(croplib)) {
       temp[i].crop_code = temp_cropcode;
       strcpy(temp[i].crop_name, str);
       temp[i].no_of_crops_in_lib = *No_ofcrops_in_cell;
       /**130417 keyvan added to include the MAD library in the croplib**/
       temp[i].MAD=MAD; //keyvan
       fscanf(croplib, "%d %s %f", &temp_cropcode, str, &MAD); //keyvan
       i++;
  }
  return temp;
}

/**********************************************************************
  free_croplib              Keyvan Malek		Jan 2013

  This routine frees the croplib structure.

  Modifications:

**********************************************************************/
void free_croplib(crop_lib_struct **crop_lib) {

  free((char*)(*crop_lib));
}
#endif
/* LML 150413
#if (VIC_CROPSYST_VERSION>=3)
//LML 141104 read crop rotation type and namespace
crop_rotation_lib_struct *read_crop_rotation_lib(FILE *crop_rotation_lib, int *number_rotation_types)
{
    crop_rotation_lib_struct *temp;
    int    i, temp_cropcode;
    int    Ncrop_rotation_type;
    char   str[MAXSTRING];


    //rewind(croplib);
    fgets(str,MAXSTRING,crop_rotation_lib);
    Ncrop_rotation_type = 0;
    while(!feof(crop_rotation_lib)) {
      if(str[0]<=57 && str[0]>=48) Ncrop_rotation_type++; //when read as a string, the str[0] has the ascii decimal equvivalent of the the first character.
                                                //ascii characters 0-9 have ascii decimals from 48 to 57...KJC(commented)
      fgets(str,MAXSTRING,crop_rotation_lib);
    }
    *number_rotation_types = Ncrop_rotation_type;
    rewind(crop_rotation_lib);

    temp = (crop_rotation_lib_struct *)calloc(Ncrop_rotation_type,sizeof(crop_rotation_lib_struct));
    fscanf(crop_rotation_lib, "%d %s", &temp_cropcode, str);
    i=0;
    while (!feof(crop_rotation_lib)) {
         temp[i].no_of_crop_rotations_in_lib = Ncrop_rotation_type;
         temp[i].crop_rotation_code = temp_cropcode;
         strcpy(temp[i].crop_rotation_name, str);
         fscanf(crop_rotation_lib, "%d %s", &temp_cropcode, str);
         i++;
    }
    return temp;
}

void free_crop_rotation_lib(crop_rotation_lib_struct **crop_rotation_lib) {

  free((char*)(*crop_rotation_lib));
}
#endif
*/

#ifndef FULL_IRRIGATION
/******************************************
allocate memory to irrigation pattern Keyvan 130530

this function is developed to calculate
effect of fdeficit irrigation
******************************************/
int alloc_irrig_pattern(
                    int nrecs,
                    //std::map<int,std::map<int,irrigation_pattern_struct>> &irrig_patt, //irrigation_pattern_struct **irrig_patt,
                    filep_struct         *filep,
                    filenames_struct     *filenames,
                    soil_con_struct      *soil
                    )
{
    extern option_struct    options;
    extern FILE *open_file(char string[], char type[]);

    /****making irrigation pattern file names, which contains the lan and long of each cell Keyvan 130710****/
    char   latchar[15], lngchar[15], junk[5];
    sprintf(junk, "%%.%if", options.GRID_DECIMAL);
    sprintf(latchar,  "%.5f", soil->lat);
    sprintf(lngchar, junk, soil->lng);

    strcpy(filenames->VCS.irrigation_pattern, filenames->VCS.irrig_fpath_pfx);
    strcat(filenames->VCS.irrigation_pattern, "_");
    strcat(filenames->VCS.irrigation_pattern, latchar);
    strcat(filenames->VCS.irrigation_pattern, "_");
    strcat(filenames->VCS.irrigation_pattern, lngchar);

    filep->VCS.irrigation_pattern=open_file(filenames->VCS.irrigation_pattern, "rb");
    return 0;
}
#endif
/********************************************************************
read irrigation pattern
********************************************************************/
int words(const char *s)
{
    const char *sep = " \t\n\r\v\f";
    int word = 0;
    size_t len;
    s += strspn(s, sep);
    while ((len = strcspn(s, sep)) > 0) {
        ++word;
        s += len;
        s += strspn(s, sep);
    }
    return word;
}
#ifndef FULL_IRRIGATION
int read_irrig_pattern(FILE *infile,
                           std::map<int,std::map<int,irrigation_pattern_struct>> &irrig_patt,
                           //irrigation_pattern_struct *irrig_patt,
                           int nrecs
                           )
{
    double ustmp;
    double *irrigation_data;
    int rec = 0;
    //fseek(infile,Nbytes,SEEK_SET);
    int rotation,year,doy;
    double irrig_amount,proration,temp;
    //doy is from 1
    CORN::Date_clad_32 first_sim_day;
    first_sim_day.set_YMD(global_param.startyear,global_param.startmonth,global_param.startday);
    char *line = NULL;
    size_t charactors,len;
    int columns;
    while ((charactors = getline(&line,&len, infile)) != -1) {
        columns = words(line);
        if (columns == 5) {
            sscanf(line, "%i %i %i %lf %lf\n",
                         &year, &doy, &rotation, &irrig_amount, &proration);
        } else if (columns == 4) {
            sscanf(line, "%i %i %i %lf\n",
                         &year, &doy, &rotation, &irrig_amount);
            proration = -1;
        } else if (columns != 0) {
            std::cerr << "Wrong irrigation pattern file format!\n";
            return 1;
        }

        irrigation_pattern_struct irr;
        irr.irrig_amount = irrig_amount;
        irr.proration_rate = proration;
        CORN::Date_clad_32 recday;
        recday.set_YD(year,doy);
        int rec = recday.days_between(first_sim_day,false);
        irrig_patt[rotation][rec] = irr;
    }
    //return(irrigation_data);
    return 0;
}
#endif
/*************************************************
reading CO2 concentrations from input file
keyvan 130605
**************************************************/
CO2_conc_struct *read_CO2_conc(FILE *infile){
extern option_struct options;
CO2_conc_struct *CO2_str;
int rec=0;
int year;
double *CO2_conc;
int CO2_nyear=737;
CO2_conc = (double *)calloc(CO2_nyear,sizeof(double));
CO2_str = (CO2_conc_struct *) calloc(CO2_nyear, sizeof(CO2_conc_struct));
//temp = (crop_lib_struct *)calloc(Ncrop_type,sizeof(crop_lib_struct));

while(!feof(infile))
    {
    fscanf(infile,"%i %lf", &year, &CO2_conc[rec]);
     // CO2_nyear++;
      CO2_str[rec].year=year;
      CO2_str[rec].CO2_conc=CO2_conc[rec];
      rec++;

    }


options.VCS.CO2_Nyear=CO2_nyear;

return(CO2_str);
//for(rec=0;rec<CO2_nyear;rec++)
//fscanf(infile,"%i %lf", &year, &CO2_conc[rec]);

}


/**************************************************
keyvan 130605
allocate memory to the CO2 structure
**************************************************/
/*alloc_CO2_conc(
                    int nrecs,
                    irrigation_pattern_struct **irrig_patt

                    )
{
*irrig_patt = (irrigation_pattern_struct *) calloc(nrecs, sizeof(irrigation_pattern_struct)+2);
//int i;

//for (i = 0; i < nrecs; i++)
//irrig_patt[i].irrig_amount = (double *) calloc(1, sizeof(double));

}
*/
#endif

