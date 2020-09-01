
#include <stdio.h>
#include <stdlib.h>
#include "vicNl.h"
#include <string.h>
#ifdef VIC_CROPSYST_VERSION
static char vcid[] = "$Id: read_cropparam.c,v 4.5.2.11 2010/11/02 21:12:16 Kiran Chinnayakanahalli Exp $";
// 150928LML____________________________________________________________________
int get_cropping_system_numbers(const veg_con_struct *veg_con_array)
{
  int cropnums = 0;
  for (int i = 0; i < veg_con_array[0].vegetat_type_num; i++)
    if (iscrop(veg_con_array[i].VCS.veg_class_code) && veg_con_array[i].Cv > 1e-8) cropnums++;
  return cropnums;
}
//______________________________________________________________________________
#if VIC_CROPSYST_VERSION==2
crop_data_struct *read_cropparam(FILE *cropparam,
                              int   gridcel
                              /*int   Nveg_type)*/)
/***********************************************************************************
read_cropparam.c       Kiran Chinnayakanahalli (KJC)       kiran.c@wsu.edu, 02102011

This routine reads the crop paramters for the current gridcell and is activated if the
CROPSYST_ON is TRUE. This reads the crop paramter file that is created in the format of
the vegparam file and gets for the current grid cell, the total number of crops that exit
in the grid cell and then for each of those crops, the crop code (identifier) and the
fraction of the grid cell that crop is grown.  Other crop specific paramters can
potentially be added.
***********************************************************************************/
{
  int   i, j;
  int   cropcell, no_of_crops_in_cell;
  char  ErrStr[MAXSTRING];
  char  str[500];
  #if (VIC_CROPSYST_VERSION==2)
  extern crop_lib_struct *crop_lib;
  #endif
  crop_data_struct *crop_temp;

#if !NO_REWIND
  rewind(cropparam);
#endif

  while ( ( fscanf(cropparam, "%d %d", &cropcell, &no_of_crops_in_cell) == 2 ) && cropcell != gridcel ) {
    if (no_of_crops_in_cell < 0) {
      sprintf(ErrStr,"ERROR number of crop tiles (%i) given for cell %i is < 0.\n",no_of_crops_in_cell,cropcell);
      nrerror(ErrStr);
    }
    for (i = 0; i <= no_of_crops_in_cell ; i++){
      if ( fgets(str, 500, cropparam) == NULL ){
        sprintf(ErrStr,"ERROR in read_cropparam. Unexpected EOF for cell %i while reading.\n",cropcell);
        nrerror(ErrStr);
      }
    }
  }
  fgets(str, 500, cropparam); // read newline at end of veg class line to advance to next line
  if (cropcell != gridcel) {
    fprintf(stderr, "Error in crop parameter file.  Grid cell %d not found\n",
            gridcel);
    exit(99);
  }

  /** Allocate memory for vegetation grid cell parameters **/
  crop_temp = (crop_data_struct*) calloc( no_of_crops_in_cell, sizeof(crop_data_struct));
  crop_temp[0].Cv_sum = 0.0;//Why should only the first crop have this inforamtion?? KJC 02102011 (I am replicating the read_vegparam.c)

  for (i = 0; i < no_of_crops_in_cell; i++) {
      crop_temp[i].no_of_crops_in_cell = no_of_crops_in_cell;

      fscanf(cropparam,"%d %f %d %d %d", &crop_temp[i].code, &crop_temp[i].Cv, &crop_temp[i].crop_creation_DOY, &crop_temp[i].TypeCropIrrigated, &crop_temp[i].isPerennial);
      crop_temp[0].Cv_sum += crop_temp[i].Cv;
      crop_temp[i].Cv_sum =crop_temp[0].Cv_sum;

      #if (VIC_CROPSYST_VERSION>=3)
      crop_temp[i].num_of_cropping_types_in_cell = no_of_crops_in_cell;
      crop_temp[i].code_cropping_type = crop_temp[i].code;
      #endif

   /*  if(crop_temp[i].code==1206 ||crop_temp[i].code==703) ||crop_temp[i].code==1210 || crop_temp[i].code==1205 || crop_temp[i].code==1814 || crop_temp[i].code==722 )*/
      //  crop_temp[i].isCropIrrigated=9;

      //LML 141015
      //for(j=0; j<crop_lib[0].no_of_crops_in_lib;j++){
      //   if(crop_temp[i].code == crop_lib[j].crop_code)strcpy(crop_temp[i].crop_name, crop_lib[j].crop_name);
      //}
      #if (VIC_CROPSYST_VERSION==2)
      //150413 RLN In version 3 we work with rotations not specific crops.
      int crop_lib_index = get_crop_lib_index(crop_temp[i].code);
      if (crop_lib_index != -1) strcpy(crop_temp[i].crop_name, crop_lib[crop_lib_index].crop_name);
      #endif
  }
  if (crop_temp->Cv_sum > 1) ///Keyvan has to check to see if k should start from 1 /// Roger
  {
    for (int k = 0; k < no_of_crops_in_cell; k++)
        crop_temp[k].Cv /= crop_temp->Cv_sum;
    crop_temp->Cv_sum = 1.0;
  }
  return no_of_crops_in_cell ? crop_temp : 0;
}
#else
/*LML 150427__________________________________________________________________*/
crop_data_struct *provide_cropparam(const veg_con_struct *veg_con_array, const int gridcel)
{
  //Creat crop_data struct for this grid cell
  int crops = 0;
  int i,j;
  double sum_cv_crop = 0.0;
  char  ErrStr[MAXSTRING];
  crop_data_struct *crop_temp = 0;
  crops = get_cropping_system_numbers(veg_con_array);
  if (crops > 0) {
    crop_temp = (crop_data_struct*) calloc(crops, sizeof(crop_data_struct));
    j = 0;
    for (i = 0; i < veg_con_array[0].vegetat_type_num; i++) {
      if (iscrop(veg_con_array[i].VCS.veg_class_code) && veg_con_array[i].Cv > 1e-8) {
        crop_temp[j].Cv                               = veg_con_array[i].Cv;
        crop_temp[j].code_cropping_type               = veg_con_array[i].VCS.veg_class_code;
        crop_temp[j].no_of_crops_in_cell              = crops;
        crop_temp[j].CropSystHandle                   = 0;                       //150723LML
        sum_cv_crop                                  += crop_temp[j].Cv;
        j++;
      }
    } //i
    crop_temp[0].Cv_sum = sum_cv_crop;
    if (crop_temp[0].Cv_sum > 1.0) {
      sprintf(ErrStr,"Cv_sum for crops cannot greater than 1.0 @ cell:%d\n",gridcel);
      nrerror(ErrStr);
    }
  }
  return crop_temp;
}
#endif
#endif

