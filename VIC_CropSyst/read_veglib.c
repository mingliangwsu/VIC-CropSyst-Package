#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vicNl.h>

static char vcid[] = "$Id: read_veglib.c,v 4.2.2.5 2012/02/05 00:15:44 vicadmin Exp $";

veg_lib_struct *read_veglib(FILE *veglib, int *Ntype)
/**********************************************************************
  read_veglib.c               Keith Cherkauer                 1997

  This routine reads in a library of vegetation parameters for all
  vegetation classes used in the model.  The veg class number is used
  to reference the information in this library.

  Modifications:
  09-24-98 Modified to remove root fractions from the library file.
           See read_vegparam.c and calc_root_fraction.c for new
           root fraction distribution information.               	KAC
  2009-Jun-09 Modified to use extension of veg_lib structure to contain
	      bare soil information, as well as other land cover types
	      used in potential evap calcualtions.			TJB
  2009-Oct-01 Added error message for case of LAI==0 and overstory==1.	TJB
  2010-Apr-28 Replaced GLOBAL_LAI with VEGPARAM_LAI and LAI_SRC.	TJB
  2012-Jan-16 Removed LINK_DEBUG code					BN
**********************************************************************/
{
  extern option_struct options;
  veg_lib_struct *temp;
  int    i, j;
  int    tmpflag;
  int    Nveg_type;
  char   str[MAXSTRING];
  char   ErrStr[MAXSTRING];
  double maxd;

  rewind(veglib);
  Nveg_type = 0;
  while(!feof(veglib)) {
    fgets(str,MAXSTRING,veglib);
    if(str[0]<=57 && str[0]>=48) Nveg_type++;
    //fgets(str,MAXSTRING,veglib);
  }
  rewind(veglib);
  temp = (veg_lib_struct *)calloc(Nveg_type+N_PET_TYPES_NON_NAT,sizeof(veg_lib_struct));
  fscanf(veglib, "%s", str);
  i=0;
  while (!feof(veglib)) {
    if(str[0]<=57 && str[0]>=48) {
      temp[i].NVegLibTypes = Nveg_type;
      temp[i].veg_class = atoi(str);
      #if (VIC_CROPSYST_VERSION>=3)
      temp[i].veg_class = i;
      temp[i].VCS.veg_class_code = atoi(str);
      #endif
      fscanf(veglib, "%d",  &tmpflag);
      if(tmpflag==0) temp[i].overstory = FALSE;
      else temp[i].overstory = TRUE;
      fscanf(veglib, "%lf", &temp[i].rarc);
      fscanf(veglib, "%lf", &temp[i].rmin);
      for (j = 0; j < 12; j++) {
        fscanf(veglib, "%lf", &temp[i].LAI[j]);
        if (options.LAI_SRC == LAI_FROM_VEGLIB && temp[i].overstory && temp[i].LAI[j] == 0) {
          sprintf(ErrStr,"ERROR: veg library: the specified veg class (%d) is listed as an overstory class, but the LAI given for this class for month %d is 0\n", temp[i].veg_class, j);
          nrerror(ErrStr);
        }
        temp[i].Wdmax[j] = LAI_WATER_FACTOR * temp[i].LAI[j];
      }
      for (j = 0; j < 12; j++) {
        fscanf(veglib, "%lf", &temp[i].albedo[j]);
      }
      for (j = 0; j < 12; j++) {
        fscanf(veglib, "%lf", &temp[i].roughness[j]);
      }
      temp[i].wind_h = 0.;
      maxd = 0;
      for (j = 0; j < 12; j++) {
        fscanf(veglib, "%lf", &temp[i].displacement[j]);
        if(temp[i].displacement[j] > maxd) maxd = temp[i].displacement[j];
        if(temp[i].LAI[j] > 0 && temp[i].displacement[j] <= 0) {
          sprintf(str,"Vegetation has leaves (LAI = %f), but no displacement (%f)",
	          temp[i].LAI[j], temp[i].displacement[j]);
          nrerror(str);
        }
        if(temp[i].albedo[j] < 0 || temp[i].albedo[j] > 1) {
          sprintf(str,"Albedo must be between 0 and 1 (%f)",
	          temp[i].albedo[j]);
          nrerror(str);
        }
      }
      fscanf(veglib, "%lf", &temp[i].wind_h);
      if(temp[i].wind_h < maxd && temp[i].overstory) {
        sprintf(str,"Vegetation reference height (%f) for vegetation class %d, must be greater than the maximum displacement height (%f) when OVERSTORY has been set TRUE.",
                temp[i].wind_h,temp[i].veg_class,maxd);
        nrerror(str);
      }
      fscanf(veglib, "%f",  &temp[i].RGL);         /* minimum value of incoming
						    solar radiation at which there
						   will still be transpiration */
      if(temp[i].RGL < 0) {
        sprintf(str,"Minimum value of incoming solar radiation at which there is transpiration (RGL) must be greater than 0 for vegetation class %d.  Check that the vegetation library has the correct number of columns.",
                temp[i].veg_class);
        nrerror(str);
      }
      fscanf(veglib, "%lf", &temp[i].rad_atten);   /* vegetation radiation
						      attenuation factor */
      if(temp[i].rad_atten < 0 || temp[i].rad_atten > 1) {
        sprintf(str,"The vegetation radiation attenuation factor must be greater than 0, and less than 1 for vegetation class %d.  Check that the vegetation library has the correct number of columns.",
                temp[i].veg_class);
        nrerror(str);
      }
      fscanf(veglib, "%lf", &temp[i].wind_atten);  /* canopy wind speed
						      attenuation factor */
      fscanf(veglib, "%lf", &temp[i].trunk_ratio); /* ratio of tree height that
						      is trunk */
      #if (VIC_CROPSYST_VERSION>=3)
      fscanf(veglib, "%s",temp[i].VCS.veg_or_rotation_name);
      #endif
      fgets(str, MAXSTRING, veglib);	/* skip over end of line comments */
      i++;
    }
    else fgets(str, MAXSTRING, veglib);
    fscanf(veglib, "%s", str);
  }
  //161031LML if(i!=Nveg_type) {
  //  sprintf(ErrStr,"ERROR: Problem reading vegetation library file - make sure the file has the right number of rows.\n"); //keyvan changed column to row in comment. 130204
  //161031LML   nrerror(ErrStr);
  //161031LML }
  *Ntype = Nveg_type;

  #if (VIC_CROPSYST_VERSION>=3)
  int max_nveg_code = MAX_VEGCLASS_LIB_CODE;//temp[Nveg_type-1].veg_class;      //LML 141016 there is a risk on original coding system
  #endif
  for (i = 0; i < N_PET_TYPES_NON_NAT; i++) {
    temp[Nveg_type+i].NVegLibTypes = Nveg_type;
    temp[Nveg_type+i].veg_class =
            //LML 150413 #if (VIC_CROPSYST_VERSION>=3)
            //LML 150413 max_nveg_code+i+1;                          //LML 141016
            //LML 150413 #else
            Nveg_type + i;
            //LML 150413 #endif

    #if (VIC_CROPSYST_VERSION>=3)
    temp[Nveg_type+i].VCS.veg_class_code = max_nveg_code + 1 + i;
    #endif
    temp[Nveg_type+i].overstory = ref_veg_over[i];
    temp[Nveg_type+i].rarc = ref_veg_rarc[i];
    temp[Nveg_type+i].rmin = ref_veg_rmin[i];
    for (j=0; j<12; j++) {
      temp[Nveg_type+i].LAI[j] = ref_veg_lai[i];
      temp[Nveg_type+i].Wdmax[j] = LAI_WATER_FACTOR*ref_veg_lai[i];
      temp[Nveg_type+i].albedo[j] = ref_veg_albedo[i];
      temp[Nveg_type+i].roughness[j] = ref_veg_rough[i];
      temp[Nveg_type+i].displacement[j] = ref_veg_displ[i];
    }
    temp[Nveg_type+i].wind_h = ref_veg_wind_h[i];
    temp[Nveg_type+i].RGL = ref_veg_RGL[i];
    temp[Nveg_type+i].rad_atten = ref_veg_rad_atten[i];
    temp[Nveg_type+i].wind_atten = ref_veg_wind_atten[i];
    temp[Nveg_type+i].trunk_ratio = ref_veg_trunk_ratio[i];
  }

  return temp;
}

/**********************************************************************
  free_veglib              Ted Bohn		April 2007

  This routine frees the veglib structure.

  Modifications:

**********************************************************************/
void free_veglib(veg_lib_struct **veg_lib) {
  free((char*)(*veg_lib));
}
//______________________________________________________________________________
#if VIC_CROPSYST_VERSION>=3
void free_cropcodelib(CropCodeLib **cropcode_lib) {
  free((char*)(*cropcode_lib));
}
#endif
//______________________________________________________________________________
#if VIC_CROPSYST_VERSION>=3
CropCodeLib *read_cropcodelib(FILE *cropcodelib, int &account)
{
    CropCodeLib *temp;
    char   str[50];
    rewind(cropcodelib);
    account = 0;
    while(fgets(str,MAXSTRING,cropcodelib)) {
      if (str[0] >= '0' && str[0] <= '9') account++;                             //190131RLN
    }
    rewind(cropcodelib);
    temp = (CropCodeLib *)calloc(account,sizeof(CropCodeLib));
    fgets(str,MAXSTRING,cropcodelib);
    int i=0;
    while (i < account) {
          fscanf(cropcodelib, "%d %[^\t\r\n]",&temp[i].code, &temp[i].cropname);
          //std::clog << "i:" << i << " cropname:" << "\"" << temp[i].cropname << "\"" << " code: " << temp[i].code << std::endl;
          i++;
    }
    return temp;
}
#endif
