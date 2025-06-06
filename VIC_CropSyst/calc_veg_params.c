#include <stdio.h>
#include <vicNl.h>
#include <math.h>

static char vcid[] = "$Id: calc_veg_params.c,v 3.1 1999/02/16 18:02:07 vicadmin Exp $";

double calc_veg_displacement(const double height) {
/**********************************************************************
  calc_veg_displacement		Keith Cherkauer		January 27, 1997
  This subroutine estimates the displacement height of vegetation
  with a given average height based on equations on page 4.12 of the
  Handbook of Hydrology.
**********************************************************************/
  return 0.67 * height;
}
//______________________________________________________________________________
double calc_veg_height(const double displacement) {
/**********************************************************************
  calc_veg_height		Keith Cherkauer		March 3, 1997

  This subroutine backs the vegetation height out of the given
  displacement using the reverse procedure from cal_veg_displacement.
**********************************************************************/
  return displacement / 0.67;
}
//______________________________________________________________________________
double calc_veg_roughness(const double height) {
/**********************************************************************
  calc_veg_roughness		Keith Cherkauer		January 27, 1997
  This subroutine estimates the roughness height of vegetation
  with a given average height based on equations on page 4.12 of the
  Handbook of Hydrology.
**********************************************************************/
  return 0.123 * height;
}
