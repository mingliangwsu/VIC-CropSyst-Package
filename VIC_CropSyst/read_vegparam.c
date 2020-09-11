#include <stdio.h>
#include <stdlib.h>
#include <vicNl.h>
#include <string.h>

static char vcid[] = "$Id: read_vegparam.c,v 4.5.2.13 2012/02/05 00:15:44 vicadmin Exp $";
#if (VIC_CROPSYST_VERSION>=3)
#include "agronomic/VIC_land_unit_C_interface.h"
#include "assert.h"
#endif

#if (VIC_CROPSYST_VERSION>=3)
  void copy_and_split_veg_con_element(const veg_con_struct &from,
                                      veg_con_struct &to,
                                      int split_num,
                                      int rotation_index,
                                      int set_zone);
#endif

veg_con_struct *read_vegparam(FILE *vegparam,
                              int   gridcel,
                              int   Nveg_type)
/**********************************************************************
  read_vegparam.c    Keith Cherkauer and Dag Lohmann       1997

  This routine reads in vegetation parameters for the current grid cell.
  It also relates each vegetation class in the cell to the appropriate
  parameters in the vegetation library.

  Modifications:
  09-24-98  Modified to read root zone distribution information so
           that soil layer root fractions can be computed for new
	   soil layer depths - see calc_root_fractions.c           KAC
  07-15-99 Modified to read LAI values from a new line in the vegetation
           parameter file.  Added specifically to work with the new
	   global LAI files.
  11-18-02 Added code to read in blowing snow parameters.          LCB
  03-27-03 Modified code to update Wdmax based on LAI values read in
           for the current grid cell.  If LAI is not obtained from this
           function, then the values cacluated in read_veglib.c are
           left unchanged.						DP & KAC
  2006-Nov-07 Allocates MaxVeg+1 veg tiles.				TJB
  2007-May-11 Changed some 'fscanf' statements to 'fgets' and 'sscanf'
	      to count rootzone and BLOWING fields. Also tests for
	      fetch < 1.						GCT
  2007-Oct-31 Added missing brackets in if(options.GLOBAL_LAI) block.	TJB
  2008-Oct-23 Added blocks to free vegarr[].				LCB via TJB
  2009-Jan-16 Added logic for COMPUTE_TREELINE option.			TJB
  2009-Jun-09 Modified to use extension of veg_lib structure to contain
	      bare soil information.					TJB
  2009-Jun-17 Modified to understand both tabs and spaces as delimiters.TJB
  2009-Jun-17 Fixed incorrect placement of free vegarr[] for case of
	      GLOBAL_LAI==FALSE.					TJB
  2009-Jul-26 Allocate extra veg tile for COMPUTE_TREELINE case.	TJB
  2009-Jul-31 Removed extra veg tile for lake/wetland case.		TJB
  2009-Sep-14 Made error messages clearer.				TJB
  2009-Oct-01 Added error message for case of LAI==0 and overstory==1.	TJB
  2010-Apr-28 Replaced GLOBAL_LAI with VEGPARAM_LAI and LAI_SRC.	TJB
  2012-Jan-16 Removed LINK_DEBUG code					BN
  2012-Nov-14  changes due to coupling with cropsyst is done        KM
**********************************************************************/
{

  void ttrim( char *string );
/*1901131 BC++ doesnt like this declared locally (it can't link)
#if (VIC_CROPSYST_VERSION>=3)
  void copy_and_split_veg_con_element(const veg_con_struct &from,
                                      veg_con_struct &to,
                                      int split_num,
                                      int rotation_index,
                                      int set_zone);
#endif
*/
  extern veg_lib_struct *veg_lib;
  extern option_struct   options;
  veg_con_struct *temp;
  int             vegcel, i, j, k, vegetat_type_num, skip, veg_class;
  int             MaxVeg;
  int             Nfields, NfieldsMax;
  int             NoOverstory;
  float           depth_sum;
  float           sum;
  char            str[500];
  char            ErrStr[MAXSTRING];
  char            line[MAXSTRING];
  char            tmpline[MAXSTRING];
  const char      delimiters[] = " \t";
  char            *token;
  char            *vegarr[500];
  size_t          length;
  #if (VIC_CROPSYST_VERSION>=3)
  int             expanded_vegetat_type_count;                                     //each sewing type of crop rotation has one element in the veg_con array
  veg_con_struct *expanded_temp;
  int             sow_index;
  int             allocated_zone = 0;                                            //For mem control
  int             set_zone;
  #endif

#if (VIC_CROPSYST_VERSION==2)
//150413RLN In V3 we work with rotations, not specific crops
  extern crop_lib_struct *crop_lib;
#endif

  if(options.VEGPARAM_LAI) skip = 2;
  else skip = 1;

  NoOverstory = 0;

#if !NO_REWIND
  rewind(vegparam);
#endif

#ifdef VIC_CROPSYST_VERSION //keyvan 11142012
  int crop_class = 0; //This tracks if the veg class is a crop class in the crop library.  If the veg class is a crop class, it will be equal to 1 or zero.
  int Cropland_class_in_veglib = 10; /*This is hardcoded for now.  This is the index of the veg_lib array that stores the global varaibles of the veg type (LAI, Wdmax etc)
                                This is used as a surrogate for a crop that is in the crop library.  The idea is to use these numbers when the crop is not in the growing period.
                                Ideally we want bare soil paramters for these crops to account for evaporation when the crop is not being grown. Will not be used when the crops
                                are in one of the growing stages.  This might go away if we use bare soil. KJC 02142011*/
#endif


  while ( ( fscanf(vegparam, "%d %d", &vegcel, &vegetat_type_num) == 2 ) && vegcel != gridcel ){
    if (vegetat_type_num < 0) {
      sprintf(ErrStr,"ERROR number of vegetation tiles (%i) given for cell %i is < 0.\n",vegetat_type_num,vegcel);
      nrerror(ErrStr);
    }
    for (i = 0; i <= vegetat_type_num * skip; i++){
      if ( fgets(str, 500, vegparam) == NULL ){
        sprintf(ErrStr,"ERROR unexpected EOF for cell %i while reading root zones and LAI\n",vegcel);
        nrerror(ErrStr);
      }
    }
  }
  fgets(str, 500, vegparam); // read newline at end of veg class line to advance to next line
  if (vegcel != gridcel) {
    fprintf(stderr, "Error in vegetation file.  Grid cell %d not found\n",
            gridcel);
    exit(99);
  }
  if(vegetat_type_num >= MAX_VEG) {
    sprintf(ErrStr,"Vegetation parameter file wants more vegetation tiles in grid cell %i (%i) than are allowed by MAX_VEG (%i) [NOTE: bare soil class is assumed].  Edit vicNl_def.h and recompile.",gridcel,vegetat_type_num+1,MAX_VEG);
    nrerror(ErrStr);
  }

  // Make sure to allocate extra memory for bare soil tile
  // and optionally an above-treeline veg tile
  MaxVeg = vegetat_type_num + 1;
  if ( options.AboveTreelineVeg >= 0 )
    MaxVeg++;

  /** Allocate memory for vegetation grid cell parameters **/
  temp = (veg_con_struct*) calloc( MaxVeg, sizeof(veg_con_struct));
  temp[0].Cv_sum = 0.0;

  for (i = 0; i < vegetat_type_num; i++) {
    temp[i].zone_depth = (float*)calloc(options.ROOT_ZONES,sizeof(float));
    temp[i].zone_fract = (float*)calloc(options.ROOT_ZONES,sizeof(float));
    temp[i].vegetat_type_num = vegetat_type_num;

    // Read the root zones line
    if ( fgets( line, MAXSTRING, vegparam ) == NULL ){
      sprintf(ErrStr,"ERROR unexpected EOF for cell %i while reading vegetat_type_num %d\n",vegcel,vegetat_type_num);
      nrerror(ErrStr);
    }
    strcpy(tmpline, line);
    ttrim( tmpline );
    token = strtok (tmpline, delimiters);    /*  token => veg_class, move 'line' pointer to next field */
    Nfields = 0;
    vegarr[Nfields] = (char*)calloc( 500, sizeof(char));
    strcpy(vegarr[Nfields],token);
    Nfields++;

    token = strtok (NULL, delimiters);
    while (token != NULL && (length=strlen(token))==0) token = strtok (NULL, delimiters);
    while ( token != NULL ) {
      vegarr[Nfields] = (char*)calloc( 500, sizeof(char));
      strcpy(vegarr[Nfields],token);
      Nfields++;
      token = strtok (NULL, delimiters);
      while (token != NULL && (length=strlen(token))==0) token = strtok (NULL, delimiters);
    }

    NfieldsMax = 2 + 2 * options.ROOT_ZONES;  /* Number of expected fields this line */
    if( options.BLOWING ){
      NfieldsMax += 3;
    }
    if ( Nfields != NfieldsMax ) {
      sprintf(ErrStr,"ERROR - cell %d - expecting %d fields but found %d in veg line %s\n",gridcel,NfieldsMax, Nfields, line);
      nrerror(ErrStr);
    }

    temp[i].LAKE = 0;
    temp[i].veg_class = atoi(vegarr[0]);
    #if (VIC_CROPSYST_VERSION>=2)
    temp[i].VCS.veg_class_code = atoi(vegarr[0]);
    temp[i].veg_class = get_veg_lib_index(temp[i].VCS.veg_class_code);
    //160817LML assert(temp[i].veg_class != -1);
    #endif
    temp[i].Cv = atof( vegarr[1] );
    depth_sum = 0;
    sum = 0.;
    for(j=0;j<options.ROOT_ZONES;j++) {
      temp[i].zone_depth[j] = atof( vegarr[2 + j*2] );
      temp[i].zone_fract[j] = atof( vegarr[3 + j*2] );
      depth_sum += temp[i].zone_depth[j];
      sum += temp[i].zone_fract[j];
      //std::clog << "zone:" << j << " depth:" << temp[i].zone_depth[j] << " fraction:" << temp[i].zone_fract[j]
      //          << std::endl;
    }
    if(depth_sum <= 0) {
      sprintf(str,"Root zone depths must sum to a value greater than 0.");
      nrerror(str);
    }
    if(sum != 1.) {
      fprintf(stderr,"WARNING: Root zone fractions sum to more than 1 ( = %f), normalizing fractions.  If the sum is large, check that your vegetation parameter file is in the form - <zone 1 depth> <zone 1 fract> <zone 2 depth> <zone 2 fract> ...\n", sum);
      for(j=0;j<options.ROOT_ZONES;j++) {
        temp[i].zone_fract[j] /= sum;
      }
    }

    if(options.BLOWING) {
      j = 2 * options.ROOT_ZONES;
      temp[i].sigma_slope = atof( vegarr[2 + j] );
      temp[i].lag_one = atof( vegarr[3 + j] );
      temp[i].fetch = atof( vegarr[4 + j]) ;
      if( temp[i].sigma_slope <= 0. || temp[i].lag_one <= 0.) {
        sprintf(str,"Deviation of terrain slope must be greater than 0.");
        nrerror(str);
      }
      if( temp[i].fetch < 1.0  ) {
        sprintf(str,"ERROR - BLOWING parameter fetch should be >> 1 but cell %i has fetch = %.2f\n", gridcel, temp[i].fetch );
        nrerror(str);
      }
    }

    //LML 150413 int veg_class_code=0; //Keyvan 130226
    //LML 150413 veg_class = MISSING;
    //LML 150413 for(j=0;j<Nveg_type;j++) {
    //LML 150413   #if (VIC_CROPSYST_VERSION>=3)
    //LML 150413   //LML 141020
    //LML 150413   if(!iscrop(temp[i].veg_class))
    //LML 150413   #endif
    //LML 150413   if(temp[i].veg_class == veg_lib[j].veg_class){
    //LML 150413      veg_class = j; /*This is little inconsistent because the veg_class in the veg library is j+1.  Also this sequence is lost when we have crops.
    //LML 150413                     i.e the crop classes are not sequetntial and should be always identified by the crop code in the crop library.
    //LML 150413                     To be consist, veg_class should have been = veg_lib[j].veg_class, not j. --COMMENTED by KJC 02142011*/
    //LML 150413     //crop_class =0;not needed /*This is original veg type class that is in the veg library and not a crop class in the crop library.  KC 02142011 */
    //LML 150413     veg_class_code=j; //Keyvan 130226
    //LML 150413     break;
    //LML 150413   }
    //LML 150413 }

    //LML 150413 #ifdef VIC_CROPSYST_VERSION //Keyvan 130226
    //LML 150413 int veg_class_index=0;
    //LML 150413 #if (VIC_CROPSYST_VERSION>=3)
    //LML 150413 if(veg_class == MISSING && is_cropping_type_exist(temp[i].veg_class))
    //LML 150413     veg_class = temp[i].veg_class;
    //LML 150413 #else
    //LML 150413 //LML 141015
    //LML 150413 int crop_lib_index = get_crop_lib_index(temp[i].veg_class);
    //LML 150413 if(crop_lib_index != -1){
    //LML 150413     //veg_class_index = veg_class;
    //LML 150413     veg_class_code = crop_lib[crop_lib_index].crop_code;
    //LML 150413     //crop_class = 1;
    //LML 150413 }

    //LML 141015
    //LML 150413 //for(j=0;j<crop_lib[0].no_of_crops_in_lib;j++)
    //LML 150413 //   if(temp[i].veg_class == crop_lib[j].crop_code)
    //LML 150413 //   {
    //LML 150413 //     veg_class_index =  veg_class;
    //LML 150413 //     veg_class_code = crop_lib[j].crop_code;
    //LML 150413 //      crop_class =1; //If veg class is missing then it must be a crop class..KJC 02142011
    //LML 150413 //      break;
    //LML 150413 //   }
    //LML 150413 #endif
    //LML 150413 #endif

    if(temp[i].veg_class == -1/*LML 150413 veg_class == MISSING*/) {
#ifdef VIC_CROPSYST_VERSION //KJC 02132011
      sprintf(ErrStr,"The vegetation class id %i in vegetation tile %i from cell %i is not defined in the vegetation library nor in the crop file.", gridcel, temp[i].VCS.veg_class_code, i, gridcel); //KJC 02132011
#else //KJC 02132011
      sprintf(ErrStr,"The vegetation class id %i in vegetation tile %i from cell %i is not defined in the vegetation library file.", temp[i].veg_class, i, gridcel);
#endif //KJC 02132011

      nrerror(ErrStr);
    }

    temp[0].Cv_sum += temp[i].Cv;

    for(k=0; k<Nfields; k++)
      free(vegarr[k]);

    int veg_lib_index = temp[i].veg_class;                                      //LML 150413
    //LML 150413 #if (VIC_CROPSYST_VERSION>=3)
    //LML 150413 //LML 141020
    //LML 150413 if(!iscrop(temp[i].veg_class))
    //LML 150413     veg_lib_index = get_veg_lib_index(temp[i].veg_class);
    //LML 150413 else
    //LML 150413     veg_lib_index = -1;
    //LML 150413 #else
    //LML 150413 veg_lib_index = temp[i].veg_class;
    //LML 150413 #endif

    if ( options.VEGPARAM_LAI ) {
      // Read the LAI line
      if ( fgets( line, MAXSTRING, vegparam ) == NULL ){
        sprintf(ErrStr,"ERROR unexpected EOF for cell %i while reading LAI for vegetat_type_num %d\n",vegcel,vegetat_type_num);
        nrerror(ErrStr);
      }
      Nfields = 0;
      vegarr[Nfields] = (char*)calloc( 500, sizeof(char));
      strcpy(tmpline, line);
      ttrim( tmpline );
      token = strtok (tmpline, delimiters);
      strcpy(vegarr[Nfields],token);
      Nfields++;

      while( ( token = strtok (NULL, delimiters)) != NULL ){
        vegarr[Nfields] = (char*)calloc( 500, sizeof(char));
        strcpy(vegarr[Nfields],token);
        Nfields++;
      }
      NfieldsMax = 12; /* For LAI */
      if ( Nfields != NfieldsMax ) {
        sprintf(ErrStr,"ERROR - cell %d - expecting %d LAI values but found %d in line %s\n",gridcel, NfieldsMax, Nfields, line);
        nrerror(ErrStr);
      }

      for ( j = 0; j < 12; j++ ) {
        if (options.LAI_SRC == LAI_FROM_VEGPARAM) {
          veg_lib[veg_lib_index/*LML 141020 temp[i].veg_class*/].LAI[j] = atof( vegarr[j] );
          if (veg_lib[veg_lib_index/*LML 141020 temp[i].veg_class*/].overstory && veg_lib[veg_lib_index/*LML 141020 temp[i].veg_class*/].LAI[j] == 0) {
            sprintf(ErrStr,"Warning: cell %d, veg tile %d: the specified veg class (%d) is listed as an overstory class in the veg LIBRARY, but the LAI given in the veg PARAM FILE for this tile for month %d is 0.\n",gridcel, i+1, temp[i].veg_class+1, j+1);
            veg_lib[veg_lib_index].LAI[j] = 0.001;
            //nrerror(ErrStr);
          }
          if(veg_lib_index != -1){  //LML 141020 for crop, this adjustment should be conducted during run-time
            veg_lib[veg_lib_index/*LML 141020 temp[i].veg_class*/].Wdmax[j] =
                    LAI_WATER_FACTOR * veg_lib[veg_lib_index/*LML 141020 temp[i].veg_class*/].LAI[j];
          }
        }
      }
      for(k=0; k<Nfields; k++)
        free(vegarr[k]);
    }

    // Determine if cell contains non-overstory vegetation
    if(veg_lib_index != -1){  //LML 141020 for crop, this counting should be conducted during run-time
        if ( options.COMPUTE_TREELINE && !veg_lib[veg_lib_index/*LML 141020 temp[i].veg_class*/].overstory )
            NoOverstory++;
    }

  } //i loop

  // Determine if we have bare soil
  if (temp[0].Cv_sum > 1.0){
    fprintf(stderr,"WARNING: Cv exceeds 1.0 at grid cell %d, fractions being adjusted to equal 1\n", gridcel);
    for (j = 0; j < vegetat_type_num; j++)
      temp[j].Cv = temp[j].Cv / temp[0].Cv_sum;
    temp[0].Cv_sum = 1.;
  }
  else if(temp[0].Cv_sum>0.99 && temp[0].Cv_sum<1.0){
    fprintf(stderr,"WARNING: Cv > 0.99 and Cv < 1.0 at grid cell %d, model assuming that bare soil is not to be run - fractions being adjusted to equal 1\n", gridcel);
    for (j = 0; j < vegetat_type_num; j++)
      temp[j].Cv = temp[j].Cv / temp[0].Cv_sum;
    temp[0].Cv_sum = 1.;
  }

  // Handle veg above the treeline
  if ( options.SNOW_BAND > 1 && options.COMPUTE_TREELINE
       && ( !NoOverstory && temp[0].Cv_sum == 1. ) ) { //keyvan in our current ver compute_treeline is zero and we do not go through this if block
    // All vegetation in the current cell is defined with overstory.
    // Add default non-overstory vegetation so that snow bands above treeline
    // can be sucessfully simulated.
    if ( options.AboveTreelineVeg < 0 ) {
      // Above treeline snowband should be treated as bare soil
      for ( j = 0; j < vegetat_type_num; j++ )
        temp[j].Cv -= ( 0.001 / (float)vegetat_type_num );
      temp[0].Cv_sum -= 0.001;

    }
    else {
      // Above treeline snowband should use the defined vegetation
      // add vegetation to typenum
      // check that veg type exists in library and does not have overstory
      if(vegetat_type_num > 0) {
        for ( j = 0; j < vegetat_type_num; j++ ) {
          temp[j].Cv -= ( 0.001 / (float)vegetat_type_num );
          temp[j].vegetat_type_num++;
        }
        temp[vegetat_type_num].Cv         = 0.001;
        temp[vegetat_type_num].veg_class  = options.AboveTreelineVeg;
        #ifdef VIC_CROPSYST_VERSION
        temp[vegetat_type_num].VCS.veg_class_code = options.AboveTreelineVeg;
        temp[vegetat_type_num].veg_class      = get_veg_lib_index(options.AboveTreelineVeg);
        assert(temp[vegetat_type_num].veg_class != -1);
        allocated_zone                        = 1;                               //LML 150427
        #endif
        temp[vegetat_type_num].Cv_sum     = temp[vegetat_type_num-1].Cv_sum;
        temp[vegetat_type_num].zone_depth = (float*)calloc( options.ROOT_ZONES,
                                                  sizeof(float));
        temp[vegetat_type_num].zone_fract = (float*)calloc( options.ROOT_ZONES,
                                                  sizeof(float));
        temp[vegetat_type_num].vegetat_type_num = vegetat_type_num+1;

        // Since root zones are not defined they are copied from the last
        // vegetation type.
        for ( j = 0; j < options.ROOT_ZONES; j++ ) {
          temp[vegetat_type_num].zone_depth[j]
            = temp[vegetat_type_num-1].zone_depth[j];
          temp[vegetat_type_num].zone_fract[j]
            = temp[vegetat_type_num-1].zone_fract[j];
        }
      }
      #ifndef VIC_CROPSYST_VERSION
      // Identify current vegetation class
      veg_class = MISSING;
      for ( j = 0; j < Nveg_type; j++ ) {
        if(temp[vegetat_type_num].veg_class == veg_lib[j].veg_class) {
          veg_class = j;
          break;
        }
      }
      if ( veg_class == MISSING ) {
        sprintf(ErrStr,"The vegetation class id %i defined for above-treeline from cell %i is not defined in the vegetation library file.", temp[vegetat_type_num].veg_class, gridcel);
        nrerror(ErrStr);
      }
      else {
        temp[vegetat_type_num].veg_class = veg_class;
      }
      #else
      veg_class = temp[vegetat_type_num].veg_class;
      #endif

      if ( veg_lib[veg_class].overstory ) {
        sprintf(ErrStr,"Vegetation class %i is defined to have overstory, so it cannot be used as the default vegetation type for above canopy snow bands.", veg_lib[veg_class].veg_class );
        nrerror(ErrStr);
      }
    }
  }

  // Bare soil tile
  #if (VIC_CROPSYST_VERSION>=3)
  int max_nveg_code = MAX_VEGCLASS_LIB_CODE; //veg_lib[Nveg_type-1].veg_class;      //LML 141016 there is a risk on original coding system
  #endif
  if (temp[0].Cv_sum < 1.) {
    j = vegetat_type_num;
    temp[j].veg_class =
            //LML 150413 #if (VIC_CROPSYST_VERSION>=3)
            //LML 150413 max_nveg_code+1;                               //LML 141016 original VIC code seems wrong. Nveg_type seems already been used by the last vegetation type.
            //LML 150413 #else
            Nveg_type; // Create a veg_class ID for bare soil, which is not mentioned in the veg library
            //LML 150413 #endif
    #ifdef VIC_CROPSYST_VERSION
    temp[j].VCS.veg_class_code = max_nveg_code+1;                                   //LML 150413
    #endif
    temp[j].Cv = 1.0 - temp[0].Cv_sum;
    // Don't allocate any root-zone-related arrays
    if(options.BLOWING) {
      if (vegetat_type_num > 0) {
        temp[j].sigma_slope = temp[0].sigma_slope;
        temp[j].lag_one = temp[0].lag_one;
        temp[j].fetch = temp[0].fetch;
      }
      else {
        temp[j].sigma_slope = 0.005;
        temp[j].lag_one = 0.95;
        temp[j].fetch = 2000;
      }
    }
  }
  #if (!defined(VIC_CROPSYST_VERSION) || (VIC_CROPSYST_VERSION==2))
  return temp;
  #else
  //LML 150427 expand the crop rotation for each sowing type
  expanded_vegetat_type_count = temp[0].vegetat_type_num;
  for (j = 0; j < vegetat_type_num; j++) {
    if (iscrop(temp[j].VCS.veg_class_code) && /*160531LML IsRotationWithMultipleCycles(temp[j].veg_class_code) &&*/
        temp[j].Cv > 1e-8) {
        i = VIC_get_rotation_cycle_years/*LML 150526 get_years_in_rotation*/(temp[j].VCS.veg_class_code) - 1;
        expanded_vegetat_type_count += i;
    }
  }
  if (expanded_vegetat_type_count == temp[0].vegetat_type_num){
    expanded_temp = temp;
  } else {
    expanded_temp = (veg_con_struct*) calloc(expanded_vegetat_type_count+1, sizeof(veg_con_struct));
    for (j = 0; j <= expanded_vegetat_type_count; j++) {
      expanded_temp[j].zone_depth = (float*)calloc(options.ROOT_ZONES,sizeof(float));
      expanded_temp[j].zone_fract = (float*)calloc(options.ROOT_ZONES,sizeof(float));
    }
    expanded_temp[0].vegetat_type_num = expanded_vegetat_type_count;
    k = 0;                                                                       //index for new expanded veg_con array
    for (j = 0; j <= vegetat_type_num; j++) {
      if (IsRotationWithMultipleCycles(temp[j].VCS.veg_class_code) &&
          temp[j].Cv > 1e-8) {
        i = VIC_get_rotation_cycle_years(temp[j].VCS.veg_class_code) - 1;
      } else {
        i = 0;
      }
      for (sow_index = 0; sow_index <= i; sow_index++) {
        if (j == vegetat_type_num) set_zone = allocated_zone;
        else set_zone = 1;
        copy_and_split_veg_con_element(temp[j],expanded_temp[k],i+1,sow_index,set_zone);
        k++;
      }
    }
    //Clear memory for temp[]
    if (allocated_zone) k = vegetat_type_num + 1;
    else k = vegetat_type_num;
    for (i = 0; i < k; i++) {
        free(temp[i].zone_depth);
        free(temp[i].zone_fract);
    }
    free(temp);
  }
  return expanded_temp;
  #endif
}
/* trim trailing newlines */
#define END '\0'
#define NEW '\n'
//_____________________________________________________________________________/
void ttrim( char *c )
{
  while( (*c++ != END) );
    --c;
  for( ; *--c == NEW; *c = END );

}


