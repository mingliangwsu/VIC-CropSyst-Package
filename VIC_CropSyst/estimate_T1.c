#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vicNl.h>

static char vcid[] = "$Id: estimate_T1.c,v 4.2 2000/05/16 21:57:54 vicadmin Exp $";

double estimate_T1(double Ts,                                                    //surface temperature
                   double T1_old,
                   double T2,
                   double D1,                                                    //the depth of the first soil layer
                   double D2,                                                    //the depth of the second soil layer
                   double kappa1,                                                //thermal conductivity
                   double kappa2,
                   double Cs1,                                                   //soil heat capacity
                   double Cs2,
                   double dp,                                                    //diurnal damping depth
                   double delta_t) {
/**********************************************************************
  estimate_T1                Keith Cherkauer          July 15, 1998

  uses Xu Liangs 3-layer energy balance formulation to estimate the 
  temperature between the first and second layers.  Formerly calculated
  independently in each of the surface energy balance equation routines.

  Modifications:
  01-20-00 removed from end of func_surf_energy_bal.c and put into a
           separate file                                           KAC

**********************************************************************/
  double tmp1 = exp(-D1 / dp);
  double tmp2 = exp(-D2 / dp);
  double C1 = Cs2 * dp / D2 * ( 1. - tmp2);
  double C2 = - ( 1. - 1. / tmp1 /*exp(D1 / dp)*/ ) * tmp2;
  double C3 = kappa1 / D1 - kappa2 / D1 + kappa2 / D1 * tmp1;
  double T1 = (kappa1 / 2. / D1 / D2 * (Ts) + C1 / delta_t * T1_old
              + (2. * C2 - 1. + tmp1) * kappa2 / 2. / D1 / D2 * T2)
              / (C1 / delta_t + kappa2 / D1 / D2 * C2 + C3 / 2. / D2);
  return T1;
}
