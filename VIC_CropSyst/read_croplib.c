
#include <stdio.h>
#include <stdlib.h>
#include <string.h>     //LML 140815
#include "vicNl.h"
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

#if (FULL_IRRIGATION==FALSE)
/******************************************
allocate memory to irrigation pattern Keyvan 130530

this function is developed to calculate
effect of fdeficit irrigation
******************************************/
alloc_irrig_pattern(
                    int nrecs,
                    irrigation_pattern_struct **irrig_patt,
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

strcpy(filenames->irrigation_pattern, filenames->irrig_fpath_pfx);
strcat(filenames->irrigation_pattern, "_");
strcat(filenames->irrigation_pattern, latchar);
strcat(filenames->irrigation_pattern, "_");
strcat(filenames->irrigation_pattern, lngchar);

filep->irrigation_pattern=open_file(filenames->irrigation_pattern,"rb");/*Keyvan 130530 */

/*******allocate the memory******/
*irrig_patt = (irrigation_pattern_struct *) calloc(nrecs, sizeof(irrigation_pattern_struct)+2);
//int i;

//for (i = 0; i < nrecs; i++)
//irrig_patt[i].irrig_amount = (double *) calloc(1, sizeof(double));

}
#endif
/********************************************************************
read irrigation pattern
********************************************************************/
#if (FULL_IRRIGATION==FALSE)
double *read_irrig_pattern(FILE *infile,
                           irrigation_pattern_struct *irrig_patt,
                           int nrecs
                           )
{


//extern param_set_struct param_set;
//int endian;
//int    i = 1;
    //if(*(char *)&i == 1)
      //endian = LITTLE;
    //else
      //endian = BIG;

//fseek(infile,int ustmp,SEEK_SET);
//rewind(infile);
double ustmp;
//fseek(infile,(int)ustmp,SEEK_SET);
 //double ustmp;
 double *irrigation_data;
// *irrig_patt = (irrigation_pattern_struct *) calloc(nrecs, sizeof(irrigation_pattern_struct));
irrigation_data = (double *)calloc(nrecs,sizeof(double));
 int rec = 0;
//fseek(infile,Nbytes,SEEK_SET);
while ( !feof(infile) && rec<nrecs)
    {

 /*   if (endian != param_set.FORCE_ENDIAN[1]) {
        ustmp = ((ustmp & 0xFF) << 8) | ((ustmp >> 8) & 0xFF);
      }*/

  // fread(&irrigation_data[rec],sizeof(double),1,infile);
  /// fread(&ustmp,sizeof(double),1,infile); I should spend sometimes to fix it
  //  irrigation_data[rec]=(double)ustmp;
   /* if((double)ustmp>1){
        int keyvan=1;
    }*/
    int day, month, year;
//irrigation_data[rec]=(double)ustmp;
   fscanf(infile,"%i %i %i %lf", &year, &month, &day, &irrigation_data[rec]);
//   irrig_patt[rec].irrig_amount=irrigation_data[rec];
    //if(year>1){
    if(*irrigation_data>1){
        int keyvan=1;
    }
    rec++;
    }

return(irrigation_data);
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

