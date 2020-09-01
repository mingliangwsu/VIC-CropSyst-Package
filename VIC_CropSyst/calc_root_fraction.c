#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vicNl.h>

static char vcid[] = "$Id: calc_root_fraction.c,v 4.1 2000/05/16 21:07:16 vicadmin Exp $";

void calc_root_fractions(const soil_con_struct &soil_con, veg_con_struct *veg_con)
/**********************************************************************
  calc_root_fraction.c    Keith Cherkauer      September 24, 1998

  This routine computes the fraction of roots in each soil layer based
  on the root zone distribution defined in the vegetation parameter
  file.  Roots are assumed to be linearly distributed within each
  root zone.

**********************************************************************/
{
  extern option_struct options;
  int Nveg      = veg_con[0].vegetat_type_num;
  for (int veg = 0; veg < Nveg; veg++) {
    veg_con_struct &cur_veg_con = veg_con[veg];
    double sum_fract = 0;
    int layer = 0;
    double Lstep = soil_con.depth[layer];
    double Lsum = Lstep;
    double Zsum = 0;
    int zone = 0;
    while (zone < options.ROOT_ZONES) {
      double Zstep = (double)cur_veg_con.zone_depth[zone];
      if((Zsum + Zstep) <= Lsum && Zsum >= Lsum - Lstep) {
        //** CASE 1: Root Zone Completely in Soil Layer **
        sum_fract += cur_veg_con.zone_fract[zone];
      } else {
        double Zmin_fract = 0;
        double Zmin_depth = 0;
        double Zmax = 0;
        //** CASE 2: Root Zone Partially in Soil Layer **
        if(Zsum < Lsum - Lstep) {
          //** Root zone starts in previous soil layer **
          Zmin_depth = Lsum - Lstep;
          Zmin_fract = linear_interp(Zmin_depth,Zsum,Zsum+Zstep,0,
                     cur_veg_con.zone_fract[zone]);
        } else {
          //** Root zone starts in current soil layer **
          Zmin_depth = Zsum;
          Zmin_fract = 0.;
        }
        if((Zsum + Zstep) <= Lsum) {
          //** Root zone ends in current layer **
          Zmax = Zsum + Zstep;
        } else {
          //** Root zone extends beyond bottom of current layer **
          Zmax = Lsum;
        }
        sum_fract += linear_interp(Zmax,Zsum,Zsum+Zstep,0,
                   cur_veg_con.zone_fract[zone]) - Zmin_fract;
      }

      /** Update Root Zone and Soil Layer **/
      if((Zsum + Zstep) < Lsum) {
        Zsum += Zstep;
        zone++;
      } else if((Zsum + Zstep) == Lsum) {
        Zsum += Zstep;
        zone++;
        if(layer < options.Nlayer) {
          cur_veg_con.root[layer] = sum_fract;
          sum_fract = 0.;
        }
        layer++;
        if(layer < options.Nlayer) {
          Lstep  = soil_con.depth[layer];
          Lsum  += Lstep;
        } else if (layer == options.Nlayer) {
          Lstep  = Zsum + Zstep - Lsum;
          if(zone<options.ROOT_ZONES-1) {
            for (int i = zone + 1; i < options.ROOT_ZONES; i++)
              Lstep += cur_veg_con.zone_depth[i];
          }
          Lsum  += Lstep;
        }
      } else if ((Zsum + Zstep) > Lsum) {
        if (layer < options.Nlayer) {
          cur_veg_con.root[layer] = sum_fract;
          sum_fract = 0.;
        }
        layer++;
        if (layer < options.Nlayer) {
          Lstep  = soil_con.depth[layer];
          Lsum  += Lstep;
        } else if (layer == options.Nlayer) {
          Lstep  = Zsum + Zstep - Lsum;
          if(zone < (options.ROOT_ZONES - 1)) {
            for (int i = zone + 1; i < options.ROOT_ZONES; i++)
              Lstep += cur_veg_con.zone_depth[i];
          }
          Lsum  += Lstep;
        }
      }
    } //while

    if(sum_fract > 0 && layer >= options.Nlayer) {
      cur_veg_con.root[options.Nlayer-1] += sum_fract;
    } else if(sum_fract > 0) {
      cur_veg_con.root[layer] += sum_fract;
    }
    double dum = 0.;
    for (int layer = 0; layer < options.Nlayer; layer++) {
      if(cur_veg_con.root[layer] < 1.e-4) cur_veg_con.root[layer] = 0.;
      dum += cur_veg_con.root[layer];
    }
    if (is_approximately(dum,0.0,1e-6)) {
      char ErrStr[MAXSTRING];
      sprintf(ErrStr,"Root fractions sum equals zero: %f , Vege Class: %d\n",
          dum, cur_veg_con.veg_class);
      nrerror(ErrStr);
    }
    for (int layer = 0; layer < options.Nlayer; layer++) {
      cur_veg_con.root[layer] /= dum;
    }
  } //veg
}

