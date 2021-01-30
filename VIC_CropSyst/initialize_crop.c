
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vicNl.h"
#ifdef VIC_CROPSYST_VERSION
#include "crop/growth_stages.h"

static char vcid[] = "$Id: initialize_crop->c,v 5.12.2.15 2010/08/17 02:32:07 vicadmin Exp $";

void initialize_crop(crop_data_struct *crop)
/**
* Created by Kiran Chinnayakanahalli for initalizing the crop object
**/
{


    extern option_struct options;
    int lidx=0;
    //LML 141020 crop_data_struct crop_local;

    //*crop_local =(&crop);

    for(lidx=0;lidx<options.Nlayer;lidx++){
        crop->transpiration[lidx] = 0;
    }
    crop->biomass_after_harvest =0.0;
    crop->biomass_current=0.0;
    crop->crop_leaf_area_index=0.0;
    crop->water_stress_index=0.0;
    crop->biomass_yield=0.0;
    crop->refill_water_depth=0.0;
    //crop->CropSystHandle = 0;
    crop->irrigation_netdemand = 0.0;
    crop->irrigation_water=0.0;
    //crop->net_irrigation=0.0;
    crop->growth_stage = NGS_QUIESCENCE; //200208RLN NGS_DORMANT_or_INACTIVE;
    crop->water_stress_index = 0;

    //(*crop)=crop_local;

}
#endif

