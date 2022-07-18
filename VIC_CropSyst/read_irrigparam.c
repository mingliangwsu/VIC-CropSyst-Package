#include <stdio.h>
#include <stdlib.h>
#include <vicNl.h>
#include <string.h>

static char vcid[] = "$Id: read_irrigparam.c,v 1.0 2016/06/09LML$";
#if (VIC_CROPSYST_VERSION>=3) && defined(USE_IRRIGATION_PARAM_FILE)
void read_irrigparam(FILE *irrigparam, const int gridcel, Irrigation_Types_In_Each_Cell &irrigation_list)
/**********************************************************************
  read irrigation parameter for gridcel. 160609LML
  In each grid cell, there is a list for the irrigation for each crop types.
  If there is no records for certain crop type (code), it means no irrigation.
  [crop_code] [1: full irrigation; 0: partial] [irrigation_type]

**********************************************************************/
{
  void ttrim( char *string );
  Irrigation_Type get_irrigation_type_from_str(const char *name);
  int             vegcel, i, vegetat_type_num;
  char            str[500];
  char            ErrStr[MAXSTRING];
  char            line[MAXSTRING];
  #if !NO_REWIND
  rewind(irrigparam);
  #endif
  while ( ( fscanf(irrigparam, "%d %d", &vegcel, &vegetat_type_num) == 2 ) && vegcel != gridcel ) {
    if (vegetat_type_num < 0) {
      sprintf(ErrStr,"ERROR number of vegetation tiles (%i) given for cell %i is < 0.\n",vegetat_type_num,vegcel);
      nrerror(ErrStr);
    }
    for (i = 0; i <= vegetat_type_num; i++){
      if ( fgets(str, 500, irrigparam) == NULL ){
        sprintf(ErrStr,"ERROR unexpected EOF for cell %i while reading irrigation_parameter\n",vegcel);
        nrerror(ErrStr);
      }
    }
  }
  fgets(str, 500, irrigparam); // read newline at end of veg class line to advance to next line
  if (vegcel != gridcel) {
    //170328LML fprintf(stderr, "Error in vegetation file.  Grid cell %d not found\n",
    //170328LML        gridcel);
    //170328LML exit(99);
    return; //If no record, it means no irrigation.
  }
  if(vegetat_type_num >= MAX_NUM_CROP_TYPES_FOR_IRRIGATION_DEFINE) {
    sprintf(ErrStr,"Irrigation parameter file wants more crop tiles in grid cell %i (%i) than are allowed by MAX_NUM_CROP_TYPES_FOR_IRRIGATION_DEFINE (%i).  Edit vicNl_def.h and recompile."
            ,gridcel,vegetat_type_num+1,MAX_NUM_CROP_TYPES_FOR_IRRIGATION_DEFINE);
    nrerror(ErrStr);
  }
  for (i = 0; i < vegetat_type_num; i++) {
    if ( fgets( line, MAXSTRING, irrigparam ) == NULL ){
      sprintf(ErrStr,"ERROR unexpected EOF for cell %i while reading vegetat_type_num %d\n",vegcel,vegetat_type_num);
      nrerror(ErrStr);
    }
    char *pch;
    ttrim(line);
    pch = strtok(line," \t");
    irrigation_list.crop_code[i] = atoi(pch);
    pch = strtok(NULL," \t");
    if ((pch[0] == '1') || (pch[0] == '0')) {
        irrigation_list.full_irrigation[i] = atoi(pch);
        irrigation_list.fraction_full_irrigation[i] = atof(pch);
        pch = strtok(NULL," \t");
    } else {
        irrigation_list.full_irrigation[i] = true;
        irrigation_list.fraction_full_irrigation[i] = 1.0;
    }
    irrigation_list.irrigation_type[i] = get_irrigation_type_from_str(pch);
  } //i loop
}
//______________________________________________________________________________
//160609LML____________________________________________________________________/
Irrigation_Type get_irrigation_type_from_str(const char *name)
{
  char            ErrStr[MAXSTRING];
  Irrigation_Type type = NO_IRRIGATION;
  if (strstr(name,"NO_IRRIGATION")                                  != 0) type = NO_IRRIGATION;
  else if (strstr(name,"CENTER_PIVOT")                              != 0) type = DEFAULT_CENTER_PIVOT;
  else if (strstr(name,"SPRINKLER")                                 != 0) type = DEFAULT_SPRINKLER;
  else if (strstr(name,"BIG_GUN")                                   != 0) type = DEFAULT_BIG_GUN;
  else if (strstr(name,"SOLID_SET")                                 != 0) type = DEFAULT_SOLID_SET;
  else if (strstr(name,"MOVING_WHEEL")                              != 0) type = DEFAULT_MOVING_WHEEL;
  else if (strstr(name,"WHEEL_LINE")                                != 0) type = DEFAULT_MOVING_WHEEL;
  else if (strstr(name,"DRIP")                                      != 0) type = DEFAULT_DRIP;
  else if (strstr(name,"FLOOD")                                     != 0) type = DEFAULT_FLOOD;
  else if (strstr(name,"RILL")                                      != 0) type = DEFAULT_RILL;
  else if (strstr(name,"FURROW")                                    != 0) type = DEFAULT_FURROW;
  else if (strstr(name,"DEFAULT_CENTER_PIVOT")                      != 0) type = DEFAULT_CENTER_PIVOT;
  else if (strstr(name,"DEFAULT_BIG_GUN")                           != 0) type = DEFAULT_BIG_GUN;
  else if (strstr(name,"DEFAULT_SOLID_SET")                         != 0) type = DEFAULT_SOLID_SET;
  else if (strstr(name,"DEFAULT_MOVING_WHEEL")                      != 0) type = DEFAULT_MOVING_WHEEL;
  else if (strstr(name,"DEFAULT_DRIP")                              != 0) type = DEFAULT_DRIP;
  else if (strstr(name,"DEFAULT_FLOOD")                             != 0) type = DEFAULT_FLOOD;
  else if (strstr(name,"DEFAULT_RILL")                              != 0) type = DEFAULT_RILL;
  else if (strstr(name,"DEFAULT_FURROW")                            != 0) type = DEFAULT_FURROW;
  else if (strstr(name,"DEFAULT_SPRINKLER")                         != 0) type = DEFAULT_SPRINKLER;
  else if (strstr(name,"DEFAULT_MICRO_SPRINKLER")                   != 0) type = DEFAULT_MICRO_SPRINKLER;
  else if (strstr(name,"IrrigTP_CP_impact_14VH_RainBird")           != 0) type = IrrigTP_CP_impact_14VH_RainBird;
  else if (strstr(name,"IrrigTP_CP_impact_M20VH_PM_RainBird")       != 0) type = IrrigTP_CP_impact_M20VH_PM_RainBird;
  else if (strstr(name,"IrrigTP_CP_impact_65PJ_RainBird")           != 0) type = IrrigTP_CP_impact_65PJ_RainBird;
  else if (strstr(name,"IrrigTP_CP_impact_30FH_30FWH_RainBird")     != 0) type = IrrigTP_CP_impact_30FH_30FWH_RainBird;
  else if (strstr(name,"IrrigTP_CP_impact_L36H_L36AH_RainBird_1")   != 0) type = IrrigTP_CP_impact_L36H_L36AH_RainBird_1;
  else if (strstr(name,"IrrigTP_CP_impact_L36H_L36AH_RainBird_2")   != 0) type = IrrigTP_CP_impact_L36H_L36AH_RainBird_2;
  else if (strstr(name,"IrrigTP_CP_impact_85EHD_RainBird_1")        != 0) type = IrrigTP_CP_impact_85EHD_RainBird_1;
  else if (strstr(name,"IrrigTP_CP_impact_85EHD_RainBird_2")        != 0) type = IrrigTP_CP_impact_85EHD_RainBird_2;
  else if (strstr(name,"IrrigTP_CP_impact_85EHD_RainBird_3")        != 0) type = IrrigTP_CP_impact_85EHD_RainBird_3;
  else if (strstr(name,"IrrigTP_CP_impact_85EHD_LA_RainBird_1")     != 0) type = IrrigTP_CP_impact_85EHD_LA_RainBird_1;
  else if (strstr(name,"IrrigTP_CP_impact_85EHD_LA_RainBird_2")     != 0) type = IrrigTP_CP_impact_85EHD_LA_RainBird_2;
  else if (strstr(name,"IrrigTP_CP_impact_85EHD_LA_RainBird_3")     != 0) type = IrrigTP_CP_impact_85EHD_LA_RainBird_3;
  else if (strstr(name,"IrrigTP_CP_spray_S3000_Nelson")             != 0) type = IrrigTP_CP_spray_S3000_Nelson;
  else if (strstr(name,"IrrigTP_CP_spray_O3000_Nelson")             != 0) type = IrrigTP_CP_spray_O3000_Nelson;
  else if (strstr(name,"IrrigTP_CP_spray_R3000_Nelson_1")           != 0) type = IrrigTP_CP_spray_R3000_Nelson_1;
  else if (strstr(name,"IrrigTP_CP_spray_R3000_Nelson_2")           != 0) type = IrrigTP_CP_spray_R3000_Nelson_2;
  else if (strstr(name,"IrrigTP_CP_spray_A3000_Nelson_1")           != 0) type = IrrigTP_CP_spray_A3000_Nelson_1;
  else if (strstr(name,"IrrigTP_CP_spray_A3000_Nelson_2")           != 0) type = IrrigTP_CP_spray_A3000_Nelson_2;
  else if (strstr(name,"IrrigTP_Big_Gun_75TR_Nelson_1")             != 0) type = IrrigTP_Big_Gun_75TR_Nelson_1;
  else if (strstr(name,"IrrigTP_Big_Gun_75TR_Nelson_2")             != 0) type = IrrigTP_Big_Gun_75TR_Nelson_2;
  else if (strstr(name,"IrrigTP_Big_Gun_75TR_Nelson_3")             != 0) type = IrrigTP_Big_Gun_75TR_Nelson_3;
  else if (strstr(name,"IrrigTP_Big_Gun_150TB_Nelson_1")            != 0) type = IrrigTP_Big_Gun_150TB_Nelson_1;
  else if (strstr(name,"IrrigTP_Big_Gun_150TB_Nelson_2")            != 0) type = IrrigTP_Big_Gun_150TB_Nelson_2;
  else if (strstr(name,"IrrigTP_Big_Gun_150TB_Nelson_3")            != 0) type = IrrigTP_Big_Gun_150TB_Nelson_3;
  else if (strstr(name,"IrrigTP_Big_Gun_200TB_Nelson_1")            != 0) type = IrrigTP_Big_Gun_200TB_Nelson_1;
  else if (strstr(name,"IrrigTP_Big_Gun_200TB_Nelson_2")            != 0) type = IrrigTP_Big_Gun_200TB_Nelson_2;
  else if (strstr(name,"IrrigTP_Big_Gun_200TB_Nelson_3")            != 0) type = IrrigTP_Big_Gun_200TB_Nelson_3;
  else if (strstr(name,"IrrigTP_Solid_set_R5_POP_UP_Nelson")        != 0) type = IrrigTP_Solid_set_R5_POP_UP_Nelson;
  else if (strstr(name,"IrrigTP_Solid_set_R2000WF_6_Nelson_1")      != 0) type = IrrigTP_Solid_set_R2000WF_6_Nelson_1;
  else if (strstr(name,"IrrigTP_Solid_set_R2000WF_6_Nelson_2")      != 0) type = IrrigTP_Solid_set_R2000WF_6_Nelson_2;
  else if (strstr(name,"IrrigTP_Solid_set_R2000WF_6_Nelson_3")      != 0) type = IrrigTP_Solid_set_R2000WF_6_Nelson_3;
  else if (strstr(name,"IrrigTP_Solid_set_R33_Nelson")              != 0) type = IrrigTP_Solid_set_R33_Nelson;
  else if (strstr(name,"IrrigTP_Moving_wheel_R2000WF_6_Nelson_1")   != 0) type = IrrigTP_Moving_wheel_R2000WF_6_Nelson_1;
  else if (strstr(name,"IrrigTP_Moving_wheel_R2000WF_6_Nelson_2")   != 0) type = IrrigTP_Moving_wheel_R2000WF_6_Nelson_2;
  else if (strstr(name,"IrrigTP_Moving_wheel_R2000WF_6_Nelson_3")   != 0) type = IrrigTP_Moving_wheel_R2000WF_6_Nelson_3;
  else if (strstr(name,"IrrigTP_drip_0_0")                          != 0) type = IrrigTP_drip_0_0;
  else if (strstr(name,"IrrigTP_Sub_surf_drip_0_0")                 != 0) type = IrrigTP_Sub_surf_drip_0_0;
  else if (strstr(name,"IrrigTP_Sub_surf_drip_perfect")             != 0) type = IrrigTP_Sub_surf_drip_perfect;
  else if (strstr(name,"IrrigTP_Sub_surf_drip_perfect_eliminate_top") != 0) type = IrrigTP_Sub_surf_drip_perfect_eliminate_top;
  else if (strstr(name,"IrrigTP_flood_0_0")                         != 0) type = IrrigTP_flood_0_0;
  else if (strstr(name,"IrrigTP_rill_0_0")                          != 0) type = IrrigTP_rill_0_0;
  else if (strstr(name,"IrrigTP_furrow_0_0")                        != 0) type = IrrigTP_furrow_0_0;
  else {
    sprintf(ErrStr,"ERROR cannot find the irrigation type: %s in irrigation parameter file!!!\n",name);
    nrerror(ErrStr);
  }
  return type;
}
#endif //VIC_CROPSYST_VERSION>=3
