#include <stdio.h>
#include <stdlib.h>
#include <vicNl.h>

static char vcid[] = "$Id: initialize_veg.c,v 4.2 2003/04/22 18:46:33 cherkaue Exp $";

void initialize_veg(veg_var_struct      **veg_var,
		    veg_con_struct       *veg_con,
            int                    Nveg
            //150929LML #if VIC_CROPSYST_VERSION>=3
            //150929LML ,crop_data_struct    ***crop_state
            //150929LML #endif
            #if VIC_CROPSYST_VERSION>=3
            ,soil_con_struct *soil_con
            //150930LML ,Grid_cell_crop_output *grid_cell_out_ref
            #endif
                    )
/**********************************************************************
  initialize_veg		Dag Lohmann	 January 1996

  This routine initailizes the vegetation variable array.

  Modifications:
  07-13-98 modified to initialize vegetation structure for all 
           defined elevation bands                                 KAC
  11-18-02 modified to get the maximum number of vegetation types
           passed to it.  This allows the maximum number of vegetation
           types to include the wetland vegetation fraction when the 
           lake model is active.                                  LCB

**********************************************************************/
{
  extern option_struct   options;
  for (int i = 0 ; i < Nveg ; i++) {
    for (int j = 0 ; j < options.SNOW_BAND ; j++ ) {
      veg_var[i][j].Wdew = 0.0;
      veg_var[i][j].throughfall = 0.0;
      //150929LML
      #if (VIC_CROPSYST_VERSION>=3)
      veg_var[i][j].crop_state = 0;
      double temp = soil_con->AreaFract[j] * veg_con[i].Cv;
      if (iscrop(veg_con[i].VCS.veg_class_code) && (temp > 1e-10)) {
        veg_var[i][j].crop_state                 = new crop_data_struct;
        crop_data_struct *&tcrop = veg_var[i][j].crop_state;
        tcrop->CropSystHandle = 0;
        tcrop->Cv             = temp;
        tcrop->crop_leaf_area_index = 0;
        //180402LML may ned initialize other values
      }
      #endif
    }
  }
}
